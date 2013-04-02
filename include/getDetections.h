#ifndef _GETDETECTIONS_H_
#define _GETDETECTIONS_H_

#include <model.h>
#include <export.h>


/** \file
 *  This file contains the definition of <tt>Model</tt> class. This class 
 *  is used to load a the model structure stored on a .mat file.
 */


/** \enum output_fields
 *  This enum define different states for the matrix <tt>info</tt> that
 *  will be generated. Elements are:
 *  \li <tt>DET_USE</tt> - current symbol is used
 *  \li <tt>DET_IND</tt> - rule index
 *  \li <tt>DET_X</tt> - x coordinate (filter and deformation))
 *  \li <tt>DET_Y</tt> - y coordinate (filter and deformation)
 *  \li <tt>DET_L</tt> - level (filter)
 *  \li <tt>DET_DS</tt> - a relavite value to the start symbol location
 *  \li <tt>DET_PX</tt> - x coordinate of \e probe (deformation)
 *  \li <tt>DET_PY</tt> - y coordinate of \e probe (deformation)
 *  \li <tt>DET_VAL</tt> - score of current symbol
 *  \li <tt>DET_SZ</tt> - count number of constants above
 */
enum output_fields 
{
  DET_USE = 0,
  DET_IND,
  DET_X,
  DET_Y,
  DET_L,
  DET_DS,
  DET_PX,
  DET_PY,
  DET_VAL,
  DET_SZ
};


///////////////////////////////
//// NODE STRUCT DEFINITION ///
///////////////////////////////

/** \struct node
 *  The struct is used to make a linked list that will be used in the
 *  main function, <tt>getDetections</tt>. The fields are:
 *  \li <tt>symbol</tt> - grammar symbol
 *  \li <tt>x</tt> - x location for symbol
 *  \li <tt>y</tt> - y location for symbol
 *  \li <tt>l</tt> - scale level for symbol
 *  \li <tt>ds</tt> - a relative value to the start symbol location
 *  \li <tt>val</tt> - score for symbol
 */
struct node 
{
  int symbol;
  int x;
  int y;
  int l;
  int ds;
  double val;
};


/** The function computes amount of virtual padding needed to align 
 *  pyramid levels with 2*ds scale separation
 *  \param padding - initial padding used
 *  \param ds - value used to compute the padding value
 *  \return The virtual padding used
 */
static inline int virtpadding(int padding, int ds) 
{
  // subtract one because each level already has a one 
  // padding wide border around it
  return padding*(pow2(ds)-1);
}


/** This is the main function. Allocates memory for the three matrixes
 *  <tt>dets</tt>, <tt>boxes</tt> and <tt>info</tt> and calls several times
 *  function <tt>trace</tt>. Finally sets the correct bounding boxes and
 *  additional information on the matrixes.
 *  \param m - is the object model used in the detection operation
 *  \param padX - is the horizontal padding used on the features pyramid
 *  \param padY - is the vertical padding used on the features pyramid
 *  \param scales - is the scales field of the <tt>FeatPyramid</tt> object
 *  \param X - get the \e x coordinates of parts detected with a score
 *  higher than given threshold 
 *  \param Y - get the \e y coordinates of parts detected with a score
 *  higher than given threshold
 *  \param L - get the features pyramid level of parts detected with a 
 *  score higher than given threshold
 *  \param S - get the scores obtained for parts detected. All this scores
 *  are higher than given threshold
 *  \param dims - indicates the dimension of the previous four vectores
 *  mentioned
 *  \param dets - is a reference parameter. Is a matrix which will contain
 *  6 columns and one row per detection at the end of the function: columns 
 *  1-4 give the bounding box coordinates (x1, y1, x2, y2) of each
 *  detection. Column 5 specifies the model component used for each
 *  detection and column 6 gives the score obtained by the detection.
 *  \param boxes - is a reference parameter. Is a matrix with one row per 
 *  detection. Each sequential group of 4 columns specifies the pixel 
 *  coordinates of each model filter bounding box (where the parts are 
 *  placed). The index in the sequence is the same as the index in field 
 *  <tt>filters</tt> of Model.
 *  \param info - is a reference parameter. Is a matrix containing detailed 
 *  informatin about each detection required for extracted feature vector 
 *  during learning part.
 */
PABOD_EXPORT void getDetections (Model *m, const int padX, const int padY, 
                   const float *scales, const int *X, const int *Y, 
                   const int *L, const double *S, int dims,
                   CvMat **dets, CvMat **boxes, CvMatND **info);

#endif

