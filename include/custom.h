#ifndef _CUSTOM_H_ 
 #define _CUSTOM_H_ 1

#undef USE_OPENMP

#if _DEBUG
#undef USE_PTHREADS
#else
#define USE_PTHREADS 1
#endif

#define PABOD_MAJOR_VERSION 0

#define PABOD_MINOR_VERSION 2

#define PABOD_PATCH_VERSION 8

#endif
