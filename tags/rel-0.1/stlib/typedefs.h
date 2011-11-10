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
// $Id: typedefs.h 56 2011-11-05 20:51:21Z filipek $
//

// we assume this is included by tools.h only and so
// we do not include anything outselves

namespace smart_table
{

///////////////////////////////////////////////////////////////////////////////
// api types

typedef char						   byte_type;
typedef size_t						   size_type;
typedef Loki::TypeInfo				type_t;

typedef std::string					string_type;
typedef std::wstring				   wstring_type;
typedef boost::gregorian::date	date_type;
typedef boost::posix_time::ptime	datetime_type;

///////////////////////////////////////////////////////////////////////////////

static const type_t t_int			( typeid(int) );
static const type_t t_double		( typeid(double) );
static const type_t t_string		( typeid(string_type) );
static const type_t t_wstring		( typeid(wstring_type) );
static const type_t t_date			( typeid(date_type) );
static const type_t t_datetime	( typeid(datetime_type) );

///////////////////////////////////////////////////////////////////////////////

// used in sqlvm
typedef boost::any					sqlvar;
typedef std::vector<sqlvar>		sqlvarvec;
typedef std::vector<sqlvar>		sqlcodevec;

} // namespace smart_table
