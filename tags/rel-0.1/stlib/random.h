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
// $Id: random.h 52 2009-11-18 10:27:28Z filipek $
//

#include "stlib/stlib.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace smart_table
{

namespace tools
{

using namespace boost::gregorian;
using namespace boost::posix_time;

///////////////////////////////////////////////////////////////////////////////
struct random
{
	inline static std::string string(size_type maxlen)
	{
		static const char alpha[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		// remember to exclude the terminating null
		static const size_type alphalen =
			(sizeof(alpha)/sizeof(alpha[0])) - 1;

		size_type len;
		do
		{
			len = positive(0) % maxlen;
		}
		while (len == 0);

		std::string s(len, 'x');

		for (size_type i = 0; i < len; ++i)
		{
			const size_type x = ((size_type) rand()) % alphalen;
			s[i] = alpha[x];
		}

		return s;
	}
	inline static std::wstring wstring(size_type maxlen)
	{
		static const wchar_t walpha[] =
			L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		// remember to exclude the terminating null
		static const size_type walphalen =
			(sizeof(walpha)/sizeof(walpha[0])) - 1;

		size_type len;
		do
		{
			len = positive(0) % maxlen;
		}
		while (len == 0);

		std::wstring ws(len, L'x');

		for (size_type i = 0; i < len; ++i)
		{
			size_type wal = walphalen;
			const size_type x = ((size_type) rand()) % walphalen;
			ws[i] = walpha[x];
		}

		return ws;
	}
	inline static size_t positive(size_type)
	{
		const int r = integer(0);
		const bool is_neg = (r < 0);
		return (size_t) (is_neg ? -r : r );
	}
	inline static long integer(size_type)
	{
		return int_random_fun()();
	}
	// NOTE: returns reals from the interval [0,1).
	inline static double real(size_type)
	{
		return real_random_fun()();
	}
	inline static boost::gregorian::date date(size_type)
	{
		typedef
			boost::date_time::month_functor<boost::gregorian::date>
			add_month;
	
		const int is_neg = int_random_fun()() % 2;
		const int months = int_random_fun()() % 36;
		const add_month mf( is_neg ? -months : months );
		const boost::gregorian::date d( day_clock::local_day() );

		return d + mf.get_offset(d);
	}
	inline static ptime datetime(size_type)
	{
		const ptime now = second_clock::local_time();
		const time_duration randh = hours(int_random_fun()()%4096 + 123);
		const int is_neg = int_random_fun()() % 2;
		
		return ( is_neg ? now - randh : now + randh );
	}

	template <typename T> static T randval(size_type)
		{ BOOST_STATIC_ASSERT( false ); throw; }
	template <> static int randval<int>(size_type)
		{ return random::integer(0); }
	template <>	static double randval<double>(size_type)
		{ return random::real(0); }
	template <>	static boost::gregorian::date randval<boost::gregorian::date>(size_type)
		{ return random::date(0); }
	template <>	static ptime randval<ptime>(size_type)
		{ return random::datetime(0); }
	template <>	static std::string randval<std::string>(size_type len)
		{ return random::string(len); }
	template <>	static std::wstring randval<std::wstring>(size_type len)
		{ return random::wstring(len); }

	template <typename T>
	inline static std::vector<T> datavec(size_type size)
	{
		std::vector<T> v;
		
		for (size_type i = 0; i<size; ++i)
		{
			// just the strings use the rlen so don't
			// make them too long
			const size_type rlen = 7 + positive() % 256;
			v.push_back( randval<T>(rlen) );
		}

		return v;
	}
	//inline static void datatable(
	//	datatable& dt
	//	, size_type rows
	//	, size_type cols)
	//{
	//	for (size_type c = 0; c < cols; ++c)
	//	{
	//		const size_type randtn =
	//			(size_type) int_random_fun()() % datatable::allowed_type_count;

	//		const string_type name = random::randval<string_type>(8);

	//		switch (randtn)
	//		{
	//			case 0:
	//				dt.add_column<bool>(name);
	//				break;
	//			case 1:
	//				dt.add_column<int>(name);
	//				break;
	//			case 2:
	//				dt.add_column<long>(name);
	//				break;
	//			case 3:
	//				dt.add_column<double>(name);
	//				break;
	//			case 4:
	//				dt.add_column<std::string>(name); 
	//				break;
	//			case 5:
	//				dt.add_column<std::wstring>(name);
	//				break;
	//			case 6:
	//				dt.add_column<boost::gregorian::date>(name);        
	//				break;
	//			case 7:
	//				dt.add_column<boost::posix_time::ptime>(name);        
	//				break;
	//			case 8:
	//				dt.add_column<smart_table::datatable>(name);        
	//				break;
	//		}
	//	}
	//}
};

} // namespace tools

} // namespace smart_table
