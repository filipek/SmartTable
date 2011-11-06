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
// $Id: schema.h 52 2009-11-18 10:27:28Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "schema_tools.h"

///////////////////////////////////////////////////////////////////////////////
namespace smart_table
{

using namespace schema_tools;

///////////////////////////////////////////////////////////////////////////////
class coldef
{
private:

	string_type			name_;
	size_t				id_;
	size_t				offset_;
	type_t				type_;
	type_t				ext_type_;
	size_t				size_;
	bool				is_fixed_;
	bool				is_pod_;
	bool				is_datatable_;
	col_f::ctor_f		ctorf_;
	col_f::dtor_f		dtorf_;
	col_f::cmp_f		cmpf_;
	col_f::lexcast_f	lexcastf_;
	col_f::lexuncast_f	lexuncastf_;
	col_f::sqlvar_f		sqlvarf_;

	coldef()
	: name_()
	, id_(0)
	, offset_(0)
	, type_()
	, ext_type_()
	, size_(0)
	, is_fixed_(false)
	, is_pod_(false)
	, is_datatable_(false)
	, ctorf_()
	, dtorf_()
	, cmpf_()
	, lexcastf_()
	, lexuncastf_()
	, sqlvarf_() {}

	coldef(
		const string_type& n
		, const size_t off
		, const type_t& t
		, const type_t& extt
		, const size_t sz
		, const bool isfixed
		, const bool ispod
		, const bool istable
		, const col_f::ctor_f ctor
		, const col_f::dtor_f dtor
		, const col_f::cmp_f cmpf
		, const col_f::lexcast_f lexcastf
		, const col_f::lexuncast_f lexuncastf
		, const col_f::sqlvar_f sqlvarf)
	: name_(n)
	, id_(0)
	, offset_(off)
	, type_(t)
	, ext_type_(extt)
	, size_(sz)
	, is_fixed_(isfixed)
	, is_pod_(ispod)
	, is_datatable_(istable)
	, ctorf_(ctor)
	, dtorf_(dtor)
	, cmpf_(cmpf)
	, lexcastf_(lexcastf)
	, lexuncastf_(lexuncastf)
	, sqlvarf_(sqlvarf) {}

	inline void offset(size_t off) { offset_ = off; }
	inline void id(size_t id) { id_ = id; }

	template <class> friend class coldef_create;
	friend class schema;
	friend class std::map<string_type,coldef>;

	inline void assert_valid_column(const string_type& cn) const
	{
		if ( cn.length() == 0 )
		{
			const string_type msg(
				"Column name cannot be empty.");
			throw std::bad_cast(msg.c_str());
		}
	}

public:

	coldef(const coldef& rhs)
	: name_(rhs.name_)
	, id_(rhs.id_)
	, offset_(rhs.offset_)
	, type_(rhs.type_)
	, ext_type_(rhs.ext_type_)
	, size_(rhs.size_)
	, is_fixed_(rhs.is_fixed_)
	, is_pod_(rhs.is_pod_)
	, is_datatable_(rhs.is_datatable_)
	, ctorf_(rhs.ctorf_)
	, dtorf_(rhs.dtorf_)
	, cmpf_(rhs.cmpf_)
	, lexcastf_(rhs.lexcastf_)
	, lexuncastf_(rhs.lexuncastf_)
	, sqlvarf_(rhs.sqlvarf_) {}

	inline coldef& operator=(const coldef& rhs)
	{
		if ( this != &rhs)
		{
			name_ = rhs.name_;
			id_ = rhs.id_;
			offset_ = rhs.offset_;
			type_ = rhs.type_;
			ext_type_ = rhs.ext_type_;
			size_ = rhs.size_;
			is_fixed_ = rhs.is_fixed_;
			is_pod_ = rhs.is_pod_;
			is_datatable_ = rhs.is_datatable_;
			ctorf_ = rhs.ctorf_;
			dtorf_ = rhs.dtorf_;
			cmpf_ = rhs.cmpf_;
			lexcastf_ = rhs.lexcastf_;
			lexuncastf_ = rhs.lexuncastf_;
			sqlvarf_ = rhs.sqlvarf_;
		}

		return *this;
	}

	inline const string_type& name() const { return name_; }
	inline size_t id() const { return id_; }
	inline size_t offset() const { return offset_; }
	inline const type_t& type() const { return type_; }
	inline const type_t& ext_type() const { return ext_type_; }
	inline size_t size() const { return size_; }
	inline bool is_fixed() const { return is_fixed_; }
	inline bool is_pod() const { return is_pod_; }
	inline bool is_table() const { return is_datatable_; }

	inline void construct(byte_type* p) const
	{ ctorf_(p); }
	inline void destruct(byte_type* p) const
	{ dtorf_(p); }
	inline bool compare(const byte_type* lp, const byte_type* rp) const
	{ return cmpf_(lp,rp); }
	inline string_type lexcast(const byte_type* p) const
	{ return lexcastf_(p); }
	inline void lexuncast(const string_type& s, byte_type* p) const
	{ lexuncastf_(s,p); }
	inline sqlvar sqlvar(const byte_type* p) const
	{ return sqlvarf_(p); }

	inline bool operator==(const coldef& rhs) const
	{
		return (name_ == rhs.name_) && (type_ == rhs.type_);
	}
	inline bool operator!=(const coldef& rhs) const
	{
		return (name_ != rhs.name_) || (type_ != rhs.type_);
	}

	friend inline std::ostream& operator<<(std::ostream& os, const coldef& col)
	{
		const string_type cname( col.name() );
		const string_type ctype( col.type().name() );
		const string_type cetype( col.ext_type().name() );

		os << "<coldef name='" << cname
			<< "' type='" << ctype
			<< "' ext-type='" << cetype
			<< "' is_fixed_size=" << col.is_fixed()
			<< " is_datatable=" << col.is_table()
			<< " size=" << col.size()
			<< " offset=" << col.offset() << " />";
			
		return os;
	}
};

///////////////////////////////////////////////////////////////////////////////
template <typename T>
class coldef_create : std::binary_function<string_type, size_t, coldef>
{
public:
	inline coldef operator()(const string_type& name, size_t offset) const
	{		
		typedef typename meta_col<T> meta_def;
		meta_def meta = meta_def::create();

		return coldef(
			name
			, offset
			, meta.type()
			, meta.ext_type()
			, meta.size()
			, meta.is_fixed()
			, meta.is_pod()
			, meta.is_table()
			, meta.ctor()
			, meta.dtor()
			, meta.cmp()
			, meta.lexcast()
			, meta.lexuncast()
			, meta.sqlvar() );
	}
	// syntactic sugar for our create_columns function 
	inline coldef operator()() const
	{		
		return (*this)("meta_def", 0);
	}
};

///////////////////////////////////////////////////////////////////////////////
struct coldef_generator
{
	std::vector<coldef>& vec;

	coldef_generator(std::vector<coldef>& v) : vec(v) {}

	template <typename T>
	inline void operator()(const T& t)
	{
		vec.push_back( coldef_create<T>()() );
	}
};

///////////////////////////////////////////////////////////////////////////////
template <typename TList>
inline static void create_columns(std::vector<coldef>& defs)
{
	coldef_generator genf(defs);
	boost::mpl::for_each<TList>( genf );
}

///////////////////////////////////////////////////////////////////////////////
class schema
{
protected:

	typedef std::vector<string_type>		coldefvec;
	typedef std::map<string_type,coldef>	coldefmap;

	coldefvec			colorder_;
	mutable coldefmap	cols_;

	inline const coldef* last_col() const
	{
		const size_t ncols = colorder_.size();

		if ( ncols > 0 )
		{
			const string_type& n = colorder_[ncols-1];
			return &cols_[n];
		}

		return NULL;
	}
	inline void remove_(coldefmap::iterator mi, coldefvec::iterator vi)
	{
		if ( colorder_.size() == 1 )
		{
			colorder_.clear();
			cols_.clear();
			return;
		}

		const coldefvec::iterator vbegin = colorder_.begin();
		const coldefvec::iterator vend = colorder_.end();

		assert( vi != vend );

		// we can now assume that there will be
		// at least 1 col left when we remove this
		// one - either before or after it.

		coldefvec::iterator vnext = vi + 1;
		coldefvec::iterator vprev = (vi == vbegin ? vend : vi - 1);

		// when removing last col there is nothing to do, so
		// handle the case where its not the last col
		if ( vnext != vend )
		{
			coldefvec::iterator newfront = vprev;
			if ( vprev == vend )
			{
				// removing from front of vector
				// adjust offsets of following columns
				coldef& tmpcol = cols_[*vnext];
				tmpcol.offset(0);
				tmpcol.id(0);
				newfront = vnext;
			}
			else
			{
				const coldef& nfc = cols_[*newfront];

				coldef& tmpcol = cols_[*vnext];
				tmpcol.offset(nfc.offset() + nfc.size());
				tmpcol.id(nfc.id() + 1);
			}

			coldefvec::const_iterator previ = newfront;
			for (coldefvec::iterator i = ++newfront; i != vend; ++i)
			{
				if ( i == vi ) continue;

				const coldef& prevcol = cols_[*previ];
				coldef& tmpcol = cols_[*i];
				tmpcol.offset(prevcol.offset() + prevcol.size());
				tmpcol.id(prevcol.id() + 1);

				previ = i;
			}
		}

		// now we can safely remove from containers
		colorder_.erase( vi );
		cols_.erase( mi );
	}

public:

	schema()
	: colorder_()
	, cols_() {}

	schema(const schema& s)
	: colorder_(s.colorder_)
	, cols_(s.cols_)
	{
		if ( colorder_.size() != cols_.size() )
			throw std::exception("Bad schema");

		// since we just copied the containers
		// lets verify they make sense
		size_type expectedoff = 0;
		size_type lastsize = 0;
		
		for (coldefvec::iterator vi = colorder_.begin();
			vi != colorder_.end(); ++vi)
		{
			coldefmap::const_iterator mi = cols_.find(*vi);
			if ( mi == cols_.end() )
				throw std::exception("Bad schema");

			const coldef& def = mi->second;
			if ( def.offset() != expectedoff )
				throw std::exception("Bad schema");

			if ( def.size() == 0 )
				throw std::exception("Bad schema");

			if ( ! def.ctorf_ )
				throw std::exception("Bad schema");

			if ( ! def.dtorf_ )
				throw std::exception("Bad schema");

			if ( ! def.cmpf_ )
				throw std::exception("Bad schema");

			if ( ! def.lexcastf_ )
				throw std::exception("Bad schema");

			if ( ! def.lexuncastf_ )
				throw std::exception("Bad schema");

			if ( ! def.sqlvarf_ )
				throw std::exception("Bad schema");

			// ok, next
			expectedoff = def.offset() + def.size();
		}
	}

	~schema()
	{
		colorder_.clear();
		cols_.clear();
	}

	template <typename U>
	inline size_t add(const string_type& n)
	{
		// new col offset is current row size
		coldef newcol = coldef_create<U>()(n, row_size());

		// no column can have zero size
		if ( newcol.size() == 0 )
			throw std::exception("Bad schema");

		const size_type cid = colorder_.size();
		newcol.id(cid);
		cols_.insert( std::make_pair(n, newcol) );
		colorder_.push_back(n);
		return cid;
	}
	inline void remove(const string_type& n)
	{
		coldefmap::iterator mi = cols_.find(n);
		if ( mi == cols_.end() )
		{
			std::stringstream strm;
			strm << "Column '" << n << "' not in schema";
			throw std::exception(strm.str().c_str());
		}

		coldefvec::iterator vi =
			std::find(colorder_.begin(), colorder_.end(), n);

		remove_(mi, vi);
	}
	inline void remove(size_t c) { remove(colorder_[c]); }
	inline const string_type& column_at(size_t c) { return colorder_[c]; }
	inline size_t size() const { return colorder_.size(); }
	inline bool contains(const string_type& cn) const
	{
		return cols_.find(cn) != cols_.end();
	}
	inline const coldef& operator[](const string_type& n) const
	{
		return cols_[n];
	}
	inline const coldef& operator[](size_t i) const
	{
		assert( i >= 0 && i < size() );
		return cols_[ colorder_[i] ];
	}
	inline bool operator==(const schema& rhs) const
	{
		if ( this == &rhs )
			return true;

		const size_t ncols = size();
		const size_t rw = rhs.size();
		if ( ncols != rw )
			return false;

		// so they are the same size...now compare schemas
		for (size_t c = 0; c < ncols; ++c)
		{
			const coldef& def = cols_[ colorder_[c] ];
			const coldef& defrhs = rhs.cols_[ colorder_[c] ];

			if ( def != defrhs )
				return false;
		}

		return true;
	}
	inline bool operator!=(const schema& rhs) const { return !(*this == rhs); }
	inline size_t row_size() const
	{
		const coldef* lastcol = last_col();

		size_t total;
		if ( lastcol )
			total = lastcol->offset() + lastcol->size();
		else
			total = 0;

		return total;
	}

	friend inline std::ostream& operator<<(std::ostream& os, const schema& s)
	{
		os << "<row-schema rowsize=" << s.row_size() << ">\n";

		for ( coldefvec::const_iterator ci = s.colorder_.begin();
			ci != s.colorder_.end(); ++ci)
		{
			os << "  " << s.cols_[*ci] << std::endl;
		}

		os << "</row-schema>";
		return os;
	}
};

} // namespace smart_table
