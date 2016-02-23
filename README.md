==============================================================================
libpabod: Library for PArt-Based Object Detection in C++
==============================================================================
Daniel Rodriguez Molina and Manuel J. Marin-Jimenez

This software implements the object detection system described in Felzenszwalb et al. [1].
--------------------------------------------------------------------------------

   Source-Code:   https://github.com/mjmarin/libpabod

--------------------------------------------------------------------------------
Contents of the package:
--------------------------------------------------------------------------------
- bin - the executable file will be created here
- data:
  - models - contains all the object model files 
  - testimages - a set of images to test the software
- doc - folder where the documentation will be generated
- include - contains all the software header files
- libs - folder where the library will be generated
- obj - folder where the object files will be generated during the compilation
- src - contains all the software source files
- tests - contains some programs to test the library
- makefile - used to compile the library, the documentation and the test program

--------------------------------------------------------------------------------
Requirements:
--------------------------------------------------------------------------------
This software has been tested on Ubuntu 10.10 (Maverick Meerkat) with the following libraries:
 - libmatio-dev - v1.3.4-2 (required)
 - libcv-dev - v2.1.0-2 (required)
 - libhighgui-dev - v2.1.0-2 (required)
 - doxygen - v1.7.1-1   (optional; used to generate the documentation)
 - graphviz - v2.26.3-4 (optional; used to generate figures for the documentation)

--------------------------------------------------------------------------------
Quick start:
--------------------------------------------------------------------------------

A) Installing with CMake:
   Please, follow the steps described in the file named INSTALL.

B) Testing the library:
  (assuming test program has been already generated)
  1. cd <libpabod_directory> 
  2.  ./bin/detectobj2 -m <model_path> -i <image_path> [-t <threshold>] [-o <detections_path>] [-d <0/1>]
  Example:
    > ./bin/detectobj2 -m data/models/person_v6.mat -i data/testimages/2008_007537.jpg -t -0.3 -o detections.txt

If the program has finished correctly, you will find a text file named 'detections.txt' with the following structure:
  <number_of_detected_objects>
  <x1_i> <y1_i> <x2_i> <y2_i> <score_i>
where <x1_i> <y1_i> <x2_i> <y2_i> are the pairs of coordinates of the i-th bounding box and <score_i> is its corresponding detection score.

Tip: object detection can be performed directly on video frames (e.g. avi file) with the test program named 'detectvid'.

--------------------------------------------------------------------------------
Citation:
--------------------------------------------------------------------------------
If you use this library for your publications, please cite it as:
@misc{libpabod,
 author = {Rodriguez-Molina, Daniel and Marin-Jimenez, Manuel J.},
 title = {{LibPaBOD}: A Library for Part-Based Object Detection in {C++}},
 year = {2011},
 note =   {Software available at \url{http://www.uco.es/~in1majim/}}
}

--------------------------------------------------------------------------------
Contact the authors:
--------------------------------------------------------------------------------
Daniel Rodriguez Molina (developer) - i72romod@uco.es / daniel.rodriguez.molina@gmail.com
Manuel J. Marin-Jimenez (advisor) - mjmarin@uco.es

--------------------------------------------------------------------------------
References:
--------------------------------------------------------------------------------
[1] P. Felzenszwalb, R. Girshick, D. McAllester, D. Ramanan. "Object Detection with Discriminatively Trained Part Based Models." IEEE Transactions on Pattern Analysis and Machine Intelligence, Vol. 32, No. 9, September 2010.
URL: http://people.cs.uchicago.edu/~pff/latent/

--------------------------------------------------------------------------------
Version history:
--------------------------------------------------------------------------------
 - v0.2.10: new option '-k' added to 'detectobj2' to show only K top detections
 - v0.2.9: moving CvMat to cv::Mat
 - v0.2.8: new class Detection
 - v0.2.7: memory leaks found by github/yokox solved. Thanks!
 - v0.2.6: new file 'export.h' for properly exporting functions to dll's.
 - v0.2.5: updated CMake files and other minor improvements for Windows compatibility.
 - v0.2.4: CMake and code updated to properly manage libmatio on Windows (thanks to Eric Sommerlade for his invaluable contributions). See new file 'INSTALL.windows'. libmatio 1.5.x is supported.
 - v0.2.2: detector component is now included in detections matrix. Other minor improvements.
 - v0.2.1: OpenMP is now optional via CMake. Selection of OpenMP overrides pthread's choice.
 - v0.2: new class Pabod. It encapsulates the class model and detection process. New demo file 'detectobj2.cpp' uses Pabod class. pthread is now optional via CMake. New 'quickstart.pdf' in directory 'doc'.
 - v0.1.7: fixed bugs (i.e. memory leaks) reported by <anonymous>.
 - v0.1.6: fixed documentation option for CMake (thanks to eichnerm). Use 'cmake .. -DINSTALL_DOC=ON" to generate the library documentation.
 - v0.1.5: CMake is supported (thanks to rmsalinas@uco.es). By default, the library is built in shared mode (non-static).
 - v0.1.4: added an alternative 'makeDetection' function. Updated makefile. Now 'make all' builds both the library and the test programs. 
 - v0.1.3: added 'detectvid' program. It performs detection on video sequences (e.g. avi). Use 'make test3' to generate it. Use 'make alltests' to generate all test programs. 
 - v0.1.2: small fixes in code.
 - v0.1.1: added 'detectobj' program. It can save detections to disk. Use 'make test2' to generate it. 
 - v0.1: first release.

