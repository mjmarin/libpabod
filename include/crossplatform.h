//! \file crossplatform.h
//! \author Manuel J. Marin-Jimenez

#if defined _WIN32
  #include <windows.h> 
  #include <ctime>
  typedef time_t TIMER;
  #include <cv.h>
  #define GET_TIME(var) (*var = cv::getTickCount()) //time(var)
  #define TIME_DIFF(tend, tini) (((double)tend -tini)/cv::getTickFrequency()) //difftime(tend, tini)
  
  #if !defined _CRT_SECURE_NO_DEPRECATE && _MSC_VER > 1300
    #define _CRT_SECURE_NO_DEPRECATE /* to avoid multiple Visual Studio 2005 warnings */
  #endif  
  
#ifdef _DEBUG   
 #define _CRTDBG_MAP_ALLOC
 #include <stdlib.h>
 #include <crtdbg.h>
#ifndef DBG_NEW      
  #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
  #define new DBG_NEW   
#endif
#endif  // _DEBUG

#else
  #include <sys/time.h>
  typedef struct timeval TIMER;
  #define GET_TIME(var) gettimeofday(var, NULL)
  #define TIME_DIFF(tend, tini) ((double)(tend.tv_sec + (double)tend.tv_usec/1000000.0) - (double)(tini.tv_sec + (double)tini.tv_usec/1000000.0))
  
  
  #if (CV_MAJOR_VERSION >= 2) && (CV_MINOR_VERSION >= 3)
    #include <opencv2/opencv.hpp>
  #endif
  
#endif

