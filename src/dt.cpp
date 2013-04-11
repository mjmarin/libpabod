#include <dt.h>


static void dtHelper (const std::vector<double>& src, int src_offset,
                      std::vector<double>& dst, int dst_offset,
                      std::vector<int>&    ptr, int ptr_offset,
                      int step,
                      int s1, int s2,
                      int d1, int d2,
                      double a, double b)
{
  if (d2 >= d1)
  {
    int d = (d1+d2) >> 1;
    int s = s1;

    for (int p = s1+1; p <= s2; p++)
    {
      if (src[src_offset + s*step] - a*squared(d-s) - b*(d-s) <
          src[src_offset + p*step] - a*squared(d-p) - b*(d-p))
      {
        s = p;
      }
    }

    dst[dst_offset + d*step] = src[src_offset + s*step] - a*squared(d-s) - b*(d-s);
    ptr[ptr_offset + d*step] = s;

    dtHelper (src, src_offset, dst, dst_offset, ptr, ptr_offset, step, s1, s, d1, d-1, a, b);
    dtHelper (src, src_offset, dst, dst_offset, ptr, ptr_offset, step, s, s2, d+1, d2, a, b);
  }
}


static void dt1d (std::vector<double>& src, int src_offset,
                  std::vector<double>& dst, int dst_offset,
                  std::vector<int>&    ptr, int ptr_offset,
                  int step, int n,
                  double a, double b)
{
  dtHelper (src, src_offset, dst, dst_offset, ptr, ptr_offset, step, 0, n-1, 0, n-1, a, b);
}


template <typename T>
static void print_vec(const std::vector<T>& v) {
  for(const auto& a : v) {
    std::cout << "  " << a << std::endl;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}


void dt (const CvMat* score, double d0, double d1, double d2, double d3,
         CvMat **scoreRet, CvMat** mIx, CvMat** mIy)
{
  int dims[2];
  getDimensions (score, dims);
  std::vector<double> vals = get_mat_data<double>(score);
  double ax = d0;
  double bx = d1;
  double ay = d2;
  double by = d3;

  createMatrix (2, dims, CV_64FC1, scoreRet);
  createMatrix (2, dims, CV_32SC1, mIx);
  createMatrix (2, dims, CV_32SC1, mIy);

  const int z = dims[0] * dims[1];
  std::vector<double> M (z);
  std::vector<int>    Ix(z);
  std::vector<int>    Iy(z);
  std::vector<double> tmpM (z);
  std::vector<int>    tmpIx(z);
  std::vector<int>    tmpIy(z);

  for (int x = 0; x < dims[1]; x++)
    dt1d (vals ,x*dims[0],
          tmpM ,x*dims[0],
          tmpIy,x*dims[0],
          1,
          dims[0],
          ay,
          by);

  for (int y = 0; y < dims[0]; y++)
    dt1d (tmpM ,y,
          M    ,y,
          tmpIx,y,
          dims[0],
          dims[1],
          ax,
          bx);

  int p;

  // Get argmins and adjust for matlab indexing from 1
  for (int x = 0; x < dims[1]; x++)
  {
    for (int y = 0; y < dims[0]; y++)
    {
      p = x*dims[0]+y;
      Ix[p] = tmpIx[p];
      const int tmpixp = tmpIx[p];
      Iy[p] = tmpIy[tmpixp*dims[0]+y];
    }
  }

  setMatData (*scoreRet, M.data());
  setMatData (*mIx, Ix.data());
  setMatData (*mIy, Iy.data());
}

