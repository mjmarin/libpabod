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

      if (_score != NULL)
      {
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
      }
    }

    if (_Ix != NULL)
    {
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
    }

    if (_Iy != NULL)
    {
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
    }
  }

  setFlagStr (INVALID_STR);
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
  _score = NULL;

  _IxDim = -1;
  _Ix = NULL;

  _IyDim = -1;
  _Iy = NULL;
}


void Cell::loadCell (matvar_t *matVar, int i)
{
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
  _Ix = NULL;

  _IyDim = -1;
  _Iy = NULL;

  _scoreDim = -1;
  _score = NULL;
}


void Cell::loadType (matvar_t *matVar, int i)
{
  setType(read_string(matVar, "type", i)[0]);
}


void Cell::loadLhs (matvar_t *matVar, int i)
{
  std::vector<int> el = read_number<int>(matVar, "lhs", i);
  setLhs(el[0]-1);
}


void Cell::loadRhs (matvar_t *matVar, int i, bool scalar)
{
  (void)scalar;
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
  std::vector<int> el = read_number<int>(matVar, "i", i);
  setI(el[0]-1);
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

  initializeDef (field);

  delete[] variable;
}



/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

void Cell::setRhs (int *rhs)
{
  assert (getRhsDim() > 0);

  _rhs = new int [getRhsDim()];

  assert (_rhs != NULL);

  for (int i = 0; i < getRhsDim(); i++)
    _rhs[i] = rhs[i]-1;
}


void Cell::setDetwindow (double *d)
{
  assert (getDetwindowDim() > 0);

  _detwindow = new int [getDetwindowDim()];

  assert (_detwindow != NULL);

  for (int i = 0; i < getDetwindowDim(); i++)
    _detwindow[i] = (int)d[i];
}


void Cell::setAnchor (anchor *a)
{
  assert (getAnchorDim() > 0);

  _anchor = new anchor [getAnchorDim()];

  assert (_anchor != NULL);

  for (int i = 0; i < getAnchorDim(); i++)
    _anchor[i] = a[i];
}


void Cell::setScore (CvMat **score)
{
  assert (getScoreDim() > 0);

  _score = new CvMat* [getScoreDim()];

  assert (_score != NULL);

  for (int i = 0; i < getScoreDim(); i++)
    _score[i] = score[i];
}


void Cell::setIx (CvMat **Ix)
{
  assert (getIxDim() > 0);

  _Ix = new CvMat* [getIxDim()];

  assert (_Ix != NULL);

  for (int i = 0; i < getIxDim(); i++)
    _Ix[i] = Ix[i];
}


void Cell::setIy (CvMat **Iy)
{
  assert (getIyDim() > 0);

  _Iy = new CvMat* [getIyDim()];

  assert (_Iy != NULL);

  for (int i = 0; i < getIyDim(); i++)
    _Iy[i] = Iy[i];
}


////////////////////////
///// INITIALIZERS /////
////////////////////////

void Cell::initializeOffset (matvar_t *offsetStructure)
{
  std::vector<float> auxF = read_number<float>(offsetStructure, "w");
  std::vector<int>   auxI = read_number<int>(offsetStructure, "blocklabel");
  offset of = { auxF[0], auxI[0] };
  setOffset (of);
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
  def d;

  std::vector<double> auxD = read_number<double>(defStructure, "w");
  for (std::vector<double>::size_type i = 0; i < auxD.size(); i++)
    d.w[i] = auxD[i];

  d.blocklabel = read_number<int>(defStructure, "blocklabel")[0];
  d.flip       = read_logical(defStructure, "flip");
  d.symmetric  = read_string(defStructure, "symmetric")[0];

  setDef (d);
}


