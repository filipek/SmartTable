#pragma once
//
// This file is part of smart_table, a library and tools implementing
// data structures which manage data (add, remove, query) in tabular form.
//
// Copyright (C) 2009 Filip Fodemski
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// More information at http://www.gnu.org/licenses/lgpl.html
//
// $Id: test_tools.h 46 2009-11-16 21:23:58Z filipek $
//

#include "stlib/random.h"

namespace smart_table
{

using namespace smart_table;

#define RANDOM_STR_BASE(x) random::string(random::positive(0)%x)
#define RANDOM_STR RANDOM_STR_BASE(1024)
#define RANDOM_WSTR_BASE(x) random::wstring(random::positive(0)%x)
#define RANDOM_WSTR RANDOM_WSTR_BASE(1024)

} // namespace smart_table
