#include <nms.h>


void nms (int** pick, int *pickDim,
          CvMat *dets, double overlap)
{
  if (dets == NULL)
  {
    *pick = NULL;
    *pickDim = 0;
  }

  else
  {
    *pick = NULL;
    *pickDim = 0;

    std::vector<int> rows(dets->rows);
    std::iota(rows.begin(), rows.end(), 0);

    CvMat *x1 = sub_mat (dets, rows, {0});
    std::vector<double> x1Ptr = get_mat_data<double>(x1);

    CvMat *y1 = sub_mat (dets, rows, {1});
    std::vector<double> y1Ptr = get_mat_data<double>(y1);

    CvMat *x2 = sub_mat (dets, rows, {2});
    std::vector<double> x2Ptr = get_mat_data<double>(x2);

    CvMat *y2 = sub_mat (dets, rows, {3});
    std::vector<double> y2Ptr = get_mat_data<double>(y2);

    CvMat *s = sub_mat (dets, rows, {5});
    std::vector<double> sPtr = get_mat_data<double>(s);

    int areaDims[2] = {dets->rows, 1};

    CvMat *area;
    createMatrix (2, areaDims, CV_64FC1, &area);
    CvMat *p1, *p2;
    p1 = cvCloneMat (x1);
    p2 = cvCloneMat (y1);
    cvSub (x2, x1, p1);
    cvAddS (p1, cvScalar(1), p1);
    cvSub (y2, y1, p2);
    cvAddS (p2, cvScalar(1), p2);
    cvMul (p1, p2, area);

    std::vector<double> areaPtr = get_mat_data<double>(area);

    cvReleaseMat (&x1);
    cvReleaseMat (&y1);
    cvReleaseMat (&x2);
    cvReleaseMat (&y2);
    cvReleaseMat (&x1);
    cvReleaseMat (&s);
    cvReleaseMat (&area);
    cvReleaseMat (&p1);
    cvReleaseMat (&p2);

    std::vector<size_t> idx;

    shell_sort (sPtr, ASCEND, idx);

    while (idx.size() > 0)
    {
      int last = idx.size()-1;
      int i = idx[last];
      appendArray (pick, (*pickDim), &i, 1);
      (*pickDim)++;

      std::vector<size_t> suppress;
      suppress.push_back(last);

      for (int pos = 0; pos < last; pos++)
      {
        int j = idx[pos];

        int xx1 = max (x1Ptr[i], x1Ptr[j]);
        int yy1 = max (y1Ptr[i], y1Ptr[j]);
        int xx2 = min (x2Ptr[i], x2Ptr[j]);
        int yy2 = min (y2Ptr[i], y2Ptr[j]);
        int w = xx2 - xx1 + 1;
        int h = yy2 - yy1 + 1;

        if (w > 0 && h > 0)
        {
          // Compute overlap
          double o = (w * h) / areaPtr[j];

          if (o > overlap)
          {
            suppress.push_back(pos);
          }
        }
      }

      remove_indexes (idx, suppress);
    }
  }
}

