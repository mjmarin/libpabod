#include <makeDetection.h>


PABOD_EXPORT float makeDetection (CvMat **results, IplImage *img, Model * model, float thresh)
{

  CvMat *dets = NULL;
  CvMat *boxes = NULL;
  CvMatND *info = NULL;

  if (thresh == POSITIVE_INF)
    thresh = (float)model->getThresh();

  bool found = imgDetect (img, model, thresh, NULL, NEGATIVE_INF, &dets, &boxes, &info);

  int detected = 0;

  if (found)
  {
    int *pick;
    int pickDim;

    nms (&pick, &pickDim, dets, 0.5);

    (*results) = cvCreateMat (pickDim, 6, CV_32FC1);

    for (int i = 0; i < pickDim; i++)
    {
      cvSetReal2D ((*results), i, 0, cvGetReal2D (dets, pick[i], 0));
      cvSetReal2D ((*results), i, 1, cvGetReal2D (dets, pick[i], 1));
      cvSetReal2D ((*results), i, 2, cvGetReal2D (dets, pick[i], 2));
      cvSetReal2D ((*results), i, 3, cvGetReal2D (dets, pick[i], 3));
//cout << cvGetReal2D (dets, pick[i], 4) << "+ " << cvGetReal2D (dets, pick[i], 5) << " |";
      cvSetReal2D ((*results), i, 4, cvGetReal2D (dets, pick[i], 5));
      cvSetReal2D ((*results), i, 5, cvGetReal2D (dets, pick[i], 4));
    }

    detected = pickDim;

    delete[] pick;
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

PABOD_EXPORT float makeDetection (CvMat **results, IplImage *img, string modelType, float thresh)
{
  float athresh;
  Model *model = new Model(modelType);

  athresh = makeDetection (results, img, model, thresh);

  delete model;

  return athresh;
}


