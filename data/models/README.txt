Frontal Upper-Body Detector for libpabod
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This frontal upper-body detector has been trained* with the data available at:
   http://www.robots.ox.ac.uk/~vgg/software/UpperBody/
	
If you use this upper-body model in your work, please, cite:
   [1] Ferrari, V., Marin-Jimenez, M. and Zisserman, A.
       "Progressive Search Space Reduction for Human Pose Estimation"
       Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition (2008) 	
	
(*) Note that this version of the upper-body detector is not exactly the same one used in [1].
	
Installation
============
   Copy the file named 'ubf_libpabod.mat' into your '<libpabod_dir>/data/models' directory.	 
	
Test
====
   > cd <libpabod_dir>
   > bin/detectobj2 -i data/testimages/2009_004848.jpg -m data/models/ubf_libpabod.mat -d 1 -o ubdets.txt -t 0.0
	
Contact	
=======
   mjmarin (at) uco (dot) es
	
