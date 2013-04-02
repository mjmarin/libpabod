// File: detectobj.cpp
//
// This file is part of the libpabod release
//
// (c) Daniel Rodriguez Molina and Manuel J. Marin-Jimenez
//     University of Cordoba, Spain


#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include <highgui.h>

#include "makeDetection.h"
#include "pabod.h"

#define TAGGED	0
#define CUT		1

static void print_all_rects(CvMat *results, int nDetected)
{
    if(nDetected > 0)
    {
        for(int i = 0;i < nDetected; i++)
        {
            int width  = cvGetReal2D(results,i,2) - cvGetReal2D(results,i,0);
            int height = cvGetReal2D(results,i,3) - cvGetReal2D(results,i,1);

            if(i != 0) {
                std::cout << ",";
            }

            std::cout << cvGetReal2D(results,i,0) << ","
                      << cvGetReal2D(results,i,1) << ","
                      << width << ","
                      << height;
        }
    }
}

static int process_one_image(Pabod& detector, const std::string& im_filename, float thresh, std::ostream& os)
{
    IplImage *im = cvLoadImage (im_filename.c_str(), CV_LOAD_IMAGE_COLOR);
    if (im == NULL) {
        os << ">> ERROR: the image cannot be loaded" << std::endl;
        return -1;
    }

    os << "  Image: " << im_filename << std::endl;
    if (thresh == POSITIVE_INF)
        os << "  Threshold used:      auto" << std::endl;
    else
        os << "  Threshold used:      " << thresh << std::endl;

    os << "Searching for objects... This operation may take a few seconds" << std::endl ;

    CvMat* results;
    float usedThresh = detector.detect(im, thresh, &results);

    int nDetected = results != NULL ?
                    results->rows :
                    0;

    os << nDetected << " object(s) found using threshold = " << usedThresh << std::endl;
    os << "----------------------------------------------" << std::endl << std::endl;

    if (nDetected > 0)
    {
        float minScore = std::numeric_limits<int>::max();
        float maxScore = std::numeric_limits<int>::min();

        for (int i = 0; i < nDetected; i++)
        {
            if (cvGetReal2D(results, i, 4) < minScore)
                minScore = cvGetReal2D(results, i, 4);
            if (cvGetReal2D(results, i, 4) > maxScore)
                maxScore = cvGetReal2D(results, i, 4);
        }

        if (maxScore == minScore) {
            minScore = usedThresh;
        }

        print_all_rects(results, nDetected);
        detector.drawDetections(im, results);

        for (int i = 0; i < nDetected; i++) {
            os << " - car " << i+1 << ", score = " << cvGetReal2D (results, i, 4) << std::endl;
        }
    }
    else {
        os << std::endl << "  >> No objects found" << std::endl << std::endl;
    }

    cvReleaseImage (&im);
    cvReleaseMat (&results);

    return nDetected;
}

class NullOutputStream : public std::ostream {
  public:
    template<typename T>
    std::ostream& operator<<(const T& v) {
        return *this;
    }
} ;

int main ()
{
    const bool debug_on = true;
    NullOutputStream null_os;
    std::ostream& os = debug_on ? std::cerr : null_os;

    os << "Loading model ..." << std::endl;
    Pabod detect_vehicle("car_v6.mat");
    os << "Model loaded." << std::endl;

    std::string imName;
    while(std::cin >> imName)
    {
        float thresh = POSITIVE_INF;
        process_one_image(detect_vehicle, imName, thresh, os);
    }

    os << "Program gracefully shotdown." << std::endl;
    return 0;
}

