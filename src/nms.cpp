#include <nms.h>


void nms (int** pick, int *pickDim,  
          CvMat *dets, double overlap)
{
  if (dets == NULL)
  {
    *pick = NULL;
    *pickDim = 0;
  }

  else
  {
    *pick = NULL;
    *pickDim = 0;

    int *rows = new int [dets->rows];
    for (int i = 0; i < dets->rows; i++)
      rows[i] = i;  

    int col;
    col = 0;
    CvMat *x1 = subMat (dets, rows, dets->rows, &col, 1);
    double *x1Ptr = new double [x1->rows * x1->cols];
    getMatData <double> (x1, x1Ptr);

    col = 1;
    CvMat *y1 = subMat (dets, rows, dets->rows, &col, 1);
    double *y1Ptr = new double [y1->rows * y1->cols];
    getMatData <double> (y1, y1Ptr);

    col = 2;
    CvMat *x2 = subMat (dets, rows, dets->rows, &col, 1);
    double *x2Ptr = new double [x2->rows * x2->cols];
    getMatData <double> (x2, x2Ptr);

    col = 3;
    CvMat *y2 = subMat (dets, rows, dets->rows, &col, 1);
    double *y2Ptr = new double [y2->rows * y2->cols];
    getMatData <double> (y2, y2Ptr);

    col = 5;
    CvMat *s = subMat (dets, rows, dets->rows, &col, 1);
    double *sPtr = new double [s->rows * s->cols];
    getMatData <double> (s, sPtr);

    int areaDims[2] = {dets->rows, 1};

    CvMat *area;
    createMatrix (2, areaDims, CV_64FC1, &area);
    CvMat *p1, *p2;
    p1 = cvCloneMat (x1);
    p2 = cvCloneMat (y1);
    cvSub (x2, x1, p1);
    cvAddS (p1, cvScalar(1), p1);
    cvSub (y2, y1, p2);
    cvAddS (p2, cvScalar(1), p2);
    cvMul (p1, p2, area);

    double *areaPtr = new double [area->rows * area->cols];
    getMatData <double> (area, areaPtr);

    cvReleaseMat (&x1);
    cvReleaseMat (&y1);
    cvReleaseMat (&x2);
    cvReleaseMat (&y2);
    cvReleaseMat (&x1);
    cvReleaseMat (&s);
    cvReleaseMat (&area);
    cvReleaseMat (&p1);
    cvReleaseMat (&p2);
    delete[] rows;

    int idxDim = dets->rows;
    int *idx = new int [idxDim];

    shellSort (sPtr, dets->rows, ASCEND, &idx);

    int last;
    int i;
    int *suppress = NULL;
    int suppressDim = 0;

    int j;

    double xx1;
    double yy1;
    double xx2;
    double yy2;  
    double w;
    double h;

    double o;

    while (idxDim > 0)
    {
      last = idxDim-1;
      i = idx[last];
      appendArray (pick, (*pickDim), &i, 1);
      (*pickDim)++;

      suppress = NULL;
      suppressDim = 0;
      appendArray (&suppress, suppressDim, &last, 1);
      suppressDim++;

      for (int pos = 0; pos < last; pos++)
      {  
        j = idx[pos];

        xx1 = max (x1Ptr[i], x1Ptr[j]);
        yy1 = max (y1Ptr[i], y1Ptr[j]);
        xx2 = min (x2Ptr[i], x2Ptr[j]);
        yy2 = min (y2Ptr[i], y2Ptr[j]);  
        w = xx2 - xx1 + 1;
        h = yy2 - yy1 + 1;

        if (w > 0 && h > 0)
        {
          // Compute overlap
          o = (w * h) / areaPtr[j];

          if (o > overlap)
          {
            appendArray (&suppress, suppressDim, &pos, 1);
            suppressDim++;
          }
        }
      }

      removeIndexes (&idx, idxDim, suppress, suppressDim);
      idxDim -= suppressDim;

      delete[] suppress;
    }

    delete[] x1Ptr;
    delete[] y1Ptr;
    delete[] x2Ptr;
    delete[] y2Ptr;
    delete[] sPtr;
    delete[] areaPtr;
    delete[] idx;
  }
}

void nms2 (std::vector<int> & pick,  
          CvMat *dets, double overlap)
{
#ifdef _DEBUG
	std::cout << "debug: using nms2" << std::endl;
#endif
	//int pickDim = 0;
  if (dets == NULL)
  {
    //*pick = NULL;
	  pick.clear();
    //*pickDim = 0;
  }

  else
  {
    //*pick = NULL;
	  pick.clear();
    //*pickDim = 0;

    int *rows = new int [dets->rows];
    for (int i = 0; i < dets->rows; i++)
      rows[i] = i;  

    int col;
    col = 0;
    CvMat *x1 = subMat (dets, rows, dets->rows, &col, 1);
    double *x1Ptr = new double [x1->rows * x1->cols];
    getMatData <double> (x1, x1Ptr);

    col = 1;
    CvMat *y1 = subMat (dets, rows, dets->rows, &col, 1);
    double *y1Ptr = new double [y1->rows * y1->cols];
    getMatData <double> (y1, y1Ptr);

    col = 2;
    CvMat *x2 = subMat (dets, rows, dets->rows, &col, 1);
    double *x2Ptr = new double [x2->rows * x2->cols];
    getMatData <double> (x2, x2Ptr);

    col = 3;
    CvMat *y2 = subMat (dets, rows, dets->rows, &col, 1);
    double *y2Ptr = new double [y2->rows * y2->cols];
    getMatData <double> (y2, y2Ptr);

    col = 5;
    CvMat *s = subMat (dets, rows, dets->rows, &col, 1);
    double *sPtr = new double [s->rows * s->cols];
    getMatData <double> (s, sPtr);

    int areaDims[2] = {dets->rows, 1};

    CvMat *area;
    createMatrix (2, areaDims, CV_64FC1, &area);
    CvMat *p1, *p2;
    p1 = cvCloneMat (x1);
    p2 = cvCloneMat (y1);
    cvSub (x2, x1, p1);
    cvAddS (p1, cvScalar(1), p1);
    cvSub (y2, y1, p2);
    cvAddS (p2, cvScalar(1), p2);
    cvMul (p1, p2, area);

    double *areaPtr = new double [area->rows * area->cols];
    getMatData <double> (area, areaPtr);

    cvReleaseMat (&x1);
    cvReleaseMat (&y1);
    cvReleaseMat (&x2);
    cvReleaseMat (&y2);
    cvReleaseMat (&x1);
    cvReleaseMat (&s);
    cvReleaseMat (&area);
    cvReleaseMat (&p1);
    cvReleaseMat (&p2);
    delete[] rows;

    int idxDim = dets->rows;
    int *idx = new int [idxDim];	

    shellSort (sPtr, dets->rows, ASCEND, &idx);

    int last;
    int i;
    //int *suppress = NULL;
    //int suppressDim = 0;
	std::vector<int> suppress;

    int j;

    double xx1;
    double yy1;
    double xx2;
    double yy2;  
    double w;
    double h;

    double o;

    while (idxDim > 0)
    {
      last = idxDim-1;
      i = idx[last];
      //appendArray (pick, (*pickDim), &i, 1);
	  pick.push_back(i);
      //(*pickDim)++;

      //suppress = NULL;
      //suppressDim = 0;
      //appendArray (&suppress, suppressDim, &last, 1);
      //suppressDim++;
	  suppress.push_back(last);

      for (int pos = 0; pos < last; pos++)
      {  
        j = idx[pos];

        xx1 = max (x1Ptr[i], x1Ptr[j]);
        yy1 = max (y1Ptr[i], y1Ptr[j]);
        xx2 = min (x2Ptr[i], x2Ptr[j]);
        yy2 = min (y2Ptr[i], y2Ptr[j]);  
        w = xx2 - xx1 + 1;
        h = yy2 - yy1 + 1;

        if (w > 0 && h > 0)
        {
          // Compute overlap
          o = (w * h) / areaPtr[j];

          if (o > overlap)
          {
            //appendArray (&suppress, suppressDim, &pos, 1);
			  suppress.push_back(pos);
            //suppressDim++;
          }
        }
      }

      removeIndexes(&idx, idxDim, suppress);
      idxDim -= suppress.size();

      //delete[] suppress;
	  suppress.clear();
    }

    delete[] x1Ptr;
    delete[] y1Ptr;
    delete[] x2Ptr;
    delete[] y2Ptr;
    delete[] sPtr;
    delete[] areaPtr;
    delete[] idx;
  }
}
