//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Declaration of normal form game representation
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

#ifndef NFG_H
#define NFG_H

#include "base/base.h"
#include "math/gnumber.h"
#include "strategy.h"
#include "player.h"
#include "outcome.h"

class StrategyProfile;
class Lexicon;
class efgGame;

class Nfg  {
friend class Lexicon;
friend class NfgFileReader;
friend void SetEfg(Nfg *, const efgGame *);
protected:
  mutable bool m_dirty;
  mutable long m_revision;
  mutable long m_outcome_revision;
  gText title, comment;
  gArray<int> dimensions;

  gArray<gbt_nfg_player_rep *> players;
  gBlock<gbt_nfg_outcome_rep *> outcomes;

  gArray<gbt_nfg_outcome_rep *> results;

  const efgGame *efg;

  // PRIVATE AUXILIARY MEMBER FUNCTIONS
  void IndexStrategies(void);
  int Product(const gArray<int> &);

  void BreakLink(void);


public:
  // CONSTRUCTORS, DESTRUCTORS, CONSTRUCTIVE OPERATORS
  Nfg(const gArray<int> &dim);
  Nfg(const Nfg &b);
  ~Nfg();
    
  // GENERAL DATA ACCESS AND MANIPULATION  
  void SetTitle(const gText &s);
  const gText &GetTitle(void) const;

  void SetComment(const gText &);
  const gText &GetComment(void) const;

  long RevisionNumber(void) const { return m_revision; }
  bool IsDirty(void) const { return m_dirty; }
  void SetIsDirty(bool p_dirty) { m_dirty = p_dirty; }

  void WriteNfgFile(gOutput &, int) const;

  // PLAYERS AND STRATEGIES
  int NumPlayers(void) const;
  gbtNfgPlayer GetPlayer(int i) const;

  int NumStrats(int pl) const;
  const gArray<int> &NumStrats(void) const  { return dimensions; }
  int ProfileLength(void) const;

  // OUTCOMES
  gbtNfgOutcome NewOutcome(void);
  void DeleteOutcome(gbtNfgOutcome);

  gbtNfgOutcome GetOutcomeId(int p_id) const;
  int NumOutcomes(void) const   { return outcomes.Length(); }

  void SetOutcome(const gArray<int> &profile, const gbtNfgOutcome &outcome);
  gbtNfgOutcome GetOutcome(const gArray<int> &profile) const;
  void SetOutcome(const StrategyProfile &p, const gbtNfgOutcome &outcome);
  gbtNfgOutcome GetOutcome(const StrategyProfile &p) const;

  void SetOutcomeIndex(int index, const gbtNfgOutcome &outcome);
  gbtNfgOutcome GetOutcomeIndex(int index) const   { return results[index]; }

  void SetPayoff(gbtNfgOutcome, int pl, const gNumber &value);
  gNumber Payoff(gbtNfgOutcome, int pl) const;
  gNumber Payoff(gbtNfgOutcome p_outcome, gbtNfgPlayer p_player) const 
    { return Payoff(p_outcome, p_player.GetId()); }

  void InitPayoffs(void) const;

    // defined in nfgutils.cc
  friend void RandomNfg(Nfg &);
  friend bool IsConstSum(const Nfg &);
  friend gNumber MinPayoff(const Nfg &, int pl = 0);
  friend gNumber MaxPayoff(const Nfg &, int pl = 0);

  const efgGame *AssociatedEfg(void) const   { return efg; }
};

Nfg *ReadNfgFile(gInput &);
Nfg *ReadComLabSfg(gInput &);
void WriteComLabSfg(gOutput &, Nfg *);

#include "nfstrat.h"
#include "mixed.h"

extern Nfg *CompressNfg(const Nfg &nfg, const gbtNfgSupport &S);

#endif    // NFG_H
