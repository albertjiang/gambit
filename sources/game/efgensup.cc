//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Enumerate undominated subsupports
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

#include "efgensup.h"

//
// These two static functions were formerly members of the class
// gbtAllActionIterator.  They were removed in converting that class
// to a more general iterator, since these aren't truly appropriate
// for member functions.
// They still need better names, though!
//
static bool
DeletionsViolateActiveCommitments(gbtAllActionIterator &cursor,
				  const gbtEfgSupportWithActiveInfo *S,
				  const gbtList<gbtEfgInfoset> *infosetlist)
{
  for (int i = 1; i <= infosetlist->Length(); i++) {
    gbtEfgInfoset infoset = (*infosetlist)[i];
    if (infoset.GetPlayer().GetId() < cursor.GetPlayerId() ||
	( infoset.GetPlayer().GetId() == cursor.GetPlayerId() &&
	  infoset.GetId() < cursor.GetInfosetId()) )
      if (S->NumActions(infoset) > 0)
	return true;
    if (infoset.GetPlayer().GetId() == cursor.GetPlayerId() &&
	infoset.GetId() == cursor.GetInfosetId() )
      for (int act = 1; act < cursor.GetActionId(); act++)
	if ( S->Contains(infoset.GetPlayer().GetId(),
			 infoset.GetId(),
			 act) )
	  return true;
  }
  return false;
}


static bool
InfosetGuaranteedActiveByPriorCommitments(gbtAllActionIterator &cursor,
					  const gbtEfgSupportWithActiveInfo *S,
					  gbtEfgInfoset infoset)
{
  for (int i = 1; i <= infoset.NumMembers(); i++) {
    gbtEfgNode current = infoset.GetMember(i);
    if (current == S->GetGame().GetRoot()) {
      return true;
    }
    else
      while (S->Contains(current.GetPriorAction()) &&
	     cursor.IsSubsequentTo(current.GetPriorAction())) {
	current = current.GetParent();
	if (current == S->GetGame().GetRoot()) {
	  return true;
	}
      }
  }
  return false;
}

// We build a series of functions of increasing complexity.  The
// final one, which is our goal, is the undominated support function.
// We begin by simply enumerating all subsupports.

void AllSubsupportsRECURSIVE(const gbtEfgSupport *s,
			     gbtEfgSupportWithActiveInfo *sact,
			     gbtAllActionIterator *c,
			     gbtList<const gbtEfgSupport> *list)
{ 
  (*list) += *sact;

  gbtAllActionIterator c_copy(*c);

  do {
    if ( sact->Contains(c_copy.GetAction()) ) {
      sact->RemoveAction(c_copy.GetAction());
      AllSubsupportsRECURSIVE(s,sact,&c_copy,list);
      sact->AddAction(c_copy.GetAction());
    }
  } while (c_copy.GoToNext()) ;
}

gbtList<const gbtEfgSupport> AllSubsupports(const gbtEfgSupport &S)
{
  gbtList<const gbtEfgSupport> answer;
  gbtEfgSupportWithActiveInfo SAct(S);
  gbtAllActionIterator cursor(S);

  AllSubsupportsRECURSIVE(&S,&SAct,&cursor,&answer);

  return answer;
}


// Subsupports of a given support are _path equivalent_ if they
// agree on every infoset that can be reached under either, hence both,
// of them.  The next routine outputs one support for each equivalence
// class.  It is not for use in solution routines, but is instead a 
// prototype of the eventual path enumerator, which will also perform
// dominance elimination.

void AllInequivalentSubsupportsRECURSIVE(const gbtEfgSupport *s,
					 gbtEfgSupportWithActiveInfo *sact,
					 gbtAllActionIterator *c,
					 gbtList<const gbtEfgSupport> *list)
{ 
  if (sact->HasActiveActionsAtActiveInfosetsAndNoOthers()) {
    (*list) += *sact;
  }

  gbtAllActionIterator c_copy(*c);

  do {
    if ( sact->Contains(c_copy.GetAction()) ) {
      gbtList<gbtEfgInfoset> deactivated_infosets;
      sact->RemoveActionReturningDeletedInfosets(c_copy.GetAction(),
						 &deactivated_infosets); 

      if (!DeletionsViolateActiveCommitments(c_copy, sact,
					     &deactivated_infosets))
	AllInequivalentSubsupportsRECURSIVE(s,sact,&c_copy,list);
      sact->AddAction(c_copy.GetAction());
    }
  } while (c_copy.GoToNext()) ;
}

gbtList<const gbtEfgSupport> AllInequivalentSubsupports(const gbtEfgSupport &S)
{
  gbtList<const gbtEfgSupport> answer;
  gbtEfgSupportWithActiveInfo SAct(S);
  gbtAllActionIterator cursor(S);

  AllInequivalentSubsupportsRECURSIVE(&S,&SAct,&cursor,&answer);

  return answer;
}

void AllUndominatedSubsupportsRECURSIVE(const gbtEfgSupport *s,
					 gbtEfgSupportWithActiveInfo *sact,
					 gbtAllActionIterator *c,
					const bool strong,
					const bool conditional,
					 gbtList<const gbtEfgSupport> *list,
					 const gbtStatus &status)
{ 
  bool abort = false;
  bool no_deletions = true;
  bool check_domination = false;
  if (sact->HasActiveActionsAtActiveInfosets()) 
    check_domination = true;
  gbtList<gbtEfgAction> deletion_list;
  gbtAllActionIterator scanner(*s);

  // First we collect all the actions that can be deleted.
  do {
    gbtEfgAction this_action = scanner.GetAction();
    bool delete_this_action = false;

    if ( sact->Contains(this_action) ) 
      if ( !sact->InfosetIsActive(this_action.GetInfoset()) ) 
	delete_this_action = true;  
      else 
	if (check_domination) 
	  if (sact->IsDominated(this_action,strong,conditional) ) 
	    delete_this_action = true;
	
    if (delete_this_action) {
      no_deletions = false;
      if (c->IsSubsequentTo(this_action)) 
	abort = true;
      else 
	deletion_list += this_action;
    }
  } while (!abort && scanner.GoToNext());

  // Now we delete them, recurse, then restore
  if (!abort && !no_deletions) {
    gbtList<gbtEfgAction> actual_deletions;
    for (int i = 1; !abort && i <= deletion_list.Length(); i++) {
      actual_deletions += deletion_list[i];
      gbtList<gbtEfgInfoset> deactivated_infosets;
      
      sact->RemoveActionReturningDeletedInfosets(deletion_list[i],
						   &deactivated_infosets); 
	
      if (DeletionsViolateActiveCommitments(*c,sact,&deactivated_infosets))
	abort = true;
    }

    if (!abort)
      AllUndominatedSubsupportsRECURSIVE(s,
					 sact,
					 c,
					 strong,
					 conditional,
					 list,
					 status);
    
    for (int i = 1; i <= actual_deletions.Length(); i++)
      sact->AddAction(actual_deletions[i]);
  }

  // If there are no deletions, we ask if it is consistent, then recurse.
  if (!abort && no_deletions) {
    if (sact->HasActiveActionsAtActiveInfosetsAndNoOthers()) {
      (*list) += *sact;
    }
    
    gbtAllActionIterator c_copy(*c);
    
    do {
      if ( sact->Contains(c_copy.GetAction()) ) {
	
	gbtList<gbtEfgInfoset> deactivated_infosets;
	sact->RemoveActionReturningDeletedInfosets(c_copy.GetAction(),
						   &deactivated_infosets); 
	
	if (!DeletionsViolateActiveCommitments(c_copy, sact,
					       &deactivated_infosets))
	  AllUndominatedSubsupportsRECURSIVE(s,
					     sact,
					     &c_copy,
					     strong,
					     conditional,
					     list,
					     status);
	sact->AddAction(c_copy.GetAction());
	
      }
    } while (c_copy.GoToNext()) ;
  }
}
  
gbtList<const gbtEfgSupport> AllUndominatedSubsupports(const gbtEfgSupport &S,
						 const bool strong,
						 const bool conditional,
						 const gbtStatus &status)
{
  gbtList<const gbtEfgSupport> answer;
  gbtEfgSupportWithActiveInfo sact(S);
  gbtAllActionIterator cursor(S);

  AllUndominatedSubsupportsRECURSIVE(&S,
				     &sact,
				     &cursor,
				     strong,
				     conditional,
				     &answer,
				     status);

  return answer;
}

void PossibleNashSubsupportsRECURSIVE(const gbtEfgSupport *s,
					    gbtEfgSupportWithActiveInfo *sact,
				            gbtAllActionIterator *c,
				            gbtList<const gbtEfgSupport> *list,
				      const gbtStatus &status)
{ 
  status.Get();

  bool abort = false;
  bool add_support = true;

  bool check_domination = false;
  if (sact->HasActiveActionsAtActiveInfosets()) 
    check_domination = true;
  gbtList<gbtEfgAction> deletion_list;
  gbtAllActionIterator scanner(*s);

  do {
    gbtEfgAction this_action = scanner.GetAction();
    bool delete_this_action = false;

    if ( sact->Contains(this_action) ) 
      if ( !sact->InfosetIsActive(this_action.GetInfoset()) )
	delete_this_action = true;  
      else
	if (check_domination) 
	  if ( sact->IsDominated(this_action,true,true) ||
	       sact->IsDominated(this_action,true,false) ) {
	    add_support = false;
	    if (InfosetGuaranteedActiveByPriorCommitments(*c, sact,
							  this_action.GetInfoset()) )
	      delete_this_action = true;
	  }
    if (delete_this_action) {
      if (c->IsSubsequentTo(this_action)) 
	abort = true;
      else 
	deletion_list += this_action;
    }
  } while (!abort && scanner.GoToNext());
  
  if (!abort) {
    gbtList<gbtEfgAction> actual_deletions;
    for (int i = 1; !abort && i <= deletion_list.Length(); i++) {
      actual_deletions += deletion_list[i];
      gbtList<gbtEfgInfoset> deactivated_infosets;
      sact->RemoveActionReturningDeletedInfosets(deletion_list[i],
						   &deactivated_infosets); 
      if (DeletionsViolateActiveCommitments(*c,sact,&deactivated_infosets))
	abort = true;
    }

    if (!abort && deletion_list.Length() > 0) 
      PossibleNashSubsupportsRECURSIVE(s,sact,c,list,status);
        
    for (int i = 1; i <= actual_deletions.Length(); i++) {
      sact->AddAction(actual_deletions[i]);
    }
  }

  if (!abort && deletion_list.Length() == 0) {

    if (add_support && sact->HasActiveActionsAtActiveInfosetsAndNoOthers()) {
      (*list) += *sact;
    }
    gbtAllActionIterator c_copy(*c);
    do {
      if ( sact->Contains(c_copy.GetAction()) ) {
	gbtList<gbtEfgInfoset> deactivated_infosets;
	sact->RemoveActionReturningDeletedInfosets(c_copy.GetAction(),
						   &deactivated_infosets); 
	if (!DeletionsViolateActiveCommitments(c_copy,sact,
					       &deactivated_infosets))
	  PossibleNashSubsupportsRECURSIVE(s,sact,&c_copy,list,status);

	sact->AddAction(c_copy.GetAction());
      }
    } while (c_copy.GoToNext()) ;
  }
}

gbtList<const gbtEfgSupport> SortSupportsBySize(gbtList<const gbtEfgSupport> &list) 
{
  gbtArray<int> sizes(list.Length());
  for (int i = 1; i <= list.Length(); i++)
    sizes[i] = list[i].NumDegreesOfFreedom();

  gbtArray<int> listproxy(list.Length());
  for (int i = 1; i <= list.Length(); i++)
    listproxy[i] = i;

  int maxsize(0);
  for (int i = 1; i <= list.Length(); i++)
    if (sizes[i] > maxsize)
      maxsize = sizes[i];

  int cursor(1);

  for (int j = 0; j < maxsize; j++) {
    int scanner(list.Length());
    while (cursor < scanner)
      if (sizes[scanner] != j)
	scanner--;
      else {
	while (sizes[cursor] == j)
	  cursor++;
	if (cursor < scanner) {
	  int tempindex = listproxy[cursor];
	  listproxy[cursor] = listproxy[scanner];
	  listproxy[scanner] = tempindex;
	  int tempsize = sizes[cursor];
	  sizes[cursor] = sizes[scanner];
	  sizes[scanner] = tempsize;
	  cursor++;
	}
      }
  }

  gbtList<const gbtEfgSupport> answer;
  for (int i = 1; i <= list.Length(); i++)
    answer += list[listproxy[i]];

  return answer;
}
  
gbtList<const gbtEfgSupport> PossibleNashSubsupports(const gbtEfgSupport &S,
					       gbtStatus &status)
{
  gbtList<const gbtEfgSupport> answer;
  gbtEfgSupportWithActiveInfo sact(S);
  gbtAllActionIterator cursor(S);

  status.SetProgress(0);
  PossibleNashSubsupportsRECURSIVE(&S,&sact,&cursor,&answer,status);
  status.SetProgress(.5);

  // At this point answer has all consistent subsupports without
  // any strong dominations.  We now edit the list, removing all
  // subsupports that exhibit weak dominations, and we also eliminate
  // subsupports exhibiting domination by currently inactive actions.

  for (int i = answer.Length(); i >= 1; i--) {
    status.SetProgress((2.0-((double)i/(double)answer.Length()))/2.0);
    status.Get();
    gbtEfgSupportWithActiveInfo current(answer[i]);
    gbtAllActionIterator crsr(S);
    bool remove = false;
    do {
      gbtEfgAction act = crsr.GetAction();
      if (current.Contains(act)) 
	for (int j = 1; j <= act.GetInfoset().NumActions(); j++) {
	  gbtEfgAction other_act = act.GetInfoset().GetAction(j);
	  if (other_act != act)
	    if (current.Contains(other_act)) {
	      if (current.Dominates(other_act,act,false,true) ||
		  current.Dominates(other_act,act,false,false)) 
		remove = true;
	    }
	    else { 
	      current.AddAction(other_act);
	      if (current.HasActiveActionsAtActiveInfosetsAndNoOthers())
		if (current.Dominates(other_act,act,false,true) ||
		    current.Dominates(other_act,act,false,false)) {
		  remove = true;
		}
	      current.RemoveAction(other_act);
	    }
      }
    } while (crsr.GoToNext() && !remove);
    if (remove)
      answer.Remove(i);
  }

  return SortSupportsBySize(answer);
}
