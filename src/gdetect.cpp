#include <gdetect.h>


void gdetect (CvMat **dets, CvMat **boxes, CvMatND **info,
              const FeatPyramid &pyra, Model *model, double thresh,
              double *bbox, double overlap)
{
  bool latent;

  if ( bbox != NULL && overlap != NEGATIVE_INF)
  {
    latent = true;
    thresh = -1000;
  }

  else
    latent = false;

  // Set defaults for optional arguments
  if ( bbox == NULL )
    bbox = NULL;

  if ( overlap == NEGATIVE_INF )
    overlap = 0.7;

  // Cache filter response
  filterResponses (model, pyra, latent, bbox, overlap);

  // Compute parse scores
  LStructure* L = new LStructure;
  assert (L != NULL);
  L = modelSort (model, L); // mjmarin: uses def params --> i=-1, V=NULL

  for (int s = 0; s < L->LDim; s++)
  {
    for (int r = 0; r < model->getRules()[L->L[s]].n; r++)
      applyRule (model, model->getRules()[L->L[s]].structure[r],
                 pyra.getPadY(), pyra.getPadX());

    symbolScore (model, L->L[s], latent, pyra, bbox, overlap);
  }

  delete[] L;

  // Find scores above threshold
  std::vector<int> X;
  std::vector<int> Y;
  std::vector<int> I;
  std::vector<int> Lvl;
  double *S = NULL;
  int SDim = 0;
  int *idx = NULL;
  int idxDim = 0;

  int tmpsDim;
  int *tmpI;

  double* ptrScore;

  int *tmpX;
  int *tmpY;
  int *tmpL;
  double *tmpS;

  for (int level = model->getInterval() + 1; level < pyra.getDim(); level++)
  {
    CvMat* score = model->getSymbols()[(int) model->getStart()].score[level];

    ptrScore = new double [score->rows * score->cols];
    getMatData <double> (score, ptrScore);

    // Returns all values of score which are greater or equal to thresh
    tmpsDim = find (HIGHER, ptrScore, score->rows * score->cols, thresh, &tmpI);

    ind2sub (score->rows, score->cols, tmpI, tmpsDim, &tmpY, &tmpX);

    X.insert(X.end(), tmpX, tmpX + tmpsDim);
    Y.insert(Y.end(), tmpY, tmpY + tmpsDim);
    I.insert(I.end(), tmpI, tmpI + tmpsDim);

    createConstVector (level, tmpsDim, &tmpL);

    Lvl.insert(Lvl.end(), tmpL, tmpL + tmpsDim);

    getMatData <double> (score, ptrScore);

    getElemOnIdx (ptrScore, score->rows * score->cols, tmpI, tmpsDim, &tmpS);

    appendArray (&S, SDim, tmpS, tmpsDim);

    SDim += tmpsDim;

    delete[] ptrScore;
    delete[] tmpX;
    delete[] tmpY;
    delete[] tmpI;
    delete[] tmpL;
    delete[] tmpS;
  }

  idxDim = SDim;

  shellSort (S, SDim, DESCEND, (&idx));

  X = get_elem_on_idx(X, idx, idx + idxDim);
  Y = get_elem_on_idx(Y, idx, idx + idxDim);
  I = get_elem_on_idx(I, idx, idx + idxDim);
  Lvl = get_elem_on_idx(Lvl, idx, idx + idxDim);

  for (int m = 0; m < X.size(); m++)
    X[m]++;

  for (int m = 0; m < Y.size(); m++)
    Y[m]++;

  for (int m = 0; m < I.size(); m++)
    I[m]++;

  for (int m = 0; m < Lvl.size(); m++)
    Lvl[m]++;

  // Compute detection bounding boxes and parse information
  getDetections (model, pyra.getPadX(), pyra.getPadY(), pyra.getScales(),
                 X.data(), Y.data(), Lvl.data(), S, X.size(), dets, boxes, info);

  delete[] idx;
  delete[] S;
}



void symbolScore (Model *model, int s, bool latent, const FeatPyramid &pyra,
                  double *bbox, double overlap)
{
  // Take pointwise max over scores for each rule with s as the lhs
  rules r = model->getRules()[s];
  CvMat **score = r.structure[0].getScore();

  CvMat **sc = new CvMat* [r.structure[0].getScoreDim()];

  assert (sc != NULL);

  for (int i = 0; i < r.structure[0].getScoreDim(); i++)

    sc[i] = cvCloneMat (score[i]);

  for (int j = 1; j < r.n; j++)
    for (int i = 0; i < r.structure[j].getScoreDim(); i++)
      cvMax (sc[i], r.structure[j].getScore()[i], sc[i]);

  model->getSymbols()[s].dimScore = r.structure[0].getScoreDim();
  model->getSymbols()[s].score = sc;
}



void applyRule (Model *model, const Cell &r, int padY, int padX)
{
  if (r.getType() == 'S')
    applyStructuralRule (model, r, padY, padX);

  else
    applyDeformationRule (model, r);
}



void applyStructuralRule (Model *model, const Cell &r, int padY, int padX)
{
  // Structural rule -> shift and sum scores from rhs symbols
  // Prepare score for this rule
  CvMat** score = new CvMat* [model->getScoretptDim()];
  assert (score != NULL);

  int scoreDims[2];

  for (int i = 0; i < model->getScoretptDim(); i++)
  {
    scoreDims[0] = model->getScoretpt()[i]->rows;
    scoreDims[1] = model->getScoretpt()[i]->cols;

    createMatrix (2, scoreDims, model->getScoretpt()[i]->type, &(score[i]));

    assert (score[i] != NULL);

    fillMat (score[i], r.getOffset().w);
  }


  int *iy;
  int *ix;
  CvMat *sp;
  CvMat *sTmp;
  int sz[2];

  double ax;
  double ay;
  double ds;

  int step;

  int virtPadY;
  int virtPadX;

  int startY;
  int startX;

  int startLevel;

  int level;
  int iter = 0;

  int endY;
  int endX;

  int iyDim;
  int oy;

  int ixDim;
  int ox;

  int sTmpDims[2];

  // Sum scores from rhs (with appropriate shift and down sample)
  for (int j = 0; j < r.getRhsDim(); j++)
  {
    ax = r.getAnchor()[j].array[0];
    ay = r.getAnchor()[j].array[1];
      ds = r.getAnchor()[j].array[2];

    // Step size for down sampling
    step = int(pow (2, ds));

    // Amount of (virtual) padding to halucinate
    virtPadY = (step-1) * padY;
    virtPadX = (step-1) * padX;

    // Starting points (simulates additional padding at finer scales)
    startY = int(ay - virtPadY);
    startX = int(ax - virtPadX);

    // Starting level
    startLevel = int( (model->getInterval() + 1) * ds);

    // Score table to shift and down sample
    CvMat** s = model->getSymbols()[(int) r.getRhs()[j]].score;
    assert (s != NULL);

    for (int i = startLevel;
         i < (int) (model->getSymbols()[(int)r.getRhs()[j]].dimScore);
         i++)
    {
      level = i - int( (model->getInterval() + 1) * ds);

      // Ending points
      endY = min ((int) s[level]->rows, (int) (startY +
                                              (step * (score[i]->rows))) );
      endX = min ((int) s[level]->cols, (int) (startX +
                                              (step * (score[i]->cols))) );

      // Y sample points
      assert ( (endY-startY) > 0);
      assert ( step > 0);

      iyDim = round((float)(endY-startY)/step);
      iy = new int [iyDim];
      assert (iy != NULL);

      iter = 0;

      for (int k = startY; k < endY; k += step)
      {
        iy[iter] = k;
        iter++;
      }

      oy = countElementsWhich (LOWER, 0, &iy, iyDim, true);

      iyDim = iyDim - oy;
      assert (iyDim > 0);

      // X sample points
      assert ( (endX-startX) > 0);
      assert (step > 0);

      ixDim = round((float)(endX-startX)/step);
      ix = new int [ixDim];
      assert (ix != NULL);

      iter = 0;

      for (int k = startX; k < endX; k += step)
      {
        ix[iter] = k;
        iter++;
      }

      ox = countElementsWhich (LOWER, 0, &ix, ixDim, true);

      ixDim = ixDim - ox;
      assert (ixDim > 0);

      // Sample scores
      sp = subMat (s[level], iy, iyDim, ix, ixDim);
      assert (sp != NULL);
      getDimensions (sp, sz);

      // Sum with correct offset
      assert (score[i]->rows > 0);
      assert (score[i]->cols > 0);

      sTmpDims[0] = score[i]->rows;
      sTmpDims[1] = score[i]->cols;

      createMatrix (2, sTmpDims, CV_64FC1, &sTmp);

      assert (sTmp != NULL);

      fillMat (sTmp, NEGATIVE_INF);

      for (int j = oy; j < oy+sz[0]; j++)
        for (int k = ox; k < ox+sz[1]; k++)
          cvSetReal2D (sTmp, j, k, cvGetReal2D (sp, j-oy, k-ox));

      cvAdd (score[i], sTmp, score[i]);

      delete[] iy;
      delete[] ix;
      cvReleaseMat (&sp);
      cvReleaseMat (&sTmp);
    }
  }

  model->getRules()[(int) r.getLhs()].structure
                   [(int) r.getI()].setScoreDim(model->getScoretptDim());
  model->getRules()[(int) r.getLhs()].structure
                   [(int) r.getI()].setScore(score);
}



void applyDeformationRule (Model *model, const Cell &r)
{
  // Deformation rule -> apply distance transform
  double *d = r.getDef().w;
  int dim = model->getSymbols()[(int) r.getRhs()[0]].dimScore;
  CvMat** score = model->getSymbols()[(int) r.getRhs()[0]].score;

  CvMat **Ix = new CvMat* [dim];
  assert (Ix != NULL);

  CvMat **Iy = new CvMat* [dim];
  assert (Iy != NULL);

  for (int i = 0; i < dim; i++)
  {
    // Note: dt has been changed so it is not necesarry to pass in -score[i]
    dt (score[i], d[0], d[1], d[2], d[3], &score[i], &Ix[i], &Iy[i]);
    cvAddS (score[i], cvRealScalar(r.getOffset().w), score[i]);
  }

  model->getRules()[(int) r.getLhs()].structure[(int) r.getI()]
                                     .setScoreDim(dim);
  model->getRules()[(int) r.getLhs()].structure[(int) r.getI()]
                                     .setScore(score);

  model->getRules()[(int) r.getLhs()].structure[(int) r.getI()]
                                     .setIxDim(dim);
  model->getRules()[(int) r.getLhs()].structure[(int) r.getI()]
                                     .setIx(Ix);

  model->getRules()[(int) r.getLhs()].structure[(int) r.getI()]
                                     .setIyDim(dim);
  model->getRules()[(int) r.getLhs()].structure[(int) r.getI()]
                                     .setIy(Iy);
}



void filterResponses (Model *model, const FeatPyramid  &pyra, bool latent,
                      double *bbox, double overlap)
{
  int i = 0;
  int dim = 0;
  int spadX, spadY;

  // Firstable the function has to count how many elements there will be
  for (int s = 0; s < model->getSymbolsDim(); s++)
  {
    if (model->getSymbols()[s].type == 'T')
      i++;
  }

  // Gather filters for computing match quality responses
  assert (i > 0);

  CvMatND **filters = new CvMatND* [i];
  assert (filters != NULL);

  int filtersDim = i;

  int *filtersDimF = new int [i];
  assert (filtersDimF != NULL);

  int *filtersDimC = new int [i];
  assert (filtersDimC != NULL);

  int *filtersDimA = new int [i];
  assert (filtersDimA != NULL);

  int *filter_to_symbol = new int [i];
  assert (filter_to_symbol != NULL);

  i = 0;

  for (int s = 0; s < model->getSymbolsDim(); s++)
  {
    if (model->getSymbols()[s].type == 'T')
    {
      filters[i] = model->getFilters()[(int) model->getSymbols()[s]
                                             .filter - 1].w;
      filtersDimF[i] = model->getFilters()[(int) model->getSymbols()[s]
                                          .filter - 1].w->dim[0].size;
      filtersDimC[i] = model->getFilters()[(int) model->getSymbols()[s]
                                          .filter - 1].w->dim[1].size;
      filtersDimA[i] = model->getFilters()[(int) model->getSymbols()[s]
                                          .filter - 1].w->dim[2].size;
      filter_to_symbol[i] = model->getSymbols()[s].i;

      i++;
    }
  }

  // Determine which levels to compute responses for
  // (optimization for the latent = true case)
  int *levels = validateLevels (model, pyra, latent, bbox, overlap, &dim);
  assert (levels != NULL);

  CvMat **r;
  int rDim = filtersDim;

  model->setScoretptDim (dim);
  model->setScoretpt(NULL);

  for (int i = 0; i < rDim; i++)
  {
    model->getSymbols()[filter_to_symbol[i]].dimScore = dim;
    model->getSymbols()[filter_to_symbol[i]].score = new CvMat* [dim];
  }

  int s[2];
  int dimPad[2];

  for (int j = 0; j < dim; j++)
  {
    // Compute filter response for all filters at this level
    r = fconv (pyra.getFeat()[levels[j]], filters, 1, filtersDim);

    // Find max response array size for this level
    s[0] = NEGATIVE_INF_INT; // mjmarin fix
    s[1] = NEGATIVE_INF_INT; // mjmarin fix

    for (int i = 0; i < rDim; i++)
    {
      if (r[i]->rows > s[0])
        s[0] = r[i]->rows;

      if (r[i]->cols > s[1])
        s[1] = r[i]->cols;
    }

    // Set filter response as the score for each filter terminal
    for (int i = 0; i < rDim; i++)
    {
      // Normalize response array size so all responses at this level
      // have the same dimension
      spadY = s[0] - r[i]->rows;
      spadX = s[1] - r[i]->cols;

      dimPad[0] = spadY;
      dimPad[1] = spadX;

      // mjmarin : r[i] es sobreescrito!!!
      //r[i] = padArray (r[i], dimPad, NEGATIVE_INF, POST);
      CvMat * newArray = NULL; // Temporal var
      newArray = padArray (r[i], dimPad, NEGATIVE_INF, POST);
      cvReleaseMat(&r[i]); // Release old memory
      r[i] = newArray;

      model->getSymbols()[filter_to_symbol[i]].score[levels[j]] = r[i];
    }

    assert (s[0] > 0);
    assert (s[1] > 0);

    CvMat *scoretpt;
    createMatrix (2, s, CV_64FC1, &scoretpt);

    assert (scoretpt != NULL);

    model->getScoretpt()[j] = scoretpt;
  }

  delete[] filters;
  delete[] filtersDimF;
  delete[] filtersDimC;
  delete[] filtersDimA;
  delete[] filter_to_symbol;
  delete[] levels;
}



int* validateLevels (const Model *model, const FeatPyramid &pyra,
                     bool latent, double *bbox, double overlap,
                     int *dim)
{
  int *levels = NULL;

  if (!latent)
  {
    assert (pyra.getDim() > 0);

    levels = new int [pyra.getDim()];

    assert (levels != NULL);

    *dim = pyra.getDim();

    for (int i = 0; i < pyra.getDim(); i++)
      levels[i] = i;
  }

  return levels;
}



