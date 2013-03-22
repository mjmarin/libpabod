#include <readTypes.h>

char* readString (matvar_t *matVar, char* var, int pos)
{
  matvar_t *field;
  char* auxString = NULL;

  field = Mat_VarGetStructField (matVar, (char*) var, BY_NAME, pos);

  if ( field != NULL )
    if ( field->data_type == MAT_T_UINT8 )
      auxString = (char*) field->data;

  return auxString;
}


bool readLogical (matvar_t *matVar, char* var, int pos)
{
  matvar_t *field;
  int *auxInt = NULL;
  bool flag = false;

  field = Mat_VarGetStructField (matVar, (char*) var, BY_NAME, pos);

  if ( field != NULL )
  {
    if ( field->data_type == MAT_T_UINT8 )
    {
    auxInt = (int*) field->data;

    if (*auxInt == 1)
      flag = true;
    }
  }

  return flag;
}

int vectorLength (matvar_t *matVar)
{
  if (matVar->dims[1] > matVar->dims[0])
    return matVar->dims[1];

  else
    return matVar->dims[0];
}

