#include <model.h>
#include <readTypes.h>

////////////////////////
///// CONSTRUCTORS /////
////////////////////////

Model::Model ()
  : _filters(0)
  , _scoretpt(0)
{
  loadEmptyModel();
}

Model::Model (std::string fileName)
  : _filters(0)
  , _scoretpt(0)
{
  loadFile (fileName);
}


Model::~Model ()
{
  destroyModel();
}

void Model::destroyModel ()
{
  if (_filters != NULL)
  {
    for (int i = 0; i < getFiltersDim(); i++)
    {
      if (_filters[i].w != NULL)
      {
        cvReleaseMatND(&(_filters[i].w));
        _filters[i].w = NULL;
      }
    }
    delete[] _filters;
    _filters = NULL;
  }

  if (_rules != NULL)
  {
    for (int i = 0; i < getRulesDim(); i++)
    {
      if (getRules()[i].structure != NULL)
      {
        if (getRules()[i].n == 1)
        {
          getRules()[i].structure[0].destroyCell();

          delete getRules()[i].structure;
        }

        else if (getRules()[i].n > 1)
        {
          for (int j = 0; j < getRules()[i].n; j++)
            getRules()[i].structure[j].destroyCell();

          delete[] getRules()[i].structure;
        }

        getRules()[i].structure = NULL;
      }
    }

    delete[] _rules;
    _rules = NULL;
  }

  if (_symbols != NULL)
  {
    for (int i = 0; i < getSymbolsDim(); i++)
    {
      for (size_t j = 0; j < getSymbols()[i].score.size(); j++)
      {
        if (getSymbols()[i].score[j] != NULL)
        {
          cvReleaseMat(&(getSymbols()[i].score[j]));
          getSymbols()[i].score[j] = NULL;
        }
      }
    }
    delete[] _symbols;
    _symbols = NULL;
  }

  if (_lowerbounds != NULL)
  {
    for (int i = 0; i < getLowerboundsDim(); i++)
    {
    }
    delete[] _lowerbounds;
    _lowerbounds = NULL;
  }

  if (_bboxpred != NULL)
  {
    delete[] _bboxpred;
    _bboxpred = NULL;
  }

  if (_scoretpt != NULL)
  {
    for (int i = 0; i < getScoretptDim(); i++)
    {
      if (getScoretpt()[i] != NULL)
      {
        cvReleaseMat (&(getScoretpt()[i]));
        getScoretpt()[i] = NULL;
      }
    }
    delete[] _scoretpt;
    _scoretpt = NULL;
  }
}

///////////////////////////////
///// LOAD FILE FUNCTIONS /////
///////////////////////////////

void Model::loadEmptyModel ()
{
  ///////////////////
  // READS 'CLASS' //
  ///////////////////

  setClass ("-");

  //////////////////
  // READS 'YEAR' //
  //////////////////

  setYear ("-");

  //////////////////
  // READS 'NOTE' //
  //////////////////

  setNote ("");

  /////////////////////
  // READS 'FILTERS' //
  /////////////////////

  setFiltersDim(-1);
  setFilters(NULL);

  ///////////////////
  // READS 'RULES' //
  ///////////////////

  setRulesDim(-1);
  setRules(NULL);

  /////////////////////
  // READS 'SYMBOLS' //
  /////////////////////

  setSymbolsDim(-1);
  setSymbols(NULL);

  ////////////////////////
  // READS 'NUMFILTERS' //
  ////////////////////////

  setNumFilters(-1);

  ///////////////////////
  // READS 'NUMBLOCKS' //
  ///////////////////////

  setNumBlocks(-1);

  ////////////////////////
  // READS 'NUMSYMBOLS' //
  ////////////////////////

  setNumSymbols(-1);

  ////////////////////////
  // READS 'BLOCKSIZES' //
  ////////////////////////

  //setBlockSizesDim(-1);
  //setBlockSizes(NULL);

  ///////////////////
  // READS 'START' //
  ///////////////////

  setStart (-1);

  /////////////////////
  // READS 'MAXSIZE' //
  /////////////////////

  //setMaxSize (NULL);

  /////////////////////
  // READS 'MINSIZE' //
  /////////////////////

  //setMinSize (NULL);

  //////////////////////
  // READS 'INTERVAL' //
  //////////////////////

  setInterval(-1);

  //////////////////
  // READS 'SBIN' //
  //////////////////

  setSbin (-1);

  ////////////////////
  // READS 'THRESH' //
  ////////////////////

  setThresh (-1);

  /////////////////////
  // READS 'REGMULT' //
  /////////////////////

  //setRegmultDim(-1);
  //setRegmult (NULL);

  ///////////////////////
  // READS 'LEARNMULT' //
  ///////////////////////

  //setLearnmultDim(-1);
  //setLearnmult (NULL);

  /////////////////////////
  // READS 'LOWERBOUNDS' //
  /////////////////////////

  setLowerboundsDim(-1);
  setLowerbounds (NULL);

  ////////////////////
  // READS 'FUSAGE' //
  ////////////////////

  //setFusageDim(-1);
  //setFusage (NULL);

  //////////////////////
  // READS 'BBOXPRED' //
  //////////////////////

  setBboxpredDim(-1);
  setBboxpred (NULL);

  //////////////////////
  // READS 'SCORETPT' //
  //////////////////////

  setScoretptDim  (-1);
}



void Model::loadFile (std::string fileName)
{
  mat_t *file = Mat_Open(fileName.c_str(), MAT_ACC_RDONLY);
  matvar_t *matVar;

  // It there was any error to open the file
  if ( file == NULL )
  {
      std::cout << "ERROR >> Model file <" << fileName << "> can not be opened" << std::endl;
    exit(1);
  }

  //std::cout << "Matfile version:" << file->version << std::endl;

  // The first variable is read
  try {
     matVar = Mat_VarReadNext(file);
  } catch(...)
  {
      std::cerr << "ERROR: invalid mat file. Version > 6?" << std::endl;
      exit(1);
  }

  // If there was any error to read the variable
  if ( matVar == NULL )
  {
      std::cout << "ERROR >> File's variables cannot be read" << std::endl;
    exit(1);
  }

  // The main data is read, struct model
  else
  {
    // The total number of fields of structure model is catch
    Mat_VarGetNumberOfFields(matVar);

    ///////////////////
    // READS 'CLASS' //
    ///////////////////

    loadClass (matVar);

    //////////////////
    // READS 'YEAR' //
    //////////////////

    loadYear (matVar);

    //////////////////
    // READS 'NOTE' //
    //////////////////

    loadNote (matVar);

    /////////////////////
    // READS 'FILTERS' //
    /////////////////////

    loadFilters (matVar);

    ///////////////////
    // READS 'RULES' //
    ///////////////////

    loadRules (matVar);

    /////////////////////
    // READS 'SYMBOLS' //
    /////////////////////

    loadSymbols (matVar);

    ////////////////////////
    // READS 'NUMFILTERS' //
    ////////////////////////

    loadNumFilters (matVar);

    ///////////////////////
    // READS 'NUMBLOCKS' //
    ///////////////////////

    loadNumBlocks (matVar);

    ////////////////////////
    // READS 'NUMSYMBOLS' //
    ////////////////////////

    loadNumSymbols (matVar);

    ////////////////////////
    // READS 'BLOCKSIZES' //
    ////////////////////////

    loadBlockSizes (matVar);

    ///////////////////
    // READS 'START' //
    ///////////////////

    loadStart (matVar);

    /////////////////////
    // READS 'MAXSIZE' //
    /////////////////////

    loadMaxSize (matVar);

    /////////////////////
    // READS 'MINSIZE' //
    /////////////////////

    loadMinSize (matVar);

    //////////////////////
    // READS 'INTERVAL' //
    //////////////////////

    loadInterval (matVar);

    //////////////////
    // READS 'SBIN' //
    //////////////////

    loadSbin (matVar);

    ////////////////////
    // READS 'THRESH' //
    ////////////////////

    loadThresh (matVar);

    /////////////////////
    // READS 'REGMULT' //
    /////////////////////

    loadRegmult (matVar);

    ///////////////////////
    // READS 'LEARNMULT' //
    ///////////////////////

    loadLearnmult (matVar);

    /////////////////////////
    // READS 'LOWERBOUNDS' //
    /////////////////////////

    loadLowerbounds (matVar);

    ////////////////////
    // READS 'FUSAGE' //
    ////////////////////

    loadFusage (matVar);

    //////////////////////
    // READS 'BBOXPRED' //
    //////////////////////

    loadBboxpred (matVar);

    //////////////////////
    // READS 'SCORETPT' //
    //////////////////////

    setScoretptDim  (-1);
  }  // Ends else

  Mat_Close(file);
  Mat_VarFree(matVar);
}  // loadFile


void Model::loadClass (matvar_t *matVar)
{
  std::string variable = "class";

  if (exist_field(matVar, variable))
    setClass(read_string(matVar, variable));
  else
    setClass ("-");
}


void Model::loadYear (matvar_t *matVar)
{
  std::string variable = "year";

  if (exist_field (matVar, variable))
    setYear(read_string(matVar, variable));
  else
    setYear ("-");
}

void Model::loadNote (matvar_t *matVar )
{
  std::string variable = "note";

  if (exist_field (matVar, variable))
    setNote(read_string(matVar, variable));

  else
    setNote ("");
}

void Model::loadFilters (matvar_t *matVar )
{
  char *variable = new char [8];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "filters");

  if (existField (matVar, variable))
  {
    // Reads the whole struct array 'filters'
    field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, 0);

    // Set the read values to each element of the struct array
    initializeFilters (field);
  }

  else
  {
    setFiltersDim (-1);
    setFilters (NULL);
  }

  delete[] variable;
}


void Model::loadRules (matvar_t *matVar )
{
  char *variable = new char [6];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "rules");

  if (existField (matVar, variable))
  {
    // Reads the whole cell array 'rules'
    field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, 0);

    // Set the read values to each element of the struct array
    initializeRules (field);
  }

  else
  {
    setRulesDim (-1);
    setRules (NULL);
  }

  delete[] variable;
}


void Model::loadSymbols (matvar_t *matVar )
{
  char *variable = new char [8];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "symbols");

  if (existField (matVar, variable))
  {
    // Reads the whole struct array 'symbols'
    field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, 0);

    // Set the read values to each element of the struct array
    initializeSymbols (field);
  }

  else
  {
    setSymbolsDim (-1);
    setSymbols (NULL);
  }

  delete[] variable;
}

void Model::loadNumFilters (matvar_t *matVar )
{
  std::string variable = "numfilters";

  if (exist_field (matVar, variable))
  {
    std::vector<int> el = read_number<int>(matVar, variable);
    setNumFilters (el[0]);
  }

  else
    setNumFilters (-1);
}

void Model::loadNumBlocks (matvar_t *matVar )
{
  std::string variable = "numblocks";

  if (exist_field (matVar, variable))
  {
    std::vector<int> el = read_number<int>(matVar, variable);
    setNumBlocks (el[0]);
  }

  else
    setNumBlocks (-1);
}

void Model::loadNumSymbols (matvar_t *matVar )
{
  std::string variable = "numsymbols";

  if (exist_field (matVar, variable))
  {
    std::vector<int> el = read_number<int>(matVar, variable);
    setNumSymbols (el[0]);
  }

  else
    setNumSymbols (-1);
}

void Model::loadBlockSizes (matvar_t *matVar )
{
  std::string variable = "blocksizes";

  if (exist_field (matVar, variable))
    _blockSizes = read_number<int>(matVar, variable);

  else
  {
    setBlockSizes(std::vector<int>());
  }
}

void Model::loadStart (matvar_t *matVar )
{
  std::string variable = "start";

  if (exist_field (matVar, variable))
  {
    std::vector<int> el = read_number<int>(matVar, variable);
    setStart (el[0]-1);
  }

  else
    setStart (-1);
}

void Model::loadMaxSize (matvar_t *matVar )
{
  std::string variable = "maxsize";

  if (exist_field (matVar, variable))
    _maxSize = read_number<int>(matVar, variable);

  else
    setMaxSize(std::vector<int>());
}

void Model::loadMinSize (matvar_t *matVar )
{
  std::string variable = "minsize";

  if (exist_field (matVar, variable))
    _minSize = read_number<int>(matVar, variable);

  else
    setMinSize (std::vector<int>());
}

void Model::loadInterval (matvar_t *matVar )
{
  std::string variable = "interval";

  if (exist_field (matVar, variable))
  {
    std::vector<int> el = read_number<int>(matVar, variable);
    setInterval (el[0]-1);
  }

  else
    setInterval (-1);
}

void Model::loadSbin (matvar_t *matVar )
{
  std::string variable = "sbin";

  if (exist_field (matVar, variable))
  {
    std::vector<int> el = read_number<int>(matVar, variable);
    setSbin (el[0]);
  }

  else
    setSbin (-1);
}

void Model::loadThresh (matvar_t *matVar )
{
  std::string variable = "thresh";

  if (exist_field (matVar, variable))
  {
    std::vector<double> el = read_number<double>(matVar, variable);
    setThresh (el[0]);
  }

  else
    setThresh (-1);
}

void Model::loadRegmult (matvar_t *matVar )
{
  std::string variable = "regmult";

  if (exist_field (matVar, variable))
    _regmult = read_number<int>(matVar, variable);
  else
    setRegmult (std::vector<int>());
}

void Model::loadLearnmult (matvar_t *matVar )
{
  std::string variable = "learnmult";

  if (exist_field (matVar, variable))
    _learnmult = read_number<float>(matVar, variable);

  else
  {
    setLearnmult (std::vector<float>());
  }
}

void Model::loadLowerbounds (matvar_t *matVar )
{
  char *variable = new char [12];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "lowerbounds");

  if (existField (matVar, variable))
  {
    // Reads the whole cell array 'lowerbounds'
    field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, 0);

    // Set the read values to each element of the struct array
    initializeLowerBounds (field);
  }

  else
  {
    setLowerboundsDim (-1);
    setLowerbounds (NULL);
  }

  delete[] variable;
}

void Model::loadFusage (matvar_t *matVar )
{
  std::string variable = "fusage";

  if (exist_field (matVar, variable))
    _fusage = read_number<int>(matVar, variable);

  else
  {
    setFusage(std::vector<int>());
  }
}

void Model::loadBboxpred (matvar_t *matVar )
{
  char *variable = new char [12];
  matvar_t *field;

  assert (variable != NULL);

  strcpy (variable, "bboxpred");

  if (existField (matVar, variable))
  {
    // Reads the whole cell array 'bboxpred'
    field = Mat_VarGetStructField (matVar, (char*) variable, BY_NAME, 0);

    // Set the read values to each element of the array struct
    initializeBboxpred (field);
  }

  else
  {
    setBboxpredDim (-1);
    setBboxpred (NULL);
  }

  delete[] variable;
}


/////////////////////////////
///// SETTERS & GETTERS /////
/////////////////////////////

void Model::setFilters (filters *f)
{
  if (f != NULL)
  {
    assert (getFiltersDim() > 0);

    _filters = new filters [getFiltersDim()];

    assert (_filters != NULL);

    for (int i = 0; i < getFiltersDim(); i++)
      _filters[i] = f[i];
  }

  else
    _filters = f;
}

void Model::setRules (rules *r)
{
  if (r != NULL)
  {
    assert (getRulesDim() > 0);

    _rules = new rules [getRulesDim()];

    assert (_rules != NULL);

    for (int i = 0; i < getRulesDim(); i++)
      _rules[i] = r[i];
  }

  else
    _rules = r;
}

void Model::setSymbols (symbols *s)
{
  if (s != NULL)
  {
    assert (getSymbolsDim() > 0);

    _symbols = new symbols [getSymbolsDim()];

    assert (_symbols != NULL);

    for (int i = 0; i < getSymbolsDim(); i++)
      _symbols[i] = s[i];
  }

  else
    _symbols = s;
}


void Model::setBlockSizes (const std::vector<int>& d)
{
  _blockSizes = d;
}

void Model::setMaxSize (const std::vector<int>& d)
{
  assert(d.size() == 2);
  _maxSize = d;
}

void Model::setMinSize (const std::vector<int>& d)
{
  assert(d.size() == 2);
  _minSize = d;
}

void Model::setRegmult (const std::vector<int>& d)
{
  _regmult = d;
}

void Model::setLearnmult (const std::vector<float>& d)
{
  _learnmult = d;
}

void Model::setLowerbounds (lowerbounds *l)
{
  if (l != NULL)
  {
    assert (getLowerboundsDim() > 0);

    _lowerbounds = new lowerbounds [getLowerboundsDim()];

    assert (_lowerbounds != NULL);

    for (int i = 0; i < getLowerboundsDim(); i++)
      _lowerbounds[i] = l[i];
  }

  else
    _lowerbounds = l;
}

void Model::setFusage (const std::vector<int>& d)
{
  _fusage = d;
}

void Model::setBboxpred (bboxpred *b)
{
  if (b != NULL)
  {
    assert (getBboxpredDim() > 0);

    _bboxpred = new bboxpred [getBboxpredDim()];

    assert (_bboxpred != NULL);

    for (int i = 0; i < getBboxpredDim(); i++)
      _bboxpred[i] = b[i];
  }

  else
    _bboxpred = b;
}

void Model::allocateScoretpt()
{
  _scoretpt = new CvMat* [getScoretptDim()];

  assert (_scoretpt != NULL);
}


void Model::setScoretpt (CvMat **scoretpt)
{
  if (scoretpt != NULL)
    assert (getScoretptDim() > 0);

  _scoretpt = new CvMat* [getScoretptDim()];

  assert (_scoretpt != NULL);

  if (scoretpt != NULL)
  {
    for (int i = 0; i < getScoretptDim(); i++)
      _scoretpt[i] = scoretpt[i];
  }
}


void Model::setScoretpt (CvMat *scoretpt, int idx)
{
  assert (idx < getScoretptDim());

  _scoretpt[idx] = scoretpt;
}

////////////////////////
///// INITIALIZERS /////
////////////////////////

void Model::initializeFilters (matvar_t *filtersStructure)
{
  int length = vectorLength(filtersStructure);
  filters *f = new filters [length];
  char *variable = new char [12];

  assert (f != NULL);
  assert (variable != NULL);

  setFiltersDim (length);

  for (int i = 0; i < length; i++ )
  {
    //double *auxD = NULL;
    //int *auxI = NULL;
    std::vector<double> auxD;
    std::vector<int> auxI;

    strcpy (variable, "w");
    auxD = read_number<double>(filtersStructure, variable, i);

    f[i].w = allocateFiltersW (filtersStructure, i);

    convertVectorTo3DArray (auxD.data(), f[i].w);

    strcpy (variable, "blocklabel");
    auxI = read_number<int>(filtersStructure, variable, i);
    f[i].blocklabel = auxI[0];
    f[i].symmetric =  read_string(filtersStructure, "symmetric", i)[0];
    f[i].size = read_number<int>(filtersStructure, "size", i);
    f[i].flip = read_logical (filtersStructure, "flip", i);
    f[i].symbol = read_number<int>(filtersStructure, "symbol", i)[0];
  }

  setFilters (f);

  delete[] variable;
  delete[] f;
}


void Model::initializeRules (matvar_t *rulesStructure)
{
  int length = vectorLength(rulesStructure);
  int lengthField;
  matvar_t *field;
  rules *r = new rules [length];

  assert (r != NULL);

  setRulesDim (length);

  for (int i = 0; i < length; i++)
  {
    field = Mat_VarGetCell (rulesStructure, i);

    // Type of field is invalid character (double 0x0)
    if ( field->data_type == MAT_T_UNKNOWN )
      r[i].n = 0;

    // Type of field is a valid structure
    else
    {
      /* Catch the number of fields. Is used to distinguish between type
      of structure 1 or 2 */
      lengthField = vectorLength(field);

      r[i].n = lengthField;

      // Type of structure 1 (scalar, last field def)
      if ( lengthField == 1 )
      {
        /* Allocate memory for a scalar element, the field structure
        of r[i] variable (Cell*) and calls the constructor of Cell */
        r[i].structure = new Cell(field);
        assert (r[i].structure != NULL);
      }

      // Type of structure 2 (array, last field anchor)
      else
      {
        /* Allocate memory for an array element, the field structure of
        r[i] variable (Cell*) */
        r[i].structure = new Cell [lengthField];
        assert (r[i].structure != NULL);

        // Calls the Cell constructor for each element of the r[i] array
        for (int j = 0; j < lengthField; j++)
			r[i].structure[j].loadCell (field, j);
      }
    }
  }

  // Set the value of the whole private structure rules
  setRules (r);

  delete[] r;
}



void Model::initializeSymbols (matvar_t *symbolsStructure)
{
  int length = vectorLength(symbolsStructure);
  symbols *s = new symbols [length];

  assert (s != NULL);

  setSymbolsDim (length);

  for (int j = 0; j < length; j++ )
  {
    s[j].type = read_string(symbolsStructure, "type", j)[0];

    /* The value of 'i' is decremented because it means an array index.
    MatLab indexes start on 1, and C indexes start on 0, so it must be
    decremented by 1 */
    std::vector<int> auxI = read_number<int>(symbolsStructure, "i", j);
    s[j].i = auxI[0]-1;

    auxI = read_number<int>(symbolsStructure, "filter", j);

    if ( auxI.empty() )
      s[j].filter = INVALID;
    else
      s[j].filter = auxI[0];

    /* The number of scores is setted like -1, what it means empty
    (invalid value) */
    s[j].score.clear();
  }

  setSymbols (s);

  delete[] s;
}


void Model::initializeLowerBounds (matvar_t *lowerboundsStructure)
{
  int length = vectorLength(lowerboundsStructure);
  lowerbounds *l = new lowerbounds [length];

  assert (l != NULL);

  setLowerboundsDim (length);

  for (int j = 0; j < length; j++ )
  {
    l[j].v = read_number<float>(lowerboundsStructure, "", j);
    l[j].f = 1;
  }

  setLowerbounds (l);

  delete[] l;
}


void Model::initializeBboxpred (matvar_t *bboxpredStructure)
{
  int length = vectorLength(bboxpredStructure);
  matvar_t *field;
  bboxpred *b = new bboxpred [bboxpredStructure->dims[0]];
  char *variable = new char [12];

  assert (b != NULL);
  assert (variable != NULL);

  setBboxpredDim (length);

  for (int i = 0; i < length; i++ )
  {
    field = Mat_VarGetCell (bboxpredStructure, i);
    b[i].x1 = read_number<float>(field, "x1", 0);
    b[i].y1 = read_number<float>(field, "y1", 0);
    b[i].x2 = read_number<float>(field, "x2", 0);
    b[i].y2 = read_number<float>(field, "y2", 0);
  }

  setBboxpred (b);

  delete[] variable;
  delete[] b;
}


/////////////////////////
///// MISCELLANEOUS /////
/////////////////////////

CvMatND *Model::allocateFiltersW (matvar_t *filtersStructure, int pos)
{
  matvar_t *field;
  char *variable = new char [2];
  CvMatND *aux = NULL;
  int dimF, dimC, dimA, dims[3];

  assert (variable != NULL);

  strcpy (variable, "w");
  field = Mat_VarGetStructField (filtersStructure, (char*) variable,
  BY_NAME, pos);

  if ( field != NULL )
  {
    if ( field->data_type == MAT_T_DOUBLE )
    {
      dims[0] = dimF = field->dims[0];
      assert (dims[0] > 0);
      dims[1] = dimC = field->dims[1];
      assert (dims[1] > 0);
      dims[2] = dimA = field->dims[2];
      assert (dims[2] > 0);

      createMatrix (3, dims, CV_64FC1, &aux);
      assert (aux != NULL);
    }
  }

  delete[] variable;

  return aux;
}


void Model::convertVectorTo3DArray (double *vector, CvMatND *array)
{
  for (int i = 0; i < array->dim[0].size; i++)          // dimF
    for (int j = 0; j < array->dim[1].size; j++)        // dimC
      for (int k = 0; k < array->dim[2].size; k++)      // dimA
        cvSetReal3D (array, i, j, k, (float)vector[(i) +
        (j*array->dim[0].size)+(k*array->dim[0].
        size*array->dim[1].size)]);
}


bool Model::existField (matvar_t *matVar, char* var)
{
  matvar_t *field;
  bool flag = false;

  field = Mat_VarGetStructField (matVar, (char*) var, BY_NAME, 0);

  if ( field != NULL )
    flag = true;

  return flag;
}

bool Model::exist_field (matvar_t *matVar, const std::string& var)
{
  matvar_t *field;
  bool flag = false;

  field = Mat_VarGetStructField (matVar, const_cast<char*>(var.c_str()), BY_NAME, 0);

  if ( field != NULL )
    flag = true;

  return flag;
}

std::ostream & operator<<(std::ostream & co, const Model & m)
{
   co << "Model: " << m.getClass() << std::endl;
   co << "Note:  " << m.getNote() << std::endl;
   co << "Year:  " << m.getYear() << std::endl;
   return co;
}


