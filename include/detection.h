/**
   \file detection.h
   \author Manuel J. Marin-Jimenez
// (c) MJMJ/2013
*/
	
#ifndef _DETECTION_H_
#define _DETECTION_H_
	
#include "export.h"
	
/** \class Detection
 *  This class represents a single detection returned by a Pabod detector.
 */		
class PABOD_EXPORT Detection{	
   public:
	   Detection();
		Detection(int x1, int y1, int x2, int y2, float score, int component)
		{
			_x1 = x1;
			_x2 = x2;
			_y1 = y1;
			_y2 = y2;
			_score = score;
			_component = component;
		};
		~Detection();
		
		int getX1(void){return _x1;};
		int getX2(void){return _x2;};
		int getY1(void){return _y1;};
		int getY2(void){return _y2;};
		int getW(void){return (_x2-_x1+1);};
		int getH(void){return (_y2-_y1+1);};
		
   private:
		int _x1;
		int _y1;
		int _x2;
		int _y2;
		float _score;   //! Detection score
		int _component; //! Component that fired
};	
	
#endif
	