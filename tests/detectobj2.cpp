// File: detectobj2.cpp
//
// This file is part of the libpabod release
// This version uses Pabod method for drawing the detections
//
// (c) Daniel Rodriguez Molina and Manuel J. Marin-Jimenez
//     University of Cordoba, Spain


#include <iostream>
#include <string>
#include <fstream>
#include <highgui.h>
#include <makeDetection.h>
#include <pabod.h>
#include <crossplatform.h>


#define TAGGED	0
#define CUT		1

std::string extractModelName (std::string modelPath)
{
    std::string name;
	size_t pos = modelPath.find_last_of("_");

	name = modelPath.substr (0, pos);

	pos = name.find_last_of("/");

	name = name.substr (pos+1);

	if (name.compare("ubf") == 0)
	{
		name.clear();
		name.append("upper body");
	}

	return name;
}

std::string saveImage (const IplImage *im, std::string imgPath, int mode, const CvMat *results)
{
    std::string name, path;
	size_t pos;
	char imgNameCh[8];
	IplImage *cut = NULL;
	int x, y, w, h;

	pos = imgPath.find_last_of ("/");
	path = imgPath.substr(0, pos);

	if (mode == TAGGED)
	{
		pos = imgPath.find_last_of(".");

		name = imgPath.substr (0, pos);

		name.append ("_tagged");
		name.append(imgPath.substr(pos));

		cvSaveImage (name.c_str(), im);
	}

	else if (mode == CUT)
	{
		for (int i = 0; i < results->rows; i++)
		{
			x = min (cvGetReal2D (results, i, 0), cvGetReal2D (results, i, 2));
			y = min (cvGetReal2D (results, i, 1), cvGetReal2D (results, i, 3));
			w = std::abs(cvGetReal2D (results, i, 0) - cvGetReal2D (results, i, 2));
			h = std::abs(cvGetReal2D (results, i, 1) - cvGetReal2D (results, i, 3));

			cut = cvCreateImage (cvSize (w, h), im->depth, im->nChannels);

			for (int m = x; m < w+x; m++)
				for (int n = y; n < h+y; n++)
					cvSet2D (cut, n-y, m-x, cvGet2D(im, n, m));


			pos = imgPath.find_last_of(".");

			name = imgPath.substr (0, pos);

			sprintf (imgNameCh, "_cut%d", i+1);

			name.append (imgNameCh);
			name.append(imgPath.substr(pos));

			cvSaveImage	(name.c_str(), cut);

			cvReleaseImage (&cut);
			cut = NULL;
		}
	}

	return path;
}


int main ( int argc, char *argv[] )
{
	TIMER t_ini, t_fin;
	double secs=0;
    std::string modelfile(""), imName(""), aux, datafile;
	IplImage *im = NULL, *copy = NULL;
	CvMat *results = NULL;
	int nDetected = 0;
	float usedThresh=NEGATIVE_INF, thresh = POSITIVE_INF;
	float minScore, maxScore;
        bool savedata = false, display = true;

	if (argc < 5)
	{
        std::cout << "  >> ERROR: the general form is:\n"
				"            ./detectobj -m <model_path> -i <image_path> [-t <threshold> -o <detections_path> -d <0/1>]" << std::endl;
		return -1;
	}

        // Parse options
        for (int i = 1; i < argc; i+=2)
        {

            if (i + 1 != argc) // Check that we haven't finished parsing already
            {
                if (std::string(argv[i]) == "-i") {
                    // We know the next argument *should* be the image filename:
                    imName = argv[i + 1];

                } else if (std::string(argv[i]) == "-m") {
                    modelfile = argv[i + 1];

                } else if (std::string(argv[i]) == "-o") {
                    datafile = argv[i + 1];
                    savedata = true;
                } else if (std::string(argv[i]) == "-d") { // Display images?
                    display = atoi(argv[i+1]);

                } else if (std::string(argv[i]) == "-t") {
                    thresh = atof(argv[i + 1]);

                } else {
                    std::cerr << ">> ERROR: Not enough or invalid arguments, please try again.\n";

                    exit(1);
                }
            }
        }

     if (imName == "")
     {
         std::cerr << ">> ERROR: input image is needed (-i option)" << std::endl;
         exit(1);
     }
     if (modelfile == "")
     {
         std::cerr << ">> ERROR: model file is needed (-m option)" << std::endl;
         exit(1);
     }

       // Load image
      im = cvLoadImage (imName.c_str(), CV_LOAD_IMAGE_COLOR);

    if (im == NULL)
   {
       std::cerr << ">> ERROR: the image cannot be loaded" << std::endl;
	exit(-1);
   }


    std::cout << std::endl << std::endl << std::endl << std::endl;
    std::cout << "  Model: " << modelfile << std::endl;
    std::cout << "  Image: " << imName << std::endl;
	if (thresh < POSITIVE_INF)
		std::cout << "  Threshold used:      " << thresh << std::endl;
	else
		std::cout << "  Threshold used:      auto" << std::endl;
    std::cout << std::endl;

        if (display)
        {
	   cvNamedWindow("Input image", CV_WINDOW_AUTOSIZE);
	   cvShowImage ("Input image", im);
	   cvWaitKey(250);
        }

	Pabod detector(modelfile);
    std::cout << " Class: " << detector.getClass() << std::endl;
    std::cout << " Searching for objects... This operation may take a few seconds" << std::endl << std::endl;

	// Get the current time before starting detection
	GET_TIME(&t_ini);

       // Call to main function
	usedThresh = detector.detect(im, thresh, &results);

	if (results != NULL)
		nDetected = results->rows;
	else
		nDetected = 0;

	// Get the current time after detection
	GET_TIME(&t_fin);

	// Number of secs taken to run detection
	secs = TIME_DIFF(t_fin, t_ini);

    std::cout << "Elapsed time: " << secs << " seconds" << std::endl << std::endl;

    std::cout << nDetected << " object(s) found using threshold = " << usedThresh << std::endl;
    std::cout << "----------------------------------------------" << std::endl << std::endl;

	if (nDetected > 0)
	{
		minScore = 50;
        maxScore = -50;

        for (int i = 0; i < nDetected; i++)
        {
            if (cvGetReal2D(results, i, 4) < minScore)
                minScore = cvGetReal2D(results, i, 4);
            if (cvGetReal2D(results, i, 4) > maxScore)
                maxScore = cvGetReal2D(results, i, 4);
        }

        if (maxScore == minScore)
            minScore = usedThresh;

		copy = cvCloneImage (im);

                // Save data?
                if (savedata)
                {
                   // Open file to save detections
                    std::ofstream fid(datafile.c_str());
                   if (!fid.is_open())
                   {
                       std::cerr << "WARNING: cannot create output file." << std::endl;
                   }
                   else
                   {
                      // Save to file
                      fid << nDetected << std::endl;
                      for (int i = nDetected - 1; i >= 0; i--)
		      {

			fid << cvGetReal2D(results, i, 0) << " " << cvGetReal2D(results, i, 1) << " ";
			fid << cvGetReal2D(results, i, 2) << " " << cvGetReal2D(results, i, 3) << " ";
			fid << cvGetReal2D (results, i, 4) << std::endl;
		      }

                      // Close file
                      fid.close();
                   }
                }

		//------------- NEW CODE -------------
	        detector.drawDetections(im, results);
		//------------------------------------

		for (int i = 0; i < nDetected; i++)
			std::cout << "  - " << extractModelName(modelfile) << " " << i+1 << ", score = " << cvGetReal2D (results, i, 4) << std::endl;

                if (display)
                {
		   cvNamedWindow("Detected image", CV_WINDOW_AUTOSIZE);
		   cvShowImage ("Detected image", im);


           std::cout << std::endl << "Push 't' key to save a copy of (t)agged image" << std::endl;
           std::cout << "Push 'c' key to save each of objects found on differents (c)ut images" << std::endl;
           std::cout << "Push 'q' key to (q)uit" << std::endl << std::endl;

		   char c = 0;

		  do
		  {
			c = cvWaitKey(100);

			if (c == 't' || c == 'T')
			{
				aux = saveImage (im, imName, TAGGED, NULL);
                std::cout << "  >> Tagged image saved on <" << aux << "> folder" << std::endl << std::endl;

				c = 0;

                std::cout << std::endl << "Push 't' key to save a copy of (t)agged image" << std::endl;
                std::cout << "Push 'c' key to save each of objects found on differents (c)ut images" << std::endl;
                std::cout << "Push 'q' key to (q)uit" << std::endl << std::endl;
			}

			else if (c == 'c' || c == 'C')
			{
				aux = saveImage (copy, imName, CUT, results);
                std::cout << "  >> Cut images saved on <" << aux << "> folder" << std::endl << std::endl;

				c = 0;

                std::cout << std::endl << "Push 't' key to save a copy of (t)agged image" << std::endl;
                std::cout << "Push 'c' key to save each of objects found on differents (c)ut images" << std::endl;
                std::cout << "Push 'q' key to (q)uit" << std::endl << std::endl;
			}

		} while ( (c != 'q') && (c != 'Q'));
           }
           else
           {
               std::cout << std::endl << "* End of detection process." << std::endl << std::endl;
           } // if display
	}

	else
		std::cout << std::endl << "  >> No objects found" << std::endl << std::endl;

	cvReleaseImage (&im);
	cvReleaseMat (&results);

    std::cout << "Running libPaBOD version " << PABOD_MAJOR_VERSION << "." << PABOD_MINOR_VERSION << "." << PABOD_PATCH_VERSION << std::endl;

	return 0;
}

