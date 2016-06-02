#pragma once

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>

#include "typedefs.h"

namespace smart_table
{
	namespace query_parser
	{
		using namespace std;
		using namespace BOOST_SPIRIT_CLASSIC_NS;

		///////////////////////////////////////////////////////////////////////////////
		struct identifiers : public symbols<sql_value>
		{
			typedef std::map<string_type, sql_value> symbol_table;

		private:

			symbol_table& syms;

		public:

			identifiers(symbol_table& s) : syms(s)
			{
				// NOTE: here we are ignoring map values
				// as we want to populate the values later
				// when the vm is executing. for now we copy
				// the variable name as the value. this will
				// then be used to look-up a table for a value.
				for (auto i = syms.begin(); i != syms.end(); ++i)
				{
					const string_type& istr = i->first;
					add(istr.c_str(), istr);
				}
			}

			inline size_t count() const { return syms.size(); }
		};
	}
}