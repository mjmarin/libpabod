#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <cell.h>
#include <iostream>

using namespace std;

/** \file
 *  This file contains the definition of <tt>Model</tt> class. This class 
 *  is used to load a the model structure stored on a .mat file.
 */


/** \def INVALID
 *  Is used on struct <tt>symbols</tt>, field <tt>filter</tt>. When the value 
 *  read from the file is none or invalid, this value will be set 
 */
#define INVALID -9999


///////////////////////////////
// FILTERS STRUCT DEFINITION //
///////////////////////////////

/** \struct filters
 *  The struct contains the definition of <tt>_filters</tt> vector private 
 *  variable. Every field will be loaded from the file read.
 *  \sa _filters
 */
typedef struct filters
{
  CvMatND *w;
  int blocklabel;
  char symmetric;
  int *size;
  int sizeDim;
  bool flip;
  int symbol;

  filters()
  {
     w = NULL;
     blocklabel = 0;
     symmetric = '\0';
     size = NULL;
     sizeDim = 0;
     flip = false;
     symbol = 0;
  }

} filters;

/////////////////////////////
// RULES STRUCT DEFINITION //
/////////////////////////////

/** \struct rules
 *  The struct contains the definition of <tt>_rules</tt> vector private 
 *  variable. The field <tt>n</tt> contains the number of objects <tt>Cell</tt>
 *  stored in the structure. The other field, <tt>structure</tt>, is the vector
 *  of <tt>n</tt> elements of the <tt>Cell</tt> class.
 *  \sa _rules
 */
typedef struct rules
{
  int n;
  Cell *structure;

  rules()
  {
    n=0;
    structure = NULL;
  }

} rules;

///////////////////////////////
// SYMBOLS STRUCT DEFINITION //
///////////////////////////////

/** \struct symbols
 *  The struct contains the definition of <tt>_symbols</tt> vector private 
 *  variable. Every field will be loaded from the file read.
 *  \sa _score
 */
typedef struct symbols
{
  char type;
  int i;
  int filter;
  CvMat **score;
  int dimScore;

  symbols()
  {
     type = '\0';
     i = 0;
     filter = 0;
	  score = NULL;
	  dimScore = 0;
  }

} symbols;

///////////////////////////////////
// LOWERBOUNDS STRUCT DEFINITION //
///////////////////////////////////

/** \struct lowerbounds
 *  The struct contains the definition of <tt>_lowerbounds</tt> vector private 
 *  variable. Every field will be loaded from the file read.
 *  \sa _lowerbounds
 */
typedef struct lowerbounds
{
  float *v;
  int f;
  int c;

  lowerbounds()
  {
	  v = NULL;
     f = 0;
     c = 0;
  }

} lowerbounds;

////////////////////////////////
// BBOXPRED STRUCT DEFINITION //
////////////////////////////////

/** \struct bboxpred
 *  The struct contains the definition of <tt>_bboxpred</tt> vector private 
 *  variable. Every field will be loaded from the file read.
 *  \sa _bboxpred
 */
typedef struct bboxpred
{
  float *x1;
  float *y1;
  float *x2;
  float *y2;
  int dim;

  bboxpred()
  {
     x1 = x2 = y1 = y2 = NULL;
	  dim = 0;
  }
} bboxpred;


/** \class Model
 *  The class implements methods and variables to load a model from a .mat
 *  file. The parametrized constructor reads field by field the file and 
 *  set each the value of each struct and variable 
 */

class Model
{
public:

//////////////////////////////////////
///// CONSTRUCTORS & DESTRUCTORS /////
//////////////////////////////////////

/** Empty constructor. Sets the value of each private variable to an invalid 
 *  value or <tt>NULL</tt> in case of pointers. Calls the function 
 *  <tt>loadEmptyModel</tt> for that purpose.
 *  \sa loadEmptyModel
 */
  Model ();

/** Parametrized constructor. Reads the file <tt>fileName</tt> and initialize 
 *  the class private variables calling the function <tt>loadFile</tt>.
 *  \param fileName - it contains the name of the file to read
 *  \sa loadFile
 */
  Model (string fileName);

/** Destructor. Releases all the allocated memory for each private variable of
 *  the class and set the values to invalid. Calls the function 
 *  <tt>destroyModel</tt> which handles this.
 *  \sa destroyModel
 */
  ~Model ();


///////////////////////////////
///// LOAD FILE FUNCTIONS /////
///////////////////////////////


/** The function sets each field of the class to an invalid value
 */
  void loadEmptyModel ();  

/** The function loads each field of the file to each private variable of the 
 *  class. To do this, use several specific functions, one for each field
 *  variable.
 *  \param fileName - it contains the name of the file to read
 *  \sa loadClass
 *  \sa loadYear
 *  \sa loadNote
 *  \sa loadFilters
 *  \sa loadRules
 *  \sa loadSymbols
 *  \sa loadNumFilters
 *  \sa loadNumBlocks
 *  \sa loadNumSymbols
 *  \sa loadBlockSizes
 *  \sa loadStart
 *  \sa loadMaxSize
 *  \sa loadMinSize
 *  \sa loadInterval
 *  \sa loadSbin
 *  \sa loadThresh
 *  \sa loadRegmult
 *  \sa loadLearnmult
 *  \sa loadLowerbounds
 *  \sa loadFusage
 *  \sa loadBboxpred
 */
  void loadFile (string fileName);


/** The function loads the field <tt>class</tt> from file into private
 *  variable <tt>_class</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _class
 */
  void loadClass (matvar_t *matVar);


/** The function loads the field <tt>year</tt> from file into private
 *  variable <tt>_year</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _year
 */
  void loadYear (matvar_t *matVar);


/** The function loads the field <tt>note</tt> from file into private
 * variable <tt>_note</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _note
 */
  void loadNote (matvar_t *matVar );


/** The function loads the field <tt>filters</tt> from file into private
 *  struct variable <tt>_filters</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa filters
 *  \sa _filters
 */
  void loadFilters (matvar_t *matVar );


/** The function loads the field <tt>rules</tt> from file into private
 *  struct variable <tt>_rules</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa rules
 *  \sa _rules
 *  \sa Cell
 */
  void loadRules (matvar_t *matVar );


/** The function loads the field <tt>symbols</tt> from file into private
 *  struct variable <tt>_symbols</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa symbols
 *  \sa _symbols
 */
  void loadSymbols (matvar_t *matVar );


/** The function loads the field <tt>note</tt> from file into private
 *  variable <tt>_note</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _note
 */
  void loadNumFilters (matvar_t *matVar );


/** The function loads the field <tt>numfilters</tt> from file into private
 *  variable <tt>_numFilters</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _numFilters
 */
  void loadNumBlocks (matvar_t *matVar );


/** The function loads the field <tt>numsymbols</tt> from file into private
 *  variable <tt>_numSymbols</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _numSymbols
 */
  void loadNumSymbols (matvar_t *matVar );


/** The function loads the field <tt>blocksizes</tt> from file into private
 *  variable <tt>_blockSizes</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _blockSizes
 */
  void loadBlockSizes (matvar_t *matVar );


/** The function loads the field <tt>start</tt> from file into private
 *  variable <tt>_start</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _start
 */
  void loadStart (matvar_t *matVar );


/** The function loads the field <tt>maxsize</tt> from file into private
 *  variable <tt>_maxSize</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _maxSize
 */
  void loadMaxSize (matvar_t *matVar );


/** The function loads the field <tt>minsize</tt> from file into private
 *  variable <tt>_minSize</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _minSize
 */
  void loadMinSize (matvar_t *matVar );


/** The function loads the field <tt>interval</tt> from file into private
 *  variable <tt>_interval</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _interval
 */
  void loadInterval (matvar_t *matVar );


/** The function loads the field <tt>sbin</tt> from file into private
 *  variable <tt>_sbin</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _sbin
 */
  void loadSbin (matvar_t *matVar );


/** The function loads the field <tt>thresh</tt> from file into private
 *  variable <tt>_thresh</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _thresh
 */
  void loadThresh (matvar_t *matVar );


/** The function loads the field <tt>regmult</tt> from file into private
 *  variable <tt>_regmult</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _regmult
 */
  void loadRegmult (matvar_t *matVar );


/** The function loads the field <tt>learnmult</tt> from file into private
 *  variable <tt>_learnmult</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _learnmult
 */
  void loadLearnmult (matvar_t *matVar );


/** The function loads the field <tt>lowerbounds</tt> from file into private
 *  struct variable <tt>_lowerbounds</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa lowerbounds
 *  \sa _lowerbounds
 */
  void loadLowerbounds (matvar_t *matVar );


/** The function loads the field <tt>fusage</tt> from file into private
 *  variable <tt>_fusage</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa _fusage
 */
  void loadFusage (matvar_t *matVar );


/** The function loads the field <tt>bboxpred</tt> from file into private
 *  variable <tt>_bboxpred</tt>.
 *  \param matVar - it contains the whole struct model
 *  \sa bboxpred
 *  \sa _bboxpred
 */
  void loadBboxpred (matvar_t *matVar );



/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

/** Returns the value of private variable <tt>_class</tt>.
 *  \return The string value of <tt>_class</tt>
 *  \note Inline function
 */
  string getClass () const  {return _class;}


/** Sets the value of private variable <tt>_class</tt> to <tt>str</tt>.
 *  \param str - new value for the variable <tt>_class</tt>
 *  \note Inline function
 */
  void setClass (string str)  {_class = str;}


/** Returns the value of private variable <tt>_class</tt>.
 *  \return The string value of <tt>_class</tt>
 *  \note Inline function
 */
  string getYear () const  {return _year;}


/** Sets the value of private variable <tt>_year</tt> to <tt>str</tt>.
 *  \param str - new value for the variable <tt>_year</tt>
 *  \note Inline function
 */
  void setYear (string str)  {_year = str;}


/** Returns the value of private variable <tt>_note</tt>.
 *  \return The string value of <tt>_note</tt>
 *  \note Inline function
 */
  string getNote () const  {return _note;}


/** Sets the value of private variable <tt>_note</tt> to <tt>str</tt>.
 *  \param str - new value for the variable <tt>_note</tt>
 *  \note Inline function
 */
  void setNote (string str)  {_note = str;}


/** Returns the value of private variable <tt>_filters</tt>.
 *  \return A filters structure, which is the value of <tt>_filters</tt>
 *  \note Inline function
 */
  filters* getFilters () const  {return _filters;}


/** Sets the value of private struct variable <tt>_filters</tt> to <tt>f</tt>.
 *  \param f - new value for the struct variable <tt>_filters</tt>
 */
  void setFilters (filters *f);


/** Returns the value of auxiliar private variable <tt>_filtersDim</tt>.
 *  \return The length of the vector <tt>_filters</tt>
 *  \note Inline function
 */
  int getFiltersDim () const {return _filtersDim;}


/** Sets the value of auxiliar private variable <tt>_filtersDim</tt> to <tt>
 *  dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _filters</tt>
 *  \note Inline function
 */
  void setFiltersDim (int dim)  {_filtersDim = dim;}


/** Returns the value of private variable <tt>_rules</tt>.
 *  \return The dimension of <tt>_rules</tt>
 *  \note Inline function
 */
  rules* getRules () const  {return _rules;}


/** Sets the value of private struct variable <tt>_rules</tt> to <tt>r</tt>.
 *  \param r - new value for the struct variable <tt>_rules</tt>
 */
  void setRules (rules *r);


/** Returns the value of private variable <tt>_rulesDim</tt>
 *  \return The int value <tt>_rulesDim</tt>, which indicates <tt>_rules</tt> 
 *  array size.
 *  \sa _rules
 *  \sa _rulesDim
 */
  int getRulesDim () const  {return _rulesDim;}

/** Sets the value of auxiliar private variable <tt>_rulesDim</tt> to <tt>
 *  dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _rules</tt>
 *  \note Inline function
 */
  void setRulesDim (int dim)  {_rulesDim = dim;}

/** Returns the value of private variable <tt>_symbols</tt>.
 *  \return A symbols structure, which is the value of <tt>_symbols</tt>
 *  \note Inline function
 */
  symbols* getSymbols () const  {return _symbols;}


/** Sets the value of private struct variable <tt>_symbols</tt> to <tt>s</tt>.
 *  \param s - new value for the struct variable <tt>_symbols</tt>
 */
  void setSymbols (symbols *s);


/** Returns the value of private variable <tt>_symbols</tt>.
 *  \return The dimension of <tt>_symbols</tt>
 *  \note Inline function
 */
  int getSymbolsDim () const  {return _symbolsDim;}

/** Sets the value of auxiliar private variable <tt>_symbolsDim</tt> to <tt>
 *  dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _symbols</tt>
 *  \note Inline function
 */
  void setSymbolsDim (int dim)  {_symbolsDim = dim;}

/** Returns the value of private variable <tt>_numFilters</tt>.
 *  \return The integer value of <tt>_numFilters</tt>
 *  \note Inline function
 */
  int getNumFilters () const  {return _numFilters;}


/** Sets the value of private variable <tt>_numFilters</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_numFilters</tt>
 *  \note Inline function
 */
  void setNumFilters (int d)  {_numFilters = d;}


/** Returns the value of private variable <tt>_numBlocks</tt>.
 *  \return The integer value of <tt>_numBlocks</tt>
 *  \note Inline function
 */
  int getNumBlocks () const  {return _numBlocks;}


/** Sets the value of private variable <tt>_numBlocks</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_numBlocks</tt>
 *  \note Inline function
 */
  void setNumBlocks (int d)  {_numBlocks = d;}


/** Returns the value of private variable <tt>_numSymbols</tt>.
 *  \return The integer value of <tt>_numSymbols</tt>
 *  \note Inline function
 */
  int getNumSymbols () const  {return _numSymbols;}


/** Sets the value of private variable <tt>_numSymbols</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_numSymbols</tt>
 *  \note Inline function
 */
  void setNumSymbols (int d)  {_numSymbols = d;}


/** Returns the value of private variable <tt>_blockSizes</tt>.
 *  \return The integer value of <tt>_blockSizes</tt>
 *  \note Inline function
 */
  int* getBlockSizes () const  {return _blockSizes;}


/** Sets the value of private variable <tt>_blockSizes</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_blockSizes</tt>
 */
  void setBlockSizes (int *d);


/** Returns the value of private variable <tt>_blockSize</tt>.
 *  \return The dimension of <tt>_blockSize</tt>
 *  \note Inline function
 */
  int getBlockSizesDim () const  {return _blockSizesDim;}

/** Sets the value of auxiliar private variable <tt>_blockSizesDim</tt> to 
 *  <tt>dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _blockSizes</tt>
 *  \note Inline function
 */
  void setBlockSizesDim (int dim)  {_blockSizesDim = dim;}
 
/** Returns the value of private variable <tt>_start</tt>.
 *  \return The integer value of <tt>_start</tt>
 *  \note Inline function
 */
  int getStart () const  {return _start;}


/** Sets the value of private variable <tt>_start</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_start</tt>
 *  \note Inline function
 */
  void setStart (int d)  {_start = d;}


/** Returns the value of private variable <tt>_maxSize</tt>.
 *  \return The integer value of <tt>_maxSize</tt>
 *  \note Inline function
 */
  int* getMaxSize () const  {return _maxSize;}


/** Sets the value of private variable <tt>_maxSize</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_maxSize</tt>
 */
  void setMaxSize (int *d);


/** Returns the value of private variable <tt>_maxSize</tt>.
 *  \return The dimension of <tt>_maxSize</tt>
 *  \note Inline function
 */
  int getMaxSizeDim () const  {return PAIR;}


/** Returns the value of private variable <tt>_minSize</tt>.
 *  \return The integer value of <tt>_minSize</tt>
 *  \note Inline function
 */
  int* getMinSize () const  {return _minSize;}


/** Sets the value of private variable <tt>_minSize</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_minSize</tt>
 */
  void setMinSize (int *d);


/** Returns the value of private variable <tt>_minSize</tt>.
 *  \return The dimension of <tt>_minSize</tt>
 *  \note Inline function
 */
  int getMinSizeDim () const  {return PAIR;}


/** Returns the value of private variable <tt>_interval</tt>.
 *  \return The integer value of <tt>_interval</tt>
 *  \note Inline function
 */
  int getInterval () const  {return _interval;}


/** Sets the value of private variable <tt>_interval</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_interval</tt>
 *  \note Inline function
 */
  void setInterval (int d)  {_interval = d;}


/** Returns the value of private variable <tt>_sbin</tt>.
 *  \return The integer value of <tt>_sbin</tt>
 *  \note Inline function
 */
  int getSbin () const  {return _sbin;}


/** Sets the value of private variable <tt>_sbin</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_sbin</tt>
 */
  void setSbin (int d)  {_sbin = d;}


/** Returns the value of private variable <tt>_thresh</tt>.
 *  \return The double value of <tt>_thresh</tt>
 *  \note Inline function
 */
  float getThresh () const  {return _thresh;}


/** Sets the value of private variable <tt>_thresh</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_thresh</tt>
 *  \note Inline function
 */
  void setThresh (float d)  {_thresh = d;}


/** Returns the value of private variable <tt>_regmult</tt>.
 *  \return The integer value of <tt>_regmult</tt>
 *  \note Inline function
 */
  int* getRegmult () const  {return _regmult;}


/** Sets the value of private variable <tt>_regmult</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_regmult</tt>
 */
  void setRegmult (int *d);


/** Returns the value of private variable <tt>_regmult</tt>.
 *  \return The dimension of <tt>_regmult</tt>
 *  \note Inline function
 */
  int getRegmultDim () const  {return _regmultDim;}

/** Sets the value of auxiliar private variable <tt>_regmultDim</tt> to <tt>
 *  dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _regmult</tt>
 *  \note Inline function
 */
  void setRegmultDim (int dim)  {_regmultDim = dim;}

/** Returns the value of private variable <tt>_learnmult</tt>.
 *  \return The float value of <tt>_learnmult</tt>
 *  \note Inline function
 */
  float* getLearnmult () const  {return _learnmult;}


/** Sets the value of private variable <tt>_learnmult</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_learnmult</tt>
 */
  void setLearnmult (float *d);


/** Returns the value of private variable <tt>_learnmult</tt>.
 *  \return The dimension of <tt>_learnmult</tt>
 *  \note Inline function
 */
  int getLearnmultDim () const  {return _learnmultDim;}


/** Sets the value of auxiliar private variable <tt>_learnmultDim</tt> to <tt>
 *  dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _learnmult</tt>
 *  \note Inline function
 */
  void setLearnmultDim (int dim)  {_learnmultDim = dim;}

/** Returns the value of private variable <tt>_lowerbounds</tt>.
 *  \return A lowerbounds structure, which is the value of <tt>_lowerbounds</tt>
 *  \note Inline function
 */
  lowerbounds* getLowerbounds () const  {return _lowerbounds;}


/** Sets the value of private struct variable <tt>_lowerbounds</tt> to 
 *  <tt>l</tt>.
 *  \param l - new value for the struct variable <tt>_lowerbounds</tt>
 */
  void setLowerbounds (lowerbounds *l);


/** Returns the value of private variable <tt>_lowerbounds</tt>.
 *  \return The dimension of <tt>_lowerbounds</tt>
 *  \note Inline function
 */
  int getLowerboundsDim () const  {return _lowerboundsDim;}


/** Sets the value of auxiliar private variable <tt>_lowerboundsDim</tt> to 
 *  <tt>dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _lowerbounds</tt>
 *  \note Inline function
 */
  void setLowerboundsDim (int dim)  {_lowerboundsDim = dim;}

/** Returns the value of private variable <tt>_fusage</tt>.
 *  \return The integer value of <tt>_fusage</tt>
 *  \note Inline function
 */
  int* getFusage () const  {return _fusage;}


/** Sets the value of private variable <tt>_fusage</tt> to <tt>d</tt>.
 *  \param d - new value for the variable <tt>_fusage</tt>
 *  \note Inline function
 */
  void setFusage (int *d);


/** Returns the value of private variable <tt>_fusage</tt>.
 *  \return The dimension of <tt>_fusage</tt>
 *  \note Inline function
 */
  int getFusageDim () const  {return _fusageDim;}


/** Sets the value of auxiliar private variable <tt>_fusageDim</tt> to 
 *  <tt>dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _fusage</tt>
 *  \note Inline function
 */
  void setFusageDim (int dim)  {_fusageDim = dim;}

/** Returns the value of private variable <tt>_bboxpred</tt>.
 *  \return A bboxpred structure, which is the value of <tt>_bboxpred</tt>
 *  \note Inline function
 */
  bboxpred* getBboxpred () const  {return _bboxpred;}


/** Sets the value of private struct variable <tt>_bboxpred</tt> to 
 *  <tt>b</tt>.
 *  \param b - new value for the struct variable <tt>_bboxpred</tt>
 */
  void setBboxpred (bboxpred *b);


/** Returns the value of private variable <tt>_bboxpred</tt>.
 *  \return The dimension of <tt>_bboxpred</tt>
 *  \note Inline function
 */
  int getBboxpredDim () const  {return _bboxpredDim;}


/** Sets the value of auxiliar private variable <tt>_bboxpredDim</tt> to <tt>
 *  dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _bboxpred</tt>
 *  \note Inline function
 */
  void setBboxpredDim (int dim)  {_bboxpredDim = dim;}
 

/** Returns the value of private variable <tt>_scoretpt</tt>.
 *  \return A CvMat vector, which is the value of <tt>_scoretpt</tt>
 *  \note Inline function
 */
  CvMat** getScoretpt () const  {return _scoretpt;}


/** Sets the value of private struct variable <tt>_scoretpt</tt> to <tt>
 *  scoretpt</tt>.
 *  \param scoretpt - new value for the struct variable <tt>_scoretpt</tt>
 */
  void setScoretpt (CvMat **scoretpt);

/** Sets the value of element <tt>idx</tt> of private struct variable 
 *  <tt>_scoretpt</tt> to <tt>scoretpt</tt>.
 *  \param scoretpt - new value for the element <tt>idx</tt> of private 
 *  variable <tt>_scoretpt</tt>
 *  \param idx - indicates the index of the element in the vector 
 */
  void setScoretpt (CvMat *scoretpt, int idx);


/** Returns the value of private variable <tt>_scoretptDim</tt>.
 *  \return The dimension of <tt>_scoretpt</tt>
 *  \note Inline function
 */
  int getScoretptDim () const   {return _scoretptDim;}


/** Sets the value of auxiliar private variable <tt>_scoretptDim</tt> to 
 *  <tt>dim</tt>.
 *  \param dim - the new value. It means the dimension of the array <tt>
 *  _scoretpt</tt>
 *  \note Inline function
 */
  void setScoretptDim (int dim)  {_scoretptDim = dim;}


////////////////////////
///// INITIALIZERS /////
////////////////////////

/** Sets each field of private struct variable <tt>_filters</tt> to it 
 *  corresponding value of <tt>filtersStructure</tt>
 *  \param filtersStructure - it contains a whole struct filters
 */
  void initializeFilters (matvar_t *filtersStructure);


/** Sets each field of private struct variable <tt>_rules</tt> to it
 *  corresponding value of <tt>rulesStructure</tt>
 *  \param rulesStructure - it contains a whole struct rules
 */
  void initializeRules (matvar_t *rulesStructure);


/** Sets each field of private struct variable <tt>_symbols</tt> to it
 *  corresponding value of <tt>symbolsStructure</tt>
 *  \param symbolsStructure - it contains a whole struct symbols
 */
  void initializeSymbols (matvar_t *symbolsStructure);


/** Sets each field of private struct variable <tt>_lowerbounds</tt> to it
 *  corresponding value of <tt>lowerboundsStructure</tt>
 *  \param lowerboundsStructure - it contains a whole struct lowerbounds
 */
  void initializeLowerBounds (matvar_t *lowerboundsStructure);


/** Sets each field of private struct variable <tt>_bboxpred</tt> to it
 *  corresponding value of <tt>bboxpredStructure</tt>
 *  \param bboxpredStructure - it contains a whole struct bboxpred
 */
  void initializeBboxpred (matvar_t *bboxpredStructure);


/////////////////////////
///// MISCELLANEOUS /////
/////////////////////////

/** The function releases all the allocated memory by the class and set
 *  all the fields to an invalid value
 */
  void destroyModel ();

  /** Releases temporal memory from a previous detection.
    * Model definition is not modified. Then, same model
	* is ready for a new detection.
	*/

  void resetModel(); 

/** Allocates dinamic memory for the private vector variable 
 *  <tt>_scoretpt</tt>. This function is necessary because the vector 
 *  is initialized element by element, so the memory must be
 *  allocated
 *  \sa _scoretpt
 */
  void allocateScoretpt();


/** Allocates dynamical memory for a 3-dimensional matrix. In order to do this,
 *  reads the parameter <tt>filtersStructure</tt> and catches it dimensions. 
 *  Once this is done, the memory can be allocated.
 *  \param filtersStructure - it contains a whole struct filters. The field 
 *  <tt>w</tt> will be got.
 *  \param dimF - is a referenced parameter. Gets the width of the 3D matrix.
 *  \param dimC - is a referenced parameter. Gets the height dimension of the 3D 
 *  matrix.
 *  \param dimA - is a referenced parameter. Gets the depth of the 3D matrix.
 *  \return A tripled pointer to the allocated memory for de 3D matrix.
 */
  CvMatND *allocateFiltersW (matvar_t *filtersStructure, int pos);


/** Converts a given lineal three-dimensional matrix into a tripled pointer 
 *  sorted by width, height and depth.
 *  \param vector - is the lineal array, which contains the data. The dimension 
 *  of the vector is dimF * dimC * dimA
 *  \param array - is the place where 3D matrix will be stored neatly.
 *  \param dimF - is the width of the 3D matrix.
 *  \param dimC - is the height dimension of the 3D matrix.
 *  \param dimA - is the depth of the 3D matrix.
 */
  void convertVectorTo3DArray (double *vector, CvMatND *array);


/** Checks y field <tt>var</tt> is content inside of <tt>matVar</tt> parameter
 *  \param matVar - it contains several structs of fields
 *  \param var - is the name of the field searched
 *  \return It returns <tt>true</tt> if the field <tt>var</tt> is contained in 
 *  <tt>matVar</tt>, or <tt>false</tt> otherwise.
 */
  bool existField (matvar_t *matVar, char* var);

/** Output operator overloaded.
 */
  friend ostream & operator<<(ostream & o, const Model & m);

private:

  string _class;

  string _year;

  string _note;

  filters* _filters;

  int _filtersDim;

  rules* _rules;

  int _rulesDim;

  symbols* _symbols;

  int _symbolsDim;

  int _numFilters;

  int _numBlocks;

  int _numSymbols;

  int* _blockSizes;

  int _blockSizesDim;

  int _start;

  int* _maxSize;

  int* _minSize;

  int _interval;

  int _sbin;

  float _thresh;

  int* _regmult;

  int _regmultDim;

  float* _learnmult;

  int _learnmultDim;

  lowerbounds* _lowerbounds;

  int _lowerboundsDim;

  int* _fusage;

  int _fusageDim;

  bboxpred* _bboxpred;

  int _bboxpredDim;

  CvMat** _scoretpt;

  int _scoretptDim;

  // Private methods
  void initPtrs(void);

  void _releaseRules(bool full=false); //! Release memory allocated during a detection

  void _releaseSymbols(void); //! Release memory allocated during a detection

  void _releaseScores(void); //! Release memory allocated during a detection
};



#endif

