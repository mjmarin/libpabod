#include <cv.h>

/** \file
 *  This file contains the definition of <tt>Image</tt> class. This class is
 *  used to easier handling images and pixels accessing
 */


///////////////////////////////
//////   RGBPIXELUCHAR   //////
////// STRUCT DEFINITION //////
///////////////////////////////

/** \struct RgbPixelUChar
 *  Defines an struct whose elements are <tt>unsigned char</tt> type.
 */
typedef struct
{
  unsigned char b, g, r;
} RgbPixelUChar;


///////////////////////////////
//////   RGBPIXELFLOAT   //////
////// STRUCT DEFINITION //////
///////////////////////////////

/** \struct RgbPixelFloat
 *  Defines an struct whose elements are <tt>float</tt> type.
 */
typedef struct
{
  float b, g, r;
} RgbPixelFloat;


///////////////////////////////
//////  RGBPIXELDOUBLE   //////
////// STRUCT DEFINITION //////
///////////////////////////////

/** \struct RgbPixelDouble
 *  Defines an struct whose elements are <tt>double</tt> type.
 */
typedef struct
{
  double b, g, r;
} RgbPixelDouble;



/** \class Image
 *  The class implements structs and operators to simplify and optimize 
 *  pixels accessing and basics operations. Structs implemented contains a
 *  specific kind of image (coloured, gray...). This class is very helpful to
 *  handling type of values which represents colors on images of type
 *  <tt>IplImages</tt>
 */

template <class T> class Image
{
  public:

/** Parametrized constructor. Sets the image <tt>imgp</tt> to <tt>img</tt>
 *  \param img - is a default parameter. If it is not defined, the private 
 *  attribute <tt>imgp</tt> is set to <tt>NULL</tt>. Otherwise, is set to
 *  <tt>img</tt>.
 *  \note Inline function
 */
    Image(IplImage* img = 0) {imgp = img;}


/** Destructor. Sets to an invalid value the private attibute <tt>imgp</tt>. 
 *  The destructor does not release any possible memory allocation.
 *  \note Inline function
 */
    ~Image()  {imgp = 0;}


/** Equal operator. Copy the header and all the contend of parameter 
 *  <tt>img</tt> into private attribute <tt>imgp</tt>.
 *  \note Inline function
 */
    void operator= (IplImage* img)  {imgp = img;}
  

/** Access operator. Returns the whole row of the attribute image <tt>imgp</tt>, 
 *  indicated by the parameter <tt>rowIndx</tt>. This operation is done on a
 *  effiient way, handling it with data pointers of the struct <tt>IplImage</tt>
 *  \param rowIndx - indicates the row of the inner matrix representation of the
 *  image <tt>imgp</tt>
 *  \return The complete row <tt>rowIndx</tt> of the attribute <tt>imgp</tt> is
 *  returned. The type of elements depends on <tt>IplImage</tt> type.
 *  \note Inline function
 */
    inline T* operator[](const int rowIndx)  
    {return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}


  private:

/** Represents the core of the class, a <tt>IplImage</tt> image
 */
    IplImage* imgp;
};



/** \typedef RgbImageUChar
 *  Defines an <tt>Image</tt> object with three channels, and whose elements on 
 *  inner representation are <tt>unsigned char</tt> type.
 */
typedef Image <RgbPixelUChar>  RgbImageUChar;


/** \typedef RgbImageFloat
 *  Defines an <tt>Image</tt> object with three channels, and whose elements on 
 *  inner representation are <tt>float</tt> type.
 */
typedef Image <RgbPixelFloat>  RgbImageFloat;


/** \typedef RgbImageDouble
 *  Defines an <tt>Image</tt> object with three channels, and whose elements on 
 *  inner representation are <tt>double</tt> type.
 */
typedef Image <RgbPixelDouble>  RgbImageDouble;


/** \typedef BwImage
 *  Defines an <tt>Image</tt> object with one channel (grayscale), and whose 
 *  elements on inner representation are <tt>unsigned char</tt> type.
 */
typedef Image <unsigned char>  BwImage;


/** \typedef BwImageFloat
 *  Defines an <tt>Image</tt> object with one channel (grayscale), and whose 
 *  elements on inner representation are <tt>float</tt> type.
 */
typedef Image <float>      BwImageFloat;



