#ifndef _IMGDETECT_H_
#define _IMGDETECT_H_

#include <gdetect.h>


/** \file
 *  This file contains the function which starts the pure detection
 *  process.
 */


/** The function starts creating the pyramid features of the image,
 *  using the class <tt>FeatPyramid</tt>. Then, calls the function
 *  gdetect using the parameters given before and the pyramid.
 *  \param input - is the input image where the detection will be done
 *  \param model - is the object model, which contains every field and
 *  value loaded from the file model.
 *  \param thresh - is the minimum score may obtain an object to be
 *  considered as a valid object.
 *  \param bbox - this parameter is not used (useful in the original
 *  version)
 *  \param overlap - this parameter is not used (useful in the original
 *  version)
 *  \param dets - this matrix will contain information about the objects
 *  found on the detection operation.
 *  \param boxes - this matrix will contain information about the parts
 *  of the objects found on the detection operation
 *  \param info - this matrix will contain detailed information about the
 *  objects found in the operation
 *  \return Returns true if any object has been found, or false otherwise.
 */
bool imgDetect (IplImage *input, Model *model, double thresh, double *bbox, 
                double overlap, CvMat **dets, CvMat **boxes, CvMatND **info);

#endif
