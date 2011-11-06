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
// $Id: schema_tools.h 52 2009-11-18 10:27:28Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "tools.h"
#include "varblob.h"

namespace smart_table
{

namespace schema_tools
{

using namespace tools;

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct param_
{
private:

	typedef typename boost::mpl::if_c<
		size_traits<T>::is_table
		, boost::shared_ptr<T>
		, T>::type t_or_dt;

public:

	typedef t_or_dt type;

	typedef typename boost::mpl::if_c<
		size_traits<T>::is_pod
		, typename mem_get_pod<T,U>::type
		, typename mem_get<t_or_dt,U>::type
	>::type	get_f;

	typedef typename boost::mpl::if_c<
		size_traits<T>::is_pod
		, typename mem_set_pod<T,U>::type
		, typename mem_set<t_or_dt,U>::type
	>::type	set_f;
};

///////////////////////////////////////////////////////////////////////////////
template <typename U = byte_type, typename S = string_type>
struct column_fun_
{
	// use for in-place ctor and dtor
	// dispatching for column cells
	typedef boost::function<void (U* p)>					ctor_f;
	typedef boost::function<void (U* p)>					dtor_f;

	//// use for operator== on column cell contents
	typedef boost::function<bool (const U* l, const U* r)>	cmp_f;

	// use to coerce to/from string_type
	typedef boost::function<S (const U* p)>					lexcast_f;
	typedef boost::function<void (const S& s, U* p)>		lexuncast_f;

	// used to convert a cell to a sqlvar for use in sqlvm
	typedef boost::function<sqlvar (const U* p)>			sqlvar_f;
};

typedef column_fun_<> col_f;

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct column_
{
private:

	typedef typename boost::mpl::if_c<
		size_traits<T>::is_fixed_size
		, param_<T,U>
		, varblob_<T,U>
	>::type cell_type;

public:

	// unwrap the type we want from if_ result above

	typedef typename cell_type::type	type;
	typedef typename cell_type::get_f	get_f;
	typedef typename cell_type::set_f	set_f;
};

template <typename T>
class meta_col
{
private:

	// NOTE for ease of use this class
	// should retain value-copy semantics

	const type_t	ext_type_;
	const type_t	type_;
	const size_t	size_;
	const bool		is_fixed_;
	const bool		is_pod_;
	const bool		is_datatable_;

	typedef typename column_<T>::type cell_type;

/*
	DEFN_COL_FUNC(ctorx)
	inline static col_f::ctorx_f ctorx_(void)
	{
		typedef typename mem_ctorx<cell_type>::type fun_type;
		col_f::ctorx_f fp = fun_type();
		return fp;
	}
	const col_f::ctorx_f ctorxf_
*/

#define DEFN_COL_FUNC(n)\
inline static col_f:: ## n ## _f n ## _(void){\
typedef typename mem_ ## n ## <cell_type>::type fun_type;\
col_f:: ## n ## _f fp = fun_type();\
return fp;}\
const col_f:: ## n ## _f n ## f_

	DEFN_COL_FUNC(ctor);
	DEFN_COL_FUNC(dtor);
	DEFN_COL_FUNC(cmp);
	DEFN_COL_FUNC(lexcast);
	DEFN_COL_FUNC(lexuncast);
	DEFN_COL_FUNC(sqlvar);

	// private constructor ensures
	// factory creation is required

	meta_col()
	: type_(typeid(cell_type))
	, ext_type_(typeid(T))
	, size_(sizeof(cell_type))
	, is_fixed_(size_traits<T>::is_fixed_size)
	, is_pod_(size_traits<T>::is_pod)
	, is_datatable_(size_traits<T>::is_table)
	, ctorf_(ctor_())
	, dtorf_(dtor_())
	, cmpf_(cmp_())
	, lexcastf_(lexcast_())
	, lexuncastf_(lexuncast_())
	, sqlvarf_(sqlvar_()) {}

public:

	inline const type_t& ext_type() const { return ext_type_; }
	inline const type_t& type() const { return type_; }
	inline size_t size() const { return size_; }
	inline bool is_fixed() const { return is_fixed_; }
	inline bool is_pod() const { return is_pod_; }
	inline bool is_table() const { return is_datatable_; }

	inline col_f::ctor_f ctor(void) const { return ctorf_; }
	inline col_f::dtor_f dtor(void) const { return dtorf_; }
	inline col_f::cmp_f cmp(void) const { return cmpf_; }
	inline col_f::lexcast_f lexcast(void) const { return lexcastf_; }
	inline col_f::lexuncast_f lexuncast(void) const { return lexuncastf_; }
	inline col_f::sqlvar_f sqlvar(void) const { return sqlvarf_; }

	inline static meta_col create(void)
	{
		return meta_col();
	}
};

///////////////////////////////////////////////////////////////////////////////
// stubbed out for now
template <typename T>
struct is_valid_column_type
{
	// ensure T is not a void type
	BOOST_MPL_ASSERT(( boost::mpl::not_< boost::is_void<T> > ));
	// ensure T is not a reference type
	BOOST_MPL_ASSERT(( boost::mpl::not_< boost::is_reference<T> > )); 
	// ensure T is not a pointer type
	BOOST_MPL_ASSERT(( boost::mpl::not_< boost::is_pointer<T> > ));

	static const bool value = true;
};

} // namespace schema_tools

} // namespace smart_table
