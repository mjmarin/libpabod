#if defined _WIN32
  #include <windows.h>
  #include <ctime>
  typedef time_t TIMER;
  #define GET_TIME(var) time(var)
  #define TIME_DIFF(tend, tini) difftime(tend, tini)
  
  #define PABOD_EXPORT //__declspec(dllexport)
  
  #if !defined _CRT_SECURE_NO_DEPRECATE && _MSC_VER > 1300
    #define _CRT_SECURE_NO_DEPRECATE /* to avoid multiple Visual Studio 2005 warnings */
  #endif  
  
#else
  #include <sys/time.h>
  typedef struct timeval TIMER;
  #define GET_TIME(var) gettimeofday(var, NULL)
  #define TIME_DIFF(tend, tini) ((double)(tend.tv_sec + (double)tend.tv_usec/1000000.0) - (double)(tini.tv_sec + (double)tini.tv_usec/1000000.0))

  #define PABOD_EXPORT 
#endif

