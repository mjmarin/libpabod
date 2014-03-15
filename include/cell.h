#ifndef _CELL_H_
#define _CELL_H_

#include <vector>
#include <readTypes.h>
#include <handlerOpenCVStructs.h>


/** \file
 *  This file contains the definition of <tt>Cell</tt> class. This class is
 *  an auxiliar part of class <tt>Model</tt>, and is used to load the 
 *  field <tt>rules</tt> of .mat model file 
 */  

/** \def INVALID_STR 
 *  This define is used to indicate a structure is invalid, which means their 
 *  fields are not considered.
 */
#define INVALID_STR   0


/** \def STR_DEF
 *  This define is used to indicate a structure type is 1, which means the 
 *  structure is scalar (only 1 element) and the field <tt>def</tt> is 
 *  considered.
 */
#define STR_DEF   1


/** \def STR_ANCHOR
 *  This define is used to indicate a structure type is 2, which means the 
 *  structure is an array (more than 1 elements) and the field <tt>anchor</tt> 
 *  is considered.
 */
#define STR_ANCHOR   2


//////////////////////////////
// OFFSET STRUCT DEFINITION //
//////////////////////////////

/** \struct offset
 *  This struct contains the definition of <tt>_offset</tt> private variable. 
 *  It is considered for for Cells of type both <tt>STR_DEF</tt> and 
 *  <tt>STR_ANCHOR</tt>.
 *  \sa _offset
 */
typedef struct offset
{
  float w;
  int blocklabel;

  offset()
  {
     w = 0.0;
     blocklabel = 0;
  }
} offset;


//////////////////////////////
// ANCHOR STRUCT DEFINITION //
//////////////////////////////

/** \struct anchor
 *  This struct contains the definition of <tt>_anchor</tt> vector private 
 *  variable. This variable is considered for Cells of type <tt>STR_ANCHOR</tt>.
 *  \sa _anchor
 */
typedef struct anchor
{
  int* array;
  int dim;

  anchor()
  {
	  dim = 0;
	  array = NULL;
  }
} anchor;


///////////////////////////
// DEF STRUCT DEFINITION //
///////////////////////////

/** \struct def
 *  This struct contains the definition of <tt>_def</tt> private variable. It is 
 *  considered for Cells of type <tt>STR_DEF</tt>.
 *  \sa _def
 */
typedef struct def
{
  double w[4];
  int blocklabel;
  bool flip;
  char symmetric;

  //! Constructor
  def()
  {
     w[0]=w[1]=w[2]=w[3] = 0.0;
     blocklabel = 0;
     flip = false;
     symmetric = '\0';
  }

} def;


/** \class Cell
 *  The class implements methods and variables to store a scalar element of 
 *  type Rules. This element has to be one of next three types:
 *  \li <b> Invalid element </b> - when an invalid element is read, this will 
 *  represent setting <tt>_flagStr</tt> private variable to <tt>INVALID_STR</tt>. 
 *  The rest of parameters will not be considered.
 *  \li <b> Structure type 1 </b> - a structure of type 1 has the last field 
 *  called <tt>def</tt>. In addition, all the structures of this type are 
 *  usually scalars. By this reason, they will represent setting <tt>_flagStr</tt>
 *  to <tt>STR_DEF</tt>. The variable <tt>_rhs</tt> will be treated as a scalar 
 *  (only the first element), and the rest of variables will be used too, except 
 *  <tt>_anchor</tt>.
 *  \li <b> Structure type 2 </b> - a structure of type 2 has the last field 
 *  called <tt>anchor</tt>. In addition, the structure of this type is an array 
 *  of structures. By this reason, it will represent setting <tt>_flatStr</tt> 
 *  to <tt>STR_ANCHOR</tt>. The variable <tt>_rhs</tt> will be whole considered, 
 *  and the rest of variables will be used too, except <tt>_def</tt>.
 *  \sa INVALID_STR
 *  \sa STR_DEF
 *  \sa STR_ANCHOR
 *  \sa _flagStr
 */

class Cell
{
public:

//////////////////////////////////////
///// CONSTRUCTORS & DESTRUCTORS /////
//////////////////////////////////////

/** Empty constructor. Calls the function <tt>loadEmptyCell</tt> to generate
 *  a <tt>Cell</tt> invalid object.
 *  \sa loadEmptyCell
 */
  Cell ();


/** Parametrized constructor. Reads the variable <tt>matVar</tt> using 
 *  <tt>loadCell</tt> function, and initialize the class.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - is a default parameter. If is not specified, it value is 0. When 
 *  it is different to 0, the type of structure is 2. It is an array, so 
 *  <tt>i</tt> indicates the position of the current structure
 *  \sa loadCell
 */
  Cell (matvar_t *matVar, int i = 0);


/** Destructor. Release all the memory allocated using function 
 *  <tt>destroyCell</tt>
 *  \sa destroyCell
 */
  ~Cell ();


/** Is called by the destructor to release all the memory allocated and set
 *  variables to invalid values
*/
  void destroyCell ();

  void releaseScore(void);
  void releaseIxIy(void);



///////////////////////////////
///// LOAD FILE FUNCTIONS /////
///////////////////////////////

/** The function sets each field of the cell structure to invalid values,
 *  dimension indicators to -1 and arrays to <tt>NULL</tt>. Set the field
 *  <tt>_flagStr</tt> to value <tt>INVALID_STR</tt>
 */
  void loadEmptyCell ();


/** The function loads each field of the cell structure to each private 
 *  variable of the class, depending of the type of structure. To do this, use 
 *  several specific functions, one for each field/variable. Steps to load a 
 *  <tt>Cell</tt> are described on next diagram:
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - is a default parameter. If is not specified, it value is 0. 
 *  When it is different to 0, the type of structure is 2. Structure 2 is an 
 *  array, so <tt>i</tt> indicates the position of the current structure
 *  \sa  loadType
 *  \sa  loadLhs
 *  \sa  loadRhs
 *  \sa  loadDetwindow
 *  \sa  loadI
 *  \sa  loadOffset
 *  \sa   loadAnchor
 *  \sa  loadDef
 */
  void loadCell (matvar_t *matVar, int i);


/** The function loads the field <tt>type</tt> of cell into private variable 
 *  <tt>_type</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - when it is different to 0, the type of structure is 2. 
 *  <tt>i</tt> indicates the position of the current structure
 *  \sa _type
 */
  void loadType (matvar_t *matVar, int i);


/** The function load the field <tt>lhs</tt> of cell into private variable 
 *  <tt>_lhs</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - when it is different to 0, the type of structure is 2. 
 *  <tt>i</tt> indicates the position of the current structure
 *  \sa _lhs
 */
  void loadLhs (matvar_t *matVar, int i);


/** The function load the field <tt>rhs</tt> of cell into private variable 
 *  <tt>_rhs</tt>. If the parameter <tt>scalar</tt> is <tt>true</tt>, only
 *  the first value will be loaded. Otherwise, all the read elements will 
 *  be loaded.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - when it is different to 0, the type of structure is 2. 
 *  <tt>i</tt> indicates the position of the current structure
 *  \param scalar - is a default parameter. If it is not specified, its 
 *  value is true. Indicates if <tt>_rhs</tt> if a scalar value (only a single
 *  int data, <tt>scalar = true</tt>) or an array variable (<tt>scalar = 
 *  false</tt>).
 *  \sa _rhs
 *  \sa _rhsDim
 */
  void loadRhs (matvar_t *matVar, int i, bool scalar = true);


/** The function load the field <tt>detwindows</tt> of cell into private 
 *  variable <tt>_detwindows</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - when it is different to 0, the type of structure is 2. 
 *  <tt>i</tt> indicates the position of the current structure
 *  \sa _detwindow
 */
  void loadDetwindow (matvar_t *matVar, int i);


/** The function load the field <tt>i</tt> of cell into private variable 
 *  <tt>_i</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - when it is different to 0, the type of structure is 2.
 *  <tt>i</tt> indicates the position of the current structure
 *  \sa _i
 */
  void loadI (matvar_t *matVar, int i);


/** The function load the field <tt>offset</tt> of cell into private struct 
 *  variable <tt>_offset</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - when it is different to 0, the type of structure is 2.
 *  <tt>i</tt> indicates the position of the current structure
 *  \sa _offset
 */
  void loadOffset (matvar_t *matVar, int i);


/** The function load the field <tt>anchor</tt> of cell into private struct 
 *  variable <tt>_anchor</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - indicates the position of the current structure
 *  \note This function will be used only for type structure 2
 *  \sa _anchor
 */
  void loadAnchor (matvar_t *matVar, int i);


/** The function load the field <tt>def</tt> of cell into private struct 
 *  variable <tt>_def</tt>.
 *  \param matVar - it contains the whole scalar cell structure
 *  \param i - indicates the position of the current structure
 *  \note This function will be used only for type structure 1
 *  \sa _def
 */
  void loadDef (matvar_t *matVar, int i);


/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

/** Returns the value of private variable <tt>_flagStr</tt>.
 *  \return The integer value of <tt>_flagStr</tt>
 *  \sa _flagStr
 */
  int getFlagStr () const  {return _flagStr;}


/** Set the value of private variable <tt>_flagStr</tt> to <tt>n</tt>.
 *  \param n - new value for the variable <tt>_flagStr</tt>
 *  \sa _flagStr
 */
  void setFlagStr (int n)  {_flagStr = n;}


/** Returns the value of private variable <tt>_type</tt>.
 *  \return The char value of <tt>_type</tt>
 *  \sa _type
 */
  char getType() const  {return _type;}


/** Set the value of private variable <tt>_type</tt> to <tt>t</tt>.
 *  \param t - new value for the variable <tt>_type</tt>
 *  \sa _type
 */
  void setType (char t) {_type =  t;}


/** Returns the value of private variable <tt>_lhs</tt>.
 *  \return The double value of <tt>_lhs</tt>
 *  \sa _lhs
 */
  int getLhs () const {return _lhs;}


/** Set the value of private variable <tt>_lhs</tt> to <tt>lhs</tt>.
 *  \param lhs - new value for the variable <tt>_lhs</tt>
 *  \sa _lhs
 */
  void setLhs (int lhs)  {_lhs = lhs;}


/** Returns the value of private variable <tt>_rhs</tt>.
 *  \return The int array <tt>_rhs</tt>
 *  \sa _rhs
 *  \sa _rhsDim
 */
  int* getRhs () const  {return _rhs;}


/** Set the value of private variable <tt>_rhs</tt> to <tt>rhs</tt>.
 *  \param rhs - new array of values for the variable <tt>_rhs</tt>
 *  \sa _rhs
 *  \sa _rhsDim
 */
  void setRhs (int *rhs);


/** Returns the value of private variable <tt>_rhsDim</tt>
 *  \return The int value <tt>_rhsDim</tt>, which indicates <tt>_rhs</tt> 
 *  array size.
 *  \sa _rhs
 *  \sa _rhsDim
 */
  int getRhsDim () const  {return _rhsDim;}


/** Set the value of private variable <tt>_rhsDim</tt> to <tt>dim</tt>.
 *  \param dim - new value for the variable <tt>_rhsDim</tt>
 *  \sa _rhs
 *  \sa _rhsDim
 */
  void setRhsDim (int dim)
  {
    assert (dim > 0);
    _rhsDim = dim;
  }


/** Returns the value of private variable <tt>_detwindow</tt>.
 *  \return The int value of <tt>_detwindow</tt>
 *  \sa _detwindow
 *  \sa PAIR
 */
  int* getDetwindow () const  {return _detwindow;}


/** Set the value of private variable <tt>_detwindow</tt> to <tt>d</tt>.
 *  \param d - new array of values for the variable <tt>_detwindow</tt>
 *  \sa _detwindow
 *  \sa PAIR
 */
  void setDetwindow (double *d);


/** Returns the constant value of define <tt>PAIR</tt>
 *  \return The int constant <tt>PAIR</tt>, which indicates 
 *  <tt>_detwindow</tt> array size (a pair of values)
 *  \sa _detwindow
 *  \sa PAIR
 */
  int getDetwindowDim () const  {return PAIR;}


/** Returns the value of private variable <tt>_i</tt>.
 *  \return The int value of <tt>_i</tt>
 *  \sa _i
 */
  int getI () const  {return _i;}


/** Set the value of private variable <tt>_i</tt> to <tt>i</tt>.
 *  \param i - new value for the variable <tt>_i</tt>
 *  \sa _i
 */
  void setI (int i)  {_i = i;}


/** Returns the value of private struct variable <tt>_offset</tt>.
 *  \return An offset structure, which is the value of <tt>_offset</tt>
 *  \sa _offset
 */
  offset getOffset() const {return _offset;}
  offset& getOffset() {return _offset;}


/** Set the value of private struct variable <tt>_offset</tt> to <tt>o</tt>.
 *  \param o - new value for the struct variable <tt>_offset</tt>
 *  \sa _offset
 */
  void setOffset (offset o)  {_offset = o;}


/** Returns the value of private struct variable <tt>_anchor</tt>.
 *  \return An anchor structure, which is the value of <tt>_anchor</tt>
 *  \note This private variable only will have a value when the type of 
 *  structure is 2
 *  \sa _anchor
 *  \sa _anchorDim
 *  \sa _flagStr
 */
  anchor* getAnchor () const  {return _anchor;}


/** Set the value of private struct variable <tt>_anchor</tt> to <tt>a</tt>.
 *  \param a - new value for the struct variable <tt>_anchor</tt>
 *  \sa _anchor
 *  \sa _anchorDim
 *  \sa _flagStr
 */
  void setAnchor (anchor *a);


/** Returns the value of private variable <tt>_anchorDim</tt>
 *  \return The int value <tt>_anchorDim</tt>, which indicates 
 *  <tt>_anchor</tt> array size.
 *  \sa _anchor
 *  \sa _anchorDim
 *  \sa _flagStr
 */
  int getAnchorDim () const  {return _anchorDim;}


/** Set the value of private variable <tt>_anchorDim</tt> to <tt>dim</tt>.
 *  \param dim - new value for the variable <tt>_anchorDim</tt>
 *  \sa _anchor
 *  \sa _anchorDim
 *  \sa _flagStr
 */
  void setAnchorDim (int dim)
  {
    assert (dim > 0);
    _anchorDim = dim;
  }


/** Returns the value of private struct variable <tt>_def</tt>.
 *  \return An def structure, which is the value of <tt>_def</tt>
 *  \note This private variable only will have a value when the type of 
 *  structure is 1
 *  \sa _def
 *  \sa _flagStr
 */
  def getDef () const  {return _def;}
  def& getDef () {return _def;}


/** Set the value of private struct variable <tt>_def</tt> to <tt>d</tt>.
 *  \param d - new value for the struct variable <tt>_def</tt>
 *  \sa _def
 *  \sa _flagStr
 */
  void setDef (def d)  {_def = d;}

/** Returns the value of private variable <tt>_score</tt>.
 *  \return The CvMat array <tt>_score</tt>
 *  \sa _score
 *  \sa _scoreDim
 */
  //CvMat** getScore () const  {return _score;}
  const vectorMat & getScore () const  {return _score;}


/** Set the value of private variable <tt>_score</tt> to <tt>score</tt>.
 *  \param score - new array of matrixes for the variable <tt>_score</tt>
 *  \sa _score
 *  \sa _scoreDim
 */
  //void setScore (CvMat **score);
  void setScore (vectorMat & score);


/** Returns the value of private variable <tt>_scoreDim</tt>
 *  \return The int value <tt>_scoreDim</tt>, which indicates <tt>_score</tt> 
 *  array size.
 *  \sa _score
 *  \sa _scoreDim
 */
  int getScoreDim () const  {return _scoreDim;}


/** Set the value of private variable <tt>_scoreDim</tt> to <tt>dim</tt>.
 *  \param dim - new value for the variable <tt>_scoreDim</tt>
 *  \sa _score
 *  \sa _scoreDim
 */
  void setScoreDim (int dim)
  {
    assert (dim > 0);
    _scoreDim = dim;
  }

/** Returns the value of private variable <tt>_Ix</tt>.
 *  \return The CvMat array <tt>_Ix</tt>
 *  \sa _Ix
 *  \sa _IxDim
 */
  //CvMat** getIx () const  {return _Ix;}
  const vectorMat & getIx () const  {return _Ix;}


/** Set the value of private variable <tt>_Ix</tt> to <tt>Ix</tt>.
 *  \param Ix - new array of matrixes for the variable <tt>_Ix</tt>
 *  \sa _Ix
 *  \sa _IxDim
 */
  //void setIx (CvMat **Ix);
  void setIx (vectorMat &Ix);

  void addIxItem(cv::Mat &Ix){_Ix.push_back(Ix);};
  void addIyItem(cv::Mat &Iy){_Iy.push_back(Iy);};
  
  void addIxItem(CvMat * Ix){_Ix.push_back(Ix);};
  void addIyItem(CvMat * Iy){_Iy.push_back(Iy);};

/** Returns the value of private variable <tt>_IxDim</tt>
 *  \return The int value <tt>_IxDim</tt>, which indicates <tt>_Ix</tt> 
 *  array size.
 *  \sa _Ix
 *  \sa _IxDim
 */
  int getIxDim () const  {return _IxDim;}


/** Set the value of private variable <tt>_IxDim</tt> to <tt>dim</tt>.
 *  \param dim - new value for the variable <tt>_IxDim</tt>
 *  \sa _Ix
 *  \sa _IxDim
 */
  void setIxDim (int dim)
  {
    assert (dim > 0);
    _IxDim = dim;
  }


/** Returns the value of private variable <tt>_Iy</tt>.
 *  \return The CvMat array <tt>_Iy</tt>
 *  \sa _Iy
 *  \sa _IyDim
 */
  //CvMat** getIy () const  {return _Iy;}
  const vectorMat & getIy () const  {return _Iy;}


/** Set the value of private variable <tt>_Iy</tt> to <tt>Iy</tt>.
 *  \param Iy - new array of matrixes for the variable <tt>_Iy</tt>
 *  \sa _Iy
 *  \sa _IyDim
 */
  //void setIy (CvMat **Iy);
  void setIy (vectorMat & Iy);


/** Returns the value of private variable <tt>_IyDim</tt>
 *  \return The int value <tt>_IyDim</tt>, which indicates <tt>_Iy</tt> 
 *  array size.
 *  \sa _Iy
 *  \sa _IyDim
 */
  int getIyDim () const  {return _IyDim;}


/** Set the value of private variable <tt>_IyDim</tt> to <tt>dim</tt>.
 *  \param dim - new value for the variable <tt>_IyDim</tt>
 *  \sa _Iy
 *  \sa _IyDim
 */
  void setIyDim (int dim)
  {
    assert (dim > 0);
    _IyDim = dim;
  }


////////////////////////
///// INITIALIZERS /////
////////////////////////

/** Set each field of private struct variable <tt>_offset</tt> to it 
 *  corresponding value of <tt>offsetStructure</tt>
 *  \param offsetStructure - it contains a whole struct offset
 */
  void initializeOffset (matvar_t *offsetStructure);


/** Set each field of private struct variable <tt>_anchor</tt> to it 
 *  corresponding value of <tt>anchorStructure</tt>
 *  \param anchorStructure - it contains a whole struct anchor
 */
  void initializeAnchor (matvar_t *anchorStructure);


/** Set each field of private struct variable <tt>_def</tt> to it
 *  corresponding value of <tt>defStructure</tt>
 *  \param defStructure - it contains a whole struct def
 */
  void initializeDef (matvar_t *defStructure);


private:

/** Is an int private variable. Represents the type of structure. It 
 *  has to be one of the next three:
 *  \li <tt>INVALID_STR</tt> (0), represents an invalid character. No 
 *  other private variable is considered.
 *  \li <tt>STR_DEF</tt> (1), represents the type structure 1, whose 
 *  last field is called <tt>def</tt>.
 *  \li <tt>STR_ANCHOR</tt> (2), represents the type structure 2, 
 *  whose last field is called <tt>anchor</tt>.
 */
  int _flagStr;


/** Is a char private variable
 */
  char _type;


/** Is an int private variable
 */
  int _lhs;


/** Is an int array private variable. Depending of <tt>_flagStr</tt>'s 
 *  value, it can be:
 *  \li <tt>_flagStr = STR_DEF</tt>, only the first element of the 
 *  array will be considered.
 *  \li <tt>_flagStr = STR_ANCHOR</tt>, the number of elements is higher
 *  than 1 (value contained in <tt>_rhsDim</tt>).
 *  \sa _rhsDim
 */
  int* _rhs;


/** Is an int private variable. Indicates the size of <tt>_rhs</tt> 
 *  array.
 *  \sa _rhs
 */
  int _rhsDim;


/** Is an int array private variable
 */
  int* _detwindow;


/** Is an int private variable
 */
  int _i;


/** Is an offset struct private variable
 *  \sa offset
 */
  offset _offset;


/** Is an anchor struct private variable. Only considered if 
 *  <tt>_flagStr</tt> = 2.
 *  \sa _anchorDim
 *  \sa _flagStr
 */
  anchor* _anchor;


/** Is an int variable. Indicates the size of <tt>_anchor</tt> array.
 *  \sa _anchor
 */
  int _anchorDim;


/** Is a def struct private variable. Only considered if 
 *  <tt>_flagStr</tt> = 1.
 *  \sa _flagStr
 */
  def _def;


/** Is a CvMat* array private variable.
 *  \sa _scoreDim
 */
  //CvMat** _score;
  vectorMat _score;


/** Is an int private variable. Indicates the size of
 *  <tt>_score</tt> array.
 *  \sa _score
 */
  int _scoreDim;


/** Is a CvMat* array private variable.
 *  \sa _IxDim
 */
  //CvMat** _Ix;
  vectorMat _Ix;


/** Is an int private variable. Indicates the size of 
 *  <tt>_Ix</tt> array.
 *  \sa _Ix
 */
  int _IxDim;


/** Is a CvMat* array private variable.
 *  \sa _IyDim
 */
  //CvMat** _Iy;
  vectorMat _Iy;


/** Is an int private variable. Indicates the size of 
 *  <tt>_Iy</tt> array.
 *  \sa _Iy
 */
  int _IyDim;
};



#endif

