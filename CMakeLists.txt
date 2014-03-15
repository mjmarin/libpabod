
# ----------------------------------------------------------------------------
#   Basic Configuration
# ----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(pabod)
set(PROJECT_VERSION "0.2.10")
string(REGEX MATCHALL "[0-9]" PROJECT_VERSION_PARTS "${PROJECT_VERSION}")
list(GET PROJECT_VERSION_PARTS 0 PROJECT_VERSION_MAJOR)
list(GET PROJECT_VERSION_PARTS 1 PROJECT_VERSION_MINOR)
list(GET PROJECT_VERSION_PARTS 2 PROJECT_VERSION_PATCH)
list(GET PROJECT_VERSION_PARTS 3 PROJECT_VERSION_PATCH2)
set(PROJECT_SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")


#------------------------------------------------------
# Build type
#------------------------------------------------------

IF(NOT CMAKE_BUILD_TYPE )
   SET( CMAKE_BUILD_TYPE "Release" CACHE STRING "Release|Debug")
ENDIF()

MESSAGE(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

#------------------------------------------------------
# Lib Names and Dirs
#------------------------------------------------------

if(WIN32)
    # Postfix of DLLs:
    SET(PROJECT_DLLVERSION "${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}${PROJECT_VERSION_PATCH2}")
    SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
    SET(RUNTIME_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for dlls and binaries")
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for binaries")
    SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for dlls")
	 SET(SHARED_LIBS_DEFAULT OFF)
else()
	 SET(SHARED_LIBS_DEFAULT ON)
    # Postfix of so's:
    set(PROJECT_DLLVERSION)
    SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/lib/cmake/ /usr/lib/cmake)
endif()


OPTION(BUILD_SHARED_LIBS 	"Set to OFF to build static libraries" ${SHARED_LIBS_DEFAULT})

# ----------------------------------------------------------------------------
#   Find Dependencies
# ----------------------------------------------------------------------------

FIND_PACKAGE(OpenCV 	REQUIRED )
set (REQUIRED_LIBRARIES ${OpenCV_LIBS} )
FIND_PACKAGE(PkgConfig)	#Enable PKG-CONFIG suport
IF ( PKG_CONFIG_FOUND ) 
  PKG_CHECK_MODULES(MATIO  matio>=1.3.3 REQUIRED)
ELSE()
 	# require explicit definition of directories
 	IF ( NOT EXISTS(MATIO_INCLUDE_DIRS) )
	   SET( MATIO_INCLUDE_DIRS "" CACHE PATH "Path to Matio's include directory")
		SET( MATIO_LIBRARY_DIRS "" CACHE PATH "Path to Matio's lib directory")
		IF (WIN32)
		   SET( MATIO_LIBRARIES "libmatio.lib" CACHE STRING "Library to be linked")
		ELSE()
		   SET( MATIO_LIBRARIES "" CACHE STRING "Library to be linked")
		ENDIF()
	ENDIF ()
	
	IF (MATIO_INCLUDE_DIRS)
	 	SET ( MATIO_FOUND 1)
 		MESSAGE(STATUS "include: " ${MATIO_INCLUDE_DIRS})
 		MESSAGE(STATUS "lib dir: " ${MATIO_LIBRARY_DIRS})
 		MESSAGE(STATUS "library: " ${MATIO_LIBRARIES})
	ENDIF()
ENDIF ()	

IF (MATIO_FOUND)
  INCLUDE_DIRECTORIES(${MATIO_INCLUDE_DIRS})
  LINK_DIRECTORIES(${MATIO_LIBRARY_DIRS})
  SET(REQUIRED_LIBRARIES ${REQUIRED_LIBRARIES} ${MATIO_LIBRARIES})
  MESSAGE(STATUS "MATIO lib=${MATIO_LIBRARIES}")
ELSE()
  MESSAGE(FATAL_ERROR "Could not find matio library")
ENDIF()


# ----------------------------------------------------------------------------
#   PROJECT CONFIGURATION
#   force some variables that could be defined in the command line to be written to cache
# ----------------------------------------------------------------------------
OPTION(INSTALL_DOC 	"Set to ON to build/install Documentation" OFF)
IF (INSTALL_DOC)
    FIND_PACKAGE(Doxygen REQUIRED)
    MESSAGE( STATUS "INSTALL_DOC:         ${INSTALL_DOC} ")
    INCLUDE("${PROJECT_SOURCE_DIR}/generateDoc.cmake")
    GENERATE_DOCUMENTATION(${PROJECT_SOURCE_DIR}/dox.in)
ENDIF()

option(USE_OPENMP "Use OpenMP library? If ON, pthread is Disabled" ON)
if(WIN32)
   set(USE_OPENMP OFF)
endif()

option(USE_PTHREADS "Use pthread library?" OFF)
if(WIN32)
   set(USE_PTHREADS OFF)
endif()

# GENERATING CUSTOM_H
# --------------------------------------------------------------------
SET(CUSTOM_H "${PROJECT_SOURCE_DIR}/include/custom.h")
file(WRITE ${CUSTOM_H} "#ifndef _CUSTOM_H_ \n #define _CUSTOM_H_ 1\n\n")

if(USE_OPENMP)
 file(APPEND ${CUSTOM_H} "#define USE_OPENMP 1 \n#undef USE_PTHREADS\n")
else()
 file(APPEND ${CUSTOM_H} "#undef USE_OPENMP\n")
 if(USE_PTHREADS)
  file(APPEND ${CUSTOM_H} "\n#define USE_PTHREADS 1\n")
 else()
   file(APPEND ${CUSTOM_H} "\n#undef USE_PTHREADS\n")
 endif()
endif()

# ADDING PROJECT VERSION
file(APPEND ${CUSTOM_H} "\n#define PABOD_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}\n")
file(APPEND ${CUSTOM_H} "\n#define PABOD_MINOR_VERSION ${PROJECT_VERSION_MINOR}\n")
file(APPEND ${CUSTOM_H} "\n#define PABOD_PATCH_VERSION ${PROJECT_VERSION_PATCH}${PROJECT_VERSION_PATCH2}\n")

# DEALING WITH MATIO VERSIONS
IF(WIN32)
   IF(MATIO_MINOR_VERSION)
	   file(APPEND ${CUSTOM_H} "\n#ifndef MATIO_MINOR_VERSION\n")	
      file(APPEND ${CUSTOM_H} "\n#define MATIO_MINOR_VERSION ${MATIO_MINOR_VERSION}\n")	
		file(APPEND ${CUSTOM_H} "\n#endif\n")	
   ENDIF()

   IF(MATIO_MAJOR_VERSION)
	   file(APPEND ${CUSTOM_H} "\n#ifndef MATIO_MAJOR_VERSION\n")	
      file(APPEND ${CUSTOM_H} "\n#define MATIO_MAJOR_VERSION ${MATIO_MAJOR_VERSION}\n")	
		file(APPEND ${CUSTOM_H} "\n#endif\n")	
   ENDIF()
ENDIF()

file(APPEND ${CUSTOM_H} "\n#endif\n")

# ----------------------------------------------------------------------------
#   Uninstall target, for "make uninstall"
# ----------------------------------------------------------------------------
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# ----------------------------------------------------------------------------
# create configuration file from .in file (If you use windows take care with paths)
# ----------------------------------------------------------------------------
set(CMAKE_INCLUDE_DIRS_CONFIGCMAKE ${CMAKE_CURRENT_SOURCE_DIR}/include)
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/config.cmake.in" "${PROJECT_BINARY_DIR}/Find${PROJECT_NAME}.cmake")
INSTALL(FILES "${PROJECT_BINARY_DIR}/Find${PROJECT_NAME}.cmake" DESTINATION lib/cmake/ )

# ----------------------------------------------------------------------------
#   Program Optimization and debug (Extracted from OpenCV)
# ----------------------------------------------------------------------------
set(WARNINGS_ARE_ERRORS 		OFF CACHE BOOL "Treat warnings as errors")
#set(WHOLE_PROGRAM_OPTIMIZATION 	OFF CACHE BOOL "Flags for whole program optimization.")

set(EXTRA_C_FLAGS "")
set(EXTRA_C_FLAGS_RELEASE "")
set(EXTRA_C_FLAGS_DEBUG "")
set(EXTRA_EXE_LINKER_FLAGS "")
set(EXTRA_EXE_LINKER_FLAGS_RELEASE "")
set(EXTRA_EXE_LINKER_FLAGS_DEBUG "")

IF(CMAKE_COMPILER_IS_GNUCXX OR MINGW)
    set(ENABLE_PROFILING 		OFF CACHE BOOL "Enable profiling in the GCC compiler (Add flags: -g -pg)")
    set(USE_OMIT_FRAME_POINTER 	ON CACHE BOOL "Enable -fomit-frame-pointer for GCC")
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES arm*) # We can use only -O2 because the -O3 causes gcc crash
        set(USE_O2 ON CACHE BOOL "Enable -O2 for GCC")
        set(USE_FAST_MATH ON CACHE BOOL "Enable -ffast-math for GCC")
    endif()
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES powerpc*)
        set(USE_O3 ON CACHE BOOL "Enable -O3 for GCC")
        set(USE_POWERPC ON CACHE BOOL "Enable PowerPC for GCC")
    endif ()
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES amd64* OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64*)
        set(USE_O3 ON CACHE BOOL "Enable -O3 for GCC")
        set(USE_FAST_MATH ON CACHE BOOL "Enable -ffast-math for GCC")
        set(USE_MMX ON CACHE BOOL "Enable MMX for GCC")
        set(USE_SSE ON CACHE BOOL "Enable SSE for GCC")
        set(USE_SSE2 ON CACHE BOOL "Enable SSE2 for GCC")
        set(USE_SSE3 ON CACHE BOOL "Enable SSE3 for GCC")
    endif()
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES i686* OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES x86)
        set(USE_O3 ON CACHE BOOL "Enable -O3 for GCC")
        set(USE_FAST_MATH ON CACHE BOOL "Enable -ffast-math for GCC")
        set(USE_MMX ON CACHE BOOL "Enable MMX for GCC")
        set(USE_SSE OFF CACHE BOOL "Enable SSE for GCC")
        set(USE_SSE2 OFF CACHE BOOL "Enable SSE2 for GCC")
        set(USE_SSE3 OFF CACHE BOOL "Enable SSE3 for GCC")
    endif ()

    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wall")

    if(WARNINGS_ARE_ERRORS)
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Werror")
    endif()

    # The -Wno-long-long is required in 64bit systems when including sytem headers.
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64* OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES amd64*)
		set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-long-long")
    endif()

    # Whole program optimization
    #if(WHOLE_PROGRAM_OPTIMIZATION)
    #    set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -fwhole-program --combine")
    #endif()

    # Other optimizations
    if(USE_OMIT_FRAME_POINTER)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -fomit-frame-pointer")
    endif()
    if(USE_O2)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -O2")
    endif()
    if(USE_O3)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -O3")
    endif()
    if(USE_FAST_MATH)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -ffast-math")
    endif()
    if(USE_POWERPC)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -mcpu=G3 -mtune=G5")
    endif()
    if(USE_MMX)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -mmmx")
    endif()
    if(USE_SSE)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -msse")
    endif()
    if(USE_SSE2)
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -msse2")
    endif()
    if(USE_SSE3 AND NOT MINGW) # SSE3 should be disabled under MingW because it generates compiler errors
       set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -msse3")
    endif()

    if(ENABLE_PROFILING)
        set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -pg -g")
    else()
        if(NOT APPLE)
            set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -ffunction-sections")
        endif()

    endif()

    # Parallel mode
    if(USE_OPENMP)
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS}  -fopenmp")
        set(LINKER_LIBS ${LINKER_LIBS} gomp)
    endif()

    set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -DNDEBUG")
    set(EXTRA_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG -D_DEBUG -W -Wextra -Wno-return-type")

    MESSAGE( STATUS "-------------------------------------------------------------------------------" )
    message( STATUS "GNU COMPILER")
    MESSAGE( STATUS "-------------------------------------------------------------------------------" )

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE}")
    SET(CMAKE_CXX_FLAGS_DEBUG "${EXTRA_C_FLAGS_DEBUG}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXTRA_EXE_LINKER_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${EXTRA_EXE_LINKER_FLAGS_RELEASE}")
    SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${EXTRA_EXE_LINKER_FLAGS_DEBUG}")
ENDIF	()

# ----------------------------------------------------------------------------
#   PACKAGE GENERATOR
# ----------------------------------------------------------------------------

IF (UNIX)
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_CONTACT "mjmarin@uco.es")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "mjmarin@uco.es")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LibPaBOD")
SET(CPACK_PACKAGE_VENDOR "Daniel Rodriguez and Manuel J Marin")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
SET(CPACK_STRIP_FILES "bin/lib${PROJECT_NAME}")
SET(CPACK_SOURCE_STRIP_FILES "")
IF (CPACK_GENERATOR STREQUAL "DEB")
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "opencv (>= 2.1.0)")
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "matio (>= 1.3.4)")
ENDIF()
INCLUDE(CPack)
ENDIF()

#------------------------------------------------
# DIRS
#------------------------------------------------
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)



# ----------------------------------------------------------------------------
# display status message for important variables
# ----------------------------------------------------------------------------
message( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "General configuration for ${PROJECT_NAME} ${PROJECT_VERSION}")
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
message( STATUS )
message("    Built as dynamic libs?:" ${BUILD_SHARED_LIBS})
message("    Compiler:"                   "${CMAKE_COMPILER}"   "${CMAKE_CXX_COMPILER}")
message( STATUS "C++ flags (Release):       ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message( STATUS "C++ flags (Debug):         ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message( STATUS "CMAKE_CXX_FLAGS:         ${CMAKE_CXX_FLAGS}")
message( STATUS "CMAKE_BINARY_DIR:         ${CMAKE_BINARY_DIR}")

MESSAGE( STATUS )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )

MESSAGE( STATUS )
MESSAGE( STATUS "---------------------------     Documentation     -----------------------------" )
MESSAGE( STATUS )
MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "USE_LATEX = ${USE_LATEX}" )
MESSAGE( STATUS "USE_DOT = ${USE_DOT}" )
MESSAGE( STATUS "USE_CHM = ${USE_CHM}" )
MESSAGE( STATUS )
#MESSAGE( STATUS "FOUND OPENGL=${GL_FOUND}    LIBS=${OPENGL_LIBS}")
MESSAGE( STATUS "OpenCV_LIB_DIR=${OpenCV_LIB_DIR}")
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}")
MESSAGE( STATUS )
MESSAGE( STATUS )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS )
