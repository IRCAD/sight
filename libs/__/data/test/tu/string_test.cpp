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

void string_test::methode1()
{
    using namespace std::literals::string_literals;
    const std::array VALUES = {""s, "chaine_caractere"s};

    for(const std::string& VALUE : VALUES)
    {
        data::string::sptr s0 = std::make_shared<data::string>();
        s0->value() = VALUE;
        data::string::sptr s1 = std::make_shared<data::string>(VALUE);
        data::string::sptr s2 = std::make_shared<data::string>(VALUE + "other");

        CPPUNIT_ASSERT(*s0 == *s1);
        CPPUNIT_ASSERT(*s0 != *s2);
        CPPUNIT_ASSERT_EQUAL(VALUE, s0->value());
        CPPUNIT_ASSERT_EQUAL(VALUE, s1->value());
        CPPUNIT_ASSERT_EQUAL(VALUE, std::make_shared<data::string>(VALUE)->value());
    }
}

} // namespace sight::data::ut
