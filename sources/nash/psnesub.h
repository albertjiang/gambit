//
// FILE: psnesub.h -- Solve efg by subgames in pure mixed strategies
//
// $Id$
//

#ifndef PSNESUB_H
#define PSNESUB_H

#include "base/gstatus.h"
#include "subsolve.h"

class efgEnumPureNfgSolve : public SubgameSolver  {
private:
  int m_stopAfter;

  void SolveSubgame(const FullEfg &, const EFSupport &,
		    gList<BehavSolution> &, gStatus &);
  EfgAlgType AlgorithmID(void) const { return algorithmEfg_ENUMPURE_NFG; }    
  
public:
  efgEnumPureNfgSolve(int p_stopAfter);
  virtual ~efgEnumPureNfgSolve();
};

#endif   // PSNESUB_H
