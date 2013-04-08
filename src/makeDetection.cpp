#include <makeDetection.h>


PABOD_EXPORT float makeDetection (CvMat **results, IplImage *img, Model * model, float thresh)
{

  CvMat *dets = NULL;
  CvMat *boxes = NULL;
  CvMatND *info = NULL;

  if (thresh == POSITIVE_INF)
    thresh = (float)model->getThresh();

  bool found = imgDetect (img, model, thresh, NULL, NEGATIVE_INF, &dets, &boxes, &info);

  if (found)
  {
    std::vector<int> pick = nms (dets, 0.3);

    (*results) = cvCreateMat (pick.size(), 6, CV_32FC1);

    for (size_t i = 0; i < pick.size(); i++)
    {
      cvSetReal2D ((*results), i, 0, cvGetReal2D (dets, pick[i], 0));
      cvSetReal2D ((*results), i, 1, cvGetReal2D (dets, pick[i], 1));
      cvSetReal2D ((*results), i, 2, cvGetReal2D (dets, pick[i], 2));
      cvSetReal2D ((*results), i, 3, cvGetReal2D (dets, pick[i], 3));
//cout << cvGetReal2D (dets, pick[i], 4) << "+ " << cvGetReal2D (dets, pick[i], 5) << " |";
      cvSetReal2D ((*results), i, 4, cvGetReal2D (dets, pick[i], 5));
      cvSetReal2D ((*results), i, 5, cvGetReal2D (dets, pick[i], 4));
    }
  }

	else
		(*results) = NULL;

	if (dets != NULL)
	{
	  cvReleaseMat(&dets);
		dets = NULL;
	}
	if (boxes != NULL)
	{
	  cvReleaseMat(&boxes);
		boxes = NULL;
	}
	if (info != NULL)
	{
	  cvReleaseMatND(&info);
		info = NULL;
	}

  return thresh;
}

float makeDetection (CvMat **results, IplImage *img, std::string modelType, float thresh)
{
  float athresh;
  Model *model = new Model(modelType);

  athresh = makeDetection (results, img, model, thresh);

  delete model;

  return athresh;
}


