#include "stdafx.h"
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
// $Id: astab.cpp 46 2009-11-16 21:23:58Z filipek $
//
#define BOOST_TEST_MODULE smart_table test
#include <boost/test/unit_test.hpp>

#include "stlib/stlib.h"

#include "test_tools.h"
#include "test1.h"

using namespace smart_table;

#ifdef DUMP_SIZEOF
#	error DUMP_SIZEOF cannot already be defined!
#else // DUMP_SIZEOF
#	define DUMP_SIZEOF(t) std::cout << typeid(t).name() << " => sizeof(" << #t << ") = " << sizeof(t) << "\n"
#endif //

#if 0
BOOST_AUTO_TEST_CASE( varblob_size_test )
{
	{
		DUMP_SIZEOF(std::string);
		DUMP_SIZEOF(char);
		DUMP_SIZEOF(std::wstring);
		DUMP_SIZEOF(wchar_t);
		DUMP_SIZEOF(double);
		DUMP_SIZEOF(long);
		DUMP_SIZEOF(int);
		DUMP_SIZEOF(bool);
		DUMP_SIZEOF(date);
		DUMP_SIZEOF(ptime);
		DUMP_SIZEOF(datatable);
	}
	{
		typedef varblob_<char>::type	byte_blob;
		typedef varblob_<wchar_t>::type	wide_blob;

		// sanity check
		BOOST_MPL_ASSERT(( boost::is_same<byte_blob, column_<std::string>::type > ));
		BOOST_MPL_ASSERT(( boost::is_same<wide_blob, column_<std::wstring>::type > ));

		DUMP_SIZEOF(byte_blob);
		DUMP_SIZEOF(wide_blob);

		byte_blob bb;
		wide_blob wbb;
	}
	{
		typedef column_<unsigned int>::type		uint_column;
		typedef column_<int>::type				int_column;
		typedef column_<long>::type				long_column;
		typedef column_<float>::type			float_column;
		typedef column_<double>::type			double_column;
		typedef column_<std::string>::type		string_column;
		typedef column_<std::wstring>::type		wstring_column;

		DUMP_SIZEOF(uint_column);
		DUMP_SIZEOF(int_column);
		DUMP_SIZEOF(long_column);
		DUMP_SIZEOF(float_column);
		DUMP_SIZEOF(double_column);
		DUMP_SIZEOF(string_column);
		DUMP_SIZEOF(wstring_column);
	}
}
#endif // 0

#ifdef DUMP_SIZEOF
#	undef DUMP_SIZEOF
#endif // DUMP_SIZEOF
