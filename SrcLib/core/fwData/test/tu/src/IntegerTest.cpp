/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "IntegerTest.hpp"

#include <fwData/Integer.hpp>

#include <limits>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::IntegerTest );

namespace fwData
{
namespace ut
{

void IntegerTest::setUp()
{
    // Set up context before running a test.
}

void IntegerTest::tearDown()
{
    // Clean up after the test run.
}

void IntegerTest::methode1()
{
    const int VALUES[] = {
        std::numeric_limits< int >::min(),
        -1654, 0, 123456,
        std::numeric_limits< int >::max()
    };

    for ( int VALUE : VALUES )
    {
        ::fwData::Integer::sptr i0 = ::fwData::Integer::New();
        i0->value()                = VALUE;
        ::fwData::Integer::sptr i1 = ::fwData::Integer::New( VALUE );

        CPPUNIT_ASSERT_EQUAL( VALUE, i0->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, i1->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, ::fwData::Integer::New( VALUE )->value() );
    }
}

} //namespace ut
} //namespace fwData
