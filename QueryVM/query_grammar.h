#pragma once

#include <boost/config/warning_disable.hpp>

#include <boost/any.hpp>

#include <boost/spirit/include/classic_core.hpp>

#include <iostream>
#include <map>
#include <stack>
#include <string>

#include "grammar_actions.h"
#include "identifiers.h"

namespace smart_table
{
	namespace query_parser
	{
		using namespace std;
		using namespace BOOST_SPIRIT_CLASSIC_NS;

		////////////////////////////////////////////////////////////////////////////
		struct query_grammar : public grammar<query_grammar>
		{
			query_grammar(program& c, identifiers& v)
				: code(c)
				, vars(v) {}

			template <typename ScannerT> struct definition
			{
				definition(query_grammar const& self)
				{
					identifiers& vars = self.vars;

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
						(string_literal1 | string_literal2)[push_string(self.code)]
						;

					literal_value = 
						number
						| string_literal
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
				rule<ScannerT> number, literal_value;
				rule<ScannerT> string_literal1, string_literal2, string_literal;
				rule<ScannerT> function_call, argument_list, identifier;

				rule<ScannerT> const& start() const { return expression; }
			};

			program&		code;
			identifiers&	vars;
		};
	}
}

#pragma comment(lib, "QueryVM.lib")
