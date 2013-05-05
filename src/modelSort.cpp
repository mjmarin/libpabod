#include <modelSort.h>

LStructure *modelSort (const Model *m, LStructure* L, int i, int* V)
{
  bool topcall = false;

  // Initialize depth-first search at start symbol
  if (i == -1)
  {
    topcall = true;
    i = (int) m->getStart();
    L->L = NULL ;
    L->LDim = 0;
    assert (m->getNumSymbols() > 0);
    V = new int [(int) m->getNumSymbols()];
    assert (V != NULL);
    for (int j = 0; j < (int) m->getNumSymbols(); j++)
      V[j] = 0;
  }

  // Check for cycle containing symbol i
  if (V[i] == 1)
    cout << "ERROR >> Cycle detected in grammar!" << endl;

  // Mark symbol i as pre-visit
  V[i] = 1;

  // Condition used for rules cell whose dimension is 1 (and rhs field is
  // scalar too)
  if (m->getRules()[i].n == 1)
  {
    int p = (int) m->getRules()[i].structure->getRhs()[0];

    // Recurse if s is a non-terminal
    if (m->getSymbols()[p].type == 'N')
      L = modelSort (m, L, p, V);
  }

  // Condition used for that rules which are arrays (and rhs field is an
  // array too)
  else
  {
    for (int j = 0; j < m->getRules()[i].n; j++)
      for (int s = 0; s < m->getRules()[i].structure[j].getRhsDim(); s++)
      {
        int p = (int) m->getRules()[i].structure[j].getRhs()[s];

        // Recurse if s is a non-terminal
        if (m->getSymbols()[p].type == 'N')
          L = modelSort (m, L, p, V);
      }
  }

  // Mark symbol i as post-visit
  V[i] = 2;

  int* auxL = new int [(L->LDim)+1];
  assert (auxL != NULL);


  if ( L->L != NULL)
    for (int j = 0; j < (L->LDim); j++)
      auxL[j] = L->L[j];

  auxL[L->LDim] = i;

  // Added by mjmarin
  if (L->L != NULL)
	  delete [] L->L;

  L->L = auxL;
  (L->LDim)++;

  V[i] = 0;

// mjmarin: DONE delete V at final call
  if (topcall)
     delete [] V;

  return L;
}

