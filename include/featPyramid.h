#ifndef _FEATPYRAMID_H_
#define _FEATPYRAMID_H_

#include <model.h>


/** \file
 *  This file contains the definition of <tt>FeatPyramid</tt> class. This
 *  class is used to generate the features pyramid of the image.
 */


/** \def eps
 *  It is a small value, used to avoid division by zero
 */
#define eps 0.0001

/** \def round(x)
 *  Is a macro to faster round a real number to an integer
 */
#define round(x) (((x)-floor(x))>=(0.5)?(long)(ceil(x)):(long)(floor(x)))


/////////////////////////////////
// ALPHAINFO STRUCT DEFINITION //
/////////////////////////////////

/** \struct alphainfo
 *  The struct is used to copy interpolated values in the function
 *  <tt>alphacopy</tt>
 */
struct alphainfo 
{
  int si;
  int di;
  float alpha;

  alphainfo()
  {
     si = di = 0;
     alpha = 0.0;
  }
};


/** \class FeatPyramid
 *  The class implements the generation of the features pyramid of the image.
 *  A features pyramid consists on a set of images in different sizes. This 
 *  images will be used to compare the regions of interest with the model, 
 *  and find the correct scale.
 */

class FeatPyramid
{
public:

  //////////////////////////////////////
  ///// CONSTRUCTORS & DESTRUCTORS /////
  //////////////////////////////////////

/** Empty constructor. Sets the main attributes of the class to invalid of 
 *  <tt>NULL</tt> values.
 *  \sa _feat
 *  \sa _scales
 *  \sa _dim
 *  \sa _imSize
 */
  FeatPyramid ();

/** Parametrized constructor. Generate the features pyramid by calling the
 *  function <tt>featpyramid</tt>.
 *  \param im - is the image used to build the features pyramid
 *  \param model - is the object model used to get the HOG, used
 *  in the building of the features pyramid
 *  \param padX - default parameter. If it is not specified, the 
 *  <tt>padX</tt> value given by the model will be used.
 *  \param padY - default parameter. If it is not specified, the 
 *  <tt>padY</tt> value given by the model will be used.
 *  \sa featpyramid
 */
  FeatPyramid (const IplImage *im, const Model *model, 
               int padX = -1, 
               int padY = -1);

/** Destructor. Release all the allocated memory and set variables of the 
 *  class to invalid values calling the function <tt>destroyFeatPyramid</tt>.
 *  \sa destroyFeatPyramid
 */
  ~FeatPyramid ();


  //////////////////////////////////
  ///// FEATPYRAMID GENERATION /////
  //////////////////////////////////

/** This function is used as a step to build the features pyramid. Is used
 *  to generate a fast subsampling (resizing) of the image.
 *  \param src - is the source values to copy
 *  \param dst - is the destiny vector where the values will be copied
 *  \param ofs - is a vector of structs <tt>alphacopy</tt> used to iterate
 *  \param n - indicates the offset on the <tt>ofs</tt> vector
 */
  void alphacopy(float *src, float *dst, struct alphainfo *ofs, int n);


/** The function resizes along one dimension (columns). The result is a 
 *  transposed matrix, so applied twice will result a complete resize of
 *  the image.
 *  \param src - is a vector containing all the values of the original 
 *  (source) image
 *  \param sheight - is used to define the scale of the result image
 *  \param dst - is a vector that will containg all the values of the
 *  result (destiny) image
 *  \param dheight - is used to define the scale of the result image
 *  \param width - is the width (number of columns) of the image
 *  \param chan - is the number of channels of the image. If it is a 
 *  color image, this number will be three
 */
  void resize1dtran(float *src, int sheight, float *dst, int dheight, 
                    int width, int chan);


/** This is the main resizing function. Calls <tt>alphacopy</tt> and 
 *  <tt>resize1tran</tt> several times to generate the resized image.
 *  \param mxsrc - is the original (source) image, which will be
 *  resized
 *  \param mxscale - is the scale factor of the result image (always
 *  will be lower to 1)
 *  \return An <tt>IplImage</tt> struct is returned. Is the result
 *  image of the resizing operation
 */
  IplImage *resize(const IplImage *mxsrc, const float mxscale);


/** This function returns the second element of field 
 *  <tt>maxSize</tt> of the object <tt>Model</tt> used in the
 *  detection operation. This value means the padding in x
 *  axis, and this is used to detect partially visible objects.
 *  \param m - is the model used in the detection
 */
  int getPaddingX (const Model *m);


/** This function returns the first element of field 
 *  <tt>maxSize</tt> of the object <tt>Model</tt> used in the
 *  detection operation. This value means the padding in y
 *  axis, and this is used to detect partially visible objects.
 *  \param m - is the model used in the detection
 */
  int getPaddingY (const Model *m);


/** Is the main features pyramid generation function. Takes the
 *  given image <tt>mximage</tt> and generate the HOG (Histogram
 *  of Oriented Gradients). This HOG of the object is used to
 *  search it on the image
 *  \param mximage - is the image used to generate the HOG
 *  \param mxsbin - is a parameter which indicates a size. It comes
 *  from the model used (field <tt>sbin</tt>)
 */
  CvMatND *process(const IplImage *mximage, const float mxsbin);

  cv::Mat process2( const IplImage *mximage, const float mxsbin );

/** The function pad the mat array with so many values <tt>val</tt>
 *  like indicates the vector <tt>dimPad</tt>. Each element means
 *  one dimension.
 *  \param mat - is the matrix to be padded
 *  \param dimPad - is the vector which contains the padding 
 *  dimension. Each element is referring to one dimension
 *  \param val - is the value used to fill the new padded elements
 *  \return The padded matrix is returned. The dimension will be
 *  the original dimension of <tt>mat</tt> + <tt>dimPad</tt>. The
 *  new elements will be filled with value <tt>val</tt>
 */
  CvMatND *padArray (CvMatND *mat, int dimPad[3], float val);


/** The funciton creates the whole features pyramid of the image
 *  <tt>im</tt> using the model <tt>model</tt> using functions 
 *  like <tt>resize</tt> and <tt>process</tt>, and assign the 
 *  results to the attibutes of the class.
 *  \param im - is the image used to build the features pyramid
 *  \param model - is the object model used to get the HOG, used
 *  in the building of the features pyramid
 *  \param padX - default parameter. If it is not specified, the 
 *  <tt>padX</tt> value given by the model will be used.
 *  \param padY - default parameter. If it is not specified, the 
 *  <tt>padY</tt> value given by the model will be used.
 *  
 */
  void featpyramid (const IplImage *im, const Model *model, int padX, int padY);


/** The function release all the allocated memory and set to
 *  invalid values the released variables.
 */
	void destroyFeatPyramid();

  /////////////////////////////
  ///// SETTERS & GETTERS /////
  /////////////////////////////

/** Returns the value of private variable <tt>_feat</tt>.
 *  \return The CvMatND vector, which is the value of
 *  <tt>_feat</tt>
 *  \note Inline function
 */
  //CvMatND** getFeat() const	{return _feat;}
	vectorMat const & getFeat(void) const	{return _feat;}


/** Sets the value of private variable <tt>_feat</tt> to
 *  <tt>feat</tt>.
 *  \param feat - new value for the variable <tt>_feat</tt>
 *  \note Inline function
 */
  /*
	void setFeat (CvMatND** feat)
	{
		if (feat != NULL)
			_feat = feat;
	}
*/
  	void setFeat (vectorMat & feat)
	{		
		if (!_feat.empty())
			_feat.clear();

		_feat = feat;
	}

	void addFeatItem(cv::Mat & f)
	{
		_feat.push_back(f);
	}

/** Sets the value of element <tt>i</tt> of private vector variable
 *  <tt>_feat</tt> to <tt>feat</tt>.
 *  \param feat - new value for the element <tt>i</tt> of private
 *  variable <tt>_feat</tt>
 *  \param i - indicates the index of the element in the vector
 */
  void setFeat (CvMatND* feat, int i)
  {
    assert (i < getDim());
    _feat[i] = feat;
  }

  void setFeat (const cv::Mat & feat, int i)
  {
    assert (i < getDim());
    _feat[i] = feat;
  }


/** Returns the value of private variable <tt>_scales</tt>.
 *  \return The float vector, which is the value of
 *  <tt>_scales</tt>
 *  \note Inline function
 */
  float* getScales() const	{return _scales;}


/** Sets the value of private variable <tt>_scales</tt> to
 *  <tt>scales</tt>.
 *  \param scales - new value for the variable <tt>_scales</tt>
 *  \note Inline function
 */
  void setScales (float *scales)
	{
		if (scales != NULL)
			_scales = scales;
	}


/** Sets the value of element <tt>i</tt> of private vector variable
 *  <tt>_scales</tt> to <tt>scales</tt>.
 *  \param scales - new value for the element <tt>i</tt> of private
 *  variable <tt>_scales</tt>
 *  \param i - indicates the index of the element in the vector
 */
  void setScales (float scales, int i)
  {
    assert (i < getDim());
    _scales[i] = scales;
  }


/** Returns the value of private variable <tt>_dim</tt>.
 *  \return The dimension of both <tt>_feat</tt> and
 *  <tt>_scales</tt> vectors
 *  \note Inline function
 */
  int getDim() const	{return _dim;}


/** Sets the value o private variable <tt>_dim</tt> to 
 *  <tt>dim</tt>.
 *  \param dim - the new value. It means the dimension 
 *  of both vectors <tt>_feat</tt> and <tt>_scales</tt>
 *  \note Inline function
 */
  void setDim (int dim)
  {
    assert (dim > 0);
    _dim = dim;
  }


/** Returns the value of private variable <tt>_imSize</tt>.
 *  \return The integer value of <tt>_imSize</tt>
 *  \note Inline function
 */
  int* getImSize() const	{return _imSize;}


/** Sets the value of private variable <tt>_imSize</tt> 
 *  to <tt>imSize</tt>.
 *  \param imSize - new value for the variable 
 *  <tt>_imSize</tt>
 */
  void setImSize (int *imSize);


/** Returns the value of private variable <tt>_padX</tt>
 *  \return The integer value of <tt>_padX</tt>
 *  \note Inine function
 */
  int getPadX() const	{return _padX;}


/** Sets the value of private variable <tt>_padX</tt> 
 *  to <tt>padX</tt>.
 *  \param padX - new value for the variable 
 *  <tt>_padX</tt>
 *  \note Inline function
 */
  void setPadX (int padX)	{_padX = padX;}


/** Returns the value of private variable <tt>_padY</tt>
 *  \return The integer value of <tt>_padY</tt>
 *  \note Inine function
 */
  int getPadY() const	{return _padY;}


/** Sets the value of private variable <tt>_padY</tt> 
 *  to <tt>padY</tt>.
 *  \param padY - new value for the variable 
 *  <tt>_padY</tt>
 *  \note Inline function
 */
  void setPadY (int padY)	{_padY = padY;}


private:

  //CvMatND **_feat;
  vectorMat _feat; //! Computed features

  float *_scales;

  int _dim;

  int *_imSize;

  int _padX;

  int _padY;
};

#endif

