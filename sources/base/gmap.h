//
// $Source$
// $Date$
// $Revision$
// 
// DESCRIPTION:
// Declaration of map container types
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

#ifndef GMAP_H
#define GMAP_H

#include "gmisc.h"
#include "gstream.h"

template <class K, class T> class gbtBaseMap  {
  protected:
    int length;
    T _default;
    K *keys;
    T *values;

//
// Insert a new key-value pair at a location in the arrays.
//
    T &Insert(const K &key, int where, const T &value);
//
// Remove the key-value pair at a location in the arrays, and return the
// value which was removed.
//
    T Delete(int where);

  public:
//
// This is the basic map constructor.  It initializes the map to be the
// empty map with no relations defined.
//
    gbtBaseMap(const T &d);
//
// Construct a map to have the same set of relations as another map.
// 
    gbtBaseMap(const gbtBaseMap<K, T> &);

//
// This is the map destructor.  It deletes all allocated memory, and calls
// the destructors for the keys and values which remain in the map at the
// time of its deallocation.
//
    virtual ~gbtBaseMap(); 

//
// These implement the mapping function which maps a key to a value.  If
// the map from a key to a value is not defined, a mapping will be defined
// from the key to the default value.  The IsDefined() member can be used
// to determine whether a mapping is defined.
//
// <note> If the mapping is not defined for the key in the const map case,
//        the mapping returns the default value and no entry is created in
//        the map for that key.
//+grp
    virtual T &operator()(const K &key) = 0;
    virtual T operator()(const K &key) const = 0;
//-grp

    virtual T &Lookup(const K &key) = 0;

//
// These are the equality and assignment operators for this and all derived
// classes.
//+grp
    int operator==(const gbtBaseMap &M) const;
    int operator!=(const gbtBaseMap &M) const;
    gbtBaseMap<K, T> &operator=(const gbtBaseMap &M);
//-grp

//
// Returns the default value for the map
//+grp    
    T &Default(void);
    const T &Default(void) const;
//-grp

//
// Returns the number of mappings defined in the map
//
    int Length(void) const;

//
// Returns nonzero if the key has a mapping defined in the map
//
    virtual int IsDefined(const K &key) const = 0;

//
// These member functions implement adding and removing mapping from the map
//+grp
    virtual void Define(const K &key, const T &value) = 0;
    virtual T Remove(const K &key) = 0;
//-grp

//
// Prints out the contents of a map (for debugging purposes)
//
    void Dump(gbtOutput &) const;
};

//
// <category lib=glib sect=Containers>
//
// The gbtOrdMap is an ordered map.  That is, the index class has all the
// usual ordering operators defined upon it (==, !=, <, <=, >, >=).  These
// are used to sort the map by keys, thus making search-type operations
// logarithmic instead of linear.  This is a particularly large improvement
// when using keys which are costly to compare
//
template <class K, class T> class gbtOrdMap : public gbtBaseMap<K, T>  {
  private:
    int Locate(const K &key) const;

  public:
//
// Construct an ordered map with no mappings and the given default value.
//
    gbtOrdMap(const T &d);
//
// Construct an ordered map with the same key-value mappings as another
// ordered map.
//
    gbtOrdMap(const gbtOrdMap<K, T> &m);

    virtual ~gbtOrdMap();

//
// These implement the mapping function which maps a key to a value.  If
// the map from a key to a value is not defined, a mapping will be defined
// from the key to the default value.  The IsDefined() member can be used
// to determine whether a mapping is defined.
//
// <note> If the mapping is not defined for the key in the const map case,
//        the mapping returns the default value and no entry is created in
//        the map for that key.
//+grp
    T &operator()(const K &key);
    T operator()(const K &key) const;
//-grp

    T &Lookup(const K &key);

//
// Return nonzero exactly when the key has a defined mapping in the map
//
    int IsDefined(const K &key) const;

//
// Define a new key-value relation.  If the key already exists in the map,
// the new value overwrites the old value; otherwise, a new relation is
// created.
//
    void Define(const K &key, const T &value);

//
// Remove the mapping for a key from the relation, and return the value
// to which the key was formerly mapped.  If the key does not have a defined
// mapping, has no effect on the contents of the map, and returns the
// 
    T Remove(const K &key);
};

#endif   // GMAP_H
