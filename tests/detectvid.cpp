// File: detectvid.cpp
//
// This file is part of the libpabod release
//
// (c) Daniel Rodriguez Molina and Manuel J. Marin-Jimenez
//     University of Cordoba, Spain


#include <iostream>
#include <string>
#include <fstream>
#include <highgui.h>
#include <makeDetection.h>
#include <ctime>
#include <pabod.h>
#include <crossplatform.h>

#define TAGGED	0
#define CUT		1

using namespace std;

string extractModelName (string modelPath)
{
	string name;
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

string saveImage (const IplImage *im, string imgPath, int mode, const CvMat *results)
{
	string name, path;
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
			w = abs(cvGetReal2D (results, i, 0) - cvGetReal2D (results, i, 2));
			h = abs(cvGetReal2D (results, i, 1) - cvGetReal2D (results, i, 3));

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

string saveImage (const cv::Mat & im, string imgPath, int mode, const LDetections & results)
{
	string name, path;
	size_t pos;
	char imgNameCh[8];	
	int nresults;
	int x, y, w, h;

	pos = imgPath.find_last_of ("/");
	path = imgPath.substr(0, pos);

	if (mode == TAGGED)
	{
		pos = imgPath.find_last_of(".");

		name = imgPath.substr (0, pos);

		name.append ("_tagged");
		name.append(imgPath.substr(pos));

		cv::imwrite(name,im);
		
	}

	else if (mode == CUT)
	{
      nresults = (int) results.size();
		for (int i = 0; i < nresults; i++)
		{
			x = min (results[i].getX1(), results[i].getX2());
			y = min (results[i].getY1(), results[i].getY2());
			w = abs(results[i].getW());
			h = abs(results[i].getH());

			cv::Mat cut2(cv::Size(w, h), cv::DataType<cv::Vec<uchar,3> >::type);
			cout <<  im.channels() << endl;					

			for (int m = x; m < w+x; m++)
				for (int n = y; n < h+y; n++)					
					cut2.at<cv::Vec3b>(n-y, m-x) = im.at<cv::Vec3b>(n, m);


			pos = imgPath.find_last_of(".");

			name = imgPath.substr (0, pos);

			sprintf (imgNameCh, "_cut%d", i+1);

			name.append (imgNameCh);
			name.append(imgPath.substr(pos));

			
			cv::imwrite(name, cut2);
			cut2.release();
		}
	}

	return path;
}

int main ( int argc, char *argv[] )
{
	TIMER t_ini, t_fin;
	double secs=0;
	string modelfile(""), vidName(""), imName(""), aux, datafile;
	//IplImage *im = NULL, *copy = NULL, *frame= NULL;
	cv::Mat im, copy, frame;
	//CvMat *results = NULL;
	LDetections results;
	int nDetected = 0;
	float usedThresh=NEGATIVE_INF, thresh = POSITIVE_INF;
	float minScore, maxScore;
    bool savedata = false, display = true;
	double iouNms = 0.5;


	if (argc < 5)
	{
		cout << "  >> ERROR: the general form is:\n"
				"            ./detectvid -m <model_path> -v <video_path> [-t <threshold> -o <detections_path> -d <0/1>]" << endl;
		return -1;
	}

        // Parse options
        for (int i = 1; i < argc; i+=2)
        {

            if (i + 1 != argc) // Check that we haven't finished parsing already
            {
                if (string(argv[i]) == "-v") {
                    // We know the next argument *should* be the image filename:
                    vidName = argv[i + 1];

                } else if (string(argv[i]) == "-m") {
                    modelfile = argv[i + 1];

                } else if (string(argv[i]) == "-o") {
                    datafile = argv[i + 1];
                    savedata = true;
                } else if (string(argv[i]) == "-d") { // Display images?
                    display = atoi(argv[i+1]);

                } else if (string(argv[i]) == "-t") {
                    thresh = atof(argv[i + 1]);

                } else if (string(argv[i]) == "-n") {
                    iouNms = atof(argv[i + 1]);

                } else {
                    cerr << ">> ERROR: Not enough or invalid arguments, please try again.\n";

                    exit(1);
                }
            }
        }

     if (vidName == "")
     {
         cerr << ">> ERROR: input image is needed (-v option)" << endl;
         exit(1);
     }
     if (modelfile == "")
     {
         cerr << ">> ERROR: model file is needed (-m option)" << endl;
         exit(1);
     }

       // Open video
     CvCapture * vid = cvCaptureFromAVI( vidName.c_str() );
     if (vid == NULL)
     {
         cerr << ">> ERROR: video cannot be opened." << endl;
         exit(-1);
     }

     // Create Pabod detector from file
	  Pabod detector(modelfile);
     
     // Detection per frame 
     int frix = 0;
     while (true)
     {  
        // Grab frame  
        frame = cvQueryFrame( vid);

        // Check if frame is read
		if (frame.empty())
        {
            break; // No more frames
        }
        else
        {
           char tmpname[2048];
           sprintf(tmpname, "_frix%03d.png", frix);
           imName = vidName + tmpname;
           frix++;
        }

		im = frame.clone();
        //im = cvCloneImage(frame);

	cout << endl << endl << endl << endl;
	cout << "  Model: " << modelfile << endl;
	cout << "  Image: " << imName << endl;
	if (thresh < POSITIVE_INF)
		cout << "  Threshold used:      " << thresh << endl;
	else
		cout << "  Threshold used:      auto" << endl;
	cout << endl;

        if (display)
        {
	   cvNamedWindow("Input frame", CV_WINDOW_AUTOSIZE);
	   //cvShowImage ("Input image", im);
	   cv::imshow("Input frame", im);
	   cvWaitKey(250);
        }

	cout << "Searching for objects... This operation may take a few seconds" << endl << endl;

	// Get the current time before starting detection
	GET_TIME(&t_ini);

    // Call to main function	
	usedThresh = detector.detect(im, thresh, iouNms, results);

	/*
	if (results != NULL)
		nDetected = results->rows;
	else
		nDetected = 0;
		*/
	nDetected = results.size();

	// Get the current time after detection
	GET_TIME(&t_fin);

	// Number of secs taken to run detection	
	secs = TIME_DIFF(t_fin, t_ini);

	cout << "Elapsed time: " << secs << " seconds" << endl << endl;


	cout << nDetected << " object(s) found using threshold = " << usedThresh << endl;
	cout << "----------------------------------------------" << endl << endl;

	if (nDetected > 0)
	{
		minScore = 50;
        maxScore = -50;

        for (int i = 0; i < nDetected; i++)
        {

			if (results[i].getScore() < minScore)
				minScore = results[i].getScore();

			if (results[i].getScore() < maxScore)
				maxScore = results[i].getScore();
        }

        if (maxScore == minScore)
            minScore = usedThresh;

		//copy = cvCloneImage (im);
		copy = im.clone();

                // Save data?
                if (savedata)
                {
                   // Open file to save detections
                   ofstream fid;
                   if (frix == 1)
                      fid.open(datafile.c_str());
                   else
                      fid.open(datafile.c_str(), ios_base::app);

                   if (!fid.is_open())
                   {
                      cerr << "WARNING: cannot create output file." << endl;
                   }
                   else
                   {
                      // Save to file
                      fid << nDetected << endl;
                      for (int i = nDetected - 1; i >= 0; i--)
		      {
				  /*
			fid << cvGetReal2D(results, i, 0) << " " << cvGetReal2D(results, i, 1) << " ";
			fid << cvGetReal2D(results, i, 2) << " " << cvGetReal2D(results, i, 3) << " ";
			fid << cvGetReal2D (results, i, 4) << endl;
			*/
				  fid << results[i].getX1() << " " << results[i].getY1() << " ";
			      fid << results[i].getX2() << " " << results[i].getY2() << " ";
			      fid << results[i].getScore() << endl;
		      }

                      // Close file
                      fid.close();
                   }
                }

       // Draw detections
		detector.drawDetections(im, results);

		for (int i = 0; i < nDetected; i++)
			cout << "  - " << extractModelName(modelfile) << " " << i+1 << ", score = " << results[i].getScore() << endl;

                if (display)
                {
		   cvNamedWindow("Detected objects", CV_WINDOW_AUTOSIZE);
		   //cvShowImage ("Detected objects", im);
		   cv::imshow("Detected objects", im);


		   cout << endl << "Push 't' key to save a copy of (t)agged image" << endl;
		   cout << "Push 'c' key to save each of objects found on differents (c)ut images" << endl;
		   cout << "Push 'q' key to (q)uit" << endl << endl;

		   char c = 0;

		  do
		  {
			c = cvWaitKey(100);

			if (c == 't' || c == 'T')
			{
				aux = saveImage (im, imName, TAGGED, results);
				cout << "  >> Tagged image saved on <" << aux << "> folder" << endl << endl;

				c = 0;

				cout << endl << "Push 't' key to save a copy of (t)agged image" << endl;
				cout << "Push 'c' key to save each of objects found on differents (c)ut images" << endl;
				cout << "Push 'q' key to (q)uit" << endl << endl;
			}

			else if (c == 'c' || c == 'C')
			{
				aux = saveImage (copy, imName, CUT, results);
				cout << "  >> Cut images saved on <" << aux << "> folder" << endl << endl;

				c = 0;

				cout << endl << "Push 't' key to save a copy of (t)agged image" << endl;
				cout << "Push 'c' key to save each of objects found on differents (c)ut images" << endl;
				cout << "Push 'q' key to (q)uit" << endl << endl;
			}

		} while ( (c != 'q') && (c != 'Q'));
           }
           else
           {
              cout << endl << "* End of detection process." << endl << endl;
           } // if display
	}

	else
		cout << endl << "  >> No objects found" << endl << endl;

		results.clear();
	//cvReleaseMat (&results);
		im.release();
        //cvReleaseImage (&im);

     } // while-doProcessing

   cout << "Running libPaBOD version " << PABOD_MAJOR_VERSION << "." << PABOD_MINOR_VERSION << "." << PABOD_PATCH_VERSION << endl;

     return 0;
}

