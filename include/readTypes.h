#ifndef _READTYPES_H_
#define _READTYPES_H_
#include <matio.h>
#include <custom.h>
#include <cassert>
#if MATIO_MAJOR_VERSION>=1 && MATIO_MINOR_VERSION>=5 
   #define BY_NAME MAT_BY_NAME
#endif


/** \file
 *  This file contains functions used to load files .mat from MatLab. This
 *  functions are used by objects of classes <tt>Model</tt>  and 
 *  <tt>Cell</tt> .
 */


/** Reads the string variable <tt>var</tt> placed on the structure 
 *  <tt>matVar</tt> in position <tt>pos</tt>.
 *  \param matVar - is a structure with several fields. It can be an array 
 *  (in this case, <tt>pos</tt> will be used to reference the position)
 *  \param var - is the name of the field to read
 *  \param pos - is a default parameter. If it is not specified, position 0 
 *  is used (<tt>matVar</tt> is not an array or only the first element of 
 *  the array will be read). Otherwise, struct in position <tt>pos</tt> will 
 *  be read.
 *  \return The string read is returned in a <tt>char*</tt> type variable.
 */
char* readString (matvar_t *matVar, char* var, int pos = 0);


/** Reads the numerical variable <tt>var</tt> placed on the structure 
 *  <tt>matVar</tt> in position <tt>pos</tt>. It can be an array of  
 *  dimension <tt>dim</tt> or a scalar value (<tt>dim</tt> = 1)
 *  \param matVar - is a structure with several fields. It can be an array (in 
 *  this case, <tt>pos</tt> will be used to reference the position)
 *  \param var - is the name of the field to read
 *  \param number - is a referenced parameter. Is the numerical variable read.
 *  Its type is used to casting the number read into that type. Can be a
 *  scalar value (<tt>dim</tt> = 1) or an array (<tt>dim</tt> > 1). 
 *  \param dim - is a referenced parameter. If the value read is a scalar,
 *  <tt>dim</tt> will be 1. Otherwise, <tt>dim</tt> will be the lenght of the
 *  array (parameter <tt>number</tt>).
 *  \param pos - is a default parameter. If it is not specified, position 0 
 *  is used (<tt>matVar</tt> is not an array or only the first element of the 
 *  array will be read). Otherwise, struct in position <tt>pos</tt> will be 
 *  read.
 */

template <class Type>
void readNumber (matvar_t *matVar, char* var, Type **number, int* dim, 
                 int pos = 0);


/** Reads the logical variable <tt>var</tt> inside the structure 
 *  <tt>matVar</tt> in position <tt>pos</tt>.
 *  \param matVar - is a structure with several fields. It can be an array (in 
 *  this case, <tt>pos</tt> will be used to reference the position)
 *  \param var - is the name of the field to read
 *  \param pos - is a default parameter. If it is not specified, position 0 
 *  is used (<tt>matVar</tt> is not an array or only the first element of the 
 *  array will be read). Otherwise, struct in position <tt>pos</tt> will be 
 *  read.
 *  \return The boolean value read is returned in a <tt>bool</tt> type 
 *  variable.
 */
bool readLogical (matvar_t *matVar, char* var, int pos = 0);


/** Get the dimension of the parameter. This is necessary because the 
 *  <tt>matVar</tt> struct can be a horizontal or a vertical vector, and is 
 *  required to check what is the largest dimension.
 *  \param matVar - is the structure array
 *  \return The largest dimension of the vector. Is required the other 
 *  dimension is 1 (otherwise, the struct is not a vector). The function does 
 *  not check this.
 */
int vectorLength (matvar_t *matVar);



//////////////////////////
///// IMPLEMENTATION /////
//////////////////////////



template <class Type>
void readNumber (matvar_t *matVar, char* var, Type **number, int* dim, int pos)
{
  matvar_t *field;
  double* auxNumb = NULL;

  if (!matVar ) 
  {
     *dim = 0;
     return;
  }

  // If the variable is a Struct
  if ( matVar->data_type == MAT_T_STRUCT )
    field = Mat_VarGetStructField (matVar, (char*) var, BY_NAME, pos);

  // If the variable is a Cell
  else
    field = Mat_VarGetCell (matVar, pos);

  if ( field != NULL )
  {
    if ( field->data_type == MAT_T_DOUBLE )
    {
      auxNumb = (double*) field->data;

      *dim = field->nbytes/sizeof(double);

      *number = new Type [*dim];
      assert (number != NULL);

      for (int i = 0; i < *dim; i++)
        (*number)[i] = (Type) auxNumb[i];
    }
  }
}



#endif

