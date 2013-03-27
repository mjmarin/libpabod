#if defined _WIN32
  #include <windows.h> 
  #include <ctime>
  typedef time_t TIMER;
  #define GET_TIME(var) time(var)
  #define TIME_DIFF(tend, tini) difftime(tend, tini)
#else
  #include <sys/time.h>
  typedef struct timeval TIMER;
  #define GET_TIME(var) gettimeofday(var, NULL)
  #define TIME_DIFF(tend, tini) ((double)(tend.tv_sec + (double)tend.tv_usec/1000000.0) - (double)(tini.tv_sec + (double)tini.tv_usec/1000000.0))
#endif

