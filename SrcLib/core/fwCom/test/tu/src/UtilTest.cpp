/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "UtilTest.hpp"

#include <fwCom/util/AutoBind.hpp>
#include <fwCom/util/AutoBind.hxx>
#include <fwCom/util/convert_function_type.hpp>
#include <fwCom/util/remove_last_arg.hpp>

#include <boost/bind.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/typeof/typeof.hpp>

#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCom::ut::UtilTest );

namespace fwCom
{
namespace ut
{

//------------------------------------------------------------------------------

void UtilTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void UtilTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

int f (int a, int b)
{
    return a+b;
}

//------------------------------------------------------------------------------

std::string g (const std::string& a, const std::string& b, const std::string& c)
{
    return a+b+c;
}

//------------------------------------------------------------------------------

void UtilTest::autoBindTest ()
{
    CPPUNIT_ASSERT_EQUAL( 10, ::fwCom::util::autobind(&f)(4, 6) );
    CPPUNIT_ASSERT_EQUAL( 42, (::fwCom::util::AutoBind< int (int), 1 >::wrap( f, 40 )(2)) );
    CPPUNIT_ASSERT_EQUAL( std::string("abc"), ::fwCom::util::autobind(&g)("a", "b", "c") );
}

//-----------------------------------------------------------------------------

class C;

// double func( float, const char* ){};

void UtilTest::convertFunctionTypeTest ()
{
    typedef void (C::* CM2C)(int, float) const;
    typedef void (C::* CM2) (int, float);

    typedef std::function< void (int, float) > BF2;

    typedef void F2 (int, float);

    CPPUNIT_ASSERT( ( boost::is_same< F2, ::fwCom::util::convert_function_type< CM2C >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F2, ::fwCom::util::convert_function_type< CM2 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F2, ::fwCom::util::convert_function_type< BF2 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F2, ::fwCom::util::convert_function_type< F2* >::type >::value ) );
}

//-----------------------------------------------------------------------------

void UtilTest::removeLastArgTest ()
{
    typedef long long F9 (int*, double, float, char, double*, float*, char*, int, const char*);
    typedef long long F8 (int*, double, float, char, double*, float*, char*, int);
    typedef long long F7 (int*, double, float, char, double*, float*, char*);
    typedef long long F6 (int*, double, float, char, double*, float*);
    typedef long long F5 (int*, double, float, char, double*);
    typedef long long F4 (int*, double, float, char);
    typedef long long F3 (int*, double, float);
    typedef long long F2 (int*, double);
    typedef long long F1 (int*);
    typedef long long F0 ();

    CPPUNIT_ASSERT( ( boost::is_same< F8, ::fwCom::util::remove_last_arg< F9 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F7, ::fwCom::util::remove_last_arg< F8 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F6, ::fwCom::util::remove_last_arg< F7 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F5, ::fwCom::util::remove_last_arg< F6 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F4, ::fwCom::util::remove_last_arg< F5 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F3, ::fwCom::util::remove_last_arg< F4 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F2, ::fwCom::util::remove_last_arg< F3 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F1, ::fwCom::util::remove_last_arg< F2 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F0, ::fwCom::util::remove_last_arg< F1 >::type >::value ) );
    CPPUNIT_ASSERT( ( boost::is_same< F0, ::fwCom::util::remove_last_arg< F0 >::type >::value ) );

}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwCom
