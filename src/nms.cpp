#include <vector>
#include <nms.h>

std::vector<int> nms (CvMat *dets, double overlap)
{
    std::vector<int> pick;
    if (dets == NULL) {
        return pick;
    }

    std::vector<double> x1(dets->rows);
    std::vector<double> y1(dets->rows);
    std::vector<double> x2(dets->rows);
    std::vector<double> y2(dets->rows);
    std::vector<double> areas(dets->rows);
    std::vector<double> scores(dets->rows);

    // collect coordinate, area, score
    for(int i = 0; i < dets->rows; ++i) {
        x1[i]     = cvGetReal2D(dets, i, 0);
        y1[i]     = cvGetReal2D(dets, i, 1);
        x2[i]     = cvGetReal2D(dets, i, 2);
        y2[i]     = cvGetReal2D(dets, i, 3);
        areas[i]  = (x2[i] - x1[i] + 1) * (y2[i] - y1[i] + 1);
        scores[i] = cvGetReal2D(dets, i, 5);
    }

    // sort by score, but track index
    // see http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
    // and http://stackoverflow.com/questions/10580982/c-sort-keeping-track-of-indices
    std::vector<size_t> idx(dets->rows);
    for(size_t i = 0; i < idx.size(); ++i) {
        idx[i] = i;
    }
    std::sort(idx.begin(), idx.end(), less_by_index<double>(scores));

    while (idx.size() > 0)
    {
        size_t last = idx.size()-1;
        size_t i = idx[last];
        pick.push_back(i);

        std::vector<size_t> suppress;
        suppress.push_back(last);

        for (size_t pos = 0; pos < last; pos++)
        {
            int j = idx[pos];

            int xx1 = max (x1[i], x1[j]);
            int yy1 = max (y1[i], y1[j]);
            int xx2 = min (x2[i], x2[j]);
            int yy2 = min (y2[i], y2[j]);
            int w = xx2 - xx1 + 1;
            int h = yy2 - yy1 + 1;

            if (w > 0 && h > 0)
            {
                // Compute overlap
                double o = (w * h) / areas[j];

                if (o > overlap)
                {
                    suppress.push_back(pos);
                }
            }
        }

        remove_indexes (idx, suppress);
    }
    return pick;
}

