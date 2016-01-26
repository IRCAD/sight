/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FloatTest.hpp"

#include <fwData/Float.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::FloatTest );

namespace fwData
{
namespace ut
{

void FloatTest::setUp()
{
    // Set up context before running a test.

}

void FloatTest::tearDown()
{
    // Clean up after the test run.
}

void FloatTest::methode1()
{
    const float VALUES[]     = { -3.141592f, 0.f, 2.04f, 10, std::numeric_limits< float >::infinity() };
    const float NAN_VALUES[] = {
        std::numeric_limits< float >::quiet_NaN(),
        std::numeric_limits< float >::signaling_NaN()
    };

    for( float VALUE : VALUES )
    {
        ::fwData::Float::sptr f0 = ::fwData::Float::New();
        f0->value()              = VALUE;
        ::fwData::Float::sptr f1 = ::fwData::Float::New( VALUE );

        CPPUNIT_ASSERT_EQUAL( VALUE, f0->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, f1->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, ::fwData::Float::New( VALUE )->value() );
    }

    for( float VALUE : NAN_VALUES )
    {
        ::fwData::Float::sptr f0 = ::fwData::Float::New();
        f0->value()              = VALUE;
        ::fwData::Float::sptr f1 = ::fwData::Float::New( VALUE );

        CPPUNIT_ASSERT( !( VALUE == f0->value() ) );
        CPPUNIT_ASSERT( !( VALUE  < f0->value() ) );
        CPPUNIT_ASSERT( !( VALUE  > f0->value() ) );

        CPPUNIT_ASSERT( !( VALUE == f1->value() ) );
        CPPUNIT_ASSERT( !( VALUE  < f1->value() ) );
        CPPUNIT_ASSERT( !( VALUE  > f1->value() ) );

        CPPUNIT_ASSERT( !( VALUE == ::fwData::Float::New( VALUE )->value() ) );
        CPPUNIT_ASSERT( !( VALUE  < ::fwData::Float::New( VALUE )->value() ) );
        CPPUNIT_ASSERT( !( VALUE  > ::fwData::Float::New( VALUE )->value() ) );
    }
}

} //namespace ut
} //namespace fwData
