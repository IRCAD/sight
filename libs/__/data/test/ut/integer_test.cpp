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

#include "integer_test.hpp"

#include <data/integer.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::integer_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void integer_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void integer_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void integer_test::basic()
{
    {
        sight::data::integer i;
        CPPUNIT_ASSERT(i.is_type_of("sight::data::integer"));
        CPPUNIT_ASSERT(i.is_type_of("sight::data::string_serializable"));
    }

    const std::array values {
        std::numeric_limits<std::int64_t>::min(),
        std::int64_t(-1654), std::int64_t(0), std::int64_t(123456),
        std::numeric_limits<std::int64_t>::max()
    };

    for(std::int64_t value : values)
    {
        data::integer::sptr i0 = std::make_shared<data::integer>();
        i0->value() = value;
        data::integer::sptr i1 = std::make_shared<data::integer>(value);

        CPPUNIT_ASSERT_EQUAL(value, i0->value());
        CPPUNIT_ASSERT_EQUAL(value, i1->value());
        CPPUNIT_ASSERT_EQUAL(value, std::make_shared<data::integer>(value)->value());

        CPPUNIT_ASSERT(*i0 == *i1);
    }

    for(std::int64_t value : values)
    {
        data::integer i0;
        i0.set_value(value);
        data::integer i1 = value;

        CPPUNIT_ASSERT_EQUAL(value, i0.value());
        CPPUNIT_ASSERT_EQUAL(value, i1.value());
        CPPUNIT_ASSERT(i0 == i1);
    }
}

//------------------------------------------------------------------------------

void integer_test::string_conversion()
{
    sight::data::integer i1 = 42;
    CPPUNIT_ASSERT_EQUAL(std::string("42"), i1.to_string());

    i1 = -26972;
    CPPUNIT_ASSERT_EQUAL(std::string("-26972"), i1.to_string());

    i1.from_string("45693");
    CPPUNIT_ASSERT(45693 == i1.value());

    CPPUNIT_ASSERT_THROW(i1.from_string("-7894;-1557.2;48"), boost::bad_lexical_cast);
    CPPUNIT_ASSERT_THROW(i1.from_string("-7894.489"), boost::bad_lexical_cast);
    CPPUNIT_ASSERT_THROW(i1.from_string("74vcx7aaa"), boost::bad_lexical_cast);
}

//------------------------------------------------------------------------------

void integer_test::reset()
{
    sight::data::integer i1 = 42;
    i1.set_default_value();

    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.value());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.default_value());

    i1 = 788;
    CPPUNIT_ASSERT_EQUAL(std::int64_t(788), i1.value());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.default_value());

    i1.reset();
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.value());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.default_value());

    i1 = -4788;
    CPPUNIT_ASSERT_EQUAL(std::int64_t(-4788), i1.value());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.default_value());

    i1.set_value(i1.default_value());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.value());
    CPPUNIT_ASSERT_EQUAL(std::int64_t(42), i1.default_value());
}

} // namespace sight::data::ut
