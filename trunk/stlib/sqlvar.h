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
// $Id: sqlvar.h 56 2011-11-05 20:51:21Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "tools.h"

namespace smart_table
{

namespace sql_vm
{

///////////////////////////////////////////////////////////////////////////////
using namespace BOOST_SPIRIT_CLASSIC_NS;
using namespace tools;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
inline static T cast_(const sqlvar& v)
{
	BOOST_MPL_ASSERT(( boost::mpl::not_< boost::is_same<T,sqlvar> > ));
	
	// http://www.boost.org/doc/libs/1_47_0/doc/html/boost/any_cast.html
	return boost::any_cast<T>(v);
}

///////////////////////////////////////////////////////////////////////////////
template <template <class> class F>
struct sql_bin_op_
{
private:

	// a meta function which (finally) applies F to X.
	template <typename X>
	struct apply_f_
	{
	private:

		// cast_if_sqlvar_ is used as a syntax saving technique.
		// if the 2nd arg of the binary F is sqlvar it will be cast
		// to X, if not then not. this allows us to use apply_f_ for
		// other binary functions eg 'find item in list' where the 2nd
		// arg is a list/vector and cast_'ing makes no sense.
		template <typename A, typename B>
		struct cast_if_sqlvar_
		{
			inline const B& operator()(const B& b) const
			{
				typedef A dummy_type;
				return b;
			}
		};
		// partially specialize for sqlvar and cast
		template <typename A>
		struct cast_if_sqlvar_<A,sqlvar>
		{
			inline A operator()(const sqlvar& v) const
			{
				return cast_<A>(v);
			}
		};

	public:

		template <typename U>
		inline bool operator()(const sqlvar& x, const U& u) const
		{
			typedef typename F<X> bfx_type;

			static const bfx_type b_f;
         static cast_if_sqlvar_<X,U> cast_if_sqlvar_f;

			return b_f( cast_<X>(x), cast_if_sqlvar_f(u) );
		}
	};

public:

	// as per above, if U is sqlvar then both args will be cast
	// by apply_f_. if U is anything else we leave it alone.
	template <typename U>
	inline bool operator()(const sqlvar& a, const U& b) const
	{
		static apply_f_<int> apply_f_int;
		static apply_f_<double> apply_f_double;
		static apply_f_<string_type> apply_f_string;
		static apply_f_<wstring_type> apply_f_wstring;
		static apply_f_<date_type> apply_f_date;
		static apply_f_<datetime_type> apply_f_datetime;

		const type_t ta( a.type() );

		if ( ta == t_int )
			return apply_f_int(a,b);
		else if ( ta == t_double )
			return apply_f_double(a,b);
		else if ( ta == t_string )
			return apply_f_string(a,b);
		else if ( ta == t_wstring )
			return apply_f_wstring(a,b);
		else if ( ta == t_date )
			return apply_f_date(a,b);
		else if ( ta == t_datetime )
			return apply_f_datetime(a,b);

		string_type msg(__FUNCTION__);
		msg += ": left operand type unknown '";
		msg += ta.name();
		msg += '\'';
		throw std::exception(msg.c_str());
	}
};

///////////////////////////////////////////////////////////////////////////////
enum byte_code
{
   OP_NOT,		//  ! of top stack entry
   OP_AND,		//  && top two stack entries
   OP_OR,		//  || top two stack entries
   OP_EQ,		//  == top two stack entries
   OP_NE,		//  != top two stack entries
   OP_LT,		//  < top two stack entries
   OP_LE,		//  <= top two stack entries
   OP_GT,		//  > top two stack entries
   OP_GE,		//  >= top two stack entries

   OP_LIS,		// start of value list
   OP_LIE,		// end of value list
   OP_IN,		// find item (1st stack item) in list (2nd stack item)
   OP_NIN,		// same as above except we check for item NOT in list

   OP_VAL,		//  push constant value onto stack
   OP_VAR,		//  push variable name onto stack
   OP_FUN,		//  push function call onto stack
   OP_RET		//  return from the interpreter
};

///////////////////////////////////////////////////////////////////////////////
struct variables : public symbols<sqlvar, char>
{
	typedef std::map<string_type,sqlvar> symtab;

private:

	symtab& syms;

public:

	variables(symtab& s) : syms(s)
	{
		// NOTE: here we are ignoring map values
		// as we want to populate the values, later
		// when the vm is executing. for now we copy
		// the variable name as the value. this will
		// then be used to look-up a table for a value.
		symtab::const_iterator i = syms.begin();
		for (; i != syms.end(); ++i)
		{
			const string_type& istr = i->first;
			add(istr.c_str(), istr);
		}
	}

	inline size_t count() const { return syms.size(); }
	inline void used(const string_type& v) const { syms[v] = sqlvar(1); }
};

} // namespace sql_vm

} // namespace smart_table
