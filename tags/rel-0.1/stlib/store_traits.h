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
// $Id: store_traits.h 56 2011-11-05 20:51:21Z filipek $
//

#include "schema.h"
#include "sqlvar.h"

namespace smart_table
{

namespace schema_tools
{

using namespace sql_vm;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
class store_traits
{
public:

	typedef typename T	type;

#ifdef CONST_STORE_DEF
#	error Dummy Store defined - this can't be
#else
#	define STORE_DEF type& _store_
#	define CONST_STORE_DEF const type& _store_
#	define STORE _store_
#endif // DUMMY_STORE

	inline static size_t width(CONST_STORE_DEF) { return STORE.width(); }
	inline static size_t depth(CONST_STORE_DEF) { return STORE.depth(); }

	inline static size_t add_row(STORE_DEF) { return STORE.add_row(); }
	inline static void remove_row(STORE_DEF, size_t r) { STORE.remove_row(r); }
	inline static void clear(STORE_DEF) { STORE.clear(); }

	template <typename U>
	inline static size_t add_column(STORE_DEF, const string_type& cn)
	{ return STORE.add_column<U>(cn); }

	template <typename V>
	inline static void remove_column(STORE_DEF, const V& c)
	{ STORE.remove_column(c); }

	template <typename U, typename V>
	inline static void get(
		CONST_STORE_DEF
		, size_t r
		, const V& c
		, typename param_<U>::type& u)
	{
		typedef typename column_<U>::type cell_type;
		typedef typename column_<U>::get_f get_fun;

      static get_fun get_f;

		const byte_type* p = STORE(r,c);
		get_f(p,u);
	}
	template <typename U, typename V>
	inline static void set(
		STORE_DEF
		, size_t r
		, const V& c
		, const typename param_<U>::type& u)
	{
		typedef typename column_<U>::type cell_type;
		typedef typename column_<U>::set_f set_fun;

      static set_fun set_f;

		byte_type* p = STORE(r,c);
		set_f(p,u);
	}
	template <typename U, typename V, typename X>
	inline static size_type find(
		CONST_STORE_DEF
		, const V& c
		, const typename param_<U>::type& x
		, X& rowset)
	{
		typedef typename column_<U>::type cell_type;
		typedef typename column_<U>::set_f set_fun;
		typedef typename mem_ref<cell_type>::type ref_fun;
		
      static ref_fun ref_f;
      static set_fun set_f;

		// coerce the x value into a cell value,
		// using the correct (cell's) set functor. this
		// makes the actual table scan below easier.
		cell_type xc;
		set_f(reinterpret_cast<byte_type*>(&xc), x);

		const size_t nrows = STORE.depth();
		const coldef& def = STORE.schema_[c];

		for (size_t r = 0; r < nrows; ++r)
		{
			const byte_type* p = STORE(r,c);
			const cell_type& cell = ref_f(p);

			if ( xc == cell )
				rowset.push_back(r);
		}
		return rowset.size();
	}
	template <typename X>
	inline static size_type select(
		CONST_STORE_DEF
		, const string_type& query
		, X& rowset)
	{
		const size_t ncols = STORE.width();
		const size_t nrows = STORE.depth();
		if ( ncols == 0 || nrows == 0 || query.size() == 0)
			return 0;

		variables::symtab meta_syms;

		for (size_t c = 0; c < ncols; ++c)
		{
			const coldef& coldef = STORE.schema_[c];

			// cannot run queries on datatable columns, so we
			// do not even insert their names as sql symbols.
			if ( ! coldef.is_table() )
			{
				// the value will tell us (after parsing)
				// which symbols were used. we can then remove
				// these columns from the scan below for performance.
				meta_syms.insert(
					std::make_pair(coldef.name(), sqlvar(0)) );
			}
		}

		variables vars(meta_syms);
		vmachine mach;
		sqlvarvec code;
		calculator calc(code, vars);

		string_type errmsg;
		if (! compile(calc, query, errmsg) )
			throw std::exception(errmsg.c_str());

		// remove columns which were not referred
		// to in the query, for performance.
		{
			typedef std::list<std::string> unused_list;

			unused_list unused_cols;
			for (variables::symtab::iterator i = meta_syms.begin();
				i != meta_syms.end(); ++i)
			{
				if ( cast_<int>(i->second) == 0 )
					unused_cols.push_back(i->first);
			}
			for (unused_list::iterator li = unused_cols.begin();
				li != unused_cols.end(); ++li)
			{
				meta_syms.erase(*li);
			}
		}

		// now for each row exec the query
		variables::symtab syms;
		for (size_t r = 0; r < nrows; ++r)
		{
			for (variables::symtab::iterator c = meta_syms.begin();
				c != meta_syms.end(); ++c)
			{
				const string_type& colname = c->first;
				const coldef& def = STORE.schema_[colname];
				
				const byte_type* cp = STORE(r,def.id());
				const sqlvar v( def.sqlvar(cp) );

				syms[colname] = v;
			}

			mach.execute(code, syms);

			if ( cast_<bool>(mach.top()) )
				rowset.push_back(r);
		}

		return rowset.size();
	}

#undef STORE_DEF
#undef CONST_STORE_DEF
#undef STORE

};

} // namespace schema_tools

} // namespace smart_table
