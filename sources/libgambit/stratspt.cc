//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Implementation of strategy classes for normal forms
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

#include "libgambit.h"

namespace Gambit {

//===========================================================================
//                          class StrategySupport
//===========================================================================

//---------------------------------------------------------------------------
//                               Lifecycle
//---------------------------------------------------------------------------

StrategySupport::StrategySupport(const Game &p_nfg) 
  : m_nfg(p_nfg)
{ 
  for (int pl = 1; pl <= p_nfg->NumPlayers(); pl++) {
    m_support.Append(Array<GameStrategy>());
    for (int st = 1; st <= p_nfg->GetPlayer(pl)->NumStrategies(); st++) {
      m_support[pl].Append(p_nfg->GetPlayer(pl)->GetStrategy(st));
    }
  }
}

//---------------------------------------------------------------------------
//                          General information
//---------------------------------------------------------------------------

Array<int> StrategySupport::NumStrats(void) const
{
  Array<int> a(m_support.Length());

  for (int pl = 1; pl <= a.Length(); pl++) {
    a[pl] = m_support[pl].Length();
  }
  return a;
}

int StrategySupport::ProfileLength(void) const
{
  int total = 0;
  for (int pl = 1; pl <= m_nfg->NumPlayers();
       total += m_support[pl++].Length());
  return total;
}

bool StrategySupport::IsSubsetOf(const StrategySupport &p_support) const
{
  if (m_nfg != p_support.m_nfg)  return false;
  for (int pl = 1; pl <= m_support.Length(); pl++) {
    if (m_support[pl].Length() > p_support.m_support[pl].Length()) {
      return false;
    }
    else {
      for (int st = 1; st <= m_support[pl].Length(); st++) {
	if (!p_support.m_support[pl].Contains(m_support[pl][st])) {
	  return false;
	}
      }
    }
  }
  return true;
}

//---------------------------------------------------------------------------
//                        Modifying the support
//---------------------------------------------------------------------------

void StrategySupport::AddStrategy(GameStrategy s)
{ 
  Array<GameStrategy> &sup = m_support[s->GetPlayer()->GetNumber()];
  if (sup.Contains(s))  return;

  int index;
  for (index = 1; 
       index <= sup.Length() && sup[index]->GetNumber() < s->GetNumber(); 
       index++);
  sup.Insert(s, index);
}

bool StrategySupport::RemoveStrategy(GameStrategy s) 
{ 
  Array<GameStrategy> &sup = m_support[s->GetPlayer()->GetNumber()];
  if (!sup.Contains(s)) return false;
  if (sup.Contains(s) && sup.Length() == 1)  return false;
  sup.Remove(sup.Find(s));
  return true;
} 


//---------------------------------------------------------------------------
//                 Identification of dominated strategies
//---------------------------------------------------------------------------

bool StrategySupport::Dominates(GameStrategy s, 
				GameStrategy t, 
				bool strong) const
{
  Game n = GetGame();

  StrategyIterator A(*this, s->GetPlayer()->GetNumber(), GetIndex(s));
  StrategyIterator B(*this, t->GetPlayer()->GetNumber(), GetIndex(t));

  if (strong)  {
    do  {
      Rational ap = ((A.GetOutcome()) ? 
			A.GetOutcome()->GetPayoff<Rational>(s->GetPlayer()->GetNumber()) : Rational(0));
      Rational bp = ((B.GetOutcome()) ? 
			B.GetOutcome()->GetPayoff<Rational>(s->GetPlayer()->GetNumber()) : Rational(0));

      if (ap <= bp)  {
	return false;
      }
      A.NextContingency();
    } while (B.NextContingency());
	
    return true;
  }

  bool equal = true;
  
  do   {
    Rational ap = ((A.GetOutcome()) ? 
		      A.GetOutcome()->GetPayoff<Rational>(s->GetPlayer()->GetNumber()) : Rational(0));
    Rational bp = ((B.GetOutcome()) ? 
		      B.GetOutcome()->GetPayoff<Rational>(s->GetPlayer()->GetNumber()) : Rational(0));

    if (ap < bp) { 
      return false;
    }
    else if (ap > bp) {
      equal = false;
    }
    A.NextContingency();
  } while (B.NextContingency());

  return (!equal);
}


bool StrategySupport::IsDominated(GameStrategy s, bool strong) const
{
  for (int i = 1; i <= NumStrats(s->GetPlayer()->GetNumber()); i++) {
    if (GetStrategy(s->GetPlayer()->GetNumber(), i) != s &&
	Dominates(GetStrategy(s->GetPlayer()->GetNumber(), i), s, strong)) {
      return true;
    }
  }
  return false;
}

bool StrategySupport::Undominated(StrategySupport &newS, int pl, bool strong,
				  std::ostream &tracefile) const
{
  Array<int> set(NumStrats(pl));
  int i;
  for (i = 1; i <= set.Length(); i++)
    set[i] = i;

  int min, dis;
  double d1,d2;
  d1 = (double)(pl-1) / (double) GetGame()->NumPlayers();
  d2 = (double)pl / (double) GetGame()->NumPlayers();
  for (min = 0, dis = NumStrats(pl) - 1; min <= dis; )  {
    int pp;
    double s1 = (double)min/(double)(dis+1);
    for (pp = 0;
	 pp < min && !Dominates(GetStrategy(pl, set[pp+1]),
				GetStrategy(pl, set[dis+1]), strong); 
	 pp++);
    if (pp < min)
      dis--;
    else  {
      int foo = set[dis+1];
      set[dis+1] = set[min+1];
      set[min+1] = foo;

      for (int inc = min + 1; inc <= dis; )  {
	if (Dominates(GetStrategy(pl, set[min+1]),
		      GetStrategy(pl, set[dis+1]), strong)) { 
	  tracefile << GetStrategy(pl, set[dis+1])->GetNumber() << " dominated by " << GetStrategy(pl, set[min+1])->GetNumber() << '\n';
	  dis--;
	}
	else if (Dominates(GetStrategy(pl, set[dis+1]),
			   GetStrategy(pl, set[min+1]), strong)) { 
	  tracefile << GetStrategy(pl, set[min+1])->GetNumber() << " dominated by " << GetStrategy(pl, set[dis+1])->GetNumber() << '\n';
	  foo = set[dis+1];
	  set[dis+1] = set[min+1];
	  set[min+1] = foo;
	  dis--;
	}
	else  {
	  foo = set[dis+1];
	  set[dis+1] = set[inc+1];
	  set[inc+1] = foo;
	  inc++;
	}
      }
      min++;
    }
  }
    
  if (min + 1 <= NumStrats(pl))   {
    for (i = min + 1; i <= NumStrats(pl); i++)
      newS.RemoveStrategy(GetStrategy(pl, set[i]));
    
    return true;
  }
  else
    return false;
}

StrategySupport 
StrategySupport::Undominated(bool strong, const Array<int> &players,
			     std::ostream &tracefile) const
{
  StrategySupport newS(*this);
  
  for (int i = 1; i <= players.Length(); i++)   {
    int pl = players[i];
    tracefile << "Dominated strategies for player " << pl << ":\n";
    Undominated(newS, pl, strong, tracefile);
  }

  return newS;
}

} // end namespace Gambit