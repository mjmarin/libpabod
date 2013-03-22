#ifndef _MODELSORT_H_
#define _MODELSORT_H_

#include <model.h>

using namespace std;


/** \file
 *  This file build the formal grammar used to determine which type of
 *  rules are applied to specific image areas.
 */


//////////////////////////////////
// LSTRUCTURE STRUCT DEFINITION //
//////////////////////////////////

/** \struct LStructure
 *  The structure has to be created to contain the vector and their number
 *  of elements. This is necessary because the function is recursive. It 
 *  could be implemented in a variety of different ways, though original
 *  design has been maintained for compatibility.
 */
typedef struct LStructure
{
  int *L;
  int LDim;
} LStructure;


/** The recursive function sort out the non-terminal symbols of model
 *  grammar using vector <tt>L</tt>. The order applied is post-order.
 *  \param m - is the object model used in the detectcion operation
 *  \param L - is the vector of structure <tt>LStructure</tt> to be sorted 
 *  out in post-order way
 *  \param i - indicates the current symbol
 *  \param V - is an auxiliar vector where visited symbols are marked
 *  \return Is the current vector <tt>L</tt>. Is necessary to make the
 *  recursive function works well.
 */
LStructure* modelSort (const Model *m, LStructure* L, int i = -1, 
                       int* V = NULL);


#endif

