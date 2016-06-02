#pragma once

#include "typedefs.h"
#include "byte_code_op.h"
#include "identifiers.h"

namespace smart_table
{
	namespace query_parser
	{
		using namespace std;

		////////////////////////////////////////////////////////////////////////////
		struct op_base
		{
		protected:

			program& code;

		public:

			op_base(program& c) : code(c) {}

			inline void push_code(byte_code_op bc) { code.push_back(bc); }
		};

		///////////////////////////////////////////////////////////////////////////////
		struct push_int : public op_base
		{
			push_int(program& c) : op_base(c) {}

			void operator()(char const* s, char const* /* e */)
			{
				const int n = static_cast<int>(strtol(s, 0, 10));
				operator()(n);
			}

			void operator()(const int& n)
			{
				push_code(OP_VAL);
				code.push_back(n);
			}
		};

		///////////////////////////////////////////////////////////////////////////////
		struct push_double : public op_base
		{
			push_double(program& c) : op_base(c) {}

			void operator()(char const* s, char const* /* e */)
			{
				const double n = strtod(s, 0);
				operator()(n);
			}

			void operator()(const double& n)
			{
				push_code(OP_VAL);
				code.push_back(n);
			}
		};

		///////////////////////////////////////////////////////////////////////////////
		struct push_string : public op_base
		{
			push_string(program& c) : op_base(c) {}

			void operator()(char const* s, char const* e)
			{
				assert( s && e );

				// strip the leading wide char single quote (L')
				// and trailing single quotes ('). it just
				// so happens that the amount we need to add
				// here is the same as the size of the char!
				const char* s1 = s + sizeof(char);
				const char* e1 = e - 1;

				// empty string
				if (s1 == e)
				{
					operator()(string_type());
				}

				operator()(string_type(s1, e1));
			}

			void operator()(const string_type& s)
			{
				push_code(OP_VAL);
				code.push_back(s);
			}
		};

		///////////////////////////////////////////////////////////////////////////////
		struct push_identifier : public op_base
		{
			push_identifier(program& c) : op_base(c) {}

			void operator()(char const* s, char const* e)
			{
				assert( s && e );

				// empty string
				if ( s == e )
					operator()(string_type());

				operator()(string_type(s,e));
			}

			void operator()(const string_type& n)
			{
				push_code(OP_VAL);
				code.push_back(n);
			}
		};

		///////////////////////////////////////////////////////////////////////////////
		struct push_op : public op_base
		{
		private:

			const byte_code_op op;

		public:

			push_op(byte_code_op o, program& c) : op_base(c), op(o) {}

			// instead of overloading for the various cases
			// in which push_op is used, we use a argv-signature
			// as we always ignore the params anyway.
			inline void operator()(...) { push_code(op); }
		};

		///////////////////////////////////////////////////////////////////////////////
		struct push_var : public op_base
		{
			identifiers& vars;

			push_var(identifiers& v, program& c)
			: op_base(c)
			, vars(v) {}

			void operator()(const sql_value& v)
			{
				push_code(OP_VAR);
				code.push_back(v);

				// mark the use of this variable
				// this allows later removal of unused
				// variable references.
				const string_type vname( cast_<string_type>(v) );

				//vars.used(vname);
			}
		};
	}
}