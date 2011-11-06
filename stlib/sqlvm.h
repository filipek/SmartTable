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
// $Id: sqlvm.h 56 2011-11-05 20:51:21Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "sqlvar.h"

namespace smart_table
{

namespace sql_vm
{

////////////////////////////////////////////////////////////////////////////
class vmachine
{
public:
	vmachine(unsigned stackSize = 1024)
	: stack(stackSize),
	stack_ptr(stack.begin()) {}

    const sqlvar& top() const { return stack_ptr[-1]; };
    void execute(const sqlvarvec& code, variables::symtab& vars);

private:

////////////////////////////////////////////////////////////////////////////
	// used to find item in list. must be a template on
	// the type we want to compare (eg int, date_type)
	// to be used in sql_bin_op_, see below.
	template <typename X>
	struct item_in_list_
	{
	private:

		struct eq_
		{
		private:

			const X x;

		public:

			eq_(const sqlvar& vx) : x(cast_<X>(vx)) {}

			inline bool operator()(const sqlvar& y) const
			{
				return std::equal_to<X>()(x, cast_<X>(y));
			}
		};

	public:

		inline bool operator()(
			const sqlvar& item
			, const sqlvarvec& list) const
		{
			sqlvarvec::const_iterator fi =
				std::find_if(list.begin(), list.end(), eq_(item) );

			return (fi != list.end());
		}
	};

////////////////////////////////////////////////////////////////////////////
	typedef boost::function<sqlvar (const sqlvarvec&)> sql_op_ptr;

	template <typename S>
	struct op_base
	{
		inline static void bad_arg_count_error(void)
		{
			string_type msg(boost::mpl::c_str<S>::value);
			msg += ": wrong number of args.";
			throw std::exception(msg.c_str());
		}
	};

	template <typename S, template <class> class Op>
	struct sql_op
	{
		struct type
		{
			const string_type& name() const
			{
				static const string_type sn(boost::mpl::c_str<S>::value);
				return sn;
			}

			inline sqlvar operator()(const sqlvarvec& argv) const
			{
				typedef Op<S> op_func;
				static const op_func op_;
				return op_(argv);
			}

			inline sql_op_ptr pointer(void) const
			{
				static sql_op_ptr p = type();
				return p;
			}
		};
	};

	template <typename S>
	struct date_op : op_base<S>
	{
		inline sqlvar operator()(const sqlvarvec& argv) const
		{
			if ( argv.size() == 0 )
			{
				return boost::gregorian::day_clock::local_day();
			}
			else if ( argv.size() == 1 )
			{
				return boost::gregorian::date_from_iso_string(
					cast_<string_type>(argv[0]));
			}

			bad_arg_count_error();
		}
	};

	template <typename S>
	struct today_op : op_base<S>
	{
		inline sqlvar operator()(const sqlvarvec& argv) const
		{
			if ( argv.size() == 0 )
			{
				return boost::gregorian::day_clock::local_day();
			}

			bad_arg_count_error();
		}
	};

	template <typename S>
	struct datetime_op : op_base<S>
	{
		inline sqlvar operator()(const sqlvarvec& argv) const
		{
			if ( argv.size() == 0 )
			{
				return boost::posix_time::microsec_clock::local_time();
			}
			else if ( argv.size() == 1 )
			{
				return boost::posix_time::from_iso_string(
					cast_<string_type>(argv[0]));
			}

			bad_arg_count_error();
		}
	};

	template <typename S>
	struct now_op : op_base<S>
	{
		inline sqlvar operator()(const sqlvarvec& argv) const
		{
			if ( argv.size() == 0 )
			{
				return boost::posix_time::microsec_clock::local_time();
			}

			bad_arg_count_error();
		}
	};

	typedef std::map<string_type, sql_op_ptr> function_map;

	template <typename S, template <class> class F>
	inline static void register_f(function_map& fmap)
	{
		typedef typename sql_op<S,F>::type op_type;
		const op_type op_;
		fmap.insert( std::make_pair(op_.name(), op_.pointer()) ); 
	}
	inline static function_map register_funcs(void)
	{
		function_map fmap;

		// yes the mpl::string syntax is funny, but that
		// is the nature of the beast. make sure that each
		// '' block contains at most 4 chars. there is a limit
		// on the number of chars, but its reasonable for
		// function names.
		register_f< boost::mpl::string<'DATE'>, date_op >(fmap);
		register_f< boost::mpl::string<'TODA','Y'>, today_op >(fmap);
		register_f< boost::mpl::string<'DATE','TIME'>, datetime_op >(fmap);
		register_f< boost::mpl::string<'NOW'>, now_op >(fmap);

		return fmap;
	}

	inline sqlvar invoke_(
		const string_type& fn
		, const sqlvarvec& argv) const
	{
		static const function_map fmap = register_funcs();

		string_type fname(fn);

		// make lower-case
		std::transform(
			fname.begin()
			, fname.end()
			, fname.begin()
			, std::toupper);

		function_map::const_iterator fi = fmap.find(fname);
		if ( fi == fmap.end() )
		{
			string_type msg(__FUNCTION__);
			msg += "unknown function '";
			msg += fname;
			msg += '\'';
			throw std::exception(msg.c_str());
		}

		return (fi->second)(argv);
	}

	// all bool operators

	typedef sql_bin_op_< std::equal_to >		sql_eq;
	typedef sql_bin_op_< std::not_equal_to >	sql_ne;
	typedef sql_bin_op_< std::less >			sql_lt;
	typedef sql_bin_op_< std::less_equal >		sql_le;
	typedef sql_bin_op_< std::greater >			sql_gt;
	typedef sql_bin_op_< std::greater_equal >	sql_ge;
	typedef sql_bin_op_< item_in_list_ >		sql_in;

    sqlvarvec			stack;
	sqlvarvec::iterator	stack_ptr;
};

////////////////////////////////////////////////////////////////////////////
void vmachine::execute(const sqlvarvec& code, variables::symtab& vars)
{
    sqlcodevec::const_iterator pc = code.begin();
    sqlvarvec::iterator locals = stack.begin();
    stack_ptr = stack.begin() + vars.size();

	// nLists is used as the list id for use on the stack
	// the id can be used to index into listVec
	int nLists = 0;
	std::vector<sqlvarvec> listVec;
	// if true then we are processing a list
	// if false we're just processing.
	bool isList = false;

	bool is_running = true;
    while (is_running)
    {
		const byte_code bc = cast_<byte_code>(*pc++);

        switch (bc)
        {
            case OP_NOT:
				stack_ptr[-1] = ! cast_<bool>(stack_ptr[-1]);
                break;

            case OP_AND:
                stack_ptr--;
				stack_ptr[-1] = (cast_<bool>(stack_ptr[-1])
					&& cast_<bool>(stack_ptr[0]));
                break;

			case OP_OR:
                stack_ptr--;
				stack_ptr[-1] = (cast_<bool>(stack_ptr[-1])
					|| cast_<bool>(stack_ptr[0]));
                break;

            case OP_EQ:
                stack_ptr--;
				stack_ptr[-1] = sql_eq()(stack_ptr[-1], stack_ptr[0]);
                break;

            case OP_NE:
                stack_ptr--;
				stack_ptr[-1] = sql_ne()(stack_ptr[-1], stack_ptr[0]);;
                break;

            case OP_LT:
                stack_ptr--;
				stack_ptr[-1] = sql_lt()(stack_ptr[-1], stack_ptr[0]);
                break;

            case OP_LE:
                stack_ptr--;
				stack_ptr[-1] = sql_le()(stack_ptr[-1], stack_ptr[0]);
                break;

            case OP_GT:
                stack_ptr--;
				stack_ptr[-1] = sql_gt()(stack_ptr[-1], stack_ptr[0]);
                break;

            case OP_GE:
                stack_ptr--;
				stack_ptr[-1] = sql_ge()(stack_ptr[-1], stack_ptr[0]);
                break;

            case OP_LIS:
				// set-up the required list context
				isList = true;
				// new list in our vector of lists
				assert(listVec.size() == nLists);
				listVec.push_back(sqlvarvec());
                break;

			case OP_LIE:
				// push the listid onto stack
				*stack_ptr++ = nLists++;
				isList = false;
                break;

            case OP_IN:
			case OP_NIN:
				{
					--stack_ptr;

					const sqlvar& item = stack_ptr[-1];
					const int listid = cast_<int>(stack_ptr[0]);
					const sqlvarvec& list = listVec[listid];
					
					// 'item' in 'list'
					const bool isin = sql_in()(item, list);

					stack_ptr[-1] = (bc == OP_NIN ? !isin : isin);
				}
				break;

            case OP_VAL:
                // check stack overflow here!
				{
					const sqlvar& v = *pc++;

					if ( isList )
					{
						assert(listVec.size() == (nLists + 1));
						listVec[nLists].push_back( v );
					}
					else
					{
						*stack_ptr++ = v;
					}
				}
                break;
            case OP_VAR:
                // check stack overflow here!
				{
					const sqlvar& vn = *pc++;
					const string_type vname( cast_<string_type>(vn) );
					assert( vname.size() );
					*stack_ptr++ = vars[vname];
				}
                break;

            case OP_FUN:
                // check stack overflow here!
				{
					--stack_ptr;

					const sqlvar& fn = stack_ptr[-1];
					const string_type fname(cast_<string_type>(fn));

					const int listid = cast_<int>(stack_ptr[0]);
					const sqlvarvec& argv = listVec[listid];

					stack_ptr[-1] = invoke_( fname, argv );
				}
                break;

            case OP_RET:
                is_running = false;
                break;
        }
    }
}

} // namespace sql_vm

} // namespace smart_table
