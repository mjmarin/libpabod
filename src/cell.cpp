#include <cell.h>
#include <readTypes.h>
////////////////////////
///// CONSTRUCTORS /////
////////////////////////


Cell::Cell ()
{
  loadEmptyCell ();
}

Cell::Cell (matvar_t *matVar, int i)
{
  loadCell (matVar, i);
}


Cell::~Cell()
{
  destroyCell();
}


void Cell::destroyCell ()
{
  if (getFlagStr() != INVALID_STR)
  {
    if (_rhs != NULL)
    {
      if (getRhsDim() > 1)
        delete[] _rhs;

      else
        delete _rhs;

      _rhs = NULL;
    }

    if (_detwindow != NULL)
    {
      delete[] _detwindow;
      _detwindow = NULL;
    }

    if (getFlagStr() == STR_ANCHOR)
    {
      if (_anchor != NULL)
      {
        for (int i = 0; i < getAnchorDim(); i++)
        {
          if (getAnchor()[i].array != NULL)
          {
            delete[] getAnchor()[i].array;
            getAnchor()[i].array = NULL;
          }
        }

        delete[] _anchor;
        _anchor = NULL;
      } 

     //if (_score != NULL)
	   if (_score.size() > 0)
      {
		  /*
        for (int i = 0; i < getScoreDim(); i++)
        {
          if (_score[i] != NULL)
          {
            cvReleaseMat (&_score[i]);
            _score[i] = NULL;
          }
        }
      
        delete[] _score;
        _score = NULL;
		*/
		  _score.clear();
      }
	  		  
    }

    //if (_Ix != NULL)
	if(_Ix.size() > 0)
    {
		/*
      for (int i = 0; i < getIxDim(); i++)
      {
        if (_Ix[i] != NULL)
        {
          cvReleaseMat (&_Ix[i]);
          _Ix[i] = NULL;
        }
      }

      delete[] _Ix;
      _Ix = NULL;
	  */
		_Ix.clear();
    }

    //if (_Iy != NULL)
	if (_Iy.size() > 0)
    {
		/*
      for (int i = 0; i < getIyDim(); i++)
      {
        if (_Iy[i] != NULL)
        {
          cvReleaseMat (&_Iy[i]);
          _Iy[i] = NULL;
        }
      }

      delete[]_Iy;
      _Iy = NULL;
	  */
		_Iy.clear();
    }
	
		
  }

  setFlagStr (INVALID_STR);
}


void Cell::releaseScore(void)
{
	  //if (_score != NULL)
	if(!_score.empty())
      {
		  /*
        for (int i = 0; i < getScoreDim(); i++)
        {
          if (_score[i] != NULL)
          {
            cvReleaseMat (&_score[i]);
            _score[i] = NULL;
          }
        }
      
        delete[] _score;
        _score = NULL;
		*/
		  _score.clear();
      }
    
}

void Cell::releaseIxIy(void)
{
	
    //if (_Ix != NULL)
	if(!_Ix.empty())
    {
		/*
      for (int i = 0; i < getIxDim(); i++)
      {
        if (_Ix[i] != NULL)
        {
          cvReleaseMat (&_Ix[i]);
          _Ix[i] = NULL;
        }
      }

      delete[] _Ix;
      _Ix = NULL;
	  */
		_Ix.clear();
    }

    //if (_Iy != NULL)
	if (!_Iy.empty())
    {
		/*
      for (int i = 0; i < getIyDim(); i++)
      {
        if (_Iy[i] != NULL)
        {
          cvReleaseMat (&_Iy[i]);
          _Iy[i] = NULL;
        }
      }

      delete[]_Iy;
      _Iy = NULL;
	  */
		_Iy.clear();
    }
}


///////////////////////////////
///// LOAD FILE FUNCTIONS /////
///////////////////////////////

void Cell::loadEmptyCell ()
{
  setFlagStr(INVALID_STR);

  setType (0);

  setLhs (-1);

  _rhsDim = -1;
  _rhs = NULL;

  _detwindow = NULL;

  setI(-1);

  getOffset().w = -1;
  getOffset().blocklabel = -1;

  _anchorDim = -1;
  _anchor = NULL;

  for (int i = 0; i < 4; i++)
    getDef().w[i] = -1;

  getDef().blocklabel = -1;
  getDef().flip = false;
  getDef().symmetric = 0;

  _scoreDim = -1;
//  _score = NULL;

  _IxDim = -1;
//  _Ix = NULL;

  _IyDim = -1;
//  _Iy = NULL;
}


void Cell::loadCell (matvar_t *matVar, int i)
{
  loadEmptyCell (); // Initialize all members

  loadType (matVar, i);
  loadLhs (matVar, i);
  loadDetwindow (matVar, i);
  loadI (matVar, i);
  loadOffset (matVar, i);

  // Type of structure 1 (last field called def)
  if ( matVar->dims[0] == 1 && matVar->dims[1] == 1 )
  {
    setFlagStr(STR_DEF);
    loadRhs (matVar, i);
    loadDef (matVar, i);
  }

  // Type of structure 2 (last field called anchor)
  else if ( vectorLength(matVar) > 1 )
  {
    setFlagStr(STR_ANCHOR);
    loadRhs (matVar, i, false);
    loadAnchor (matVar, i);
  }

  _IxDim = -1;
//  _Ix = NULL;

  _IyDim = -1;
//  _Iy = NULL;

  _scoreDim = -1;
//  _score = NULL;
}


void Cell::loadType (matvar_t *matVar, int i)
{
  char *variable = new char [5];

  assert (variable != NULL);

  strcpy (variable, "type");

  setType(readString (matVar, variable, i)[0]);

  delete[] variable;
}


void Cell::loadLhs (matvar_t *matVar, int i)
{
  char *variable = new char [4];
  int dim = -1;
  int *el = NULL;

  assert (variable != NULL);

  strcpy (variable, "lhs");

  readNumber (matVar, variable, &el, &dim, i);
  setLhs(el[0]-1);

  delete[] variable;
  delete[] el;
}


void Cell::loadRhs (matvar_t *matVar, int i, bool scalar)
{
  char *variable = new char [4];
  int *auxInt = NULL;

  assert (variable != NULL);

  strcpy (variable, "rhs");

  readNumber (matVar, variable, &auxInt, &_rhsDim, i);

  setRhs (auxInt);

  delete[] variable;
  delete[] auxInt;
}


void Cell::loadDetwindow (matvar_t *matVar, int i)
{
  char *variable = new char [10];
  int dim = -1;

  assert (variable != NULL);

  strcpy (variable, "detwindow");

  readNumber (matVar, variable, &_detwindow, &dim, i);

  delete[] variable;
}


void Cell::loadI (matvar_t *matVar, int i)
{
  char *variable = new char [2];
  int dim = -1;
  int *el = NULL;

  assert (variable != NULL);

  strcpy (variable, "i");

  readNumber (matVar, variable, &el, &dim, i);
  setI(el[0]-1);

  delete[] variable;
  delete[] el;
}


void Cell::loadOffset (matvar_t *matVar, int i)
{
  char *variable = new char [7];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "offset");

  field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, i);

  initializeOffset (field);

  delete[] variable;
}


void Cell::loadAnchor (matvar_t *matVar, int i)
{
  char *variable = new char [7];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "anchor");

  field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, i);

  initializeAnchor (field);

  delete[] variable;
}


void Cell::loadDef (matvar_t *matVar, int i)
{
  char *variable = new char [7];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "def");

  field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, i);

  if (field != NULL)
     initializeDef (field);

  delete[] variable;
}



/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

void Cell::setRhs (int *rhs)
{
  assert (getRhsDim() > 0);

  if (_rhs != NULL)
	  delete [] _rhs;

  _rhs = new int [getRhsDim()]; 

  assert (_rhs != NULL);

  for (int i = 0; i < getRhsDim(); i++)
    _rhs[i] = rhs[i]-1;
}


void Cell::setDetwindow (double *d)
{
  assert (getDetwindowDim() > 0);

  if (_detwindow != NULL)
	  delete [] _detwindow;

  _detwindow = new int [getDetwindowDim()];

  assert (_detwindow != NULL);

  for (int i = 0; i < getDetwindowDim(); i++)
    _detwindow[i] = (int)d[i];
}


void Cell::setAnchor (anchor *a)
{
  assert (getAnchorDim() > 0);

  if (_anchor != NULL)
	  delete [] _anchor;

  _anchor = new anchor [getAnchorDim()]; // Suspicious

  assert (_anchor != NULL);

  for (int i = 0; i < getAnchorDim(); i++)
    _anchor[i] = a[i];
}

/*
void Cell::setScore (CvMat **score)
{  
  assert (getScoreDim() > 0);

  if (_score != NULL)
  {
	  	  for (int i = 0; i < getScoreDim(); i++)
		  cvReleaseMat(&_score[i]);
	  delete [] _score;
  }

  _score = new CvMat* [getScoreDim()];

  assert (_score != NULL);

  for (int i = 0; i < getScoreDim(); i++)
    _score[i] = score[i];
}
*/
void Cell::setScore (vectorMat  &score)
{
	_score = score;
}

/*
void Cell::setIx (CvMat **Ix)
{
  assert (getIxDim() > 0);

  if (_Ix != NULL)
  {
	  for (int i = 0; i < getIxDim(); i++)
		  cvReleaseMat(&_Ix[i]);
	  delete [] _Ix;
  }

  _Ix = new CvMat* [getIxDim()];

  assert (_Ix != NULL);

  for (int i = 0; i < getIxDim(); i++)
    _Ix[i] = Ix[i];
}
*/

void Cell::setIx (vectorMat &Ix)
{
	_Ix = Ix;
}


/*
void Cell::setIy (CvMat **Iy)
{
  assert (getIyDim() > 0);

  if (_Iy != NULL)
  {
	  for (int i = 0; i < getIyDim(); i++)
		  cvReleaseMat(&_Iy[i]);
	  delete [] _Iy;
  }

  _Iy = new CvMat* [getIyDim()];

  assert (_Iy != NULL);

  for (int i = 0; i < getIyDim(); i++)
    _Iy[i] = Iy[i];
}
*/
void Cell::setIy (vectorMat &Iy)
{
	_Iy = Iy;
}

////////////////////////
///// INITIALIZERS /////
////////////////////////

void Cell::initializeOffset (matvar_t *offsetStructure)
{
  int dim = -1;
  offset of;
  char *variable = new char [11];
  int *auxI = NULL;
  float *auxF = NULL;

  assert (variable != NULL);

  strcpy (variable, "w");
  readNumber (offsetStructure, variable, &auxF, &dim);
  of.w = auxF[0];

  delete[] auxF;

  strcpy (variable, "blocklabel");
  readNumber (offsetStructure, variable, &auxI, &dim);
  of.blocklabel = auxI[0];

  delete[] auxI;

  setOffset (of);

  delete[] variable;
}


void Cell::initializeAnchor (matvar_t *anchorStructure)
{
  int length = vectorLength (anchorStructure);
  anchor *a = new anchor [length];

  assert (a != NULL);

  setAnchorDim (length);

  for (int i = 0; i < length; i++)
    readNumber (anchorStructure, NULL, &(a[i].array), &(a[i].dim), i);

  setAnchor (a);

  delete[] a;
}


void Cell::initializeDef (matvar_t *defStructure)
{
  int dim = -1;
  def d;
  char *variable = new char [11];
  double *auxD = NULL;
  int *el = NULL;

  assert (variable != NULL);

  strcpy (variable, "w");
  readNumber (defStructure, variable, &auxD, &dim);
  for (int i = 0; i < dim; i++)
    d.w[i] = auxD[i];

  delete[] auxD;

  strcpy (variable, "blocklabel");
  readNumber (defStructure, variable, &el, &dim);
  //d.blocklabel = el[0];
  if (el) 
    d.blocklabel = el[0];
  else 
    d.blocklabel = 0;
   
  delete[] el;

  strcpy (variable, "flip");
  d.flip = readLogical (defStructure, variable);

  strcpy (variable, "symmetric");
  //d.symmetric = readString (defStructure, variable)[0];
  if ( defStructure ) 
    d.symmetric = readString (defStructure, variable)[0];
  else 
    d.symmetric = 0;
  
  setDef (d);

  delete[] variable;
}


