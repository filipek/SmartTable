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
// $Id: tools.h 56 2011-11-05 20:51:21Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

// warning C4996: 'mbstowcs': This function or variable may be unsafe.
// Consider using mbstowcs_s instead
#pragma warning(disable: 4996)

///////////////////////////////////////////////////////////////////////////////

#include <boost/any.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/function.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/value_type.hpp>
#include <boost/mpl/vector.hpp>

#include <boost/noncopyable.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <boost/pool/pool_alloc.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/serialize_ptr_vector.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/phoenix1_operators.hpp>
#include <boost/spirit/include/phoenix1_primitives.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <ctime>
#include <cerrno>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

#include "LokiTypeInfo.hpp"
#include "safe_buffer.h"
#include "hr_timer.h"
#include "md5.hpp"
#include "sys_allocator.hpp"

#include "typedefs.h"

namespace smart_table
{

///////////////////////////////////////////////////////////////////////////////
// fwd decl
template <class> class datatable_impl;

namespace tools
{

///////////////////////////////////////////////////////////////////////////////
struct tonarrow_fun
	: std::binary_function<std::wstring, std::string, void>
{
	inline void operator()(const std::wstring& ws, std::string& s) const
	{
		const size_t len = ws.size();
		s.resize(len, '\0');
		const size_t ret = wcstombs(&*s.begin(), ws.c_str(), len);
		assert( ret == len );
	}
};

struct towide_fun
{
	inline void operator()(const std::string& s, std::wstring& ws) const
	{
		const size_t len = s.size();
		ws.resize(len, L'\0');
		const size_t ret = mbstowcs(&*ws.begin(), s.c_str(), len);
		assert( ret == len );
	}
	// this simple specialization makes some code
	// in the sqlvm (push_string) more elegant.
	inline void operator()(const std::string& s, std::string& s2) const
	{
		s2 = s;
	}
};

///////////////////////////////////////////////////////////////////////////////
#define DEFINE_TYPE_NAME_ENTRY_BASE(t,n)\
	do{\
		const string_type sn(n);\
		names.insert( std::make_pair(&typeid(t), sn));\
		order.push_back(&typeid(t));\
	}while(false)

#define DEFINE_TYPE_NAME_ENTRY(t) DEFINE_TYPE_NAME_ENTRY_BASE(t,#t)

///////////////////////////////////////////////////////////////////////////////
struct typename_map
{
	typedef std::map<const std::type_info*, string_type> map_type;
	typedef std::vector<const std::type_info*> vector_type;

	inline static bool register_friendly_names(void)
	{
		DEFINE_TYPE_NAME_ENTRY_BASE(std::string,"string");
		DEFINE_TYPE_NAME_ENTRY_BASE(std::wstring,"wstring");
		DEFINE_TYPE_NAME_ENTRY_BASE(std::vector<int>,"vector<int>");
		DEFINE_TYPE_NAME_ENTRY_BASE(std::vector<double>,"vector<double>");
		DEFINE_TYPE_NAME_ENTRY_BASE(std::vector<std::string>,"vector<string>");
		DEFINE_TYPE_NAME_ENTRY_BASE(std::vector<std::wstring>,"vector<wstring>");
		DEFINE_TYPE_NAME_ENTRY_BASE(boost::gregorian::date, "date");
		DEFINE_TYPE_NAME_ENTRY_BASE(boost::posix_time::ptime, "datetime");

		return true;
	}

	static map_type		names;
	static vector_type	order;
};

#undef DEFINE_TYPE_NAME_ENTRY
#undef DEFINE_TYPE_NAME_ENTRY_BASE

typename_map::map_type typename_map::names;
typename_map::vector_type typename_map::order;

static bool initialize = typename_map::register_friendly_names();

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct name_of_fun
	: std::binary_function<const bool, const T*, string_type>
{
	inline string_type operator()(
		const bool is_friendly = true
		, const T* /*dummy*/ = 0) const
	{
		static const std::type_info& this_T = typeid(T);
		static const string_type this_T_name(this_T.name());

		string_type name;

		if ( is_friendly )
		{
			typename_map::map_type::const_iterator ti =
				typename_map::names.find(& this_T);

			// use friendly name for std::string
			if (ti != typename_map::names.end() )
				name = ti->second;
			else
			{
				name = this_T_name;

				typedef typename_map::vector_type::iterator
					vector_iterator;

				for ( vector_iterator vi = typename_map::order.begin();
					vi != typename_map::order.end(); ++vi)
				{
					const string_type& unfriendly_name =
						boost::lexical_cast<string_type>((*vi)->name());

					const string_type& friendly_name =
						typename_map::names[*vi];

					size_t pos = name.find(unfriendly_name);
					while (pos != string_type::npos)
					{
						name.replace(
							pos
							, unfriendly_name.size()
							, friendly_name);

						pos = name.find(unfriendly_name);
					}
				}
			}
		}
		else
			name = this_T_name;

		return name;
	}
};

///////////////////////////////////////////////////////////////////////////////
struct string_md5hash_fun
	: std::unary_function<string_type, string_type>
{
	inline string_type operator()(const string_type& s) const
	{
		boost::md5 sh(s.c_str());
		return string_type(sh.digest().hex_str_value());
	}
};

///////////////////////////////////////////////////////////////////////////////
struct int_random_fun
	: std::unary_function<void, string_type>
{
private:

	typedef boost::minstd_rand								gen_type;
	typedef boost::uniform_int<>							dist_type;
	typedef boost::variate_generator<gen_type&, dist_type>	rand_type;

	static gen_type		rgen_;
	static dist_type	rdist_;
	static rand_type	rand_;

public:

	inline int operator()(void) const { return rand_(); }
};

int_random_fun::gen_type
	int_random_fun::rgen_(static_cast<size_t>(hr_timer::now() * 1000));
int_random_fun::dist_type
	int_random_fun::rdist_(0,(std::numeric_limits<int>::max)());
int_random_fun::rand_type
	int_random_fun::rand_(int_random_fun::rgen_, int_random_fun::rdist_);

///////////////////////////////////////////////////////////////////////////////
struct real_random_fun
	: std::unary_function<void, string_type>
{
private:

	typedef boost::minstd_rand								gen_type;
	typedef boost::uniform_real<>							dist_type;
	typedef boost::variate_generator<gen_type&, dist_type>	rand_type;

	static gen_type		rgen_;
	static dist_type	rdist_;
	static rand_type	rand_;

public:

	inline double operator()(void) const { return rand_(); }
};

real_random_fun::gen_type
	real_random_fun::rgen_(static_cast<unsigned int>(hr_timer::now() * 1000));
real_random_fun::dist_type
	real_random_fun::rdist_(0,1);
real_random_fun::rand_type
	real_random_fun::rand_(real_random_fun::rgen_, real_random_fun::rdist_);

///////////////////////////////////////////////////////////////////////////////
struct random_md5hash_fun
	: std::unary_function<void, string_type>
{
private:

	static const string_type cookie;

public:

	inline string_type operator()(void) const
	{
		const string_type rs(
			boost::lexical_cast<string_type>( real_random_fun()() )
			+ "." + cookie);

      static string_md5hash_fun str_md5_f;
		return str_md5_f(rs);
	}
};

const string_type random_md5hash_fun::cookie(
	"448d0dc80d2baadf00dski675a9d3ad5785744baa8f");

/////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, const std::string& s)
{
	return os << s.c_str();
}
inline std::ostream& operator<<(std::ostream& os, const std::wstring& ws)
{
	std::string s;
	smart_table::tools::tonarrow_fun()(ws,s);
	
	return os << s;
}
inline std::istream& operator>>(std::istream& is, std::wstring& ws)
{
	std::string s;
	is >> s;

	smart_table::tools::towide_fun()(s,ws);
	
	return is;
}

///////////////////////////////////////////////////////////////////////////////
// helper type meta-functions, used for syntactic convenience
template <typename T>
struct str_
{
	typedef
		std::basic_string<T, std::char_traits<T>, std::allocator<T> > type;
};
template <typename T>
struct ostrm_
{
	typedef std::basic_ostream<T, std::char_traits<T> > type;
};
template <typename T>
struct sstrm_
{
	typedef std::basic_stringstream<T, std::char_traits<T> > type;
};
template <typename T>
struct istrm_
{
	typedef std::basic_istream<T, std::char_traits<T> > type;
};

// the is_datatable meta-function is not as simple as
// is_same<T, datable> because datatable_impl is a template
// class. we use partial specialization to filter out just
// the datatable_impl instantiations of the is_datatable
// template.
// generic type is not a datatable
template <typename T>
struct is_datatable : boost::mpl::bool_<false> {};
// partially specialize for datatable_impl
template <typename T>
struct is_datatable< datatable_impl<T> > : boost::mpl::bool_<true> {};

template <typename T>
struct is_date : boost::is_same<T, date_type> {};
template <typename T>
struct is_datetime : boost::is_same<T, datetime_type> {};
template <typename T>
struct is_string : boost::is_same<T, std::string> {};
template <typename T>
struct is_wstring : boost::is_same<T, std::wstring> {};
template <typename T>
struct is_a_string
{
private:
	typedef typename boost::mpl::or_<
		is_string<T>
		, is_wstring<T>
	>::type is_a_string_;

public:
	static const bool value = is_a_string_::value;
	typedef is_a_string_ type;
};

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct null_char
{
//	BOOST_STATIC_ASSERT(false);
   static const char value = '\0';
};

//template <>
//struct null_char<char>
//{
//	static const char value = '\0';
//};

template <>
struct null_char<wchar_t>
{
	static const wchar_t value = L'\0';
};

// partially specialize for strings
template <typename T>
struct null_char< std::basic_string<
	T, std::char_traits<T>, std::allocator<T>
> >
{
	static const T value = null_char<T>::value;
};

///////////////////////////////////////////////////////////////////////////////
// this meta function is used to calculate
// T's value type. so for std::string this is char.
// For std::wstring its wchar_t etc. useful for
// defining the right type of allocator for non-fixed
// size values (eg ones which will be wrapped in
// a varblob).
template <typename T>
struct char_of
{
private:

	template <typename Y>
	struct value_of_
	{
		typedef typename Y::value_type type;
	};

	typedef typename boost::mpl::if_<
		boost::is_same<wchar_t,T>
		, wchar_t
		, char> if_char_type_;

	typedef typename boost::mpl::if_<
		is_a_string<T>
		, typename value_of_<T>
		, typename if_char_type_
	>::type if_type;

public:

	typedef typename if_type::type type;
};

BOOST_STATIC_ASSERT(( sizeof(char_of<std::wstring>::type) == 2 ));
BOOST_STATIC_ASSERT(( sizeof(char_of<wchar_t>::type) == 2 ));
BOOST_STATIC_ASSERT(( sizeof(char_of<std::string>::type) == 1 ));
BOOST_STATIC_ASSERT(( sizeof(char_of<char>::type) == 1 ));
BOOST_STATIC_ASSERT(( sizeof(char_of<int>::type) == 1 ));
BOOST_STATIC_ASSERT(( sizeof(char_of<double>::type) == 1 ));


///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct size_traits
{
private:

	static const bool is_table_ = is_datatable<T>::value;

	typedef typename boost::mpl::if_c<
		is_table_
		, boost::mpl::false_
		, boost::is_pod<T>
		>::type is_pod_t;

	static const bool is_pod_ = is_pod_t::value;

	// tells us if the type T is of fixed size or not.
	// this is used below. datatable is here too because
	// we want to adjust it always to be a shared ptr which
	// behaves as a value type of fixed size.
	static const bool is_fixed_size_ =
		(is_table_ | is_pod_ | is_date<T>::value | is_datetime<T>::value);

public:

	static const bool is_table = is_table_;
	static const bool is_pod = is_pod_;
	static const bool is_fixed_size = is_fixed_size_;
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_ptr
{
	struct type
	{
		inline const T* operator()(const U* s) const
		{
			assert(s);
			return reinterpret_cast<const T*>(s);
		}
		inline T* operator()(U* s) const
		{
			assert(s);
			return reinterpret_cast<T*>(s);
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_ref
{
private:

	typedef typename mem_ptr<T,U>::type	ptr_f;

public:

	struct type
	{
		inline const T& operator()(const U* s) const
		{
			assert(s);
			return *ptr_f()(s);
		}
		inline T& operator()(U* s) const
		{
			assert(s);
			return *ptr_f()(s);
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_cmp
{
	struct type
	{
		inline bool operator()(const U* lhs, const U* rhs) const
		{
	      typedef typename mem_ref<T,U>::type ref_fun;

         static ref_fun ref_f;

			assert(lhs);
			assert(rhs);

			if ( lhs == rhs )
				return true;

			return ref_f(lhs) == ref_f(rhs);
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_ctor
{
	struct type
	{
		inline void operator()(U* p) const
		{
			assert(p);
			new(p) T;
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_dtor
{
	struct type
	{
		inline void operator()(U* p) const
		{
			assert(p);
			T* tp = reinterpret_cast<T*>(p);
			assert( tp );
			tp->~T();
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_get
{
	struct type
	{
		inline void operator()(const U* p, T& r) const
		{
	      typedef typename mem_ref<T,U>::type ref_fun;

         static ref_fun ref_f;
			r = ref_f(p);
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_set
{
	struct type
	{
		inline void operator()(U* p, const T& t) const
		{
	      typedef typename mem_ref<T,U>::type ref_fun;
         
         static ref_fun ref_f;
         ref_f(p) = t;
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_get_pod
{
	struct type
	{
		inline void operator()(const U* p, T& t) const
		{
			memcpy(&t, p, sizeof(T));
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_set_pod
{
	struct type
	{
		inline void operator()(U* p, const T& t) const
		{
			memcpy(p, &t, sizeof(T));
		}
	};
};

#ifdef ALLOCATOR_T
#	error ALLOCATOR_T cannot yet be defined!
#else
#	define ALLOCATOR_T(t) template <class> class t
#endif // ALLOCATOR_T

///////////////////////////////////////////////////////////////////////////////
template <
	typename T
	, typename U = byte_type
	, ALLOCATOR_T(_TA) = boost::malloc_allocator>
struct mem_lexuncast
{
	struct type
	{
		inline void operator()(const string_type& s, U* p) const
		{
	      typedef typename mem_ref<T,U>::type ref_fun;

         static ref_fun ref_f;
			T& t = ref_f(p);
			std::istringstream istrm(s);
			istrm >> t;
		}
	};
};

template <typename T, typename U, ALLOCATOR_T(_TA)>
struct mem_lexuncast< boost::shared_ptr<T>,U,_TA>
{
	struct type
	{
		inline void operator()(const string_type& s, U* p) const
		{
			throw std::exception("Cannot un-cast a datatable_impl column");
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U = byte_type>
struct mem_lexcast
{
	struct type
	{
		inline string_type operator()(const U* p) const
		{
	      typedef typename mem_ref<T,U>::type ref_fun;
         
         static ref_fun ref_f;

			const T& t = ref_f(p);
			
			std::stringstream strm;
			strm << t;
			return strm.str();
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <
	typename T
	, typename U = byte_type
	, ALLOCATOR_T(_TA) = boost::malloc_allocator>
struct mem_sqlvar
{
public:

	struct type
	{
		inline sqlvar operator()(const U* p) const
		{
			// datatables cannot be used in sql queries.
			assert( ! is_datatable<T>::value );

			typedef typename mem_ref<T,U>::type ref_fun;

         static ref_fun ref_f;

			sqlvar sv = ref_f(p);
			return sv;
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
// meta-functor which generates the correct memcpy impl given
// 2 types. the general case when 
template <typename S, typename D>
struct mem_cpy
{
	BOOST_STATIC_ASSERT(( sizeof(S) == sizeof(D) ));
	//BOOST_MPL_ASSERT(( boost::is_same<S,D> ));

	struct type
	{
		inline size_t operator()(
			const S* src
			, D* dest
			, size_t count) const
		{
			memcpy(dest, src, count * sizeof(D));
			return count;
		}
	};
};
template <>
struct mem_cpy<wchar_t,char>
{
	struct type
	{
		inline size_t operator()(
			const wchar_t* src
			, char* dest
			, size_t count) const
		{
			return wcstombs(dest, src, count);
		}
	};
};
template <>
struct mem_cpy<char,wchar_t>
{
	struct type
	{
		inline size_t operator()(
			const char* src
			, wchar_t* dest
			, size_t count) const
		{
			return mbstowcs(dest, src, count);
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename X>
struct data_lv
{
	inline typename char_of<X>::type* operator()(X& v) const
	{
		return v.data();
	}
};
template <typename X>
struct data_lv<
	std::basic_string<X, std::char_traits<X>, std::allocator<X> >
>
{
	inline typename char_of<X>::type* operator()(
		std::basic_string<X, std::char_traits<X>, std::allocator<X> >& v) const
	{
		return &*v.begin();
	}
};

} // namespace tools

} // namespace smart_table
