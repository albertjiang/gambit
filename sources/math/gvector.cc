//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Instantiation of vector types
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

#include "gvector.imp"
#include "gnumber.h"
#include "complex.h"

template class gVector<int>;
template class gVector<long>;
template class gVector<double>;
template class gVector<gInteger>;
template class gVector<gRational>;
template class gVector<gComplex>;
template class gVector<gNumber>;

template gbtOutput & operator<< (gbtOutput&, const gVector<int>&);
template gbtOutput & operator<< (gbtOutput&, const gVector<long>&);
template gbtOutput & operator<< (gbtOutput&, const gVector<double>&);
template gbtOutput & operator<< (gbtOutput&, const gVector<gInteger>&);
template gbtOutput & operator<< (gbtOutput&, const gVector<gRational>&);
template gbtOutput & operator<< (gbtOutput&, const gVector<gComplex>&);
template gbtOutput & operator<< (gbtOutput&, const gVector<gNumber>&);

template gVector<gDouble> TogDouble(const gVector<gRational>&);
template gVector<gDouble> TogDouble(const gVector<double>&);
template gVector<gDouble> TogDouble(const gVector<gDouble>&);

#include "base/glist.imp"

template class gbtList<gVector<double> >;
template class gbtList<gVector<gRational> >;
template class gbtList<gVector<gComplex> >;
template gbtOutput& operator << (gbtOutput& output, 
			       const gbtList<gVector<gRational> >&);
