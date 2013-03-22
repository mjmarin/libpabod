#include <fstream>
#include <handlerOpenCVStructs.h>

using namespace std;

void createMatrix (int nDims, int *sizes, int type, CvMat **mat)
{
  assert (nDims == 2);

  *mat = cvCreateMat (sizes[0], sizes[1], type);

  cvSetZero (*mat);
}

void createMatrix (int nDims, int *sizes, int type, CvMatND **mat)
{
  assert (nDims == 3);

  *mat = cvCreateMatND (nDims, sizes, type);

  cvSetZero (*mat);
}

void getDimensions (const IplImage *im, int *v)
{
  v[0] = im->height;
  v[1] = im->width;
  v[2] = im->nChannels;
}


void getDimensions (const CvMatND *mat, int *v)
{
  for (int i = 0; i < 3; i++)
    v[i] = mat->dim[i].size;
}

void getDimensions (const CvMat *mat, int *v)
{
  v[0] = mat->rows;
  v[1] = mat->cols;
}

CvMat* subMat (const CvMat* mat, int* iy, int iyDim, int* ix, int ixDim)
{
  CvMat *aux;

  if ( (iyDim >= mat->rows && ixDim >= mat->cols) || 
        iyDim <= 0 || 
        ixDim <= 0)
    aux = cvCloneMat (mat);

  else
  {
    aux = cvCreateMat (iyDim, ixDim, mat->type);

    for (int i = 0; i < iyDim; i++)
      for (int j = 0; j < ixDim; j++)
        cvSetReal2D (aux, i, j, cvGetReal2D (mat, iy[i], ix[j]));
  }

  return aux;
}

void ind2sub (const int nRows, const int nCols, const int *v, const int nV, 
              int **rowsIdx, int **colsIdx)
{
  (*rowsIdx) = new int [nV];
  (*colsIdx) = new int [nV];

  for (int i = 0; i < nV; i++)
  {
    (*rowsIdx)[i] = v[i] % nRows;

    if (v[i] != 0)
      (*colsIdx)[i] = v[i] / nRows;

    else
      (*colsIdx)[i] = 0;
  }
}

bool matToFile(const CvMat * mat, const char * filename)
{
   ofstream fid;
   fid.open(filename);
   if (!fid.is_open())
      return false;

   int i, j;
   for (i = 0; i < mat->rows; i++)
   {
      for (j = 0; j < mat->cols; j++)
         fid << cvGetReal2D(mat, i,j) << " ";
      fid << endl;
   }
   fid.close();
   return true;
}

