#ifndef _FCONV_H_
#define _FCONV_H_

#include <custom.h>

#ifdef USE_OPENMP
   #include <omp.h>
   #undef USE_PTHREADS
#endif

#ifdef USE_PTHREADS
   #include <pthread.h>
#endif 

#include <handlerOpenCVStructs.h>
#include <iostream>

using namespace std;


/** \file
 *  This file contains functions which resolve some intermediate steps
 *  focused on the detection operation
 */


///////////////////////////////////
// THREAD_DATA STRUCT DEFINITION //
///////////////////////////////////

/** \struct thread_data
 *  The struct contains the three-dimensional matrixes given in a lineal vector
 *  way. \e A and \e B are the operands of convolution operation. C is the 
 *  resultant matrix. \e mxC is the resultant matrix given in a matrix way. 
 *  \e ADims, \e BDims and \e CDims are the dimensiones of the respective 
 *  matrix.
 */
struct thread_data 
{
  double *A;
  double *B;
  double *C;
  CvMat *mxC;
  int ADims[3];
  int BDims[3];
  int CDims[2];
};


/** The function computes a set of filters with a feature map to get the
 *  filter responses. This is the multithreaded version of the computing.
 *  \param features - operand \e A of convolution operation, containing the
 *  features map
 *  \param filters - operand \e B of convolution operation. In fact is a
 *  vector of filters, whose dimension is <tt>filtersDim</tt>
 *  \param a - indicates the range of elements (first element of vector 
 *  <tt>filters</tt>) used in this call to the function
 *  \param filtersDim - indicates the number of elements of vector 
 *  <tt>filters</tt>
 *  \return A vector of bidimensional matrixes. Each of this matrixes is the
 *  response of convolve features map with each element of filters vector
 */
  CvMat** fconv (CvMatND* features, CvMatND** filters, int a, int filtersDim);


/** Computes the convolution operation between matrixes \e A and \e B, 
 *  contained in <tt>thread_arg</tt> parameter. Is the slowest operation, so
 *  it is expanded in threads.
 *  \param thread_arg - is the void parameter allowed to expand in the
 *  thread call. In fact it is a struct <tt>thread_data</tt>
 *  \sa thread_data
 */
  void* process (void *thread_arg);


#endif

