#ifndef _HANDLER_OPENCV_STRUCTS_H_
#define _HANDLER_OPENCV_STRUCTS_H_

#include <image.h>
#include <limits>
#include <algorithm>
#include <crossplatform.h>

/** \file
 *  The file contains a wide variety of different handling matrixes and
 *  vectors functions. These functions have been developed for two reasons:
 *  \li Creation of similar functions to \e MatLab environment to wrap
 *  easy and the most similar performance
 *  \li Developing of a useful module to use in different projects which
 *  uses library \e OpenCV, arrays and matrixes
 */

/** \typedef vectorMat 
   Vector of OpenCV Mat
  */
typedef std::vector<cv::Mat> vectorMat;


/** \def PAIR
 *  Is used in some vectors which stores a pair of values.
 */
#define PAIR 2


/** \def POSITIVE_INF
 *  Representation of positive infinite
 */
#define POSITIVE_INF LONG_MAX //(1./0.)
#define POSITIVE_INF_INT INT_MAX

/** \def NEGATIVE_INF
 *  Representation of negative infinite
 */
#define NEGATIVE_INF LONG_MIN //(-1./0.)
#define NEGATIVE_INF_INT INT_MIN


/** \def LOWER
 *  Used in some functions as a condition. It means \e x < \e y
 */
#define LOWER       0


/** \def HIGHER
 *  Used in some functions as a condition. It means \e x > \e y
 */
#define HIGHER       1


/** \def EQUAL
 *  Used in some functions as a condition. It means \e x == \e y
 */
#define EQUAL       2


/** \def NONEQUAL
 *  Used in some functions as a condition. It means \e x != \e y
 */
#define NONEQUAL     3


/** \def PRE
 *  Used in function <tt>padArray</tt>. It means padding is added before
 *  matrix elements
 */

#define PRE       0


/** \def POST
 *  Used in function <tt>padArray</tt>. It means padding is added after
 *  matrix elements
 */
#define POST       1


/** \def ASCEND
 *  Used in function <tt>shellSort</tt>. It means the order is from lowest value
 *  (first element) to highest value (last element)
 */
#define ASCEND       0


/** \def DESCEND
 *  Used in function <tt>shellSort</tt>. It means the order is from highest value
 *  (first element) to lowest value (last element)
 */
#define DESCEND     1


/** \def SEDGEWICK_DIM
 *  Indicates how many elements contains vector <tt>incrementSedgewick</tt>
 */
#define SEDGEWICK_DIM   12


/** Contains the supposely best skips used in shell sorting function
 */
const int incrementSedgewick[SEDGEWICK_DIM] = {33936, 13776, 4592, 1968, 861,
                                               336, 112, 48, 21, 7, 3, 1};

//#if !defined(_WIN32) && !defined(WIN32)

/** Returns the minimum element between <tt>double</tt> given \e x and \e y
 */
static inline double (min)(double x, double y) { return (x <= y ? x : y); };


/** Returns the maximum element between <tt>double</tt> given \e x and \e y
 */
static inline double (max)(double x, double y) { return (x <= y ? y : x); };


/** Returns the minimum element between <tt>int</tt> given \e x and \e y
 */
static inline int (min)(int x, int y) { return (x <= y ? x : y); };


/** Returns the maximum element between <tt>int</tt> given \e x and \e y
 */
static inline int (max)(int x, int y) { return (x <= y ? y : x); };

//#endif

/** Returns the square of an <tt>int</tt> element \e x
 */
static inline int squared(int x) { return x*x; }


/** Returns the result of pow \e p^2
 */
static inline int pow2(int p) { return (1<<p); }



///////////////////////////////
///// CREATE & INITIALIZE /////
///////////////////////////////


/** The function creates and initialize a matrix of type <tt>CvMat</tt>. This
 *  function is created to stardarize the creation between bidimensional and
 *  n-dimensional matrixes
 *  \param nDims - number of dimensions (must be 2)
 *  \param sizes - is a vector of <tt>nDims</tt> elements containing how
 *  many rows and cols have the matrix
 *  \param type - is the type of matrix. Is given by \e OpenCV documentation
 *  \param mat - is a reference parameter. Is the matrix created
 */
  void createMatrix (int nDims, int *sizes, int type, CvMat **mat);


/** The function creates and initialize a matrix of type <tt>CvMatND</tt>.
 *  This function is created to stardarize the creation between bidimensional
 *  and n-dimensional matrixes
 *  \param nDims - number of dimensions (must be at least 3)
 *  \param sizes - is a vector of <tt>nDims</tt> elements containing how
 *  many elements per dimension have the matrix
 *  \param type - is the type of matrix. Is given by \e OpenCV documentation
 *  \param mat - is a reference parameter. Is the matrix created
 */
  void createMatrix (int nDims, int *sizes, int type, CvMatND **mat);

/** Save CvMat to disk.
   \param mat The target matrix
   \param filename Path to file
   \return true if everthing went fine, false otherwise.
*/
   bool matToFile(const CvMat * mat, const char * filename);

/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

/** The function returns the data contained on the image <tt>im</tt>, given
 *  in a linear vector way.
 *  \param im - is the image whose data is going to be returned
 *  \return A vector filled with the data of <tt>im</tt>
 */
  template <class Type>
  Type* getImgData (const IplImage *im);


/** The function sets the data contained on vector <tt>v</tt> into the
 *  image <tt>img</tt>. Data are given in a linear vector way
 *  \param img - is the image to be filled with data of <tt>v</tt>
 *  \param v - is the vector containing the data
 */
  template <class Type>
  void setImgData (IplImage *img, const Type *v);


/** The function returns the data contained on matrix <tt>mat</tt>, and
 *  introduce them in the reference vector <tt>v</tt> in a linear vector
 *  way.
 *  \param mat - is the matrix whose data is going to be returned
 *  \param v - is a reference parameter. It will be filled with the data
 *  of <tt>mat</tt>
 *  \note Vector <tt>v</tt> is a paremeter for optimization reasons
 */
  template <class Type>
  void getMatData (const CvMat *mat, Type *v);


/** The function sets the data contained on vector <tt>v</tt> into the
 *  matrix <tt>mat</tt>. Data are given in linear vector way
 *  \param mat - is the matrix to be filled with data of <tt>v</tt>
 *  \param v - is the vector containing the data
 */
  template <class Type>
  void setMatData (CvMat *mat, const Type *v);


/** The function returns the data contained on n-dimensional matrix
 *  <tt>mat</tt>, given in a linear vector way.
 *  \param mat - is the matrix whose data is going to be returned
 *  \return A vector filled with the data of <tt>mat</tt>
 */
  template <class Type>
  Type* getMatNData (const CvMatND *mat);


/** The function sets the data contained on vector <tt>v</tt> into the
 *  matrix <tt>mat</tt>. Data are given in linear vector way
 *  \param mat - is the n-dimensional matrix to be filled with data of
 *  <tt>v</tt>
 *  \param v - is the vector containing the data
 */
  template <class Type>
  void setMatNData (CvMatND *mat, const Type *v);


/** The function returns width, height and number of channels of image
 *  <tt>im</tt> and introduce them into vector <tt>v</tt>
 *  \param im - is the image whose dimensions will be got
 *  \param v - is the vector where dimensions will be introduced
 */
  void getDimensions (const IplImage *im, int *v);


/** The function returns number of rows and columns of matrix
 *  <tt>mat</tt>, and introduce them into vector <tt>v</tt>
 *  \param mat - is the matrix whose dimensions will be got
 *  \param v - is the vector where dimensions will be introduced
 */
  void getDimensions (const CvMatND *mat, int *v);


/** The function returns number of elementes per dimension of
 *  n-dimensional matrix <tt>mat</tt>, and introduce them into vector
 *  <tt>v</tt>
 *  \param mat - is the matrix whose dimensions will be got
 *  \param v - is the vector where dimensions will be introduced
 */
  void getDimensions (const CvMat *mat, int *v);

/////////////////////////////
//////// MANIPULATION ///////
/////////////////////////////

/** Creates a new padded matrix. The new values can be added before or
 *  later old values
 *  \param mat - is the matrix to be padded
 *  \param dimPad - is a vector which contains the increment of size of
 *  the matrix <tt>mat</tt>
 *  \param val - is the value which will fill the new incremented size
 *  \param mode - can be <tt>PRE</tt> or <tt>POST</tt>. First mode adds
 *  new position before (top-left) older values. <tt>POST</tt> mode adds
 *  the new values later (bottom-right) original values.
 *  \return The new padded matrix is returned by the function
 */
  template <class Type>
  CvMat* padArray (const CvMat *mat, int dimPad[2], Type val, int mode);


/** Returns a matrix containing specific rows and columns of matrix
 *  <tt>mat</tt>.
 *  \param mat - is the matrix where submatrix comes
 *  \param iy - indicates rows of <tt>mat</tt> to be part of the submatrix
 *  \param iyDim - indicates the size of vector <tt>iy</tt> (number of
 *  rows of the submatrix)
 *  \param ix - indicates columns of <tt>mat</tt> to be part of the
 *  submatrix
 *  \param ixDim - indicates the size of vector <tt>ix</tt> (number of
 *  columns of the submatrix)
 */
  CvMat* subMat (const CvMat* mat, int* iy, int iyDim, int* ix,
                 int ixDim);


/** The function converts values of vector <tt>v</tt> to matrix indexes.
 *  For this, treat each value of the vector like a position of the matrix
 *  sorted by columns, like is shown in the figure.
 *  \image html ind2sub.png "Function \p ind2sub result"
 *  For example, if vector <tt>v</tt> contains the value 3, the result
 *  will be vector <tt>rowsIdx</tt> will contain value 1, and vector
 *  <tt>colsIdx</tt> will contain value 0.
 *  \param nRows - indicates the number of rows of the matrix
 *  \param nCols - indicates the number of cols of the matrix
 *  \param v - contains all the values to be converted into matrix indexes
 *  \param nV - number of elements of the vector <tt>v</tt>
 *  \param rowsIdx - reference parameter. It will contain the indexes
 *  of vector <tt>v</tt> after converting them into matrix indexes
 *  \param colsIdx - reference parameter. It will contain the indexes
 *  of vector <tt>v</tt> after converting them into matrix indexes
 */
  void ind2sub (const int nRows, const int nCols, const int *v, const int nV,
                int **rowsIdx, int **colsIdx);


/** Sets all the elements of matrix <tt>mat</tt> to value <tt>val</tt>
 *  \param mat - matrix whose elements will be setted
 *  \param val - value to set to elements of matrix <tt>mat</tt>
 */
  template <class Type>
  void fillMat (CvMat *mat, Type val);


/** Appends elements of vector <tt>appended</tt> at the end of vector
 *  <tt>original</tt>
 *  \param original - reference parameter. Is the main vector where
 *  other elements will be appended. Its final dimension will be
 *  <tt>dimOr</tt> + <tt>dimApp</tt>
 *  \param dimOr - indicates the dimension of vector <tt>original</tt>
 *  (before adding <tt>appended</tt> elements)
 *  \param appended - contains the elements to be appended to
 *  <tt>original</tt> vector
 *  \param dimApp - indicates the dimension of vector <tt>appended</tt>
 */
  template <class Type>
  void appendArray (Type **original, int dimOr, const Type *appended,
                    int dimApp);


/** Removes elements of vector <tt>original</tt> indicated by the vector
 *  <tt>indexes</tt>.
 *  \param original - reference parameter. Is the main vector whose
 *  elements will be removed. Its final dimension will be
 *  <tt>dimOr</tt> - <tt>dimIdx</tt>
 *  \param dimOr - indicates the dimension of vector <tt>original</tt>
 *  (before removing <tt>indexes</tt> elements)
 *  \param indexes - contains the indexes of elements of vector
 *  <tt>original</tt> to be removed
 *  \param dimIdx - indicates the dimension of vector <tt>indexes</tt>
 */
  template <class Type>
  void removeIndexes (Type **original, int dimOr, const int *indexes,
                      int dimIdx);


/** Performs sorting of vector <tt>v</tt>, ascending sorting if
 *  <tt>mode</tt> is <tt>ASCEND</tt> or descending sort if <tt>mode</tt>
 *  is <tt>DESCEND</tt>
 *  \param v - is the vector to be sorted
 *  \param n - indicates the number of elements of vector <tt>v</tt>
 *  \param mode - if it value is <tt>ASCEND</tt>, the sorting will be
 *  ascending. If it value is <tt>DESCEND</tt>, the sorting will be
 *  descending
 *  \param idx - reference parameter. If it is specified, stores the
 *  indexes of vector <tt>v</tt>, ordered like elements of it. This can
 *  be clearer by the next image:
 *
 *  \image html shellSort.png "Function \p shellSort, parameter \p idx"
 */
  template <class Type>
  void shellSort (Type *v, int n, int mode, int **idx = NULL);


/////////////////////////
///// MISCELLANEOUS /////
/////////////////////////

/** The function counts the number of elements of vector <tt>v</tt> which
 *  pass a condition, compared to element <tt>elem</tt>.
 *  \param condition - can be one of next values: <tt>LOWER</tt>,
 *  <tt>HIGHER</tt>, <tt>EQUAL</tt> or <tt>NONEQUAL</tt>.
 *  \param elem - is the value used to compare with elements of <tt>v</tt>
 *  \param v - contains a set of elements to compare with value
 *  <tt>elem</tt>
 *  \param dim - indicates the number of elements contained by vector
 *  <tt>v</tt>
 *  \param filter - if it value is <tt>true</tt>, elements of vector
 *  <tt>v</tt> will be modified, keeping only the elements whose pass the
 *  condition specified. Otherwise, vector <tt>v</tt> will keep intact
 *  \return The function returns the number of elements whose pass the
 *  condition specified
 */
  template <class Type>
  int countElementsWhich (int condition, int elem, Type **v, int dim,
                          bool filter = false);


/** The function counts the number of elements of vector <tt>mat</tt>
 *  which pass a condition, compared to element <tt>elem</tt>. Indexes of
 *  those elements whose pass the condition will be stored in param
 *  <tt>found</tt>
 *  \param condition - can be one of next values: <tt>LOWER</tt>,
 *  <tt>HIGHER</tt>, <tt>EQUAL</tt> or <tt>NONEQUAL</tt>.
 *  \param elem - is the value used to compare with elements of
 *  <tt>mat</tt>
 *  \param mat - contains a set of elements to compare with value
 *  <tt>elem</tt>
 *  \param dim - indicates the number of elements contained by vector
 *  <tt>mat</tt>
 *  \param found - will store indexes of those elements whose pass the
 *  condition set by <tt>condition</tt>
 *  \return The function returns the number of elements whose pass the
 *  condition specified
 */
  template <class Type>
  int find (int condition, const Type *mat, int dim, Type elem,
            int **found);


/** The function introduces in vector <tt>elems</tt> those elements of
 *  vector <tt>mat</tt> whose indexes are contained in vector <tt>idx</tt>.
 *  \param mat - is the vector where elements will be extracted from
 *  \param matDim - indicates the number of elements of vector <tt>mat</tt>
 *  \param idx - contains the elements of vector <tt>mat</tt> that will be
 *  introduced in vector <tt>elems</tt>
 *  \param idxDim - indicates the number of elements of vector <tt>idx</tt>
 *  and vector <tt>elems</tt>
 *  \param elems - reference parameter. It will contain all the elements of
 *  <tt>mat</tt> indicated by vector <tt>idx</tt>
 */
  template <class Type>
  void getElemOnIdx (const Type *mat, int matDim, const int *idx,
                     int idxDim, Type **elems);


/** The function allocates memory for a vector <tt>v</tt> and sets all
 *  its elements to value <tt>value</tt>
 *  \param value - is the value of each element of vector <tt>v</tt>
 *  \param dim - indicates the number of elements of vector <tt>v</tt>
 *  \param v - reference parameter. It will point to the vector allocated
 */
  template <class Type>
  void createConstVector (Type value, int dim, Type **v);


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


//////////////////////////
///// IMPLEMENTATION /////
//////////////////////////


template <class Type>
Type* getImgData (const IplImage *im)
{
  Type *v;
  int counter = 0;
  IplImage *aux = cvCloneImage (im);
  const unsigned int height = im->height;
  const unsigned int width = im->width;
  const unsigned int imSize = height * width;
  const unsigned int imSizex2 = imSize * 2;
  const unsigned int channels = im->nChannels;

  assert (height > 0);
  assert (width > 0);
  assert (channels > 0);

  v = new Type [height * width * channels];

  assert (v != NULL);

  if (im->depth == IPL_DEPTH_64F)
  {
    RgbImageDouble img(aux);

    for (unsigned int i = 0; i < width; i++ )
    {
      for (unsigned int j = 0 ; j < height; j++ )
      {
        v[counter + (0)] = img[j][i].b;
        v[counter + (imSize)] = img[j][i].g;
        v[counter + (imSizex2)] = img[j][i].r;

        counter++;
      }
    }
  }

  else if (im->depth == IPL_DEPTH_32F)
  {
    RgbImageFloat img(aux);

    for (unsigned int i = 0; i < width; i++ )
    {
      for (unsigned int j = 0 ; j < height; j++ )
      {
        v[counter + (0)] = img[j][i].b;
        v[counter + (imSize)] = img[j][i].g;
        v[counter + (imSizex2)] = img[j][i].r;

        counter++;
      }
    }
  }

  else
  {
    RgbImageUChar img(aux);

    for (unsigned int i = 0; i < width; i++ )
    {
      for (unsigned int j = 0 ; j < height; j++ )
      {
        v[counter + (0)] = img[j][i].b;
        v[counter + (imSize)] = img[j][i].g;
        v[counter + (imSizex2)] = img[j][i].r;

        counter++;
      }
    }
  }

  cvReleaseImage(&aux);

  return v;
}


template <class Type>
void setImgData (IplImage *img, const Type *v)
{
  CvScalar pixel;
  unsigned int height = img->height;
  unsigned int width = img->width;
  unsigned int imSize = height * width;
  unsigned int channels = img->nChannels;

  for (unsigned int i = 0; i < height; i++)
  {
    for (unsigned int j = 0; j < width; j++ )
    {
      for (unsigned int k = 0; k < channels; k++)
        pixel.val[channels -1 -k] = v[i + j*height + k*imSize];

      cvSet2D (img, i, j, pixel);
    }
  }
}


template <class Type>
void getMatData (const CvMat *mat, Type *v)
{
  int counter = 0;
  unsigned int cols = mat->cols;
  unsigned int rows = mat->rows;
  unsigned int offset = mat->step/sizeof(uchar);

  for (unsigned int i = 0; i < cols; i++)
  {
    for (unsigned int j = 0; j < rows; j++)
    {
      v[counter] = ((Type*) (mat->data.ptr + (offset*j)))[i];

      counter++;
    }
  }
}


template <class Type>
void setMatData (CvMat *mat, const Type *v)
{
  int counter = 0;
  unsigned int cols = mat->cols;
  unsigned int rows = mat->rows;
  unsigned int offset;

  if (sizeof(Type) == sizeof(int))
  {
    offset = mat->step/sizeof(int);

    for (unsigned int i = 0; i < cols; i++)
    {
      for (unsigned int j = 0; j < rows; j++)
      {
        (mat->data.i + (offset*j))[i] = int(v[counter]);

        counter++;
      }
    }
  }

  else
  {
    offset = mat->step/sizeof(double);

    for (unsigned int i = 0; i < cols; i++)
    {
      for (unsigned int j = 0; j < rows; j++)
      {
        (mat->data.db + (offset*j))[i] = v[counter];

        counter++;
      }
    }
  }
}


template <class Type>
Type* getMatNData (const CvMatND *mat)
{
  Type *v;
  int counter = 0;
  unsigned int dim0 = mat->dim[0].size;
  unsigned int dim1 = mat->dim[1].size;
  unsigned int dim2 = mat->dim[2].size;

  assert (dim2 > 0);
  assert (dim1 > 0);
  assert (dim0 > 0);

  v = new Type [dim2 * dim1 * dim0];

  assert (v != NULL);

  for (unsigned int i = 0; i < dim2; i++)
  {
    for (unsigned int j = 0; j < dim1; j++)
    {
      for (unsigned int k = 0; k < dim0; k++)
      {
        v[counter] = (Type) cvGetReal3D (mat, k, j, i);
        counter++;
      }

    }
  }

  return v;
}

template <class Type>
Type* getMatNData (const cv::Mat & mat_)
{
  Type *v;
  CvMatND mat__ = mat_;
  CvMatND * mat = &mat__;

  int counter = 0;
  unsigned int dim0 = mat->dim[0].size;
  unsigned int dim1 = mat->dim[1].size;
  unsigned int dim2 = mat->dim[2].size;

  assert (dim2 > 0);
  assert (dim1 > 0);
  assert (dim0 > 0);

  v = new Type [dim2 * dim1 * dim0];

  assert (v != NULL);

  for (unsigned int i = 0; i < dim2; i++)
  {
    for (unsigned int j = 0; j < dim1; j++)
    {
      for (unsigned int k = 0; k < dim0; k++)
      {
        v[counter] = (Type) cvGetReal3D (mat, k, j, i);
        counter++;
      }

    }
  }

  return v;
}


template <class Type>
void setMatNData (CvMatND *mat, const Type *v)
{
  int counter = 0;
  unsigned int dim0 = mat->dim[0].size;
  unsigned int dim1 = mat->dim[1].size;
  unsigned int dim2 = mat->dim[2].size;

  for (unsigned int i = 0; i < dim2; i++)
  {
    for (unsigned int j = 0; j < dim1; j++)
    {
      for (unsigned int k = 0; k < dim0; k++)
      {
        cvSetReal3D (mat, k, j, i, v[counter]);
        counter++;
      }

    }
  }
}


template <class Type>
void setMatNData (cv::Mat & mat_, const Type *v)
{
  CvMatND mat = mat_; // Intermediate var

  int counter = 0;
  unsigned int dim0 = mat.dim[0].size;
  unsigned int dim1 = mat.dim[1].size;
  unsigned int dim2 = mat.dim[2].size;

  for (unsigned int i = 0; i < dim2; i++)
  {
    for (unsigned int j = 0; j < dim1; j++)
    {
      for (unsigned int k = 0; k < dim0; k++)
      {
        cvSetReal3D (&mat, k, j, i, v[counter]);
        counter++;
      }

    }
  }
}


template <class Type>
CvMat* padArray (const CvMat *mat, int dimPad[2], Type val, int mode)
{
  int rows = mat->rows + dimPad[0];
  int cols = mat->cols + dimPad[1];

  // New bigger matrix is created
  CvMat *padMat = cvCreateMat (rows, cols, CV_64FC1);

  if (mode == PRE)
  {
    // Firstable, new elements are added
    for (int i = 0; i < dimPad[0]; i++)
      for (int j = 0; j < cols; j++)
        cvSetReal2D (padMat, i, j, val);

    for (int i = 0; i < dimPad[1]; i++)
      for (int j = 0; j < rows; j++)
        cvSetReal2D (padMat, j, i, val);

    // Original values are setted
    for (int i = dimPad[0]; i < rows; i++)
      for (int j = dimPad[1]; j < cols; j++)
        cvSetReal2D (padMat, i, j, cvGetReal2D (mat,
                                                i-dimPad[0],
                                                j-dimPad[1]));
  }

  else if (mode == POST)
  {
    // Firstable, new elements are added
    for (int i = mat->rows; i < rows; i++)
      for (int j = 0; j < cols; j++)
        cvSetReal2D (padMat, i, j, val);

    for (int i = mat->cols; i < cols; i++)
      for (int j = 0; j < rows; j++)
        cvSetReal2D (padMat, j, i, val);

    // Original values are setted
    for (int i = 0; i < mat->rows; i++)
      for (int j = 0; j < mat->cols; j++)
        cvSetReal2D (padMat, i, j, cvGetReal2D (mat, i, j));
  }

  return padMat;
}


template <class Type>
void fillMat (CvMat *mat, Type val)
{
  cvSetZero (mat);
  cvAddS (mat, cvScalar(val), mat);
}


template <class Type>
void appendArray (Type **original, int dimOr, const Type *appended, int dimApp)
{
  if (dimApp > 0)
  {
     Type *aux = (*original);

     (*original) = new Type [dimOr + dimApp];

     for (int i = 0; i < dimOr; i++)
       (*original)[i] = aux[i];

     for (int i = 0; i < dimApp; i++)
       (*original)[i+dimOr] = appended[i];

     delete[] aux;
  }
}


template <class Type>
void removeIndexes (Type **original, int dimOr, const int *indexes, int dimIdx)
{
  Type *aux = (*original);
  int *sortedIdx = (int*) indexes;

  if (dimOr - dimIdx > 0)
  {
    (*original) = new Type [dimOr - dimIdx];

    shellSort (sortedIdx, dimIdx, DESCEND);

    for (int i = 0; i < dimIdx; i++)
      for (int j = 0; j < dimOr - sortedIdx[i]; j++)
        aux[sortedIdx[i] + j] = aux[sortedIdx[i] + j + 1];

    for (int i = 0; i < dimOr - dimIdx; i++)
      (*original)[i] = aux[i];
  }

  else
    (*original) = NULL;
}

template <class Type>
bool greaterThan (Type i, Type j) { return (i > j); }

//! Remove from original elements at locations defined by indexes
template <class Type>
void removeIndexes (Type **original, int dimOr, std::vector<int> &indexes)
{
  Type *aux = (*original);
  //int *sortedIdx = (int*) indexes;
  std::vector<int> sortedIdx = indexes;
  int dimIdx = indexes.size();

  if (dimOr - dimIdx > 0)
  {
    (*original) = new Type [dimOr - dimIdx];

    //shellSort (sortedIdx, dimIdx, DESCEND);
	std::sort(sortedIdx.begin(), sortedIdx.end(), greaterThan<Type>);

    for (int i = 0; i < dimIdx; i++)
      for (int j = 0; j < dimOr - sortedIdx[i]; j++)
        aux[sortedIdx[i] + j] = aux[sortedIdx[i] + j + 1];

    for (int i = 0; i < dimOr - dimIdx; i++)
      (*original)[i] = aux[i];
  }

  else
    (*original) = NULL;
}


template <class Type>
void removeIndexes (std::vector<Type> & original, int dimOr, const int *indexes, int dimIdx)
{
  //Type *aux = (*original);
  std::vector<Type> aux = original;
  int *sortedIdx = (int*) indexes;
  int siz = dimOr - dimIdx;

  if (siz > 0)
  {
    //(*original) = new Type [dimOr - dimIdx];
	  original.clear();
	  original.reserve(siz);
	  for (int i = 0; i < siz; i++)
		  original.push_back(0);

    shellSort (sortedIdx, dimIdx, DESCEND);

    for (int i = 0; i < dimIdx; i++)
      for (int j = 0; j < dimOr - sortedIdx[i]; j++)
        //aux[sortedIdx[i] + j] = aux[sortedIdx[i] + j + 1];
		aux[sortedIdx[i] + j] = aux[sortedIdx[i] + j + 1];

	/*
    for (int i = 0; i < dimOr - dimIdx; i++)
      (*original)[i] = aux[i];
	  */
	original = aux;
  }

  else
    //(*original) = NULL;
	original.clear();
}


template <class Type>
void shellSort (Type *v, int n, int mode, int **idx)
{
  int curIncr = 0;
  bool flagIdx = false;
  int id;

  if (idx != NULL)
  {
    (*idx) = new int [n];
    assert ((*idx) != NULL);

    flagIdx = true;

    for (int i = 0; i < n; i++)
      (*idx)[i] = i;
  }

  // A valid increment is used (lower than vector dimension)
  while (incrementSedgewick[curIncr] > n )
    curIncr++;

  if (mode == ASCEND)
  {
    // While increment is higher than 1
    while (curIncr < SEDGEWICK_DIM)
    {
      int k = incrementSedgewick[curIncr];

      for (int i = k; i < n; i++)
      {
        Type aux = v[i];

        if (flagIdx)
          id = (*idx)[i];

        int j = i - k;

        while (j >= 0 && v[j] > aux)
        {
          v[j + k] = v[j];

          if (flagIdx)
            (*idx)[j + k] = (*idx)[j];

          j -= k;
        }

        v[j + k] = aux;

        if (flagIdx)
           (*idx)[j + k] = id;
      }

      // Next increment
      curIncr++;

    }
  }

  else
  {
    // While increment is higher than 1
    while (curIncr < SEDGEWICK_DIM)
    {
      int k = incrementSedgewick[curIncr];

      for (int i = k; i < n; i++)
      {
        Type aux = v[i];

        if (flagIdx)
          id = (*idx)[i];

        int j = i - k;

        while (j >= 0 && v[j] < aux)
        {
          v[j + k] = v[j];

          if (flagIdx)
            (*idx)[j + k] = (*idx)[j];

          j -= k;
        }

        v[j + k] = aux;

        if (flagIdx)
           (*idx)[j + k] = id;
      }

      // Next increment
      curIncr++;
    }
  }
}


template <class Type>
int countElementsWhich (int condition, int elem, Type **v, int dim,
                        bool filter)
{
  int count = 0;

  for (int i = 0; i < dim; i++)
  {
    if (condition == HIGHER)
    {
      if ((*v)[i] > elem)
        count++;
    }

    else if (condition == LOWER)
    {
      if ((*v)[i] < elem)
        count++;
    }

    else if (condition == EQUAL)
    {
      if ((*v)[i] == elem)
        count++;
    }

    else
    {
      if ((*v)[i] != elem)
        count++;
    }
  }

  if (filter)
  {
    Type *aux = new Type [dim-count];

    assert (aux != NULL);

    int idx = 0;

    for (int i = 0; i < dim; i++)
    {
      if (condition == HIGHER)
      {
        if ((*v)[i] <= elem)
        {
          aux[idx] = (Type) (*v)[i];
          (*v)[idx] = aux[idx];
          idx++;
        }
      }

      else if (condition == LOWER)
      {
        if ((*v)[i] >= elem)
        {
          aux[idx] = (Type) (*v)[i];
          (*v)[idx] = aux[idx];
          idx++;
        }
      }

      else if (condition == EQUAL)
      {
        if ((*v)[i] == elem)
        {
          aux[idx] = (Type) (*v)[i];
          (*v)[idx] = aux[idx];
          idx++;
        }
      }

      else
      {
        if ((*v)[i] != elem)
        {
          aux[idx] = (Type) (*v)[i];
          (*v)[idx] = aux[idx];
          idx++;
        }
      }
    }

    delete[] (*v);
    (*v) = new Type [dim-count];

    for (int i = 0; i < (dim-count); i++)
      (*v)[i] = aux[i];

    // mjmarin: DONE delete aux
    delete [] aux;
  }

  return count;
}


template <class Type>
int find (int condition, const Type *mat, int dim, Type elem,
          int **found)
{
  int count = 0;
  int *aux = new int [dim];

  for (int i = 0; i < dim; i++)
  {
    if (condition == HIGHER)
    {
      if (mat[i] > elem)
      {
        aux[count] = i;
        count++;
      }
    }

    else if (condition == LOWER)
    {
      if (mat[i] < elem)
      {
        aux[count] = i;
        count++;
      }
    }

    else if (condition == EQUAL)
    {
      if (mat[i] == elem)
      {
        aux[count] = i;
        count++;
      }
    }

    else
    {
      if (mat[i] != elem)
      {
        aux[count] = i;
        count++;
      }
    }
  }

  (*found) = new int [count];

  for (int i = 0; i < count; i++)
    (*found)[i] = aux[i];

  delete[] aux;

  return count;
}


template <class Type>
void getElemOnIdx (const Type *mat, int matDim, const int *idx, int idxDim,
                   Type **elems)
{
  assert (matDim >= idxDim);

  (*elems) = new Type [idxDim];

  for (int i = 0; i < idxDim; i++)
    (*elems)[i] = mat[idx[i]];
}


template <class Type>
void createConstVector (Type value, int dim, Type **v)
{
  (*v) = new Type [dim];

  for (int i = 0; i < dim; i++)
    (*v)[i] = value;
}


#endif

