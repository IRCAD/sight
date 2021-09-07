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

#include "BooleanTest.hpp"

#include <data/Boolean.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::BooleanTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void BooleanTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void BooleanTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void BooleanTest::methode1()
{
    const bool TRUE_VALUE  = true;
    const bool FALSE_VALUE = !TRUE_VALUE;

    data::Boolean::sptr p1 = data::Boolean::New(TRUE_VALUE);
    data::Boolean::sptr p2 = data::Boolean::New(FALSE_VALUE);

    // check
    CPPUNIT_ASSERT_EQUAL(TRUE_VALUE, data::Boolean::New(TRUE_VALUE)->value());
    CPPUNIT_ASSERT_EQUAL(FALSE_VALUE, data::Boolean::New(FALSE_VALUE)->value());

    CPPUNIT_ASSERT_EQUAL(TRUE_VALUE, p1->value());
    CPPUNIT_ASSERT_EQUAL(FALSE_VALUE, p2->value());
}

} //namespace ut

} //namespace sight::data
