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
// $Id: sqlparser.h 52 2009-11-18 10:27:28Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "sqlvar.h"

namespace smart_table
{

namespace sql_vm
{

///////////////////////////////////////////////////////////////////////////////
using namespace BOOST_SPIRIT_CLASSIC_NS;

struct op_base
{
protected:

    mutable sqlvarvec& code;

public:

	op_base(sqlvarvec& c) : code(c) {}

	inline void push_code(byte_code bc) const { code.push_back(bc); }
};

///////////////////////////////////////////////////////////////////////////////
struct push_int : public op_base
{
	push_int(sqlvarvec& c) : op_base(c) {}

    void operator()(char const* s, char const* /* e */) const
    {
        const int n = static_cast<int>(strtol(s, 0, 10));
		operator()(n);
    }

    void operator()(const int& n) const
    {
        push_code(OP_VAL);
        code.push_back(n);
    }
};

///////////////////////////////////////////////////////////////////////////////
struct push_double : public op_base
{
	push_double(sqlvarvec& c) : op_base(c) {}

    void operator()(char const* s, char const* /* e */) const
    {
        const double n = strtod(s, 0);
		operator()(n);
    }

    void operator()(const double& n) const
    {
        push_code(OP_VAL);
        code.push_back(n);
    }
};

///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct push_string : public op_base
{
private:

	BOOST_MPL_ASSERT(( boost::mpl::or_<
		boost::is_same<char,T>
		, boost::is_same<wchar_t,T> >));

public:

    push_string(sqlvarvec& c) : op_base(c) {}

    void operator()(char const* s, char const* e) const
    {
		assert( s && e );

		// strip the leading wide char single quote (L')
		// and trailing single quotes ('). it just
		// so happens that the amount we need to add
		// here is the same as the size of the char!
		const char* s1 = s + sizeof(T);
		const char* e1 = e - 1;

		// empty string
		if ( s1 == e )
			operator()(string_type());

		operator()(string_type(s1, e1));
    }

	void operator()(const string_type& s) const
    {
        push_code(OP_VAL);

		// towide_fun will do nothing if 2nd
		// param is of the same type as the first
		typedef typename str_<T>::type str_type;
		str_type s2;
		towide_fun()(s, s2);

        code.push_back(s2);
    }
};

///////////////////////////////////////////////////////////////////////////////
struct push_identifier : public op_base
{
    push_identifier(sqlvarvec& c) : op_base(c) {}

    void operator()(char const* s, char const* e) const
    {
		assert( s && e );

		// empty string
		if ( s == e )
			operator()(string_type());

		operator()(string_type(s,e));
    }

	void operator()(const string_type& n) const
    {
        push_code(OP_VAL);
		code.push_back(n);
    }
};

///////////////////////////////////////////////////////////////////////////////
struct push_op : public op_base
{
private:

    const byte_code op;

public:

	push_op(byte_code o, sqlvarvec& c) : op_base(c), op(o) {}

	// instead of overloading for the various cases
	// in which push_op is used, we use a argv-signature
	// as we always ignore the params anyway.
    inline void operator()(...) const { push_code(op); }
};

///////////////////////////////////////////////////////////////////////////////
struct push_var : public op_base
{
	variables& vars;

    push_var(variables& v, sqlvarvec& c)
    : op_base(c)
	, vars(v) {}

    void operator()(const sqlvar& v) const
    {
        push_code(OP_VAR);
        code.push_back(v);

		// mark the use of this variable
		// this allows later removal of unused
		// variable references.
		const string_type vname( cast_<string_type>(v) );

		vars.used(vname);
    }
};

///////////////////////////////////////////////////////////////////////////////
template <typename GrammarT>
static bool compile(
	GrammarT const& calc
	, const string_type& expr
	, string_type& errMsg)
{
    parse_info<> result = parse(expr.c_str(), calc, space_p);

    if (result.full)
    {
        calc.code.push_back(OP_RET);
		errMsg.clear();
		return true;
    }

	const size_t errorpos =
		(size_t) (result.stop - expr.c_str());

	std::stringstream strm;
	strm << "Failed parsing at position "
		 << errorpos << " in expression '"
		<< expr << "'";
	errMsg = strm.str();

	return false;
}

///////////////////////////////////////////////////////////////////////////////
struct calculator : public grammar<calculator>
{
	calculator(sqlvarvec& c, variables& vartab)
    : code(c)
	, vars(vartab) {}

    template <typename ScannerT>
    struct definition
    {
        definition(calculator const& self)
        {
			variables& vars = self.vars;

			identifier =
				lexeme_d[
					(alpha_p | ch_p('_'))
						>> *(alpha_p | ch_p('_') | digit_p)
				][push_identifier(self.code)]
				;

            number =
				strict_real_p[push_double(self.code)]
                | int_p[push_int(self.code)]
                ;

			string_literal1 =
				lexeme_d[
					ch_p('\'') >> *(anychar_p - ch_p('\''))
						>> ch_p('\'') ]
				;

			string_literal2 =
				lexeme_d[
					ch_p('\"') >> *(anychar_p - ch_p('\"'))
						>> ch_p('\"') ]
				;

			string_literal =
				(string_literal1 | string_literal2)[push_string<char>(self.code)]
				;

			wstring_literal =
				lexeme_d[
					ch_p('L') >> (string_literal1 | string_literal2)
				][push_string<wchar_t>(self.code)]
				;

			literal_value = 
				number
				| string_literal
				| wstring_literal
				;

			argument_list =
				literal_value >> *(ch_p(',') >> literal_value)
				;

			function_call =
				identifier >>
				(ch_p('(')[push_op(OP_LIS, self.code)])
					>> ! argument_list
				>> (ch_p(')')[push_op(OP_LIE, self.code)])
				;

			factor =
				literal_value
				| vars[push_var(self.vars, self.code)]
				| function_call[push_op(OP_FUN, self.code)]
				| ch_p('(') >> expression >> ch_p(')')
				| (str_p("not") >> factor)[push_op(OP_NOT, self.code)]
				;

			value_list =
				(ch_p('(')[push_op(OP_LIS, self.code)])
					>> literal_value					
					>> *(ch_p(',') >> literal_value)
				>> (ch_p(')')[push_op(OP_LIE, self.code)])
				;

            term =
				factor
				>> *( (ch_p('=') >> factor)[push_op(OP_EQ, self.code)]
					| (str_p("!=") >> factor)[push_op(OP_NE, self.code)]
					| (ch_p('<') >> factor)[push_op(OP_LT, self.code)]
					| (str_p("<=") >> factor)[push_op(OP_LE, self.code)]
					| (ch_p('>') >> factor)[push_op(OP_GT, self.code)]
					| (str_p(">=") >> factor)[push_op(OP_GE, self.code)]
					| (str_p("not") >> str_p("in") >> value_list)[push_op(OP_NIN, self.code)]
					| (str_p("in") >> value_list)[push_op(OP_IN, self.code)]
					)
				;

			expression =
				term
				>> *( (str_p("and") >> term)[push_op(OP_AND, self.code)]
					| (str_p("or") >> term)[push_op(OP_OR, self.code)]
					)
				;
        }

		rule<ScannerT> expression, factor, term, value_list;
		rule<ScannerT> number, literal_value, wstring_literal;
		rule<ScannerT> string_literal1, string_literal2, string_literal;
		rule<ScannerT> function_call, argument_list, identifier;

        rule<ScannerT> const& start() const { return expression; }
    };

    sqlvarvec&	code;
	variables&	vars;
};

} // namespace sql_vm

} // namespace smart_table
