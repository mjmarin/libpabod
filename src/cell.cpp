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
    if (getFlagStr() == STR_ANCHOR)
    {
      if (_anchor != NULL)
      {
        delete[] _anchor;
        _anchor = NULL;
      }

      for (int i = 0; i < getScoreDim(); i++)
      {
        if (_score[i] != NULL)
        {
          cvReleaseMat (&_score[i]);
          _score[i] = NULL;
        }
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
  std::vector<int> auxInt = read_number<int>(matVar, "rhs", i);
  setRhs (auxInt);
}


void Cell::loadDetwindow (matvar_t *matVar, int i)
{
  _detwindow = read_number<int>(matVar, "detwindow", i);
}


void Cell::loadI (matvar_t *matVar, int i)
{
  std::vector<int> el = read_number<int>(matVar, "i", i);
  setI(el[0]-1);
}


void Cell::loadOffset (matvar_t *matVar, int i)
{
  matvar_t* field = Mat_VarGetStructField (matVar, const_cast<char*>("offset"), BY_NAME, i);
  initializeOffset (field);
}


void Cell::loadAnchor (matvar_t *matVar, int i)
{
  matvar_t* field = Mat_VarGetStructField (matVar, const_cast<char*>("anchor"), BY_NAME, i);
  initializeAnchor (field);
}


void Cell::loadDef (matvar_t *matVar, int i)
{
  matvar_t* field = Mat_VarGetStructField (matVar, const_cast<char*>("def"), BY_NAME, i);
  initializeDef (field);
}



/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

void Cell::setRhs (const std::vector<int>& rhs)
{
  _rhs = rhs;
  for (std::vector<int>::size_type i = 0; i < rhs.size(); i++)
    --_rhs[i];
}


void Cell::setDetwindow (const std::vector<int>& d)
{
  _detwindow = d;
}


void Cell::setAnchor (anchor *a)
{
  assert (getAnchorDim() > 0);

  _anchor = new anchor [getAnchorDim()];

  assert (_anchor != NULL);

  for (int i = 0; i < getAnchorDim(); i++)
    _anchor[i] = a[i];
}


void Cell::setScore (const std::vector<CvMat*>& score)
{
  _score = score;
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
    a[i].array = read_number<int>(anchorStructure, "", i);

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


