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
// $Id: test1.h 55 2011-10-17 19:29:56Z filipek $
//

namespace smart_table
{

using namespace boost::gregorian;
using namespace boost::posix_time;

///////////////////////////////////////////////////////////////////////////////

struct MyPOD
{
   int      field1;
   double   field2;
};

inline static bool operator==(const MyPOD& lhs, const MyPOD& rhs)
{
   return (lhs.field1 == rhs.field1 && lhs.field2 == rhs.field2);
}

inline static std::istream& operator>>(std::istream& str, MyPOD& mp)
{
   str >> mp.field1 >> mp.field2;
   return str;
}

inline static std::ostream& operator<<(std::ostream& str, const MyPOD& mp)
{
   str << mp.field1 << mp.field2;
   return str;
}

///////////////////////////////////////////////////////////////////////////////

template <typename T>
static void datatable_schema()
{
	typedef datatable_impl<T> datatable_t;

	{
		datatable_t tab("tab1");

		size_type col1 = tab.add_column<int>("col1");
		size_type col2 = tab.add_column<double>("col2");
      size_type col3 = tab.add_column<MyPOD>("col3");

		size_type row1 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 3 );
		BOOST_REQUIRE( tab.depth() == 1 );

		tab.set<int>(row1, col1, -123);
		tab.set<double>(row1, col2, -32.1);
      
      MyPOD mp1;
      mp1.field1 = 1;
      mp1.field2 = 1.2;
      tab.set<MyPOD>(row1, col3, mp1);

		BOOST_REQUIRE( tab.get<int>(row1, col1) == -123);
		BOOST_REQUIRE( tab.get<double>(row1, col2) == -32.1);
      BOOST_REQUIRE( tab.get<MyPOD>(row1, col3) == mp1);

		size_type row2 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 3 );
		BOOST_REQUIRE( tab.depth() == 2 );

		tab.set<int>(row2, col1, -765);
		tab.set<double>(row2, col2, -56.7);
      
      MyPOD mp2;
      mp2.field1 = 3;
      mp2.field2 = 3.2;
      tab.set<MyPOD>(row2, col3, mp2);

		BOOST_REQUIRE( tab.depth() == 2 );
		BOOST_REQUIRE( tab.get<int>(row2, col1) == -765);
		BOOST_REQUIRE( tab.get<double>(row2, col2) == -56.7);
      BOOST_REQUIRE( tab.get<MyPOD>(row2, col3) == mp2);
	}
	{
		datatable_t tab("tab1");

		size_type col1 = tab.add_column<int>("col1");
		size_type col2 = tab.add_column<int>("col2");

		size_type row1 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 1 );

		tab.set<int>(row1, col1, -123);
		int b1;
		tab.set<int>(row1, col2, b1);

		BOOST_REQUIRE( tab.get<int>(row1, col1) == -123);
		BOOST_REQUIRE( tab.get<int>(row1, col2) == b1);
	}
}

template <typename T>
static void datatable_rows()
{
	typedef datatable_impl<T> datatable_t;

	{
		datatable_t tab("tab1");

		size_type col1 = tab.add_column<int>("col1");
		size_type col2 = tab.add_column<double>("col2");
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 0 );

		size_type row1 = tab.add_row();
		BOOST_REQUIRE( tab.depth() == 1 );

		tab.set<int>(row1, col1, -123);
		tab.set<double>(row1, col2, -32.1);

		size_type row2 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 2 );

		tab.set<int>(row2, col1, -765);
		tab.set<double>(row2, col2, -56.7);

		tab.remove_row(row2);

		BOOST_REQUIRE( tab.depth() == 1 );
		BOOST_REQUIRE( tab.get<int>(row1, col1) == -123);
		BOOST_REQUIRE( tab.get<double>(row1, col2) == -32.1);
	}
	{
		datatable_t tab("tab1");

		size_type col1 = tab.add_column<std::wstring>("col1");
		size_type col2 = tab.add_column<double>("col2");
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 0 );

		size_type row1 = tab.add_row();
		BOOST_REQUIRE( tab.depth() == 1 );

		const std::wstring ws_expected_1(RANDOM_WSTR);
		tab.set<std::wstring>(row1, col1, ws_expected_1);
		tab.set<double>(row1, col2, -32.1);

		size_type row2 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 2 );

		const std::wstring ws_expected_2(RANDOM_WSTR);
		tab.set<std::wstring>(row2, col1, ws_expected_2);
		tab.set<double>(row2, col2, -56.7);

		size_type row3 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 3 );

		const std::wstring ws_expected_3(RANDOM_WSTR);
		tab.set<std::wstring>(row3, col1,ws_expected_3);
		tab.set<double>(row3, col2, -66.6);

		tab.remove_row(row2);
		BOOST_REQUIRE( tab.depth() == 2 );

		BOOST_REQUIRE( tab.get<std::wstring>(0, col1) == ws_expected_1);
		BOOST_REQUIRE( tab.get<double>(0, col2) == -32.1);
		BOOST_REQUIRE( tab.get<std::wstring>(1, col1) == ws_expected_3);
		BOOST_REQUIRE( tab.get<double>(1, col2) == -66.6);
	}
	{
		datatable_t tab("tab1");

		size_type col1 = tab.add_column<std::wstring>("col1");
		size_type col2 = tab.add_column<double>("col2");
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 0 );

		size_type row1 = tab.add_row();
		BOOST_REQUIRE( tab.depth() == 1 );

		const std::wstring ws_expected_1(RANDOM_WSTR);
		tab.set<std::wstring>(row1, col1, ws_expected_1);
		tab.set<double>(row1, col2, -32.1);

		size_type row2 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 2 );

		const std::wstring ws_expected_2(RANDOM_WSTR);
		tab.set<std::wstring>(row2, col1, ws_expected_2);
		tab.set<double>(row2, col2, -56.7);

		size_type row3 = tab.add_row();
		BOOST_REQUIRE( tab.width() == 2 );
		BOOST_REQUIRE( tab.depth() == 3 );

		const std::wstring ws_expected_3(RANDOM_WSTR);
		tab.set<std::wstring>(row3, col1, ws_expected_3);
		tab.set<double>(row3, col2, -66.6);

		tab.remove_row(row1);
		BOOST_REQUIRE( tab.depth() == 2 );

		BOOST_REQUIRE( tab.get<std::wstring>(0, col1) == ws_expected_2);
		BOOST_REQUIRE( tab.get<double>(0, col2) == -56.7);
		BOOST_REQUIRE( tab.get<std::wstring>(1, col1) == ws_expected_3);
		BOOST_REQUIRE( tab.get<double>(1, col2) == -66.6);
	}
}

#define GEN_TAB_BASE(t,w)\
	{\
		size_type col1 = t.add_column<int>("col1");\
		size_type col2 = t.add_column<double>("col2");\
		BOOST_REQUIRE( t.width() == w );\
		BOOST_REQUIRE( t.depth() == 0 );\
		size_type row1 = t.add_row();\
		BOOST_REQUIRE( t.depth() == 1 );\
		t.set<int>(row1, col1, -123);\
		t.set<double>(row1, col2, -32.1);\
		size_type row2 = t.add_row();\
		BOOST_REQUIRE( t.width() == w );\
		BOOST_REQUIRE( t.depth() == 2 );\
		t.set<int>(row2, col1, -765);\
		t.set<double>(row2, col2, -56.7);\
		t.remove_row(row2);\
		BOOST_REQUIRE( t.depth() == 1 );\
		BOOST_REQUIRE( t.get<int>(row1, col1) == -123);\
		BOOST_REQUIRE( t.get<double>(row1, col2) == -32.1);\
	}

#define GEN_TAB(t) GEN_TAB_BASE(t,2)

#define GEN_TABP(t)\
	{\
		size_type col1 = t->add_column<int>("col1");\
		size_type col2 = t->add_column<double>("col2");\
		BOOST_REQUIRE( t->width() == 2 );\
		BOOST_REQUIRE( t->depth() == 0 );\
		size_type row1 = t->add_row();\
		BOOST_REQUIRE( t->depth() == 1 );\
		t->set<int>(row1, col1, -123);\
		t->set<double>(row1, col2, -32.1);\
		size_type row2 = t->add_row();\
		BOOST_REQUIRE( t->width() == 2 );\
		BOOST_REQUIRE( t->depth() == 2 );\
		t->set<int>(row2, col1, -765);\
		t->set<double>(row2, col2, -56.7);\
		t->remove_row(row2);\
		BOOST_REQUIRE( t->depth() == 1 );\
		BOOST_REQUIRE( t->get<int>(row1, col1) == -123);\
		BOOST_REQUIRE( t->get<double>(row1, col2) == -32.1);\
	}

template <typename T>
static void datatable_compare()
{
	typedef datatable_impl<T> datatable_t;

	{
		datatable_t tab1("tab1");
		GEN_TAB(tab1);

		datatable_t tab2("tab2");
		GEN_TAB(tab2);

		BOOST_REQUIRE( tab1 == tab2 );
	}
	{
		datatable_t tab1("tab1");
		{
			tab1.add_column<datatable_t>("col0");
			GEN_TAB_BASE(tab1,3);

			datatable_t::ptr subtabp( new datatable_t("tab11"));
			GEN_TABP(subtabp);

			tab1.set<datatable_t>(0,0,subtabp);
		}

		datatable_t tab2("tab2");
		{
			tab2.add_column<datatable_t>("col0");
			GEN_TAB_BASE(tab2,3);

			datatable_t::ptr subtabp(new datatable_t("tab21"));
			GEN_TABP(subtabp);

			tab2.set<datatable_t>(0,0,subtabp);
		}

		BOOST_REQUIRE( tab1 == tab2 );
	}
	{
		datatable_t tab1("tab1");
		datatable_t::ptr subtab1p( new datatable_t("tab11"));
		{
			tab1.add_column<datatable_t>("col0");
			GEN_TAB_BASE(tab1,3);

			GEN_TABP(subtab1p);

			tab1.set<datatable_t>(0,0,subtab1p);
		}

		datatable_t tab2("tab2");
		datatable_t::ptr subtab2p(new datatable_t("tab21"));
		{
			tab2.add_column<datatable_t>("col0");
			GEN_TAB_BASE(tab2,3);

			GEN_TABP(subtab2p);

			BOOST_REQUIRE( *subtab1p == *subtab2p );

			// change a cell in the subtable so the overall
			// recursion fails
			subtab2p->set<double>(0,1,-0.777);
			BOOST_REQUIRE( *subtab1p != *subtab2p );

			tab2.set<datatable_t>(0,0,subtab2p);

			BOOST_REQUIRE(tab1 != tab2);
		}
	}
}

template <typename T>
static void datatable_lexcast()
{
	typedef datatable_impl<T> datatable_t;

	{
		datatable_t t;
		size_type col1 = t.add_column<int>("col1");
		size_type col2 = t.add_column<double>("col2");
		size_type col3 = t.add_column<std::string>("col3");
		size_type col4 = t.add_column<std::wstring>("col4");
		BOOST_REQUIRE( t.width() == 4 );
		BOOST_REQUIRE( t.depth() == 0 );
		size_type row1 = t.add_row();
		BOOST_REQUIRE( t.depth() == 1 );

		const string_type s1( "-123" );
		t.set<int>(0, 0, -123);
		BOOST_REQUIRE( t.get_string(0,0) == s1 );
		const string_type s12( "-1234" );
		t.set_string(0,0,s12);
		BOOST_REQUIRE( t.get_string(0,0) == s12 );

		const string_type s2( "-32.1" );
		t.set<double>(0, 1, -32.1);
		const string_type act2 = t.get_string(0,1);
		BOOST_REQUIRE( act2 == s2 );

		const std::string s3(RANDOM_STR);
		t.set<std::string>(0, 2, s3);
		const std::string act3 = t.get_string(0,2);
		BOOST_REQUIRE( act3 == s3 );

		const std::wstring ws4(RANDOM_WSTR);
		std::string s4;
		tonarrow_fun()(ws4,s4);
		t.set<std::wstring>(0, 3, ws4);
		const std::string act4 = t.get_string(0,3);
		BOOST_REQUIRE( act4 == s4 );
		const string_type s42( "hello worldski" );
		t.set_string(0,3,s42);
		const string_type tmp = t.get_string(0,3);
		BOOST_REQUIRE( tmp == s42 );
	}
}

template <typename T>
static void datatable_find()
{
	typedef datatable_impl<T> datatable_t;

	const date_type today = day_clock::local_day();
	const datetime_type now = second_clock::local_time();

	{
		datatable_t t;

		size_type col1 = t.add_column<int>("col1");
		size_type col2 = t.add_column<double>("col2");
		size_type col3 = t.add_column<std::string>("col3");
		size_type col4 = t.add_column<std::wstring>("col4");
		size_type col5 = t.add_column<date_type>("col5");
		size_type col6 = t.add_column<datetime_type>("col6");
		
		t.add_row();
		t.add_row();
		t.add_row();
		t.add_row();
		t.add_row();

		for ( size_t r = 0; r < t.depth(); ++r)
		{
			t.set<int>(r, col1, static_cast<int>(r));
			t.set<double>(r, col2, static_cast<double>(r));

			const string_type rs = boost::lexical_cast<std::string>(r);
			t.set_string(r, col3, rs);

			std::wstring wrs;
			towide_fun()(rs, wrs);
			t.set<std::wstring>(r,col4,wrs);

			// date
			date_type adate;
			do { adate = random::date(0); }
			while ( adate == today );

			t.set<date_type>(r,col5,adate);

			// datetime
			datetime_type adatetime;
			do { adatetime = random::datetime(0); }
			while ( adatetime == now );

			t.set<datetime_type>(r,col6,adatetime);
		}

		t.set<int>(t.depth()-1, col1, 0);
		t.set<double>(t.depth()-1, col2, 0.0);
		t.set<std::string>(t.depth()-1, col3, "0");
		t.set<std::wstring>(t.depth()-1, col4, L"0");
		t.set<date_type>(t.depth()-1, col5, today);
		t.set<datetime_type>(t.depth()-1, col6, now);

		// find
		typedef std::list<size_t> matchedrows;

		{
			const std::string query("0");
			matchedrows res;
			const size_t nfound = t.find<std::string>(col3, query, res);
			BOOST_REQUIRE(nfound == 2);
		}
		{
			const int query = 1;
			matchedrows res;
			const size_t nfound = t.find<int>(col1, query, res);
			BOOST_REQUIRE(nfound == 1);
		}
		{
			const date_type query = today;
			matchedrows res;
			const size_t nfound = t.find<date_type>(col5, query, res);
			BOOST_REQUIRE(nfound == 1);
		}
		{
			const datetime_type query = now;
			matchedrows res;
			const size_t nfound = t.find<datetime_type>(col6, query, res);
			BOOST_REQUIRE(nfound == 1);
		}
		
		typedef boost::mpl::list<
			int
			, double
			, std::string
			, std::wstring
			, date_type
			, datetime_type
			, datatable_t> coltypes;

		std::vector<coldef> colvec;
		create_columns< coltypes >(colvec);

		//std::copy(
		//	colvec.begin()
		//	, colvec.end()
		//	, std::ostream_iterator<coldef>(std::cout, "\n"));
	}
}

template <typename T>
static void datatable_select()
{
	typedef datatable_impl<T> datatable_t;

	const date_type today = day_clock::local_day();
	const datetime_type now = second_clock::local_time();

	{
		datatable_t t;

		size_type col1 = t.add_column<int>("col1");
		size_type col2 = t.add_column<double>("col2");
		size_type col3 = t.add_column<std::string>("col3");
		size_type col4 = t.add_column<std::wstring>("col4");
		size_type col5 = t.add_column<date_type>("col5");
		size_type col6 = t.add_column<datetime_type>("col6");
		
		t.add_row();
		t.add_row();
		t.add_row();
		t.add_row();
		t.add_row();

		for ( size_t r = 0; r < t.depth(); ++r)
		{
			t.set<int>(r, col1, static_cast<int>(r));
			t.set<double>(r, col2, static_cast<double>(r));

			const string_type rs = boost::lexical_cast<std::string>(r);
			t.set_string(r, col3, rs);

			std::wstring wrs;
			towide_fun()(rs, wrs);
			t.set<std::wstring>(r,col4,wrs);
			
			// date
			date_type adate;
			do { adate = random::date(0); }
			while ( adate == today );

			t.set<date_type>(r,col5,adate);

			// datetime
			datetime_type adatetime;
			do { adatetime = random::datetime(0); }
			while ( adatetime == now );

			t.set<datetime_type>(r,col6,adatetime);
		}

		t.set<int>(t.depth()-1, col1, 0);
		t.set<double>(t.depth()-1, col2, 0.0);
		t.set<std::string>(t.depth()-1, col3, "0");
		t.set<std::wstring>(t.depth()-1, col4, L"0");
		t.set<date_type>(t.depth()-1, col5, today);
		t.set<datetime_type>(t.depth()-1, col6, now);

		// find
		typedef std::list<size_t> matchedrows;

		{
			const string_type q( "col1 = 0" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 2);
		}
		{
			const string_type q( "col2 = 0.0" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 2);
		}
		{
			const string_type q( "col3 = '0'" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 2);
		}
		{
			const string_type q( "col4 = L'0'" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 2);
		}
		{
			const string_type q( "col5 = DATE()" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 1);
		}
		{
			string_type q( "col5 = DATE('" );
			q += boost::gregorian::to_iso_string(today);
			q +="')";
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 1);
		}
		{
			string_type q( "col6 = DATETIME('" );
			q += boost::posix_time::to_iso_string(now);
			q +="')";
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 1);
		}
		{
			string_type q( "col5 = TODAY()" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 1);
		}
		{
			string_type q( "col6 = NOW()" );
			matchedrows res;
			const size_t nfound = t.select(q, res);
			BOOST_REQUIRE(nfound == 0);
		}
	}
}

} // namespace smart_table

using namespace smart_table;

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(coldef_test)
{
	const string_type col1name("col1");
	typedef int col1type;

	const string_type col2name("col2");
	typedef double col2type;

	const string_type col3name("col3");
	typedef std::string col3type;

	coldef col1 = coldef_create<col1type>()( col1name, 0 );
	BOOST_REQUIRE( col1.size() == sizeof(column_<col1type>::type) );
	BOOST_REQUIRE( col1.name() == col1name );

	coldef col2 = coldef_create<col2type>()( col2name, 0 );
	BOOST_REQUIRE( col2.size() == sizeof(column_<col2type>::type) );
	BOOST_REQUIRE( col2.name() == col2name );

	BOOST_REQUIRE( col1 != col2 );

	coldef col3 = coldef_create<col3type>()( col1.name(), 0 );
	BOOST_REQUIRE( col3.size() == sizeof(column_<col3type>::type) );
	BOOST_REQUIRE( col3.name() == col1.name() );

	// different types, but the same name..and this
	// is enough to make columns different
	BOOST_REQUIRE( col1 != col3 );
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(schema_test_remove_middle)
{
	// remove column from middle of column list
	schema s;
	s.add<int>("col_L");
	s.add<double>("col_S");
	s.add<date_type>("col_M");
	s.add<string_type>("col_string");

	BOOST_REQUIRE( s.size() == 4 );

	const coldef& col_L = s["col_L"];
	const coldef& col_S = s["col_S"];
	const coldef& col_M = s["col_M"];
	const coldef& col_string = s["col_string"];

	BOOST_REQUIRE( col_L.size() == sizeof(int) );
	BOOST_REQUIRE( col_S.size() == sizeof(double) );
	BOOST_REQUIRE( col_M.size() == sizeof(date_type) );
	BOOST_REQUIRE( col_string.size() == sizeof(column_<string_type>::type) );

	// remove column
	s.remove( col_S.name() );
	BOOST_REQUIRE( s.size() == 3 );

	BOOST_REQUIRE( col_L.offset() == 0 );
	BOOST_REQUIRE( col_M.offset() == col_L.size() );
	BOOST_REQUIRE( col_string.offset() == col_M.offset() + col_M.size() );

	BOOST_REQUIRE( s.row_size() ==
		(col_L.size()
		+ col_M.size()
		+ col_string.size()));
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(schema_test_remove_first)
{
	// remove first column
	schema s;
	s.add<int>("col_L");
	s.add<double>("col_S");
	s.add<date_type>("col_M");
	s.add<string_type>("col_string");

	BOOST_REQUIRE( s.size() == 4 );

	const coldef& col_L = s["col_L"];
	const coldef& col_S = s["col_S"];
	const coldef& col_M = s["col_M"];
	const coldef& col_string = s["col_string"];

	BOOST_REQUIRE( col_L.size() == sizeof(int) );
	BOOST_REQUIRE( col_S.size() == sizeof(double) );
	BOOST_REQUIRE( col_M.size() == sizeof(date_type) );
	BOOST_REQUIRE( col_string.size() == sizeof(column_<string_type>::type) );

	// remove column
	s.remove( col_L.name() );
	BOOST_REQUIRE( s.size() == 3 );

	BOOST_REQUIRE( col_S.offset() == 0 );
	BOOST_REQUIRE( col_M.offset() == col_S.offset() + col_S.size() );
	BOOST_REQUIRE( col_string.offset() == col_M.offset() + col_M.size() );

	BOOST_REQUIRE( s.row_size() ==
		(col_S.size()
		+ col_M.size()
		+ col_string.size()));
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(schema_test_remove_last)
{
	// remove last column
	schema s;
	s.add<int>("col_L");
	s.add<double>("col_S");
	s.add<date_type>("col_M");
	s.add<string_type>("col_string");

	BOOST_REQUIRE( s.size() == 4 );

	const coldef& col_L = s["col_L"];
	const coldef& col_S = s["col_S"];
	const coldef& col_M = s["col_M"];
	const coldef& col_string = s["col_string"];

	BOOST_REQUIRE( col_L.size() == sizeof(int) );
	BOOST_REQUIRE( col_S.size() == sizeof(double) );
	BOOST_REQUIRE( col_M.size() == sizeof(date_type) );
	BOOST_REQUIRE( col_string.size() == sizeof(column_<string_type>::type) );

	// remove column
	s.remove( col_string.name() );
	BOOST_REQUIRE( s.size() == 3 );

	BOOST_REQUIRE( col_L.offset() == 0 );
	BOOST_REQUIRE( col_S.offset() == col_L.size() );
	BOOST_REQUIRE( col_M.offset() == col_S.offset() + col_S.size() );

	BOOST_REQUIRE( s.row_size() ==
		(col_L.size()
		+ col_S.size()
		+ col_M.size()));
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(schema_test_remove_only)
{
	// remove only column
	schema s;
	s.add<int>("col_L");

	BOOST_REQUIRE( s.size() == 1 );

	const coldef& col_L = s["col_L"];

	BOOST_REQUIRE( col_L.size() == sizeof(int) );

	// remove column
	s.remove( col_L.name() );

	BOOST_REQUIRE( s.size() == 0 );
	BOOST_REQUIRE( s.row_size() == 0 );
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(schema_test_remove_1_of_2)
{
	// remove one of two columns
	schema s;
	s.add<int>("col_L");
	s.add<date_type>("col_M");

	BOOST_REQUIRE( s.size() == 2 );

	const coldef& col_L = s["col_L"];
	const coldef& col_M = s["col_M"];

	BOOST_REQUIRE( col_L.size() == sizeof(int) );
	BOOST_REQUIRE( col_M.size() == sizeof(date_type) );

	// remove column
	s.remove( col_L.name() );
	BOOST_REQUIRE( s.size() == 1 );

	BOOST_REQUIRE( col_M.offset() == 0 );
	BOOST_REQUIRE( s.row_size() == col_M.size() );
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(schema_test_remove_2_of_2)
{
	schema s;
	s.add<int>("col_L");
	s.add<date_type>("col_M");

	BOOST_REQUIRE( s.size() == 2 );

	const coldef& col_L = s["col_L"];
	const coldef& col_M = s["col_M"];

	BOOST_REQUIRE( col_L.size() == sizeof(int) );
	BOOST_REQUIRE( col_M.size() == sizeof(date_type) );

	// remove column
	s.remove( col_M.name() );
	BOOST_REQUIRE( s.size() == 1 );

	BOOST_REQUIRE( col_L.offset() == 0 );
	BOOST_REQUIRE( s.row_size() == col_L.size() );
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(datatable_schema_test)
{
	datatable_schema<row_store>();
	datatable_schema<column_store>();
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(datatable_rows_test)
{
	datatable_rows<row_store>();
	datatable_rows<column_store>();
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(datatable_compare_test)
{
	datatable_compare<row_store>();
	datatable_compare<column_store>();
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(datatable_lexcast_test)
{
	datatable_lexcast<row_store>();
	datatable_lexcast<column_store>();
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(datatable_find_test)
{
	datatable_find<row_store>();
	datatable_find<column_store>();
}

///////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(datatable_select_test)
{
	datatable_select<row_store>();
	datatable_select<column_store>();
}
