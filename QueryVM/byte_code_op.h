#pragma once

namespace smart_table
{
	namespace query_parser
	{
		////////////////////////////////////////////////////////////////////////////
		enum byte_code_op
		{
		   OP_NOT,		//  ! of top stack entry
		   OP_AND,		//  && top two stack entries
		   OP_OR,		//  || top two stack entries
		   OP_EQ,		//  == top two stack entries
		   OP_NE,		//  != top two stack entries
		   OP_LT,		//  < top two stack entries
		   OP_LE,		//  <= top two stack entries
		   OP_GT,		//  > top two stack entries
		   OP_GE,		//  >= top two stack entries

		   OP_LIS,		// start of value list
		   OP_LIE,		// end of value list
		   OP_IN,		// find item (1st stack item) in list (2nd stack item)
		   OP_NIN,		// same as above except we check for item NOT in list

		   OP_VAL,		//  push constant value onto stack
		   OP_VAR,		//  push variable name onto stack
		   OP_FUN,		//  push function call onto stack
		   OP_RET		//  return from the interpreter
		};
	}
}