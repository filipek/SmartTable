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
// $Id: row_store.h 56 2011-11-05 20:51:21Z filipek $
//

#include "schema.h"
#include "store_traits.h"

namespace smart_table
{

///////////////////////////////////////////////////////////////////////////////
class row_store : boost::noncopyable
{
private:

	schema&							   schema_;
	std::auto_ptr<boost::pool<> >	pool_;
	size_t							   nrows_;
	byte_type*						   data_;

	// always lazy construct the pool
	inline boost::pool<>& allocator()
	{
		if ( !pool_.get() )
		{
			assert( data_ == NULL && nrows_ == 0 );

			pool_ = std::auto_ptr<boost::pool<> >(
				new boost::pool<>( schema_.row_size() ));
		}

		return *pool_;
	}
	inline byte_type* rowptr(size_t r) const
	{
		const size_t cellpos = r * schema_.row_size();
		return data_ + cellpos;
	}

	class row_adder
	{
	private:
		row_store& self_;

	public:
		row_adder(row_store& s) : self_(s) {}

		inline size_t operator()(void)
		{
			// always add rows at the end, so we do not
			// need to keep track of the various rows other
			// than through the nrows_ counter and position
			// of first row (self_.data_).
			byte_type* newrow = reinterpret_cast<byte_type*>(
				self_.allocator().ordered_malloc());

			// is it the first row?
			if ( self_.data_ == NULL )
				self_.data_ = newrow;

	#ifdef _DEBUG
			else
			{
				size_t newrowpos = self_.nrows_ * self_.schema_.row_size();
				assert( newrow == (self_.data_ + newrowpos) );
			}
	#endif // _DEBUG

			// call any ctors for each column
			const size_t ncols = self_.schema_.size();
			for (size_t c = 0; c < ncols; ++c)
			{
				const coldef& def = self_.schema_[c];
				def.construct(newrow + def.offset());
			}

			return (self_.nrows_)++;
		}
	};
	friend class row_adder;

	class row_deleter
	{
	private:
		row_store& self_;

	public:
		row_deleter(row_store& s) : self_(s) {}

		inline void operator()(size_t r)
		{
			// since rows are always added at the end
			// we must make sure to shuffle rows 'up'
			// to fill any holes left by the removed row.
			// everything is contiguous so all we need to
			// do is an adjusted memmove - should be quick.
			assert( r >= 0 && r < self_.nrows_ );

			byte_type* row2go = self_.rowptr(r);
			const size_t lastrow = self_.nrows_ - 1;

			bool doMove = false;
			size_t movesize = 0;
			byte_type* movefrom = NULL;

			if ( r != lastrow )
			{
				// move all rows after the one we're deleting
				// up a notch
				movesize = (lastrow - r) * self_.schema_.row_size();
				movefrom = row2go + self_.schema_.row_size();

				// do the move after we call any dtors, otherwise
				// we end up corrupting the memory we just moved.
				doMove = true;
			}
			// else its the last row - no adjustments required

			// call any dtors on column cells
			const size_t ncols = self_.schema_.size();
			for (size_t c = 0; c < ncols; ++c)
			{
				const coldef& def =  self_.schema_[c];

				// remember to call relevant destructors
				if ( ! def.is_pod() )
					def.destruct(row2go + def.offset());
			}

			// only move now - the dtors have been called.
			// this ensures we do not corrupt moved memory.
			if ( doMove )
			{
				// handles overlaps so we can just move
				::memmove(row2go, movefrom, movesize);

				// the move has happened so we must
				// be careful to find the last row
				row2go = self_.rowptr(lastrow);
			}

			self_.allocator().ordered_free(row2go);
			--(self_.nrows_);
		}
	};
	friend class row_deleter;

	template <typename U>
	class column_adder
	{
	private:
		row_store& self_;

	public:

		column_adder(row_store& s) : self_(s) {}

		template <typename V>
		inline size_type operator()(const V& cn)
		{
			const size_type old_size = self_.schema_.row_size();

			const size_type nc = self_.schema_.add<U>(cn);
			const size_type new_size = self_.schema_.row_size();

			const coldef& newcol = self_.schema_[cn];
			assert( new_size == (old_size + newcol.size()) );

			// lets use a different pool
			std::auto_ptr<boost::pool<> > new_pool = std::auto_ptr<boost::pool<> >(
					new boost::pool<>( new_size ));

			// nrows_ the same
			byte_type* new_data = reinterpret_cast<byte_type*>(
				new_pool->ordered_malloc( self_.nrows_ * new_size ));

			for (size_type r = 0; r < self_.nrows_; ++r)
			{
				byte_type* oldrowp = self_.data_ + r * old_size;
				byte_type* newrowp = new_data + r * new_size;

				// move whole old row
				memmove(newrowp, oldrowp, old_size);
				// construct just the new column cell in-place
				newcol.construct(newrowp + old_size);
			}

			self_.data_ = new_data;
			self_.pool_ = new_pool;

			return nc;
		}
	};
	template <typename U>
	friend class column_adder;

	class column_deleter
	{
	private:
		row_store& self_;

	public:

		column_deleter(row_store& s) : self_(s) {}

		template <typename V>
		inline void operator()(const V& c)
		{
			const coldef olddef( self_.schema_[c] );
			const size_type old_size = self_.schema_.row_size();
			const size_type old_col_size = olddef.size();

			self_.schema_.remove(c);

			const size_type new_size = schema_.row_size();
			assert( new_size == (old_size - old_col_size) );

			// lets use a different pool
			boost::pool<> new_pool = std::auto_ptr<boost::pool<> >(
					new boost::pool<>( new_size ));

			// nrows_ stay the same
			byte_type* new_data = new_pool.ordered_malloc( self_.nrows_ * new_size );

			for (size_type r = 0; r < self_.nrows_; ++r)
			{
				byte_type* oldrowp = self_.data_ + r * new_size;
				byte_type* newrowp = new_data + r * new_size;

				// move entire old row
				memmove(newrowp, oldrowp, new_size);

				// destruct old column cell in-place
				if ( ! olddef.is_pod() )
					olddef.destruct(oldrowp + new_size);
			}

			self_.data_ = new_data;
			self_.pool_ = new_pool;
		}
	};
	friend class column_deleter;

	template <typename T>
	friend class store_traits;

public:

	// in the cell methods below type V is associated with
	// the column index and can be either size_t or string. its
	// the U type that is interesting to us, V is deduced by compiler.
	template <typename V>
	inline byte_type* operator()(size_t r, const V& c)
	{
		return rowptr(r) + schema_[c].offset();
	}
	template <typename V>
	inline const byte_type* operator()(size_t r, const V& c) const
	{
		return rowptr(r) + schema_[c].offset();
	}

	row_store(schema& s)
	: schema_(s)
	, pool_(NULL)
	, nrows_(0)
	, data_(NULL) {}

	~row_store()
	{
		clear();

#if _DEBUG
		// schema_ and pool_ take care of thelseves
		nrows_ = 0;
		data_ = NULL;
#endif // _DEBUG
	}

	inline size_t width() const { return schema_.size(); }
	inline size_t depth() const { return nrows_; }

	//inline void remove_column(const string_type& n) { schema_.remove(n); }
	inline size_t add_row()
	{
		if ( schema_.size() == 0 )
			throw std::exception("Bad schema");

		return row_adder(*this)();
	}
	inline void remove_row(size_t r)
	{
		if ( nrows_ > 0 )
			row_deleter(*this)(r);
	}
	inline void clear()
	{
		if ( nrows_ > 0 )
		{
			// must call destructors for all variable
			// columns, as they need to clean up their
			// stuff. we can leave all other types as
			// the pool takes care of that.

			// ensure we clear from the last to the first
			// row to prevent memmove's (see row_deleter).
			row_deleter deleterow(*this);
			for (int r = nrows_ - 1; r >= 0; --r)
				deleterow(r);

			// re-set all rows, data and memory pool
			// but leave the schema alone
			nrows_ = 0;
			data_ = NULL;
			pool_.reset();
		}
	}

	template <typename U>
	inline size_t add_column(const string_type& cn)
	{
		return column_adder<U>(*this)(cn);
	}
	template <typename V>
	inline void remove_column(const V& c)
	{
		column_deleter(*this)(c);
	}
};

} // namespace smart_table
