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
#include <boost/filesystem.hpp>

#include <highgui.h>

#include "makeDetection.h"
#include "pabod.h"

#define TAGGED	0
#define CUT		1

static void print_all_rects(CvMat *results)
{
    std::cout << results->rows;
    if(results->rows > 0)
    {
        for(int i = 0;i < results->rows; i++)
        {
            int width  = cvGetReal2D(results,i,2) - cvGetReal2D(results,i,0);
            int height = cvGetReal2D(results,i,3) - cvGetReal2D(results,i,1);

            std::cout << " ";
            std::cout << cvGetReal2D(results,i,0) << ","
                      << cvGetReal2D(results,i,1) << ","
                      << width << ","
                      << height;
        }
    }
    std::cout << std::endl;
}

static int process_one_image(Pabod& detector, const std::string& im_filename, float thresh, std::ostream& os, const bool debug_on, const bool save_canvas)
{
    IplImage *im = cvLoadImage (im_filename.c_str(), CV_LOAD_IMAGE_COLOR);
    if (im == NULL) {
        os << ">> ERROR: the image cannot be loaded" << std::endl;
        return -1;
    }

    os << "  Image: " << im_filename << std::endl;
    if (thresh == POSITIVE_INF)
        os << "  Threshold used: auto" << std::endl;
    else
        os << "  Threshold used: " << thresh << std::endl;

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

        print_all_rects(results);
        detector.drawDetections(im, results);

        for (int i = 0; i < nDetected; i++) {
            os << " - car " << i+1 << ", score = " << cvGetReal2D (results, i, 4) << std::endl;
        }

        if(save_canvas) {
            boost::filesystem::path p(im_filename);
            boost::filesystem::path out_im_name = "debug-output" / p.filename();
            cv::Mat mat(im);
            cv::imwrite(out_im_name.string(), mat);
        }

        if(debug_on) {
            cv::namedWindow("result", cv::WINDOW_NORMAL);
            cv::Mat mat(im);
            cv::imshow("result", mat);
            cv::waitKey(0);
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

namespace fs = boost::filesystem;

static std::vector<fs::path> glob_pic_files(fs::path dir_or_file) {
    if(fs::is_regular_file(dir_or_file))
        return {dir_or_file};

    typedef fs::directory_iterator dir_iter;
    std::vector<fs::path> files;
    for(dir_iter i(dir_or_file); i != dir_iter(); ++i) {
        const fs::path& p(i->path());
        if(!fs::is_regular_file(p)) continue;
        if(p.extension() != ".png" &&
           p.extension() != ".jpg" &&
           p.extension() != ".JPG" &&
           p.extension() != ".bmp"
           )
        {
            continue;
        }
        files.push_back(p);
    }
    std::sort(files.begin(), files.end());
    return files;
}

int main (int argc, char* argv[])
{
    const bool debug_on = false;
    const bool save_canvas = true;
    NullOutputStream null_os;
    std::ostream& os = debug_on ? std::cerr : null_os;
    float thresh = -0.35;

    os << "Loading model ..." << std::endl;
    Pabod detect_vehicle("car_v6.mat");
    os << "Model loaded." << std::endl;

    if(argc == 2) {
        fs::path input_dir = argv[1];
        auto pic_files = glob_pic_files(input_dir);
        for(std::vector<fs::path>::size_type i = 0; i < pic_files.size(); ++i) {
            std::cerr << "Processing [ " << i + 1 << " / " << pic_files.size() << " ]" << pic_files[i] << "..." << std::endl;
            process_one_image(detect_vehicle, pic_files[i].string(), thresh, os, debug_on, save_canvas);
        }
    } else {
        std::string imName;
        while(std::cin >> imName)
        {
            //float thresh = POSITIVE_INF;
            process_one_image(detect_vehicle, imName, thresh, os, debug_on, save_canvas);
        }
        os << "Program gracefully shotdown." << std::endl;
    }
    return 0;
}

