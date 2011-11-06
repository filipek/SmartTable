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
// $Id: sqlvm.cpp 52 2009-11-18 10:27:28Z filipek $
//

#include "stlib/sqlparser.h"
#include "stlib/sqlvm.h"

using namespace smart_table;
using namespace smart_table::sql_vm;

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    std::cout << "Welcome to SqlVm!\n";
    std::cout << "Type an expression or [q or Q] to quit\n\n";

	const string_type filipek("filipek");
	const string_type agusia("agusia");

	variables::symtab syms1;
	syms1.insert( std::make_pair("col1", sqlvar(1)) );
	syms1.insert( std::make_pair("col2", sqlvar(2.1)) );
	syms1.insert( std::make_pair("col3", sqlvar(filipek)) );

	variables vars( syms1 );

	vmachine mach;
    sqlvarvec code;
    calculator calc(code, vars);

    std::string q;
    while (std::getline(std::cin, q))
    {
        if (q.empty() || q[0] == 'q' || q[0] == 'Q')
            break;

        code.clear();
		std::string errmsg;
        if (compile(calc, q, errmsg))
        {
			try
			{
				mach.execute(code, syms1);
				
				std::cout << "\n\nresult = "
					<< cast_<bool>(mach.top()) << "\n\n";

				variables::symtab syms2;
				syms2.insert( std::make_pair("col1", sqlvar(123)) );
				syms2.insert( std::make_pair("col2", sqlvar(2.456)) );
				syms2.insert( std::make_pair("col3", sqlvar(agusia)) );
				mach.execute(code, syms2);
				
				std::cout << "\n\nresult 2 = "
					<< cast_<bool>(mach.top()) << "\n\n";
			}
			catch(std::exception& ex)
			{
				std::cout << "exception:\n" << ex.what() << "\n";
			}
        }
    }

    std::cout << "done.\n";
    return 0;
}
