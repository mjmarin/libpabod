#ifndef MAKEDETECTION_H
#define MAKEDETECTION_H

#include <imgdetect.h>
#include <nms.h>
#include <export.h>
#include <detection.h>


/** \file
 *  This file contains the main function to start the detection operation.
 *  It acts like an interface function between a general code and the
 *  detection library
 */


/** The function creates a <tt>Model</tt> object where <tt>modelType</tt>
 *  model file will be loaded. Next call <tt>imgDetect</tt> function, and
 *  lastly filter the detection calling <tt>nms</tt> funcion. Results are
 *  placed in reference parameter <tt>results</tt> and threshold used is
 *  returned by the function.
 *  \param results - the file contains, ordered by rows, every found 
 *  object data. Columns 1-4 contains the two points which define the
 *  bounding box marking the object, ordered x1, y1, x2, y2. Column 5 
 *  contains the score obtained by the object. Column 6 contains the 
 *  filter component used for the detection.
 *  \param img - is the input image where the detection will be done
 *  \param modelType - is the path of the model file is going to be used
 *  in the detection operation
 *  \param thresh - is the threshold value used for the detection 
 *  operation. To be considered, a object may obtain a higher score
 *  value. If it is not given, default value is used (infinite). The
 *  function will use the threshold value define in the model file.
 */
PABOD_EXPORT float makeDetection (CvMat **results, IplImage *img, string modelType, 
                     float thresh = POSITIVE_INF);

/** To be documented
   \param iouNms - intersection-over-union threshold used during non-maxima supression. Default is 0.5
*/
PABOD_EXPORT float makeDetection (CvMat **results, IplImage *img, Model * model, float thresh = POSITIVE_INF, double iouNms = 0.5);
PABOD_EXPORT float makeDetection (cv::Mat & results, cv::Mat & img, Model * model, float thresh = POSITIVE_INF, double iouNms = 0.5);
PABOD_EXPORT float makeDetection (LDetections & D, IplImage *img, Model * model, float thresh, double iouNms);

#endif // MAKEDETECTION_H

