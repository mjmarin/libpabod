/**
   \file pabod.h
   \author Manuel J. Marin-Jimenez and D. Rodriguez-Molina
// (c) MJMJ/2012
*/
#ifndef _PABOD_H_
#define _PABOD_H_

#include <limits.h>
#include <highgui.h>
#include "model.h"

/** \class Pabod
 *  This class implements a part-based object detector.
 */

class Pabod{

 public:
	Pabod();
	Pabod(const Pabod & pabod);
	/** Creates a new Pabod detector and loads the class model from modelfile.
		\param modelfile full path to model file.
	*/
	Pabod(std::string modelfile);
        Pabod(const char * modelfile);
	~Pabod();

	/** Loads model into object.
	*/
	bool loadModel(std::string modelfile);

	/** Checks whether the detector is empty (i.e. model not loaded) or not.
		\return true if model is not already loaded, false otherwise
	*/
	bool empty(void) { return (_model == NULL); }

	/** Runs the multi-scale sliding-window detector on the target image. Model must be loaded in advance.
		\note Since new memory is allocated for detections, it is recommended that detections == NULL
		\return Used detection threshold
		*/
	float detect(IplImage * img, float thr, CvMat ** detections);
        float detect(const char * imgfilename, float thr, CvMat ** detections);

	/** Draws all detections as rectangles on image.
		\param img Target image
		\param detections As returned by method detect
		\return Number of detections drawn into image
	*/
	int drawDetections(IplImage * img, CvMat * detections);

	/** Gets the class registered in the model
	 */
    std::string getClass(){return (_model != NULL)?(_model->getClass()):std::string("empty");};

 private:
   Model * _model; //! Class model
	bool _isCopied; //! Controls is model has been either copied or loaded from file
	bool drawBB(IplImage * img, CvPoint p1, CvPoint p2, CvScalar color=cvScalar(128,255,128), float score = -(std::numeric_limits<float>::infinity()), int component = -1);

};

#endif

