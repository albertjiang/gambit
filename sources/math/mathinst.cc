//
// $Source$
// $Date$
// $Revision$
// 
// DESCRIPTION:
// Instantiation of math container types
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

#include "base/garray.imp"
#include "base/gblock.imp"
#include "base/glist.imp"
#include "base/grarray.imp"
#include "base/grblock.imp"
#include "gnumber.h"
#include "complex.h"

template class gbtArray<gInteger>;
template class gbtArray<gRational>;
template class gbtArray<gComplex>;
template class gbtArray<gNumber>;
template gbtOutput &operator<<(gbtOutput &, const gbtArray<gInteger> &);
template gbtOutput &operator<<(gbtOutput &, const gbtArray<gRational> &);
template gbtOutput &operator<<(gbtOutput &, const gbtArray<gbtText> &);

template class gbtBlock<gRational>;
template class gbtBlock<gNumber>;
template gbtOutput &operator<<(gbtOutput &, const gbtBlock<gRational> &);
template gbtOutput &operator<<(gbtOutput &, const gbtBlock<gNumber> &);

template class gbtList<gRational>;
template class gbtList<gNumber>;

template class gbtRectArray<gInteger>;
template class gbtRectArray<gRational>;
template class gbtRectArray<gNumber>;
template gbtOutput &operator<<(gbtOutput &, const gbtRectArray<gInteger> &);
template gbtOutput &operator<<(gbtOutput &, const gbtRectArray<gRational> &);
template gbtOutput &operator<<(gbtOutput &, const gbtRectArray<gNumber> &);

template class gbtRectBlock<gNumber>;
