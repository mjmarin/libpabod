#include <dt.h>


void dtHelper (double *src, double *dst, int *ptr, int step, int s1, int s2, 
              int d1, int d2, double a, double b) 
{
  if (d2 >= d1) 
  {
    int d = (d1+d2) >> 1;
    int s = s1;

    for (int p = s1+1; p <= s2; p++)
    {
      if (src[s*step] - a*squared(d-s) - b*(d-s) < 
      src[p*step] - a*squared(d-p) - b*(d-p))
      {
        s = p;
      }
    }

    dst[d*step] = src[s*step] - a*squared(d-s) - b*(d-s);
    ptr[d*step] = s;

    dtHelper (src, dst, ptr, step, s1, s, d1, d-1, a, b);
    dtHelper (src, dst, ptr, step, s, s2, d+1, d2, a, b);
  }
}


void dt1d (double *src, double *dst, int *ptr, int step, int n, double a, 
           double b) 
{
  dtHelper (src, dst, ptr, step, 0, n-1, 0, n-1, a, b);
}


void dt (const CvMat* score, double d0, double d1, double d2, double d3, 
         CvMat **scoreRet, CvMat** mIx, CvMat** mIy) 
{ 
  int dims[2];
  getDimensions (score, dims);
  double *vals = new double [score->rows * score->cols];
  getMatData <double> (score, vals);
  double ax = d0;
  double bx = d1;
  double ay = d2;
  double by = d3;

  createMatrix (2, dims, CV_64FC1, scoreRet);
  createMatrix (2, dims, CV_32SC1, mIx);
  createMatrix (2, dims, CV_32SC1, mIy);

  double *M = new double [dims[0]*dims[1]];
  int *Ix = new int [dims[0]*dims[1]];
  int *Iy = new int [dims[0]*dims[1]];

  double *tmpM = new double [dims[0]*dims[1]];
  int *tmpIx = new int [dims[0]*dims[1]];
  int *tmpIy = new int [dims[0]*dims[1]];

  for (int x = 0; x < dims[1]; x++)
    dt1d (vals+x*dims[0], tmpM+x*dims[0], tmpIy+x*dims[0], 1, dims[0], 
    ay, by);

  for (int y = 0; y < dims[0]; y++)
    dt1d (tmpM+y, M+y, tmpIx+y, dims[0], dims[1], ax, bx);

  int p;

  // Get argmins and adjust for matlab indexing from 1
  for (int x = 0; x < dims[1]; x++) 
  {
    for (int y = 0; y < dims[0]; y++) 
    {
      p = x*dims[0]+y;
      Ix[p] = tmpIx[p];
      Iy[p] = tmpIy[tmpIx[p]*dims[0]+y];
    }
  }

  delete[] tmpM;
  delete[] tmpIx;
  delete[] tmpIy;

  setMatData (*scoreRet, M);
  setMatData (*mIx, Ix);
  setMatData (*mIy, Iy);

  delete[] M;
  delete[] Ix;
  delete[] Iy;
  delete[] vals;
}

