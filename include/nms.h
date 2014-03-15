#ifndef _NMS_H_
#define _NMS_H_

#include <handlerOpenCVStructs.h>


/** \file
 *  This file contains the function which performs the non maximum 
 *  suppression on the detections done
 */


/** The function check the detections done, given in the parameter 
 *  <tt>dets</tt>. Select high-scoring detections and skip detections that
 *  are significantly covered by a previous selected detection. This operation
 *  is called non-maximum suppression. At the end, only maximum score and
 *  not repeated detections will be considered.
 *  \param pick - reference parameter. Is a vector which will contain the
 *  valid detections (rows of matrix <tt>dets</tt>) 
 *  \param pickDim - reference parameter. It will contain the number of 
 *  elements of vector <tt>pick</tt> (valid detections)
 *  \param dets - is the matrix containing the detections found
 *  \param overlap - indicates how much overlapping can exist between 
 *  detections. This is used to determine when a detection is repeated or not
 */
void nms (int** pick, int *pickDim,  
          CvMat *dets = NULL, double overlap = POSITIVE_INF);


//! This version uses std::vector
void nms2 (std::vector<int> & pick,  
          CvMat *dets, double overlap);

#endif
