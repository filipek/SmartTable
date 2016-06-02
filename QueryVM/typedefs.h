#pragma once

#include <boost/config/warning_disable.hpp>

#include <boost/any.hpp>

#include <boost/mpl/not.hpp>

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace smart_table
{
	namespace query_parser
	{
		////////////////////////////////////////////////////////////////////////////
		typedef std::string				string_type;

		typedef boost::any				sql_value;
		typedef std::vector<sql_value>	program;

		///////////////////////////////////////////////////////////////////////////////
		template <typename T>
		inline static T cast_(const sql_value& v)
		{
			static_assert( boost::mpl::not_< boost::is_same<T, sql_value>>::type::value, "Bad cast" );
	
			return boost::any_cast<T>(v);
		}
	}
}
