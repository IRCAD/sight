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

#include "string_test.hpp"

#include <data/string.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::string_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void string_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void string_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void string_test::basic()
{
    {
        sight::data::string s;
        CPPUNIT_ASSERT(s.is_type_of("sight::data::string"));
        CPPUNIT_ASSERT(s.is_type_of("sight::data::string_serializable"));
    }

    const std::array<std::string, 2> values = {"", "chaine_caractere"};

    for(const std::string& value : values)
    {
        data::string::sptr s0 = std::make_shared<data::string>();
        s0->value() = value;
        data::string::sptr s1 = std::make_shared<data::string>(value);
        data::string::sptr s2 = std::make_shared<data::string>(value + "other");

        CPPUNIT_ASSERT(*s0 == *s1);
        CPPUNIT_ASSERT(*s0 != *s2);
        CPPUNIT_ASSERT_EQUAL(value, s0->value());
        CPPUNIT_ASSERT_EQUAL(value, s1->value());
        CPPUNIT_ASSERT_EQUAL(value, std::make_shared<data::string>(value)->value());
    }
}

} // namespace sight::data::ut
