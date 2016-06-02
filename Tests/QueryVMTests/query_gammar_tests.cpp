#include "stdafx.h"
#include "CppUnitTest.h"

#include "QueryVM/query_grammar.h"

////////////////////////////////////////////////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;
using namespace BOOST_SPIRIT_CLASSIC_NS;

namespace smart_table
{
	namespace query_parser
	{
		namespace tests
		{
			TEST_CLASS(query_grammar_tests)
			{
			public:

				TEST_METHOD(query_grammar_Constructor)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);
				}

				TEST_METHOD(query_grammar_Parse_Empty)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);
					query_grammar target(code, vars);

					auto info = parse("", target, space_p);

					Assert::IsFalse(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Tautology)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);
					query_grammar target(code, vars);

					auto info = parse("1=1", target, space_p);

					Assert::IsTrue(info.full);
				}

				////////////////////////////////////////////////////////////////////////////
				// Identifiers
				////////////////////////////////////////////////////////////////////////////

				TEST_METHOD(query_grammar_Parse_UnknownIdentifier)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=1", target, space_p);

					Assert::IsFalse(info.full);
				}

				TEST_METHOD(query_grammar_Parse_KnownIdentifier)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=1", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_IdentifierStartsWithNumeric_Invalid)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["1c"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("1c=1", target, space_p);

					Assert::IsFalse(info.full);
				}

				TEST_METHOD(query_grammar_Parse_IdentifierContainsNonAlphanumeric_Invalid)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["c¬"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("c¬=1", target, space_p);

					Assert::IsFalse(info.full, L"An identifier can only contain alphanumeric characters and '_'");
				}

				TEST_METHOD(query_grammar_Parse_IdentifierStartsWithUnderscore)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["_1c"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("_1c=1", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_IdentifierContainsNumeric)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["c1"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("c1=1", target, space_p);

					Assert::IsTrue(info.full);
				}

				////////////////////////////////////////////////////////////////////////////
				// Literal
				////////////////////////////////////////////////////////////////////////////

				TEST_METHOD(query_grammar_Parse_Integer)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("1", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Number)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("1.23", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_SingleQuoteLiteral)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("'abc'", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_DoubleQuoteLiteral)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("\"abc\"", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_FunctionCallWithoutParameters)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("sum()", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_FunctionCallWithOneParameter)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("sum(1)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_FunctionCallWithListOfParameters)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("sum(1,2,3)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_NegationOfLiteral)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("not 1", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Negation)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("not 1=1", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Parenthesis)
				{
					program code;
					identifiers::symbol_table symtab;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("(1=1)", target, space_p);

					Assert::IsTrue(info.full);
				}

				////////////////////////////////////////////////////////////////////////////
				// Op Equality
				// Note: Other binary operators not tested due to laziness, add if you want.
				////////////////////////////////////////////////////////////////////////////

				TEST_METHOD(query_grammar_Parse_Equality_Integer)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=1", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Equality_WithParenthesis)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("(col=1)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Equality_Double)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1.1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=1.234", target, space_p);

					Assert::IsTrue(info.full);
				}


				TEST_METHOD(query_grammar_Parse_Equality_SingleQuoteString)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1.1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col='xyz'", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Equality_SingleQuoteEmptyString)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1.1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=''", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Equality_DoubleQuoteString)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1.1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=\"xyz\"", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_Equality_DoubleQuoteEmptyString)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1.1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col=\"\"", target, space_p);

					Assert::IsTrue(info.full);
				}

				////////////////////////////////////////////////////////////////////////////
				// Op 'in' and 'not in'
				////////////////////////////////////////////////////////////////////////////

				TEST_METHOD(query_grammar_Parse_In_EmptyList_Invalid)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col in ()", target, space_p);

					Assert::IsFalse(info.full);
				}

				TEST_METHOD(query_grammar_Parse_In_OneItemInList)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col in (1)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_In_ItemsList)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col in (1,2,3)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_NotIn_EmptyList_Invalid)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col not in ()", target, space_p);

					Assert::IsFalse(info.full);
				}

				TEST_METHOD(query_grammar_Parse_NotIn_OneItemInList)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col not in (1)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_NotIn_ItemsList)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col"] = 1;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col not in (1,2,3)", target, space_p);

					Assert::IsTrue(info.full);
				}

				////////////////////////////////////////////////////////////////////////////
				// Ops 'and' and 'or'
				////////////////////////////////////////////////////////////////////////////

				TEST_METHOD(query_grammar_Parse_OpAnd)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col1"] = 1;
					symtab["col2"] = 2;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col1=1 and col2=2", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_OpAnd_FirstInParenthesis)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col1"] = 1;
					symtab["col2"] = 2;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("(col1=1) and col2=2", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_OpAnd_SecondInParenthesis)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col1"] = 1;
					symtab["col2"] = 2;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col1=1 and (col2=2)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_OpAnd_BothInParenthesis)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col1"] = 1;
					symtab["col2"] = 2;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("(col1=1) and (col2=2)", target, space_p);

					Assert::IsTrue(info.full);
				}

				TEST_METHOD(query_grammar_Parse_OpOr)
				{
					program code;
					identifiers::symbol_table symtab;
					symtab["col1"] = 1;
					symtab["col2"] = 2;
					identifiers vars(symtab);

					query_grammar target(code, vars);

					auto info = parse("col1=1 or col2=2", target, space_p);

					Assert::IsTrue(info.full);
				}
			};
		}
	}
}