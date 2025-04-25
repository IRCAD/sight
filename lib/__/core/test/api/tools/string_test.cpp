/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <core/string.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::string_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void string_test::setUp()
{
}

//------------------------------------------------------------------------------

void string_test::tearDown()
{
}

//------------------------------------------------------------------------------

void string_test::concat_basic_test()
{
    const auto hello_world = core::string::concat("Hello", " ", "World", " ", "!");
    CPPUNIT_ASSERT_EQUAL(std::string("Hello World !"), hello_world);

    const auto number_of_the_beast = core::string::concat(
        std::string("The number of the beast is:"),
        " ",
        6,
        6,
        6
    );
    CPPUNIT_ASSERT_EQUAL(std::string("The number of the beast is: 666"), number_of_the_beast);
}

//------------------------------------------------------------------------------

void string_test::concat_empty_test()
{
    const auto hello_world = core::string::concat(
        "",
        "Hello",
        " ",
        "",
        "World",
        " ",
        std::string(""),
        "!",
        std::string("")
    );
    CPPUNIT_ASSERT_EQUAL(std::string("Hello World !"), hello_world);
}

//------------------------------------------------------------------------------

void string_test::join_basic_test()
{
    const auto hello_world = core::string::join(" ", "Hello", "World", "!");
    CPPUNIT_ASSERT_EQUAL(std::string("Hello World !"), hello_world);

    const auto service_id = core::string::join("-", "config_manager", 6, 6, 6, "scene_srv");
    CPPUNIT_ASSERT_EQUAL(std::string("config_manager-6-6-6-scene_srv"), service_id);
}

//------------------------------------------------------------------------------

void string_test::join_empty_test()
{
    const auto hello_world = core::string::join(
        " ",
        "",
        std::string(""),
        "Hello",
        "",
        std::string(""),
        "World",
        "!",
        "",
        std::string("")
    );
    CPPUNIT_ASSERT_EQUAL(std::string("Hello World !"), hello_world);

    const auto service_id = core::string::join(
        "-",
        "",
        std::string(""),
        "config_manager",
        "",
        std::string(""),
        6,
        6,
        6,
        "",
        std::string(""),
        "scene_srv",
        "",
        std::string("")
    );
    CPPUNIT_ASSERT_EQUAL(std::string("config_manager-6-6-6-scene_srv"), service_id);
}

} // namespace sight::core::tools::ut
