#include <fstream>
#include <handlerOpenCVStructs.h>

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

CvMat* sub_mat (const CvMat* mat, const std::vector<int>& iy, const std::vector<int>& ix)
{
  CvMat *aux;

  if ( (iy.size() >= mat->rows && ix.size() >= mat->cols) ||
        iy.size() <= 0 ||
        ix.size() <= 0)
    aux = cvCloneMat (mat);

  else
  {
    aux = cvCreateMat (iy.size(), ix.size(), mat->type);

    for (int i = 0; i < iy.size(); i++)
      for (int j = 0; j < ix.size(); j++)
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

void ind_to_sub (const int nRows, const int nCols, const size_t *v, const int nV,
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
    std::ofstream fid;
   fid.open(filename);
   if (!fid.is_open())
      return false;

   int i, j;
   for (i = 0; i < mat->rows; i++)
   {
      for (j = 0; j < mat->cols; j++)
         fid << cvGetReal2D(mat, i,j) << " ";
      fid << std::endl;
   }
   fid.close();
   return true;
}

