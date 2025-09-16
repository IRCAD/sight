/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <core/string.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::tools::string")
{
//------------------------------------------------------------------------------

    TEST_CASE("concat_basic")
    {
        const auto hello_world = sight::core::string::concat("Hello", " ", "World", " ", "!");
        CHECK_EQ(std::string("Hello World !"), hello_world);

        const auto number_of_the_beast = sight::core::string::concat(
            std::string("The number of the beast is:"),
            " ",
            6,
            6,
            6
        );
        CHECK_EQ(std::string("The number of the beast is: 666"), number_of_the_beast);
    }

//------------------------------------------------------------------------------

    TEST_CASE("concat_empty")
    {
        const auto hello_world = sight::core::string::concat(
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
        CHECK_EQ(std::string("Hello World !"), hello_world);
    }

//------------------------------------------------------------------------------

    TEST_CASE("join_basic")
    {
        const auto hello_world = sight::core::string::join(" ", "Hello", "World", "!");
        CHECK_EQ(std::string("Hello World !"), hello_world);

        const auto service_id = sight::core::string::join("-", "config_manager", 6, 6, 6, "scene_srv");
        CHECK_EQ(std::string("config_manager-6-6-6-scene_srv"), service_id);
    }

//------------------------------------------------------------------------------

    TEST_CASE("join_empty")
    {
        const auto hello_world = sight::core::string::join(
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
        CHECK_EQ(std::string("Hello World !"), hello_world);

        const auto service_id = sight::core::string::join(
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
        CHECK_EQ(std::string("config_manager-6-6-6-scene_srv"), service_id);
    }
} // TEST_SUITE
