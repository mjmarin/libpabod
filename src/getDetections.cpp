#include <getDetections.h>


static const Model *model = NULL;
static const rules *rul = NULL;
static node *Q = NULL;
static int startSymbol = 0;
static int interval = 0;



/** Inserts the node (symbol) <tt>n</tt> on the symbol list (stack)
 *  \param n - is the insertion node
 *  \param cur - is the position of insertion of the node
 *  \param probex - value used to generate the node score
 *  \param probey - value used to generate the node score
 *  \param px - \e x coordinate of the symbol
 *  \param py - \e y coordinate of the symbol
 *  \param pl - features pyramid level of the symbol
 *  \param ds - start position of the grammar
 *  \param rhs - given by the model used, this vector contains indexes
 *  \param rhsind - indicates the specific element of the vector <tt>rhs</tt>
 *  that contains the symbol
 */
static void push(const node& n, int& cur, int probex, int probey, int px,
                 int py, int pl, int ds, const int *rhs, int rhsind)
{
  // Acccumulate # of 2x rescalings
  int pds = n.ds + ds;
  // symbol to push onto the stack
  int ps = (int)rhs[rhsind];

  // Locate score (or set to zero if the symbol is hallucinated beyond
  // the feature pyramid boundary)
  CvMat *mxScore = model->getSymbols()[ps].score[pl];
  double *score = new double [mxScore->rows * mxScore->cols];
  getMatData  <double> (mxScore, score);
  int sz[2];
  getDimensions (mxScore, sz);
  double pval = score[probex*sz[0] + probey];

  // Push symbol @ (px,py,pl) with score pval onto the stack
  cur++;
  Q[cur].symbol = ps;
  Q[cur].x = px;
  Q[cur].y = py;
  Q[cur].l = pl;
  Q[cur].ds = pds;
  Q[cur].val = pval;

  delete[] score;
}


/** This function traces a single detection.
 *  \param padX - is the horizontal padding used on the features pyramid
 *  \param padY - is the vertical padding used on the features pyramid
 *  \param scales - is the scales field of the <tt>FeatPyramid</tt> object
 *  \param sx - indicates the current detection \e x coordinate
 *  \param sy - indicates the current detection \e y coordinate
 *  \param sl - indicates the current detection features pyramid level
 *  \param sval - indicates the current detection score
 *  \param out - contains learning information. Given in a linear vector way
 *  \param dets - contains each detection coordinates and scores. Given in
 *  a linear vector way
 *  \param detsdim - indicates the dimension of the bidimensional matrix
 *  <tt>dets</tt>
 *  \param boxes - contains each part of the detection coordinates. Given in
 *  a linear vector way
 *  \param boxesdim - indicates the dimension of the bidimensional matrix
 *  <tt>boxes</tt>
 */
static void trace(int padX, int padY, const float *scales, int sx, int sy,
                  int sl, double sval, double *out, double *dets,
                  int *detsdim, double *boxes, int *boxesdim)
{
  // initial stack for tracing the detection
  int cur = 0;
  Q[cur].symbol = startSymbol;
  Q[cur].x = sx;
  Q[cur].y = sy;
  Q[cur].l = sl;
  Q[cur].ds = 0;
  Q[cur].val = sval;

  node n;
  double *info;
  char type;
  int fi;
  int *fsz;
  double scale;
  int x1;
  int y1;
  int x2;
  int y2;

  bool success = false;
  rules symRules;
  int rulesDim;
  int r = 0;

  int probeY;
  int probeX2;
  int probeX;
  int probeY2;
  CvMat *mxScore;
  double *score;
  int sz[2];

  int *detwin;

  int *rhs;
  int rhsDim;

  anchor anch;
  int ax;
  int ay;
  int ds;

  int px;
  int py;
  int pl;

  CvMat *mxIx;
  CvMat *mxIy;
  int *Ix;
  int *Iy;

  int isz[2];
//cout << "Init trace" << endl;
  while (cur >= 0)
  {
    // Pop a node off the stack
    n = Q[cur];
    cur--;

    // Detection information for the current symbol
    info = out + DET_SZ * n.symbol;
    info[DET_USE] = 1;
    info[DET_VAL] = n.val;

    type = model->getSymbols()[n.symbol].type;

    // Symbol is a terminal
    if (type == 'T')
    {
      // Detection info for terminal
      info[DET_IND] = -1;
      info[DET_X] = n.x + 1;
      info[DET_Y] = n.y + 1;
      info[DET_L] = n.l + 1;
      info[DET_DS] = n.ds;

      // Terminal symbol
      fi = model->getSymbols()[n.symbol].filter-1;

      // Filter size
      fsz = model->getFilters()[fi].size;

      // Detection scale
      scale = model->getSbin()/scales[n.l];

      // Compute and record image coordinates for the filter
      x1 = cvRound((n.x-padX*pow2(n.ds))*scale);
      y1 = cvRound((n.y-padY*pow2(n.ds))*scale);
      x2 = cvRound(x1 + fsz[1]*scale - 1);
      y2 = cvRound(y1 + fsz[0]*scale - 1);

      boxes[boxesdim[0]*(4*fi + 0)] = x1 + 1;
      boxes[boxesdim[0]*(4*fi + 1)] = y1 + 1;
      boxes[boxesdim[0]*(4*fi + 2)] = x2 + 1;
      boxes[boxesdim[0]*(4*fi + 3)] = y2 + 1;

      continue;
    }

    // Find the rule that produced the current node by looking at
    // which score table holds n.val at the symbol's location
    success = false;
    symRules = rul[n.symbol];
    rulesDim = symRules.n;
    

    for (r = 0; r < rulesDim; r++)
    {
      // probe location = symbol location minus virtual padding
      probeY = n.y - virtpadding(padY, n.ds);
      probeX = n.x - virtpadding(padX, n.ds);
	  cv::Mat tmpScore;
      tmpScore = symRules.structure[r].getScore()[n.l];
	  CvMat mxScore2 = tmpScore;
	  mxScore = &mxScore2;
      score = new double [mxScore->rows * mxScore->cols];
      getMatData  <double> (mxScore, score);
      getDimensions(mxScore, sz);

      // pick this rule if the score at the probe location matches n.val
      if (score[probeX*sz[0] + probeY] - 0.005 <= n.val && score[probeX*sz[0] + probeY] + 0.005 >= n.val)
      {
        success = true;
        delete [] score;
        break;
      }

      delete[] score;
    }

    // Record the rule index used (same as model "component" for mixtures of
    // star models)
    info[DET_IND] = r;

    // Record a detection window for the start symbol
    if (n.symbol == startSymbol)
    {
      // Get detection window for startSymbol and rule r
      detwin = symRules.structure[r].getDetwindow();

      // Detection scale
      scale = model->getSbin()/scales[n.l];

      // Compute and record image coordinates of the detection window
      x1 = cvRound((n.x-padX*pow2(n.ds))*scale);
      y1 = cvRound((n.y-padY*pow2(n.ds))*scale);
      x2 = cvRound(x1 + detwin[1]*scale - 1);
      y2 = cvRound(y1 + detwin[0]*scale - 1);

      dets[detsdim[0]*0] = x1 + 1;
      dets[detsdim[0]*1] = y1 + 1;
      dets[detsdim[0]*2] = x2 + 1;
      dets[detsdim[0]*3] = y2 + 1;
      dets[detsdim[0]*4] = r + 1;
      dets[detsdim[0]*5] = n.val;

      boxes[boxesdim[0]*(boxesdim[1]-2)] = r + 1;
      boxes[boxesdim[0]*(boxesdim[1]-1)] = n.val;

      info[DET_X]  = n.x + 1;
      info[DET_Y]  = n.y + 1;
      info[DET_L]  = n.l + 1;
      info[DET_DS] = n.ds;

    }

    // Push rhs symbols from the selected rule
    type = symRules.structure[r].getType();
    rhs = symRules.structure[r].getRhs();
    rhsDim = symRules.structure[r].getRhsDim();

    if (type == 'S')
    {
      // Structural rule
      for (int j = 0; j < rhsDim; j++)
      {
        anch = symRules.structure[r].getAnchor()[j];
        ax = (int) anch.array[0];
        ay = (int) anch.array[1];
        ds = (int) anch.array[2];

        // Compute location of the rhs symbol
        px = n.x * pow2(ds) + ax;
        py = n.y * pow2(ds) + ay;
        pl = n.l - (interval + 1) * ds;
        probeX = px - virtpadding(padX, n.ds+ds);

        // Remove virtual padding for to compute the probe location in the
        // score table
        probeY = py - virtpadding(padY, n.ds+ds);
        push(n, cur, probeX, probeY, px, py, pl, ds, rhs, j);
      }
    }

    else
    {
      // Deformation rule (only 1 rhs symbol)
	  cv::Mat tmpMxIx;
      tmpMxIx = symRules.structure[r].getIx()[n.l];
	  CvMat mxIx2 = tmpMxIx;
	  mxIx = &mxIx2;

	  cv::Mat tmpMxIy;
      tmpMxIy = symRules.structure[r].getIy()[n.l];
	  CvMat mxIy2 = tmpMxIy;
	  mxIy = &mxIy2;

      Ix = new int [mxIx->rows * mxIx->cols];
      getMatData <int> (mxIx, Ix);
      Iy = new int [mxIy->rows * mxIy->cols];
      getMatData <int> (mxIy, Iy);

      getDimensions (mxIx, isz);
      px = n.x;
      py = n.y;

      // Probe location for looking up displacement of rhs symbol
      probeX = n.x - virtpadding(padX, n.ds);
      probeY = n.y - virtpadding(padY, n.ds);

      // Probe location for accessing the score of the rhs symbol
      probeX2 = probeX;
      probeY2 = probeY;

      // If the probe location is in the feature pyramid retrieve the
      // deformation location from Ix and Iy
      // subtract 1 because Ix/Iy use 1-based indexing
      // *NOTE: Ix and Iy has values -1 yet
      px = Ix[probeX*isz[0] + probeY] + virtpadding(padX, n.ds);
      py = Iy[probeX*isz[0] + probeY] + virtpadding(padY, n.ds);

      // Remove virtual padding for score look up
      probeX2 = Ix[probeX*isz[0] + probeY];
      probeY2 = Iy[probeX*isz[0] + probeY];

      push(n, cur, probeX2, probeY2, px, py, n.l, 0, rhs, 0);

      // save detection information
      info[DET_X] = px + 1;     // actual location (x)
      info[DET_Y] = py + 1;     // actual location (y)
      info[DET_PX] = n.x + 1;   // Ix probe location
      info[DET_PY] = n.y + 1;   // Iy probe location

      delete[] Ix;
      delete[] Iy;
    }
  }
}



PABOD_EXPORT void getDetections (Model *m, const int padX, const int padY,
       const float *scales, const int *X, const int *Y,
       const int *L, const double *S, int dims,
       CvMat **dets, CvMat **boxes, CvMatND **info)
{
  if (dims > 0)
  {
    model = m;

    startSymbol = (int) m->getStart();
    rul = m->getRules();
    interval = (int) m->getInterval();

    const int numSymbols = (int) m->getNumSymbols();
    // Q := stack for parsing detections
    Q = new node [numSymbols];
    // dim[0] := number of detections to return

    // build output arrays

    // detections
    int detsDim[2] = {dims, 4+1+1};  // bounding box, component #, score

    createMatrix (2, detsDim, CV_64FC1, dets);
    double *detsPtr = new double [(*dets)->rows * (*dets)->cols];
    double *auxDetsPtr = detsPtr;

    // filter boxes
    int boxesDim[2] = {dims, 4 * (int)m->getNumFilters() + 2};

    createMatrix (2, boxesDim, CV_64FC1, boxes);
    double *boxesPtr = new double [(*boxes)->rows * (*boxes)->cols];
    double *auxBoxesPtr = boxesPtr;

    // detailed detection info
    int infoDim[3] = {
              DET_SZ,     // one row per output field (see enum output_fields)
              numSymbols, // one row per output field (see enum output_fields)
              dims};      // one "page" per detection

    int pagesz = infoDim[0]*infoDim[1];
    createMatrix (3, infoDim, CV_64FC1, info);
    double *infoPtr = new double [(*info)->dim[0].size *
                    (*info)->dim[1].size *
                    (*info)->dim[2].size];
    double *auxInfoPtr = infoPtr;

    // trace detections and write output into out
    int count = 0;
    for (int i = 0; i < dims; i++)
    {
      trace(padX, padY, scales, X[i]-1, Y[i]-1, L[i]-1, S[i], infoPtr,
            detsPtr, detsDim, boxesPtr, boxesDim);

      infoPtr += pagesz;
      boxesPtr++;
      detsPtr++;
      count++;
//cout << count << " " ;
    }

    setMatData ((*dets), auxDetsPtr);
    setMatData ((*boxes), auxBoxesPtr);
    setMatNData ((*info), auxInfoPtr);

    // cleanup
    delete[] Q;
    delete[] auxDetsPtr;
    delete[] auxBoxesPtr;
    delete[] auxInfoPtr;
  }

  else
  {
    *dets = NULL;
    *boxes = NULL;
    *info = NULL;
  }
}


