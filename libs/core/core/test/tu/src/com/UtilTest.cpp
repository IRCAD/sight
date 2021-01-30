/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "com/UtilTest.hpp"

#include <core/com/util/AutoBind.hpp>
#include <core/com/util/AutoBind.hxx>
#include <core/com/util/convert_function_type.hpp>
#include <core/com/util/remove_last_arg.hpp>

#include <functional>
#include <type_traits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::com::ut::UtilTest );

namespace sight::core::com
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
    CPPUNIT_ASSERT_EQUAL( 10, core::com::util::autobind(&f)(4, 6) );
    CPPUNIT_ASSERT_EQUAL( 42, (core::com::util::AutoBind< int (int), 1 >::wrap( f, 40 )(2)) );
    CPPUNIT_ASSERT_EQUAL( std::string("abc"), core::com::util::autobind(&g)("a", "b", "c") );
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

    CPPUNIT_ASSERT( ( std::is_same< F2, core::com::util::convert_function_type< CM2C >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F2, core::com::util::convert_function_type< CM2 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F2, core::com::util::convert_function_type< BF2 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F2, core::com::util::convert_function_type< F2* >::type >::value ) );
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

    CPPUNIT_ASSERT( ( std::is_same< F8, core::com::util::remove_last_arg< F9 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F7, core::com::util::remove_last_arg< F8 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F6, core::com::util::remove_last_arg< F7 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F5, core::com::util::remove_last_arg< F6 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F4, core::com::util::remove_last_arg< F5 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F3, core::com::util::remove_last_arg< F4 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F2, core::com::util::remove_last_arg< F3 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F1, core::com::util::remove_last_arg< F2 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F0, core::com::util::remove_last_arg< F1 >::type >::value ) );
    CPPUNIT_ASSERT( ( std::is_same< F0, core::com::util::remove_last_arg< F0 >::type >::value ) );

}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace sight::core::com
