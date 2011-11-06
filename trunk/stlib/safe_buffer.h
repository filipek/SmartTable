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
// $Id: safe_buffer.h 56 2011-11-05 20:51:21Z filipek $
//
namespace smart_table
{

namespace tools
{

///////////////////////////////////////////////////////////////////////////////
// so many times the following code is used:
//
//		char buf[16];
//		memset(buf, 0, sizeof(buf));
//
// so i decided to add this template wrapper
// for compile-time sizeof and memset on construction.
// its not quite a std::vector ;-) but is helpful
// according to patterns of implementation.
//
//
// for big buffers eg 1MB (== 1024*1024) you may need
// to increase default stack size (compiler option)
// if you find that execution results in stack overflows
//
template<
	const size_t BUFFER_SIZE
	, typename T = unsigned char /* BYTE */>
struct safe_buffer
{
	typedef T					buffer_type[BUFFER_SIZE];
	typedef T					value_type;
	typedef value_type*			pointer;
	typedef const value_type*	const_pointer;

	enum constants
	{
		size = sizeof(buffer_type)
		, value_size = sizeof(value_type)
	};

	buffer_type data;

	// forces calculation at compile time
	// and allows following usage eg:
	//
	//		safe_buffer<n,T>::size
	//
	// and is equal to
	//
	//		sizeof(T[n]))
	//

	safe_buffer()
	{
#ifdef _DEBUG
		reset();
#endif // _DEBUG
	}

	// auto-conversion to T* allowed
	inline operator pointer () { return data; }
	inline operator const_pointer () const { return data; }

	// zero our buffer memory
	inline void reset() { memset(data, 0, size); }
};

///////////////////////////////////////////////////////////////////////////////
// specialize for common usage eg:
//
//		char buf[N]
//
// by using the default 2nd template param for safe_buffer
//
template<const size_t BUFFER_SIZE>
struct char_buffer
	: safe_buffer<BUFFER_SIZE, char> {};

///////////////////////////////////////////////////////////////////////////////
// this is a rip-off of std::auto_ptr. we only make sure to
// destruct properly using 'delete []' syntax.
template<typename X>
class auto_array
{
public:

	explicit auto_array(X* p = 0)
		: the_p(p) {}

	auto_array(const auto_array<X>& a)
		: the_p( const_cast<typename auto_array<X>& >(a).release() ) {}

	void operator=(const auto_array<X>& a)
		{ reset( const_cast<typename auto_array<X>& >(a).release()); }

	// this is different from auto_ptr
	~auto_array() { delete [] the_p; }

	X& operator*() const { return *the_p; }
	X* operator->() const { return the_p; }
	X* get() const { return the_p; }
	X* release() { X* tmp = get(); the_p = 0; return tmp; }
	void reset(X* p = 0) { delete [] the_p; the_p = p; }
	inline operator X* () { return the_p; }

private:

	X* the_p;
};

} // namespace tools

} // namespace smart_table
