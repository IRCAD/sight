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

#include "IntegerTest.hpp"

#include <data/Integer.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( data::ut::IntegerTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void IntegerTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IntegerTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IntegerTest::methode1()
{
    const std::int64_t VALUES[] = {
        std::numeric_limits< std::int64_t >::min(),
        -1654, 0, 123456,
        std::numeric_limits< std::int64_t >::max()
    };

    for ( std::int64_t VALUE : VALUES )
    {
        data::Integer::sptr i0 = data::Integer::New();
        i0->value() = VALUE;
        data::Integer::sptr i1 = data::Integer::New( VALUE );

        CPPUNIT_ASSERT_EQUAL( VALUE, i0->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, i1->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, data::Integer::New( VALUE )->value() );
    }
}

} //namespace ut
} //namespace sight::data
