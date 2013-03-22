#ifndef _DT_H_
#define _DT_H_

#include <handlerOpenCVStructs.h>


/** \file
 *  This file deal with comparing score matrixes. For that task, some
 *  normalized comparison functions are defined.
 */


/** This is a recursive function. Computes the generalized distance, either
 *  along columns or rows. Uses a divide and conquer algorithm. The code
 *  is a bit convoluted to get the function work either along a row or 
 *  column of an array.
 *  \param src - is the source matrix, given in a linear vector way
 *  \param dst - is the destiny matrix, given in a linear vector way
 *  \param ptr - is an index vector
 *  \param step - indicates how large steps are
 *  \param s1 - indicates the start index on source vector
 *  \param s2 - indicates the end index on source vector
 *  \param d1 - indicates the start index on destiny vector
 *  \param d2 - indicates the end index on destiny vector
 *  \param a - auxiliar value given by the model used
 *  \param b - auxiuliar value given by the model used
 */
void dtHelper (double *src, double *dst, int *ptr, int step, int s1, int s2, 
               int d1, int d2, double a, double b);


/** This is the function which calls the recursive function 
 *  <tt>dtHelper</tt>, which actually carry on the work. It deals with
 *  ordering the parameters.
 *  \param src - is the source matrix, given in a linear vector way 
 *  \param dst - is the destiny matrix, given in a linear vector way
 *  \param ptr - is an index vector
 *  \param step - indicates how large steps are
 *  \param n - indicates the length of any dimension of the matrix (rows 
 *  or columns), depending which one is measuring
 *  \param a - auxiliar value given by the model used
 *  \param b - auxiliar value given by the model used
 *  \sa dtHelper
 */
void dt1d (double *src, double *dst, int *ptr, int step, int n, double a, 
           double b);


/** This is the main function of normalized comparison. Allocate memory for
 *  <tt>scoreRet</tt>, <tt>mIx</tt> and <tt>mIy</tt> to place the results
 *  obtained.
 *  \param score - is the score matrix to perform the generalized distance
 *  \param d0 - this value is given by the model. It is used for columns
 *  distance computing
 *  \param d1 - this value is given by the model. It is used for columns
 *  distance computing
 *  \param d2 - this value is given by the model. It is used for rows
 *  distance computing
 *  \param d3 - this value is given by the model. It is used for rows
 *  distance computing
 *  \param scoreRet - is a reference parameter. At the end, generalized
 *  distance will be placed there
 *  \param mIx - is a reference parameter. Indicates indexes changes done in
 *  columns (horizontal axis)
 *  \param mIy - is a reference parameter. Indicates indexes changes done in
 *  rows (vertical axis)
 */
void dt (const CvMat* score, double d0, double d1, double d2, double d3, 
         CvMat **scoreRet, CvMat** mIx, CvMat** mIy);


#endif

