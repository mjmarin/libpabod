#include <imgdetect.h>

bool imgDetect (IplImage *input, Model *model, double thresh, double *bbox,
                double overlap, CvMat **dets, CvMat **boxes, CvMatND **info)
{
  bool returned = false;

  // Color image is assumed
  assert (input->nChannels == 3);

  // Get the feature pyramid
  FeatPyramid pyra = FeatPyramid (input, model);


  if (bbox == NULL)
    bbox = NULL;

  if (overlap == NEGATIVE_INF)
    overlap = NEGATIVE_INF;

  gdetect (dets, boxes, info, pyra, model, thresh, bbox, overlap);

  if (*dets == NULL && *boxes == NULL && *info == NULL)
    returned = false;

  else
    returned = true;

  return returned;
}


