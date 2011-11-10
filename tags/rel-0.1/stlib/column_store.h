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
// $Id: column_store.h 49 2009-11-17 21:46:07Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "schema.h"

namespace smart_table
{

///////////////////////////////////////////////////////////////////////////////
class column_store : boost::noncopyable
{
private:

	template <typename U>
	friend class store_traits;

	typedef boost::malloc_allocator<byte_type>		allocator_type;
	typedef std::vector<byte_type, allocator_type>	bytevec;
	typedef boost::ptr_vector<bytevec>				columnvec;

	schema&		schema_;
	size_t		nrows_;
	columnvec	data_;

	template <typename T>
	friend class store_traits;

public:

	template <typename V>
	inline byte_type* operator()(size_t r, const V& c)
	{
		const coldef& def = schema_[c];
		return &(data_[def.id()][r*def.size()]);
	}
	template <typename V>
	inline const byte_type* operator()(size_t r, const V& c) const
	{
		const coldef& def = schema_[c];
		return &(data_[def.id()][r*def.size()]);
	}

	column_store(schema& s)
	: schema_(s)
	, nrows_(0)
	, data_() {}

	~column_store()
	{
		clear();
	}

	inline size_t width() const { return schema_.size(); }
	inline size_t depth() const { return nrows_; }

	inline size_t add_row()
	{
		const size_t ncols = schema_.size();
		if ( ncols == 0 )
			throw std::exception("Bad schema");

		for (size_t c = 0; c < ncols; ++c)
		{
			const coldef& def = schema_[c];
			bytevec& data = data_[c];

			// ensure there is enough bytes for new cell and
			// insert the same number of bytes (value==0)
			data.resize(data.size() + def.size(), 0);

			// point to the newly added cell
			byte_type* sp = &*(data.begin() + (data.size()-def.size()));

			// call the appropriate in-place ctor for cell
			def.construct(sp);
		}

		return nrows_++;
	}
	inline void remove_row(size_t r)
	{
		if ( nrows_ > 0 )
		{
			// NOTE: we erase bytes last col first to
			// ensure we do not suffer memmoves when
			// removing from inside data vector, when
			// removing last row.
			for (int c = (schema_.size() - 1); c >= 0; --c)
			{
				const coldef& def = schema_[c];
				bytevec& data = data_[c];

				bytevec::iterator sp = data.begin() + r * def.size();

				// call dtor
				if ( ! def.is_pod() )
					def.destruct(&*sp);

				// remove/free all bytes that make up this cell's instance
				data.erase(sp, sp + def.size());
			}

			--nrows_;
		}
	}
	inline void clear()
	{
		if ( nrows_ > 0 )
		{
			do
			{
				// remove all rows from last to first
				remove_row(nrows_ - 1);
			}
			while (nrows_ > 0);

			nrows_ = 0;
			data_.clear();
		}
	}

	template <typename U>
	inline size_t add_column(const string_type& cn)
	{
		const size_t cid = schema_.add<U>(cn);
		const coldef& def = schema_[cn];

		data_.push_back( new bytevec(nrows_ * def.size()) );
		return cid;
	}
	template <typename V>
	inline void remove_column(const V& c)
	{
		const coldef& def = schema_[c];

		if ( nrows_ > 0 )
		{
			if ( ! def.is_pod() )
			{
				byte_type* oldcolp = &*data_[def.id()]->begin();

				for (size_t r = 0; r < nrows_; ++r)
				{
					byte_type* cellp = oldcolp + r * def.size();
					def.destruct( cellp );
				}
			}

			data_.remove(def.id());
		}

		schema_.remove(def.id());
	}
};

} // namespace smart_table
