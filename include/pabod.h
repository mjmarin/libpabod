/**
   \file pabod.h
   \author Manuel J. Marin-Jimenez and D. Rodriguez-Molina
// (c) MJMJ/2012
*/
#ifndef _PABOD_H_
#define _PABOD_H_

#include <limits>
#include <highgui.h>
#include "export.h"
#include "model.h"
#include "detection.h"	
	
/** \class Pabod
 *  This class implements a part-based object detector.
 */	
	
class PABOD_EXPORT Pabod{
	
 public:
	Pabod();
	Pabod(const Pabod & pabod);
	/** Creates a new Pabod detector and loads the class model from modelfile.
		\param modelfile full path to model file.
	*/
	Pabod(string modelfile);
        Pabod(const char * modelfile);
	~Pabod();
	
	/** Loads model into object.
	*/
	bool loadModel(string modelfile);
	
	/** Checks whether the detector is empty (i.e. model not loaded) or not.
		\return true if model is not already loaded, false otherwise
	*/
	bool empty(void) { return (_model == NULL); }
	
	/** Runs the multi-scale sliding-window detector on the target image. Model must be loaded in advance.
		\note Since new memory is allocated for detections, it is recommended that detections == NULL
		\return Used detection threshold
		*/
	float detect(IplImage * img, float thr, CvMat ** detections);
	float detect(IplImage * img, float thr, double iouNms, CvMat ** detections);
	float detect(cv::Mat & img, float thr, double iouNms, cv::Mat & detections); //! Most modern interface
	float detect(cv::Mat & img, float thr, double iouNms, LDetections & detections); //! Most modern interface
	float detect(const char * imgfilename, float thr, CvMat ** detections);
	
	/** Draws all detections as rectangles on image.
		\param img Target image
		\param detections As returned by method detect
		\param topN Shows only top N detections
		\return Number of detections drawn into image
	*/	
	int drawDetections(IplImage * img, CvMat * detections);
	int drawDetections(cv::Mat & img, cv::Mat & detections, int topN = -1); //! Most modern interface
	int drawDetections(cv::Mat & img, LDetections & detections, int topN = -1);

	/** Gets the class registered in the model
	 */
	string getClass(){return (_model != NULL)?(_model->getClass()):string("empty");};
	
 private:
   Model * _model; //! Class model	
	bool _isCopied; //! Controls if model has been either copied or loaded from file
	bool drawBB(IplImage * img, CvPoint p1, CvPoint p2, CvScalar color=cvScalar(128,255,128), float score = -(numeric_limits<float>::infinity()), int component = -1);
	
};

#endif

