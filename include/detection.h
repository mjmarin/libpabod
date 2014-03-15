/**
   \file detection.h
   \author Manuel J. Marin-Jimenez
   \date May 2013
*/
	
#ifndef _DETECTION_H_
#define _DETECTION_H_
	
#include <vector>
#include "export.h"

#define LOWESTSCORE 	-999999
	
/** \class Detection
 *  This class represents a single detection returned by a Pabod detector.
 */		
class PABOD_EXPORT Detection{	
   public:
	   Detection()
		{
			_x1 = _x2 = _y1 = _y2 = -1;
			_component = -1;
			_score = LOWESTSCORE;
		};
				
		Detection(int x1, int y1, int x2, int y2, float score, int component)
		{
			_x1 = x1;
			_x2 = x2;
			_y1 = y1;
			_y2 = y2;
			_score = score;
			_component = component;
		};
	   /** Creates a new Detection from the given parameters.
		   \param x1, y1, x2, y2 are the coordinates of the bounding-box
			\param score the higher the better
			\param component index of the component fired by the detector
	   */		
		
		//~Detection();
		
		int getX1(void) const {return _x1;};
		int getX2(void) const {return _x2;};
		int getY1(void) const {return _y1;};
		int getY2(void) const {return _y2;};
		int getW(void) const {return (_x2-_x1+1);}; //! Width of BB
		int getH(void) const {return (_y2-_y1+1);}; //! Height of BB
		int getComponent(void) const {return _component;};
		
		float getScore(void) const {return _score;}; 
		/** Returns the detection score
		*/	
		
		bool empty(void) const {return (_component == -1) && (_score == LOWESTSCORE);}; 
		/** Checks if object has not been initialized yet
		*/
		
   private:
		int _x1;
		int _y1;
		int _x2;
		int _y2;
		float _score;   //! Detection score
		int _component; //! Component that fired
};	

typedef 	std::vector<Detection> LDetections; //! List of detections

#endif
