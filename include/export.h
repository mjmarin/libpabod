/** 
    \file export.h
    \author Manuel J. Marin-Jimenez
    \brief  Useful defines for exporting functions/classes to dll's
*/	

#if defined _WIN32  
  #ifdef _DEBUG
     #define PABOD_EXPORT //__declspec(dllexport)
  #else
     #define PABOD_EXPORT __declspec(dllexport)
  #endif
  
  #if !defined _CRT_SECURE_NO_DEPRECATE && _MSC_VER > 1300
    #define _CRT_SECURE_NO_DEPRECATE /* to avoid multiple Visual Studio 2005 warnings */
  #endif  
  
#else
  #define PABOD_EXPORT 
#endif

