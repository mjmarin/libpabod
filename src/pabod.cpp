/**
  \file pabod.cpp
  \author Manuel J. Marin-Jimenez and D. Rodriguez-Molina
// (c) MJMJ/2012
*/

#include <cassert>
#include <cstdlib>
#include <pabod.h>
#include <makeDetection.h>
#include <limits>

Pabod::Pabod()
{
    _model = NULL;
    _isCopied = false;
}

Pabod::Pabod(const Pabod & pabod)
{
    _model = pabod._model; // This can be dangerous since operator= is not overloaded in class Model
    _isCopied = true;
}

Pabod::Pabod(const char * modelfile)
{
    _model = new Model(std::string(modelfile));
    _isCopied = false;
}

Pabod::Pabod(std::string modelfile)
{
    _model = new Model(modelfile);
    _isCopied = false;
}

Pabod::~Pabod()
{
    if (_model != NULL)
    {
        if (!_isCopied)
            _model->destroyModel();

        _model = NULL;
    }
}

bool Pabod::loadModel(std::string modelfile)
{
    if (_model == NULL)
        _model = new Model(modelfile);
    else
    {
        _model->destroyModel();
        _model->loadFile(modelfile);
    }

    return true;
}

float Pabod::detect(const char * imgfilename, float thr, CvMat ** detections)
{
    IplImage * img = NULL;
    float usedThr = 999999;

    img = cvLoadImage (imgfilename, CV_LOAD_IMAGE_COLOR);
    if (img == NULL)
    {
        std::cerr << "Cannot load image [Pabod::detect]" << std::endl;
    }
    else
        usedThr = detect(img, thr, detections);

    if (img != NULL)
        cvReleaseImage(&img);

    return usedThr;
}

float Pabod::detect(IplImage * img, float thr, CvMat ** detections)
{
    float usedThr = -999999;
    if (!empty())
    {
        // Call to main detection function
        usedThr = makeDetection (detections, img, _model, thr);
    }
    else
        std::cerr << "ERROR: model must be set before calling this method." << std::endl;

    return usedThr;
}

int Pabod::drawDetections(IplImage * img, CvMat * detections)
{
    int ndetections = 0;
    int i;
    CvPoint p1, p2;
    //srand(time(NULL));
    srand(123456);

    ndetections = detections->rows;
    for (i = 0; i < ndetections; i++)
    {
        p1 = cvPoint ((int)cvGetReal2D(detections, i, 0), (int)cvGetReal2D(detections, i, 1));
        p2 = cvPoint ((int)cvGetReal2D(detections, i, 2), (int)cvGetReal2D(detections, i, 3));

        drawBB(img, p1, p2, cvScalar(int(rand()%256), int(rand()%256), int(rand()%256)), (float)cvGetReal2D(detections, i, 4), cvGetReal2D(detections, i, 5));
    }

    return ndetections;
}

bool Pabod::drawBB(IplImage * img, CvPoint p1, CvPoint p2, CvScalar color, float score, int component)
{
    //CvScalar color;
    CvPoint bottomLeftCornerTag;
    CvPoint topRightCornerTag;
    CvPoint textPoint;
    CvFont font;
    CvSize textSize;
    int textBase = 0;

    char scoreStr[64];
    //float m, b;
    //int h;


    //color = cvScalar (128, 255, 128);

    cvRectangle (img, p1, p2, color, 2);

    if (score != -(std::numeric_limits<float>::infinity()))
    {
        if (component > 0)
            sprintf(scoreStr, "%.3f [%d]", score, component);
        else
            sprintf(scoreStr, "%.3f", score);
        cvInitFont (&font, CV_FONT_HERSHEY_DUPLEX, 0.4, 0.6, 0, 1, CV_AA);
        cvGetTextSize (scoreStr, &font, &textSize, &textBase);

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

        cvRectangle (img, bottomLeftCornerTag, topRightCornerTag, color, CV_FILLED);

        textPoint.x = min (bottomLeftCornerTag.x, topRightCornerTag.x) + 1;
        textPoint.y = max (bottomLeftCornerTag.y, topRightCornerTag.y) - 2;

        cvPutText (img, scoreStr, textPoint, &font, cvScalarAll (255));
    }

    return true;
}
