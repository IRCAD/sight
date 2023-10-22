/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "boolean_test.hpp"

#include <data/boolean.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::boolean_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void boolean_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void boolean_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void boolean_test::methode1()
{
    const bool true_value  = true;
    const bool false_value = !true_value;

    auto p1 = std::make_shared<data::boolean>(true_value);
    auto p2 = std::make_shared<data::boolean>(false_value);

    // check
    CPPUNIT_ASSERT_EQUAL(true_value, std::make_shared<data::boolean>(true_value)->value());
    CPPUNIT_ASSERT_EQUAL(false_value, std::make_shared<data::boolean>(false_value)->value());

    CPPUNIT_ASSERT_EQUAL(true_value, p1->value());
    CPPUNIT_ASSERT_EQUAL(false_value, p2->value());

    CPPUNIT_ASSERT(*p1 != *p2);

    p2->setValue(true_value);

    CPPUNIT_ASSERT(*p1 == *p2);
}

} // namespace sight::data::ut
