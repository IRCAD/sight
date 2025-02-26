/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

void boolean_test::basic()
{
    sight::data::boolean b1(true);
    CPPUNIT_ASSERT_EQUAL(true, b1.value());
    CPPUNIT_ASSERT(b1.is_type_of("sight::data::boolean"));
    CPPUNIT_ASSERT(b1.is_type_of("sight::data::string_serializable"));

    sight::data::boolean b2;
    CPPUNIT_ASSERT_EQUAL(false, b2.value());
    b2 = true;
    CPPUNIT_ASSERT_EQUAL(true, b2.value());

    CPPUNIT_ASSERT(b1 == b2);

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

    p2->set_value(true_value);

    CPPUNIT_ASSERT(*p1 == *p2);
}

//------------------------------------------------------------------------------

void boolean_test::string_conversion()
{
    sight::data::boolean b1;
    CPPUNIT_ASSERT_EQUAL(false, b1.value());
    CPPUNIT_ASSERT_EQUAL(std::string("false"), b1.to_string());

    b1 = true;
    CPPUNIT_ASSERT_EQUAL(std::string("true"), b1.to_string());

    sight::data::boolean b2;
    CPPUNIT_ASSERT_EQUAL(false, b2.value());
    b2.from_string("true");
    CPPUNIT_ASSERT_EQUAL(true, b2.value());

    b2.from_string("false");
    CPPUNIT_ASSERT_EQUAL(false, b2.value());

    CPPUNIT_ASSERT_THROW(b2.from_string("test"), data::exception);

    std::stringstream stream;
    stream << b2;
    CPPUNIT_ASSERT_EQUAL(std::string("false"), stream.str());
}

} // namespace sight::data::ut
