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

#include "FloatTest.hpp"

#include <data/Float.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::ut::FloatTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void FloatTest::setUp()
{
    // Set up context before running a test.

}

//------------------------------------------------------------------------------

void FloatTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void FloatTest::methode1()
{
    const float VALUES[]     = { -3.141592f, 0.f, 2.04f, 10, std::numeric_limits< float >::infinity() };
    const float NAN_VALUES[] = {
        std::numeric_limits< float >::quiet_NaN(),
        std::numeric_limits< float >::signaling_NaN()
    };

    for( float VALUE : VALUES )
    {
        data::Float::sptr f0 = data::Float::New();
        f0->value() = VALUE;
        data::Float::sptr f1 = data::Float::New( VALUE );

        CPPUNIT_ASSERT_EQUAL( VALUE, f0->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, f1->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, data::Float::New( VALUE )->value() );
    }

    for( float VALUE : NAN_VALUES )
    {
        data::Float::sptr f0 = data::Float::New();
        f0->value() = VALUE;
        data::Float::sptr f1 = data::Float::New( VALUE );

        CPPUNIT_ASSERT( !( VALUE == f0->value() ) );
        CPPUNIT_ASSERT( !( VALUE < f0->value() ) );
        CPPUNIT_ASSERT( !( VALUE > f0->value() ) );

        CPPUNIT_ASSERT( !( VALUE == f1->value() ) );
        CPPUNIT_ASSERT( !( VALUE < f1->value() ) );
        CPPUNIT_ASSERT( !( VALUE > f1->value() ) );

        CPPUNIT_ASSERT( !( VALUE == data::Float::New( VALUE )->value() ) );
        CPPUNIT_ASSERT( !( VALUE < data::Float::New( VALUE )->value() ) );
        CPPUNIT_ASSERT( !( VALUE > data::Float::New( VALUE )->value() ) );
    }
}

} //namespace ut
} //namespace sight::data
