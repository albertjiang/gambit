//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Interface to sequence form classes
//
// This file is part of Gambit
// Copyright (c) 2002, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#ifndef SFG_H
#define SFG_H

#include "base/base.h"
#include "base/odometer.h"
#include "efg.h"
#include "gnarray.h"
#include "sfstrat.h"

class Sfg  {
private:
  gbtEfgGame m_efg;
  const gbtEfgSupport &efsupp;
  gbtArray<SFSequenceSet *> *sequences;
  gNArray<gbtArray<gNumber> *> *SF;  // sequence form
  gbtArray<gbtRectArray<gNumber> *> *E;   // constraint matrices for sequence form.  
  gbtArray<int> seq;
  gPVector<int> isetFlag,isetRow;
  gbtArray<gbtList<gbtEfgInfoset> > infosets;

  void MakeSequenceForm(const gbtEfgNode &, gNumber,gbtArray<int>, 
			gbtArray<gbtEfgInfoset>, gbtArray<Sequence *>);
  void GetSequenceDims(const gbtEfgNode &);

public:
  Sfg(const gbtEfgSupport &);
  virtual ~Sfg();  

  inline int NumSequences(int pl) const {return seq[pl];}
  inline int NumInfosets(int pl) const {return infosets[pl].Length();}
  inline gbtArray<int> NumSequences() const {return seq;}
  int TotalNumSequences() const;
  int NumPlayerInfosets() const;
  inline int NumPlayers() const { return m_efg.NumPlayers(); }
  
  inline gbtArray<gNumber> Payoffs(const gbtArray<int> & index) const {return *((*SF)[index]);}
  gNumber Payoff(const gbtArray<int> & index,int pl) const;

  gbtRectArray<gNumber> Constraints(int player) const {return *((*E)[player]);};
  int InfosetRowNumber(int pl, int sequence) const;
  int ActionNumber(int pl, int sequence) const;
  gbtEfgInfoset GetInfoset(int pl, int sequence) const;
  gbtEfgAction GetAction(int pl, int sequence) const;
  gbtEfgGame GetEfg(void) const { return m_efg; }
  BehavProfile<gNumber> ToBehav(const gPVector<double> &x) const;
  const Sequence* GetSequence(int pl, int seq) const {return ((*sequences)[pl])->Find(seq);}
  
  void Dump(gbtOutput &) const;

};

#endif    // SFG_H
