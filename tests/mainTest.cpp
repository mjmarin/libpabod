#include <iostream>
#include <string>
#include <highgui.h>
#include <makeDetection.h>
#include <ctime>
#include <sys/time.h>

#define TAGGED	0
#define CUT		1

double timeval_diff(struct timeval *a, struct timeval *b)
{
	return (double)(a->tv_sec + (double)a->tv_usec/1000000) - (double)(b->tv_sec + (double)b->tv_usec/1000000);
}


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

void drawBoxes (IplImage *im, const CvPoint p1, const CvPoint p2, const std::string modelName, int i,
				float maxScore, float minScore, float curScore)
{
	CvScalar color;
	CvPoint bottomLeftCornerTag;
	CvPoint topRightCornerTag;
	CvPoint textPoint;
	CvFont font;
	CvSize textSize;
	int textBase = 0;
	char modelNameCh[modelName.length()+4];
	float m, b;
	int h;


	sprintf (modelNameCh, "%s %d", modelName.c_str(), i);

	m = (-300) / (maxScore-minScore);
	b = -(m * maxScore);
	h = m*curScore + b;

	color = cvScalar (h/2, 255, 255);

	cvCvtColor (im, im, CV_BGR2HSV);

	cvRectangle (im, p1, p2, color, 2);

	cvInitFont (&font, CV_FONT_HERSHEY_DUPLEX, 0.4, 0.6, 0, 1, CV_AA);
	cvGetTextSize (modelNameCh, &font, &textSize, &textBase);

	bottomLeftCornerTag.x = min (p1.x, p2.x) -1;
	bottomLeftCornerTag.y = min (p1.y, p2.y);

	topRightCornerTag.x = bottomLeftCornerTag.x + textSize.width + 2;
	topRightCornerTag.y = bottomLeftCornerTag.y - textSize.height - 3;

	if (bottomLeftCornerTag.x < 0)
	{
		bottomLeftCornerTag.x = max (p1.x, p2.x) +1;
		topRightCornerTag.x = bottomLeftCornerTag.x - textSize.width - 2;
	}

	if (topRightCornerTag.y < 0)
	{
		bottomLeftCornerTag.y = max (p1.y, p2.y);
		topRightCornerTag.y = bottomLeftCornerTag.y + textSize.height + 3;
	}

	cvRectangle (im, bottomLeftCornerTag, topRightCornerTag, color, CV_FILLED);

	cvCvtColor (im, im, CV_HSV2BGR);

	textPoint.x = min (bottomLeftCornerTag.x, topRightCornerTag.x) + 1;
	textPoint.y = max (bottomLeftCornerTag.y, topRightCornerTag.y) - 2;

	cvPutText (im, modelNameCh, textPoint, &font, cvScalarAll (255));
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
			x = min(cvGetReal2D (results, i, 0), cvGetReal2D (results, i, 2));
			y = min(cvGetReal2D (results, i, 1), cvGetReal2D (results, i, 3));
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
	struct timeval t_ini, t_fin;
	double secs;
    std::string file, imName, aux;
	IplImage *im = NULL, *copy = NULL;
	CvMat *results = NULL;
	int nDetected = 0;
	float usedThresh, thresh = POSITIVE_INF;
	float minScore, maxScore;

	if (argc < 3 || argc > 4)
	{
        std::cout << "  >> ERROR: the general form is:\n"
				"            ./mainTest <model_path> <image_path> [<threshold>]" << std::endl;
		return -1;
	}

	if (argc >= 3)
	{
		file = argv[1];
		imName = argv[2];

		im = cvLoadImage (imName.c_str(), CV_LOAD_IMAGE_COLOR);

		if (im == NULL)
		{
            std::cout << ">> ERROR: the image cannot be loaded" << std::endl;
			exit(-1);
		}

		if (argc == 4)
			thresh = atof(argv[3]);
	}


    std::cout << std::endl << std::endl << std::endl << std::endl;
    std::cout << "  Model file selected: " << file << std::endl;
    std::cout << "  Image file selected: " << imName << std::endl;
	if (argc == 4)
		std::cout << "  Threshold used:      " << thresh << std::endl;
	else
		std::cout << "  Threshold used:      auto" << std::endl;
    std::cout << std::endl;

	cvNamedWindow("Input image", CV_WINDOW_AUTOSIZE);
	cvShowImage ("Input image", im);
	cvWaitKey(250);

    std::cout << "Searching objects... This operation may take a few seconds" << std::endl << std::endl;

	// Get the current time before load the model
	gettimeofday(&t_ini, NULL);

	usedThresh = makeDetection (&results, im, file, thresh);

	if (results != NULL)
		nDetected = results->rows;
	else
		nDetected = 0;

	// Get the current time after load the model
	gettimeofday(&t_fin, NULL);

	// Number of secs taken to load the whole model
	secs = timeval_diff(&t_fin, &t_ini);

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

		for (int i = nDetected - 1; i >= 0; i--)
		{
			drawBoxes 	(
							im,
							cvPoint (cvGetReal2D(results, i, 0), cvGetReal2D(results, i, 1)),
							cvPoint (cvGetReal2D(results, i, 2), cvGetReal2D(results, i, 3)),
							extractModelName(file),
							i+1,
							maxScore,
							minScore,
							cvGetReal2D (results, i, 4)
						);
		}

		for (int i = 0; i < nDetected; i++)
			std::cout << "  - " << extractModelName(file) << " " << i+1 << ", score = " << cvGetReal2D (results, i, 4) << std::endl;

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
		std::cout << std::endl << "  >> No objects found" << std::endl << std::endl;

	cvReleaseImage (&im);
	cvReleaseMat (&results);

	return 0;
}

