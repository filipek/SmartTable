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
// $Id: varblob.h 56 2011-11-05 20:51:21Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include <boost/noncopyable.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>

#include <utility>

#include "tools.h"

namespace smart_table
{

namespace schema_tools
{

using namespace tools;

///////////////////////////////////////////////////////////////////////////////
template <typename T, ALLOCATOR_T(_TA)>
class varblob : boost::noncopyable
{
public:

	typedef T							type;
	typedef typename _TA<type>			allocator;

	BOOST_MPL_ASSERT((
		boost::is_same<type, typename allocator::value_type> ));

	typedef typename allocator::pointer			pointer;
	typedef typename allocator::const_pointer	const_pointer;

private:

	struct holder
	{
		pointer buffer;
		size_t	size;

		holder() : buffer(NULL), size(0) {}
	};

	mutable holder data_;

	inline static size_t bytesize_(size_t n)
	{
		return n * sizeof(type);
	}
	inline static size_t size_(size_t n)
	{
		return n / sizeof(type);
	}
	inline void copy_(const_pointer p, size_t len)
	{
		data_.buffer = allocator::allocate(len);
		assert( data_.buffer );
		const size_type bsz = bytesize_(len);
		memcpy(data_.buffer, p, bsz);
		data_.size = len;
	}
	
    friend class boost::serialization::access;

    template<typename Archive>
    void save(Archive & ar, const unsigned int /* file_version */) const {
		const size_t bsz = bytesize();
		ar & bsz;
        ar.save_binary(data_.buffer, bsz);
    }
    template<typename Archive>
    void load(Archive & ar, const unsigned int /* file_version */) const {
		size_t bsz;
		ar & bsz;

		// translate from byte to size
		// of T, as is our custom
		reserve( size_(bsz) );

        ar.load_binary(data_.buffer, bsz);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

public:

	varblob() : data_() {}

	// NOTE len is in units of sizeof(T)
	// this gets translated to size in copy_
	varblob(const_pointer p, size_t len) : data_()
	{
		copy_(p, len);
	}

	template <typename V>
	varblob(const V& s) : data_()
	{
		set(s);
	}

	~varblob() { release(); }

	inline size_t capacity() const { return size(); }
	inline const pointer data() const { return data_.buffer; }
	inline bool empty() const { return ( data_.size == 0 ); }
	inline size_t bytesize() const { return bytesize_(data_.size); }
	inline size_t size() const { return data_.size; }
	inline void store(const_pointer p, size_t len)
	{
		release();
		copy_(p, len);
	}
	inline void release()
	{
		if ( ! empty() )
		{
			allocator::deallocate(data_.buffer, data_.size);
			data_ = holder();
		}
	}
	void reserve(size_t size)
	{
		if ( size > 0 )
		{
			release();

			data_.buffer = allocator::allocate(size);
			data_.size = size;
		}
	}

	template <typename V>
	inline void get(V& v) const
	{
		v.clear();

		if ( data_.size == 0 )
			return;

		typedef typename char_of<V>::type vchar;

		// ensure s is big enough to hold our load
		v.resize(data_.size, null_char<vchar>::value);

		// use meta-functions to generate the appropriate
		// memcpy call. for same size types (ie V same size as T)
		// normal ::memcpy happens. for char <-> wchar_t the
		// appropriate to-wide/to-narrow is called.

		typedef typename mem_cpy<type,vchar>::type memcpy_func;

		static data_lv<V> data_lv_fun;
		static memcpy_func memcpy_f;
		memcpy_f(data_.buffer, data_lv_fun(v), data_.size);
	};
	template <typename V>
	inline void set(const V& v)
	{
		release();

		// ensure we're big enough to hold string's load
		const size_t len = v.size();
		if ( len == 0 )
			return;

		reserve(len);

		// use meta-functions to generate the appropriate
		// memcpy_f call. for same size types (ie V same size as T)
		// memcpy_f uses ::memcpy. for char <-> wchar_t the
		// appropriate to-wide/to-narrow string api function is called.

		typedef typename char_of<V>::type vchar;
		typedef typename mem_cpy<vchar,type>::type memcpy_func;

      static memcpy_func memcpy_f;
		memcpy_f(v.data(), data_.buffer, len);
	};
	template <typename K, ALLOCATOR_T(_KA)>
	inline bool operator==(const varblob<K,_KA>& rhs) const
	{
		// the type of varblob does not matter as much as its
		// element's size (ie char or wchar_t) for binary comparison
		BOOST_STATIC_ASSERT(( sizeof(K) == sizeof(type) ));

		if ( data_.size != rhs.data_.size )
			return false;

		const size_t bsz = bytesize();
		return (memcmp(data_.buffer, rhs.data_.buffer, bsz) == 0);
	}
};

///////////////////////////////////////////////////////////////////////////////
// std::ostream << varblob 
template <typename T, ALLOCATOR_T(_TA)>
inline typename ostrm_<T>::type&
operator<<(typename ostrm_<T>::type& os, const varblob<T,_TA>& vb)
{
	if ( ! vb.empty() )
		os.write( vb.data(), vb.size() );

	return os;
}
template <ALLOCATOR_T(_TA)>
inline std::ostream& operator<<(
	std::ostream& os
	, const varblob<wchar_t, _TA>& vb)
{
	if ( ! vb.empty() )
	{
		std::string s(vb.size(),'\0');
		vb.get(s);
		os.write(s.data(), s.size());
	}

	return os;
}
template <ALLOCATOR_T(_TA)>
inline std::wostream& operator<<(
	std::wostream& os
	, const varblob<char, _TA>& vb)
{
	if ( ! vb.empty() )
	{
		std::wstring ws(vb.size(),L'\0');
		vb.get(ws);
		os.write(ws.data(), ws.size());
	}

	return os;
}

///////////////////////////////////////////////////////////////////////////////
template <
	typename T
	, typename U = byte_type
	, ALLOCATOR_T(_TA) = boost::malloc_allocator
>
struct varblob_
{
	// NOTE: using default 'malloc' allocator for now
	typedef varblob<typename char_of<T>::type, _TA> type;

	// requirements on varblob mapped types are that they
	// have 2 methods: data() and size() returning the pointer
	// and size of memory buffer it points to, respectively.
	// it also needs a constructor that given those, creates
	// an instance of the mapped type.
	struct get_f
	{
		inline void operator()(const U* p, T& v) const
		{
			typedef mem_ref<type,U> vb_ref_t;
			typedef typename vb_ref_t::type vbref_func;

         static vbref_func vbref_f;
			const type& vb = vbref_f(p);
			vb.get(v);
		}
	};
	struct set_f
	{
		inline void operator()(U* p, const T& v) const
		{
			typedef mem_ref<type,U> vb_ref_t;
			typedef typename vb_ref_t::type vbref_func;

         static vbref_func vbref_f;
         type& vb = vbref_f(p);
			vb.set(v);
		}
	};
};

} // namespace schema_tools

namespace tools
{

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U, ALLOCATOR_T(_TA)>
struct mem_lexuncast< smart_table::schema_tools::varblob<T,_TA>,U,_TA >
{
	struct type
	{
		inline void operator()(const string_type& s, U* p) const
		{
			typedef smart_table::schema_tools::varblob<T,_TA> vb_t;
			typedef typename mem_ref<vb_t,U>::type ref_func;
			typedef typename mem_ctor<vb_t>::type ctor_func;

         static ref_func ref_f;
         static ctor_func ctor_f;

			// in-place initialize the memory so the caller
			// doesn't have to, but it has to be done!
			ctor_f(p);
			vb_t& vb = ref_f(p);
			vb.set(s);
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U, ALLOCATOR_T(_TA)>
struct mem_sqlvar< smart_table::schema_tools::varblob<T,_TA>,U,_TA >
{
	struct type
	{
		inline sqlvar operator()(const U* p) const
		{
			typedef smart_table::schema_tools::varblob<T,_TA> vb_t;
			typedef typename mem_ref<vb_t,U>::type ref_func;

         static ref_func ref_f;

			const vb_t& vb = ref_f(p);

			// create a str_t of the appropriate char
			// type and size, initialized with null chars.
			typedef typename str_<T>::type str_t;
			str_t s(vb.size(), null_char<str_t>::value);

			vb.get(s);
			return sqlvar(s);
		}
	};
};

} // namespace tools

} // namespace smart_table
