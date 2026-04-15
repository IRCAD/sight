/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

#include <core/exception.hpp>

#include <data/tools/color.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::tools::color")
{
//------------------------------------------------------------------------------

    TEST_CASE("hexa_string_to_rgba")
    {
        const std::string str_color1 = "#ff0527";
        std::array<std::uint8_t, 4> color1 {};
        CHECK_NOTHROW(sight::data::tools::color::hexa_string_to_rgba(str_color1, color1));

        CHECK_EQ(static_cast<std::uint8_t>(255), color1[0]);
        CHECK_EQ(static_cast<std::uint8_t>(5), color1[1]);
        CHECK_EQ(static_cast<std::uint8_t>(39), color1[2]);
        CHECK_EQ(static_cast<std::uint8_t>(255), color1[3]);

        const std::string str_color2 = "#45a5bc28";
        std::array<std::uint8_t, 4> color2 {};
        CHECK_NOTHROW(sight::data::tools::color::hexa_string_to_rgba(str_color2, color2));

        CHECK_EQ(static_cast<std::uint8_t>(69), color2[0]);
        CHECK_EQ(static_cast<std::uint8_t>(165), color2[1]);
        CHECK_EQ(static_cast<std::uint8_t>(188), color2[2]);
        CHECK_EQ(static_cast<std::uint8_t>(40), color2[3]);

        const std::string str_color3 = "45a5bc28";
        std::array<std::uint8_t, 4> color3 {};
        CHECK_THROWS_AS(sight::data::tools::color::hexa_string_to_rgba(str_color3, color3), sight::core::exception);

        const std::string str_color4 = "45a58";
        CHECK_THROWS_AS(sight::data::tools::color::hexa_string_to_rgba(str_color4, color3), sight::core::exception);

        const std::string str_color5 = "#45a564928";
        CHECK_THROWS_AS(sight::data::tools::color::hexa_string_to_rgba(str_color5, color3), sight::core::exception);
    }
} // TEST_SUITE("sight::data::tools")
