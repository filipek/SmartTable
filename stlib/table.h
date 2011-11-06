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
// $Id: table.h 52 2009-11-18 10:27:28Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "schema.h"
#include "row_store.h"
#include "column_store.h"
#include "store_traits.h"

#include "sqlparser.h"
#include "sqlvm.h"

namespace smart_table
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
class datatable_impl : private boost::noncopyable
{
private:

	template <typename X>
	inline static void ASSERT_ALLOWED_TYPE(void)
	{
		BOOST_STATIC_ASSERT((is_valid_column_type<X>::value));
	}

	template <typename X, typename V>
	inline void ASSERT_COLUMN_TYPE(const V& c) const
	{
		const type_t xtype( typeid(X) );
		const coldef& def = schema_[c];
		if ( def.ext_type() != xtype )
		{
			std::stringstream strm;
			strm << "Wrong type of value '" << xtype.name()
				<< "', column '" << def.name() << "' holds '"
				<< def.ext_type().name() << "'";
			throw std::bad_cast(strm.str().c_str());
		}
	}

	typedef typename store_traits<T>	traits;
	typedef typename traits::type		store_type;

	string_type			name_;
	mutable schema		schema_;
	mutable store_type	store_;

public:

	// allows for nice syntax eg datatable::ptr
	typedef typename param_<datatable_impl>::type ptr;

	datatable_impl()
	: name_(random_md5hash_fun()())
	, schema_()
	, store_(schema_) {}

	datatable_impl(const string_type& n)
	: name_(n)
	, schema_()
	, store_(schema_) {}

	datatable_impl(const string_type& n, const schema& sch)
	: name_(n)
	, schema_(sch)
	, store_(schema_) {}

	~datatable_impl()
	{
#if _DEBUG
		store_.clear();
		name_.clear();
#endif // _DEBUG
	}

	// table methods
	inline const schema& schema(void) const { return schema_; }
	inline const string_type& column_at(size_t c) { return schema_.column_at(c); }
	inline const string_type& name() const { return name_; }
	// allow empty/null table names
	inline void name(const string_type& n) { name_ = n; }

	// store methods
	inline size_t width() const { return traits::width(store_); }
	inline size_t depth() const { return traits::depth(store_); }
	inline size_t add_row() { return traits::add_row(store_); }
	inline void remove_row(size_t r) { traits::remove_row(store_,r); }
	inline void clear() { traits::clear(store_); }

	// store methods
	template <typename U>
	inline size_t add_column(const string_type& cn)
	{
		ASSERT_ALLOWED_TYPE<U>();

		return traits::add_column<U>(store_, cn);
	}

	// NOTE: V refers to a column index (name or id)
	// in methods below.

	template <typename V>
	inline void remove_column(const V& c)
	{ traits::remove_column<V>(store_, c); }

	template <typename U, typename V>
	inline void fastget(
		size_t r
		, const V& c
		, typename param_<U>::type& u) const
	{
		ASSERT_ALLOWED_TYPE<U>();
		ASSERT_COLUMN_TYPE<U>(c);

		traits::get<U,V>(store_,r,c,u);
	}

	template <typename U, typename V>
	inline void set(
		size_t r
		, const V& c
		, const typename param_<U>::type& u)
	{
		ASSERT_ALLOWED_TYPE<U>();
		ASSERT_COLUMN_TYPE<U>(c);

		traits::set<U,V>(store_,r,c,u);
	}

	template <typename U, typename V>
	inline typename param_<U>::type get(size_t r, const V& c) const
	{
		ASSERT_ALLOWED_TYPE<U>();
		ASSERT_COLUMN_TYPE<U>(c);

		typedef typename param_<U>::type result_type;

		result_type u;
		traits::get<U,V>(store_,r,c,u);

		// incurs a copy, but this get syntax is nicer
		// and for most uses the performance hit is small
		return u;
	}
	template <typename V>
	inline string_type get_string(size_t r, const V& c) const
	{
		const coldef& def = schema_[c];
		return def.lexcast( store_(r,c) );
	}
	template <typename V>
	inline void set_string(size_t r, const V& c, const string_type& s)
	{
		byte_type* cellp = store_(r,c);
		const coldef& def = schema_[c];
		if ( ! def.is_pod() )
			def.destruct(cellp);
		def.lexuncast(s, cellp);
	}
	template <typename U, typename V, typename X>
	inline size_type find(
		const V& c
		, const typename param_<U>::type& u
		, X& rowset) const
	{
		ASSERT_ALLOWED_TYPE<U>();
		ASSERT_COLUMN_TYPE<U>(c);

		return traits::find<U,V,X>(store_,c,u,rowset);
	}
	template <typename X>
	inline size_type select(const string_type& query, X& rowset) const
	{
		return traits::select<X>(store_,query,rowset);
	}

	// operator == with recursion and pointer dereference
	inline bool operator==(const typename datatable_impl::ptr& rhs) const
	{
		if ( rhs.get() )
			return (*this == *rhs);

		// we cannot be the same as a non-existant (null) table
		return false;
	}
	inline bool operator==(const datatable_impl& rhs) const
	{
		if ( this == &rhs )
			return true;

		const size_t nrows = depth();
		if ( nrows != rhs.depth() )
			return false;

		// first compare schemas
		if (schema_ !=  rhs.schema_)
			return false;

		const size_t ncols = width();

		// and if schemas are the same - compare cells one by one
		// recursing through datatable ptr's as required.
		for (size_t r = 0; r < nrows; ++r)
		{
			for (size_t c = 0; c < ncols; ++c)
			{
				const coldef& def = schema_[c];

				if ( def.is_table() )
				{
					const datatable_impl::ptr dtp =
						get<datatable_impl>(r,c);
					
					const datatable_impl::ptr dtrhsp =
						rhs.get<datatable_impl>(r,c);

					// this is the recursive step
					if ( !(*dtp == *dtrhsp) )
						return false;
				}
				else
				{
					const byte_type* cellp = store_(r,c);
					const byte_type* cellrhsp = rhs.store_(r,c);

					if ( ! def.compare(cellp, cellrhsp) )
						return false;
				}
			}
		}

		return true;
	}
	inline bool operator!=(const typename datatable_impl::ptr& rhs) const
	{
		if ( ! rhs.get() )
			return false;

		return !(*this == *rhs);
	}
	inline bool operator!=(const datatable_impl& rhs) const
	{
		return !(*this == rhs);
	}
};

typedef datatable_impl<row_store>		rowtable;
typedef datatable_impl<column_store>	columntable;
typedef rowtable						datatable;

} // namespace smart_table

///////////////////////////////////////////////////////////////////////////////
template <typename T>
inline bool operator==(
	const typename smart_table::datatable_impl<T>::ptr& lhs
	, const smart_table::datatable_impl<T>& rhs)
{
	if ( ! lhs.get() )
		return false;

	// first do a shallow pointer test
	if ( lhs.get() == &rhs )
		return true;

	// this is a recursive table test
	return (*lhs == rhs);
}
