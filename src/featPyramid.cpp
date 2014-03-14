#include <featPyramid.h>

# define bzero(buf, bytes)      ((void) memset (buf, 0, bytes)) // mjmarin added to compile it on Windows

////////////////////////////
///// GLOBAL VARIABLES /////
////////////////////////////

// Unit vectors used to compute gradient orientation
float uu[9] =
{
  1.0000,
  0.9397,
  0.7660,
  0.500,
  0.1736,
  -0.1736,
  -0.5000,
  -0.7660,
  -0.9397
};

float vv[9] =
{
  0.0000,
  0.3420,
  0.6428,
  0.8660,
  0.9848,
  0.9848,
  0.8660,
  0.6428,
  0.3420
};



////////////////////////
///// CONSTRUCTORS /////
////////////////////////

FeatPyramid::FeatPyramid ()
{
//  _feat = NULL;
  _scales = NULL;
  _dim = -1;
  _imSize = NULL;
}

FeatPyramid::FeatPyramid (const IplImage *im, const Model *model, int padX, int padY)
{
  featpyramid (im, model, padX, padY);
}


FeatPyramid::~FeatPyramid()
{
	destroyFeatPyramid();
}


////////////////////////////
///// RESIZE FUNCTIONS /////
////////////////////////////

void FeatPyramid::alphacopy (float *src, float *dst, struct alphainfo *ofs, int n)
{
  struct alphainfo *end = ofs + n;

  while (ofs != end)
  {
    dst[ofs->di] += ofs->alpha * src[ofs->si];
    ofs++;
  }
}


void FeatPyramid::resize1dtran (float *src, int sheight, float *dst, int dheight, int width, int chan)
{
  float scale = (float)dheight/(float)sheight;
  float invscale = (float)sheight/(float)dheight;

  // Cache the interpolation values since they can be
  // shared among different columns
  int len = (int)ceil(dheight*invscale) + 2*dheight;
  assert (len > 0);
  alphainfo *ofs;
  ofs = new alphainfo[len];
  int k = 0;

	float fsy1;
	float fsy2;
	int sy1;
	int sy2;

  for (int dy = 0; dy < dheight; dy++)
  {
    fsy1 = dy * invscale;
    fsy2 = fsy1 + invscale;
    sy1 = (int)ceil(fsy1);
    sy2 = (int)floor(fsy2);

    if (sy1 - fsy1 > 1e-3)
    {
      assert(k < len);
      assert(sy1 >= 0);
      ofs[k].di = dy*width;
      ofs[k].si = sy1-1;
      ofs[k++].alpha = (sy1 - fsy1) * scale;
    }

    for (int sy = sy1; sy < sy2; sy++)
    {
      assert(k < len);
      assert(sy < sheight);
      ofs[k].di = dy*width;
      ofs[k].si = sy;
      ofs[k++].alpha = scale;
    }

    if (fsy2 - sy2 > 1e-3)
    {
      assert(k < len);
      assert(sy2 < sheight);
      ofs[k].di = dy*width;
      ofs[k].si = sy2;
      ofs[k++].alpha = (fsy2 - sy2) * scale;
    }
  }

  // Resize each column of each color channel
  bzero(dst, chan*width*dheight*sizeof(float));

	float *s;
	float *d;

  for (int c = 0; c < chan; c++)
  {
    for (int x = 0; x < width; x++)
    {
      s = src + c*width*sheight + x*sheight;
      d = dst + c*width*dheight + x;
      alphacopy(s, d, ofs, k);
    }
  }

  delete [] ofs;
}


IplImage* FeatPyramid::resize (const IplImage *mxsrc, const float mxscale)
{
  float *src = getImgData <float> (mxsrc);
  int sdims[3];
	getDimensions(mxsrc, sdims);
  float scale = mxscale;

  if (scale > 1.0)
    cout << "Invalid scaling factor" << endl;

  int ddims[3];
  ddims[0] = (int)round(sdims[0]*scale);
  ddims[1] = (int)round(sdims[1]*scale);
  ddims[2] = sdims[2];

  assert (ddims[0] > 0);
  assert (ddims[1] > 0);
  CvSize size = cvSize(ddims[1], ddims[0]);
  IplImage *mxdst = cvCreateImage (size, IPL_DEPTH_32F, 3);
  assert (mxdst != NULL);

  assert (ddims[2] > 0);
  float *dst = new float [ddims[0] * ddims[1] * ddims[2]];
  assert (dst != NULL);
  for (int l = 0; l < ddims[0] * ddims[1] * ddims[2]; l++)
    dst[l] = 0;

  assert (sdims[1] > 0);
  assert (sdims[2] > 0);
  float *tmp = new float [ddims[0] * sdims[1] * sdims[2]];
  assert (tmp != NULL);
  for (int l = 0; l < ddims[0] * sdims[1] * sdims[2]; l++)
    tmp[l] = 0;

  resize1dtran(src, sdims[0], tmp, ddims[0], sdims[1], sdims[2]);
  resize1dtran(tmp, sdims[1], dst, ddims[1], ddims[0], sdims[2]);

  setImgData (mxdst, dst);

	delete[] src;
  delete[] dst;
  delete[] tmp;

  return mxdst;
}


////////////////////////////////
///// GETPADDING FUNCTIONS /////
////////////////////////////////

int FeatPyramid::getPaddingX (const Model *m)
{
  return int(ceil((double)m->getMaxSize()[1]));
}

int FeatPyramid::getPaddingY (const Model *m)
{
  return int(ceil((double)m->getMaxSize()[0]));
}


/////////////////////////////////
/// FEATURESPYRAMID FUNCTIONS ///
/////////////////////////////////

CvMatND* FeatPyramid::process ( const IplImage *mximage, const float mxsbin )
{
//cout << "Init process pyramid" << endl;
  float *im = getImgData <float> (mximage);
  int dims[3];
	getDimensions (mximage, dims);

  if (dims[2] != 3)
    cout << "Invalid input" << endl;

  int sbin = (int) mxsbin;

  // memory for caching orientation histograms & their norms
  int blocks[2];
  blocks[0] = (int)round((float)dims[0]/(float)sbin);
  blocks[1] = (int)round((float)dims[1]/(float)sbin);

  assert (blocks[0] > 0);
  assert (blocks[1] > 0);

  float *hist = new float [blocks[0]*blocks[1]*18];

  for (int i = 0; i < blocks[0]*blocks[1]*18; i++)
    hist[i] = 0;

  float *norm = new float [blocks[0]*blocks[1]];

  for (int i = 0; i < blocks[0]*blocks[1]; i++)
    norm[i] = 0;

  // memory for HOG features
  int out[3];

  out[0] = max(blocks[0]-2, 0);
  out[1] = max(blocks[1]-2, 0);
  out[2] = 27+4+1;

  CvMatND *mxfeat;
//cout << "POS" << endl;
  if (out[0] == 0 && out[1] == 0 )
    mxfeat = NULL;

  else
  {
    assert (out[0] > 0);
    assert (out[1] > 0);

    //mxfeat = createNDMatrix (3, out, CV_64FC1);
		createMatrix (3, out, CV_64FC1, &mxfeat);
    assert (mxfeat != NULL);
//cout << "POS" << endl;
    //double *feat = new double [out[0] * out[1] * out[2]];
    float *feat = getMatNData <float> (mxfeat);
    assert (feat != NULL);
//cout << "POS" << endl;
    int visible[2];
    visible[0] = blocks[0]*sbin;
    visible[1] = blocks[1]*sbin;

		float *s;
		float dy, dx;
		float v;

		float dy2, dx2;
		float v2;

		float dy3, dx3;
		float v3;

		float best_dot = 0;
    int best_o = 0;

		float dot;

		float xp, yp;

    int ixp, iyp;
    float vx0, vy0, vx1, vy1;

		float *src1, *src2;
    float *dst, *end;

		float *dst2;
    float *src, *p, n1, n2, n3, n4;

		float t1 = 0;
    float t2 = 0;
    float t3 = 0;
    float t4 = 0;

		float h1, h2, h3, h4;
//cout << "POS-antes-sum" << endl;
//cout << *src << endl;
//cout << blocks[0] << " " << blocks[1] << endl;
		float sum = 0;//= *src + *(src + 9*blocks[0]*blocks[1]);
//cout << "POS-despues-sum" << endl;
    for (int x = 1; x < visible[1]-1; x++)
    {
      for (int y = 1; y < visible[0]-1; y++)
      {
        // first color channel
        s = im + min(x, dims[1]-2)*dims[0] + min(y, dims[0]-2);
        dy = *(s+1) - *(s-1);
        dx = *(s+dims[0]) - *(s-dims[0]);
        v = dx*dx + dy*dy;

        // second color channel
        s += dims[0]*dims[1];
        dy2 = *(s+1) - *(s-1);
        dx2 = *(s+dims[0]) - *(s-dims[0]);
        v2 = dx2*dx2 + dy2*dy2;

        // third color channel
        s += dims[0]*dims[1];
        dy3 = *(s+1) - *(s-1);
        dx3 = *(s+dims[0]) - *(s-dims[0]);
        v3 = dx3*dx3 + dy3*dy3;

        // pick channel with strongest gradient
        if (v2 > v)
        {
          v = v2;
          dx = dx2;
          dy = dy2;
        }

        if (v3 > v)
        {
          v = v3;
          dx = dx3;
          dy = dy3;
        }

        // snap to one of 18 orientations
        best_dot = 0;
        best_o = 0;

        for (int o = 0; o < 9; o++)
        {
          dot = uu[o]*dx + vv[o]*dy;
          if (dot > best_dot)
          {
            best_dot = dot;
            best_o = o;
          }

          else if (-dot > best_dot)
          {
            best_dot = -dot;
            best_o = o+9;
          }
        }

        // add to 4 histograms around pixel using linear interpolation
        xp = ((float)x+0.5)/(float)sbin - 0.5;
        yp = ((float)y+0.5)/(float)sbin - 0.5;

        ixp = (int)floor(xp);
        iyp = (int)floor(yp);
        vx0 = xp-ixp;
        vy0 = yp-iyp;
        vx1 = 1.0-vx0;
        vy1 = 1.0-vy0;
        v = sqrt(v);
//cout << "POS" << endl;
        if (ixp >= 0 && iyp >= 0)
        {
          *(hist + ixp*blocks[0] + iyp + best_o*blocks[0]*blocks[1]) += vx1*vy1*v;
        }

        if (ixp+1 < blocks[1] && iyp >= 0)
        {
          *(hist + (ixp+1)*blocks[0] + iyp + best_o*blocks[0]*blocks[1]) += vx0*vy1*v;
        }

        if (ixp >= 0 && iyp+1 < blocks[0])
        {
          *(hist + ixp*blocks[0] + (iyp+1) + best_o*blocks[0]*blocks[1]) += vx1*vy0*v;
        }

        if (ixp+1 < blocks[1] && iyp+1 < blocks[0])
        {
          *(hist + (ixp+1)*blocks[0] + (iyp+1) + best_o*blocks[0]*blocks[1]) += vx0*vy0*v;
        }
      }
    }
//cout << "POS" << endl;
    // compute energy in each block by summing over orientations
    for (int o = 0; o < 9; o++)
    {
      src1 = hist + o*blocks[0]*blocks[1];
      src2 = hist + (o+9)*blocks[0]*blocks[1];
      dst = norm;
      end = norm + blocks[1]*blocks[0];

      while (dst < end)
      {
        *(dst++) += (*src1 + *src2) * (*src1 + *src2);
        src1++;
        src2++;
      }
    }
//cout << "POS" << endl;
    // compute features
    for (int x = 0; x < out[1]; x++)
    {
      for (int y = 0; y < out[0]; y++)
      {
        dst2 = feat + x*out[0] + y;

        p = norm + (x+1)*blocks[0] + y+1;
        n1 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
        p = norm + (x+1)*blocks[0] + y;
        n2 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
        p = norm + x*blocks[0] + y+1;
        n3 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
        p = norm + x*blocks[0] + y;
        n4 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);

        t1 = 0;
        t2 = 0;
        t3 = 0;
        t4 = 0;

        // contrast-sensitive features
        src = hist + (x+1)*blocks[0] + (y+1);

        for (int o = 0; o < 18; o++)
        {
          h1 = min(*src * n1, 0.2);
          h2 = min(*src * n2, 0.2);
          h3 = min(*src * n3, 0.2);
          h4 = min(*src * n4, 0.2);
          *dst2 = 0.5 * (h1 + h2 + h3 + h4);
          t1 += h1;
          t2 += h2;
          t3 += h3;
          t4 += h4;
          dst2 += out[0]*out[1];
          src += blocks[0]*blocks[1];
        }

        // contrast-insensitive features
        src = hist + (x+1)*blocks[0] + (y+1);

        for (int o = 0; o < 9; o++)
        {
          sum = *src + *(src + 9*blocks[0]*blocks[1]);
          h1 = min(sum * n1, 0.2);
          h2 = min(sum * n2, 0.2);
          h3 = min(sum * n3, 0.2);
          h4 = min(sum * n4, 0.2);
          *dst2 = 0.5 * (h1 + h2 + h3 + h4);
          dst2 += out[0]*out[1];
          src += blocks[0]*blocks[1];
        }

        // texture features
        *dst2 = 0.2357 * t1;
        dst2 += out[0]*out[1];
        *dst2 = 0.2357 * t2;
        dst2 += out[0]*out[1];
        *dst2 = 0.2357 * t3;
        dst2 += out[0]*out[1];
        *dst2 = 0.2357 * t4;

        // truncation feature
        dst2 += out[0]*out[1];
        *dst2 = 0;
      }
    }

    delete[] hist;
    delete[] norm;
//cout << "POS" << endl;
    setMatNData (mxfeat, feat);

    delete[] feat;
  }
//cout << "POS" << endl;
	delete[] im;
//cout << " Fin featpyram" << endl;
  return mxfeat;
}

// mjmarin: this version works with cv::Mat
cv::Mat FeatPyramid::process2( const IplImage *mximage, const float mxsbin )
{
//cout << "Init process pyramid" << endl;
  float *im = getImgData <float> (mximage);
  int dims[3];
	getDimensions (mximage, dims);

  if (dims[2] != 3)
    cout << "Invalid input" << endl;

  int sbin = (int) mxsbin;

  // memory for caching orientation histograms & their norms
  int blocks[2];
  blocks[0] = (int)round((float)dims[0]/(float)sbin);
  blocks[1] = (int)round((float)dims[1]/(float)sbin);

  assert (blocks[0] > 0);
  assert (blocks[1] > 0);

  float *hist = new float [blocks[0]*blocks[1]*18];

  for (int i = 0; i < blocks[0]*blocks[1]*18; i++)
    hist[i] = 0;

  float *norm = new float [blocks[0]*blocks[1]];

  for (int i = 0; i < blocks[0]*blocks[1]; i++)
    norm[i] = 0;

  // memory for HOG features
  int out[3];

  out[0] = max(blocks[0]-2, 0);
  out[1] = max(blocks[1]-2, 0);
  out[2] = 27+4+1;

  //CvMatND *mxfeat;
  cv::Mat mxfeat;
//cout << "POS" << endl;
  if (out[0] == 0 && out[1] == 0 )
  {
    cout << "Empty matrix" << endl;
    mxfeat = 0.0;
  }
  else
  {
    assert (out[0] > 0);
    assert (out[1] > 0);

    //mxfeat = createNDMatrix (3, out, CV_64FC1);
		//createMatrix (3, out, CV_64FC1, &mxfeat);
	mxfeat.create(3, out, CV_64FC1);
    //assert (mxfeat != NULL);
	assert (!mxfeat.empty());
//cout << "POS" << endl;
    //double *feat = new double [out[0] * out[1] * out[2]];
    //float *feat = getMatNData <float> (mxfeat);
	float *feat = new float [out[0] * out[1] * out[2]];
    assert (feat != NULL);
//cout << "POS" << endl;
    int visible[2];
    visible[0] = blocks[0]*sbin;
    visible[1] = blocks[1]*sbin;

		float *s;
		float dy, dx;
		float v;

		float dy2, dx2;
		float v2;

		float dy3, dx3;
		float v3;

		float best_dot = 0;
    int best_o = 0;

		float dot;

		float xp, yp;

    int ixp, iyp;
    float vx0, vy0, vx1, vy1;

		float *src1, *src2;
    float *dst, *end;

		float *dst2;
    float *src, *p, n1, n2, n3, n4;

		float t1 = 0;
    float t2 = 0;
    float t3 = 0;
    float t4 = 0;

		double h1, h2, h3, h4;
//
//
//
		float sum = 0;//
//
    for (int x = 1; x < visible[1]-1; x++)
    {
      for (int y = 1; y < visible[0]-1; y++)
      {
        // first color channel
        s = im + min(x, dims[1]-2)*dims[0] + min(y, dims[0]-2);
        dy = *(s+1) - *(s-1);
        dx = *(s+dims[0]) - *(s-dims[0]);
        v = dx*dx + dy*dy;

        // second color channel
        s += dims[0]*dims[1];
        dy2 = *(s+1) - *(s-1);
        dx2 = *(s+dims[0]) - *(s-dims[0]);
        v2 = dx2*dx2 + dy2*dy2;

        // third color channel
        s += dims[0]*dims[1];
        dy3 = *(s+1) - *(s-1);
        dx3 = *(s+dims[0]) - *(s-dims[0]);
        v3 = dx3*dx3 + dy3*dy3;

        // pick channel with strongest gradient
        if (v2 > v)
        {
          v = v2;
          dx = dx2;
          dy = dy2;
        }

        if (v3 > v)
        {
          v = v3;
          dx = dx3;
          dy = dy3;
        }

        // snap to one of 18 orientations
        best_dot = 0;
        best_o = 0;

        for (int o = 0; o < 9; o++)
        {
          dot = uu[o]*dx + vv[o]*dy;
          if (dot > best_dot)
          {
            best_dot = dot;
            best_o = o;
          }

          else if (-dot > best_dot)
          {
            best_dot = -dot;
            best_o = o+9;
          }
        }

        // add to 4 histograms around pixel using linear interpolation
        xp = ((float)x+0.5)/(float)sbin - 0.5;
        yp = ((float)y+0.5)/(float)sbin - 0.5;

        ixp = (int)floor(xp);
        iyp = (int)floor(yp);
        vx0 = xp-ixp;
        vy0 = yp-iyp;
        vx1 = 1.0-vx0;
        vy1 = 1.0-vy0;
        v = sqrt(v);
//cout << "POS" << endl;
        if (ixp >= 0 && iyp >= 0)
        {
          *(hist + ixp*blocks[0] + iyp + best_o*blocks[0]*blocks[1]) += vx1*vy1*v;
        }

        if (ixp+1 < blocks[1] && iyp >= 0)
        {
          *(hist + (ixp+1)*blocks[0] + iyp + best_o*blocks[0]*blocks[1]) += vx0*vy1*v;
        }

        if (ixp >= 0 && iyp+1 < blocks[0])
        {
          *(hist + ixp*blocks[0] + (iyp+1) + best_o*blocks[0]*blocks[1]) += vx1*vy0*v;
        }

        if (ixp+1 < blocks[1] && iyp+1 < blocks[0])
        {
          *(hist + (ixp+1)*blocks[0] + (iyp+1) + best_o*blocks[0]*blocks[1]) += vx0*vy0*v;
        }
      }
    }
//cout << "POS" << endl;
    // compute energy in each block by summing over orientations
    for (int o = 0; o < 9; o++)
    {
      src1 = hist + o*blocks[0]*blocks[1];
      src2 = hist + (o+9)*blocks[0]*blocks[1];
      dst = norm;
      end = norm + blocks[1]*blocks[0];

      while (dst < end)
      {
        *(dst++) += (*src1 + *src2) * (*src1 + *src2);
        src1++;
        src2++;
      }
    }
//cout << "POS" << endl;
    // compute features
    for (int x = 0; x < out[1]; x++)
    {
      for (int y = 0; y < out[0]; y++)
      {
        dst2 = feat + x*out[0] + y;

        p = norm + (x+1)*blocks[0] + y+1;
        n1 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
        p = norm + (x+1)*blocks[0] + y;
        n2 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
        p = norm + x*blocks[0] + y+1;
        n3 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
        p = norm + x*blocks[0] + y;
        n4 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);

        t1 = 0;
        t2 = 0;
        t3 = 0;
        t4 = 0;

        // contrast-sensitive features
        src = hist + (x+1)*blocks[0] + (y+1);

        for (int o = 0; o < 18; o++)
        {
          h1 = min(*src * n1, 0.2);
          h2 = min(*src * n2, 0.2);
          h3 = min(*src * n3, 0.2);
          h4 = min(*src * n4, 0.2);
          *dst2 = 0.5 * (h1 + h2 + h3 + h4);
          t1 += h1;
          t2 += h2;
          t3 += h3;
          t4 += h4;
          dst2 += out[0]*out[1];
          src += blocks[0]*blocks[1];
        }

        // contrast-insensitive features
        src = hist + (x+1)*blocks[0] + (y+1);

        for (int o = 0; o < 9; o++)
        {
          sum = *src + *(src + 9*blocks[0]*blocks[1]);
          h1 = min(sum * n1, 0.2);
          h2 = min(sum * n2, 0.2);
          h3 = min(sum * n3, 0.2);
          h4 = min(sum * n4, 0.2);
          *dst2 = 0.5 * (h1 + h2 + h3 + h4);
          dst2 += out[0]*out[1];
          src += blocks[0]*blocks[1];
        }

        // texture features
        *dst2 = 0.2357 * t1;
        dst2 += out[0]*out[1];
        *dst2 = 0.2357 * t2;
        dst2 += out[0]*out[1];
        *dst2 = 0.2357 * t3;
        dst2 += out[0]*out[1];
        *dst2 = 0.2357 * t4;

        // truncation feature
        dst2 += out[0]*out[1];
        *dst2 = 0;
      }
    }

    delete[] hist;
    delete[] norm;
//
    setMatNData (mxfeat, feat);

    delete[] feat;
  }
//
	delete[] im;
//
  return mxfeat;
}

CvMatND* FeatPyramid::padArray (CvMatND *mat, int dimPad[3], float val)
{
  int dims[3];

  if (mat == NULL)
  {
    for (int i = 0; i < 2; i++)
      dims[i] = dimPad[i] * 2;

    dims[2] = 32 + (dimPad[2] * 2);
  }

  else
  {
    for (int i = 0; i < 3; i++)
      dims[i] = mat->dim[i].size + (dimPad[i] * 2);
  }

  // New bigger matrix is created
  assert (dims[0] > 0);
  assert (dims[1] > 0);
  assert (dims[2] > 0);

  //CvMatND *padMat = createNDMatrix (3, dims, CV_64FC1);
	CvMatND *padMat;
	createMatrix (3, dims, CV_64FC1, &padMat);
  assert (padMat != NULL);

  // Firstable, all the elements are setted to val
  for ( int i = 0; i < padMat->dim[0].size; i++ )
    for ( int j = 0; j < padMat->dim[1].size; j++ )
      for ( int k = 0; k < padMat->dim[2].size; k++ )
        cvSetReal3D (padMat, i, j, k, val);

  if (mat != NULL)
  {
    // Original values are setted
    for ( int i = dimPad[0]; i < padMat->dim[0].size - dimPad[0]; i++ )
      for ( int j = dimPad[1]; j < padMat->dim[1].size - dimPad[1]; j++ )
        for ( int k = dimPad[2]; k < padMat->dim[2].size - dimPad[2]; k++ )
          cvSetReal3D (padMat, i, j, k, cvGetReal3D (mat, i-dimPad[0], j-dimPad[1], k-dimPad[2]));
  }

  return padMat;
}


void FeatPyramid::featpyramid (const IplImage *im, const Model *model, int padX, int padY)
{
  float sbin, interval;
  float sc;
  int imsize[2];
  float maxScale;
  IplImage *imAux = NULL;
  int pad[3];

  if (padX == -1 && padY == -1)
  {
    padX = getPaddingX(model);
    padY = getPaddingY(model);
  }

  sbin = (float)model->getSbin();
  interval = (float) model->getInterval()+1.0;
  sc = pow (2, 1/(interval));
  imsize[0] = im->height;
  imsize[1] = im->width;
  maxScale = 1 + floor ( log ( min (imsize[0], imsize[1]) /
  (5*sbin) ) / log(sc) );

  // It is the number of elements that will contain pyramid->features
  // and pyramid->scales. At less must be 2*interval
  if ( (maxScale + interval) < (2*interval) )
    setDim (int(2*interval));

  else
    setDim (int(maxScale + interval));

  assert (getDim() > 0);
//  _feat = new CvMatND* [getDim()];    // Suspicious
  _feat.reserve(getDim());
  for (int i = 0; i < getDim(); i++) // Pre-allocate memory
	  _feat.push_back(cv::Mat());
//  assert (_feat != NULL);
  assert(!_feat.empty());
  _scales = new float [getDim()]; // Suspicious
  assert (_scales != NULL);

  // Field imsize is setted
  assert (imsize[0] > 0);
  assert (imsize[1] > 0);

  setImSize (imsize);
//cout << "Antes de bucle featpyramid" << endl;
  for (int i = 0; i < interval; i++)
  {
    // Image is resized
    imAux = resize (im, (1/pow(sc, i)));

    // "First" 2x interval
    //setFeat(process(imAux, sbin/2), i);
	setFeat(process2(imAux, sbin/2), i);
    setScales(2/pow(sc, i), i);

    // "Second" 2x interval
    //setFeat (process (imAux, sbin), (int)(i+interval));
	setFeat (process2 (imAux, sbin), (int)(i+interval));
    setScales (1/pow(sc, i), (int)(i+interval));

    // Remaining intervals
    IplImage *imAux2; // mjmarin added
    for (int j = (int)(i+interval); j < (int)maxScale; j = j+(int)interval)
    {
      // mjmarin: memory leak fixed
      //imAux = resize (imAux, 0.5); // Old sentence
      imAux2 = resize (imAux, 0.5);
      cvReleaseImage(&imAux);
      imAux = imAux2;

      //setFeat (process (imAux, sbin), (int)(j+interval));
	  setFeat (process2 (imAux, sbin), (int)(j+interval));
      setScales ((float)(0.5 * getScales()[j]), (int)(j+interval));
    }

    // mjmarin: more release needed for imAux
    cvReleaseImage(&imAux);
  }
  // Second loop
  for (int i = 0; i < getDim(); i++ )
  {
    // Add 1 to padding because feature generation deletes a 1-cell
    // Wide border around the feature map
    pad[0] = padY + 1;
    pad[1] = padX + 1;
    pad[2] = 0;    
    
    //CvMatND* tmpfeat = getFeat()[i];
	cv::Mat tmpfeat = getFeat()[i];
	CvMatND tmpND = tmpfeat;
    //CvMatND* tmppad = padArray (tmpfeat, pad, 0);
	CvMatND* tmppad = padArray (&tmpND, pad, 0);
    setFeat (tmppad, i);
    //cvReleaseMatND(&tmpfeat); // mjmarin: commented out since it should be auto released with use of cv::Mat
    

    // Write boundary occlusion feature
	cv::Mat fMat = getFeat()[i];
    for (int j = 0; j <= padY; j++)
      //for (int k = 0; k < getFeat()[i]->dim[1].size; k++)
	  for (int k = 0; k < fMat.size.p[1]; k++)
        //cvSetReal3D (getFeat()[i], j, k, 31, 1);
		fMat.at<double>(j, k, 31) = 1;

    //for (int j = getFeat()[i]->dim[0].size - padY -1; j < getFeat()[i]->dim[0].size; j++)
	for (int j = fMat.size.p[0] - padY -1; j < fMat.size.p[0]; j++)
      //for (int k = 0; k < getFeat()[i]->dim[1].size; k++)
	  for (int k = 0; k < fMat.size.p[1]; k++)
        //cvSetReal3D (getFeat()[i], j, k, 31, 1);
		fMat.at<double>(j, k, 31) = 1;

    //for (int j = 0; j < getFeat()[i]->dim[0].size; j++)
	for (int j = 0; j < fMat.size.p[0]; j++)
      for (int k = 0; k <= padX; k++)
        //cvSetReal3D (getFeat()[i], j, k, 31, 1);
		fMat.at<double>(j, k, 31) = 1;

    //for (int j = 0; j < getFeat()[i]->dim[0].size; j++)
	for (int j = 0; j < fMat.size.p[0]; j++)
      //for (int k = getFeat()[i]->dim[1].size - padX - 1; k < getFeat()[i]->dim[1].size; k++)
	  for (int k = fMat.size.p[1] - padX - 1; k < fMat.size.p[1]; k++)
        //cvSetReal3D (getFeat()[i], j, k, 31, 1);
		fMat.at<double>(j, k, 31) = 1;
  }

  setPadX (padX);
  setPadY (padY);
}



void FeatPyramid::destroyFeatPyramid()
{
	//if (_feat != NULL)
	if (!_feat.empty())
	{
		/*
		for (int i = 0; i < getDim(); i++)
		{
			if (_feat[i] != NULL)
			{
				cvReleaseMatND (&(_feat[i]));
				_feat[i] = NULL;
			}
		}

		delete[] _feat;
		_feat = NULL;
		*/
		_feat.clear();
	}

	if (_scales != NULL)
	{
		delete[] _scales;
		_scales = NULL;
	}

   if (_imSize != NULL)
   {
      delete[] _imSize;
      _imSize = NULL;
   }
}

/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////


void FeatPyramid::setImSize(int *imSize)
{
  _imSize = new int [PAIR];  // Suspicious

  assert (imSize[0] > 0);
  assert (imSize[1] > 0);

	//_imSize = imSize;
  for (int i=0; i<PAIR; i++)
      _imSize[i] = imSize[i];
}



