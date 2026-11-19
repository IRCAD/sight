/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <doctest/doctest.h>

#include <cstdlib>
#include <string_view>

TEST_SUITE("sight::core::test_main")
{
    TEST_CASE("keeps_a_unit_test_off_the_desktop_of_the_user")
    {
        // A real window here would bring the other applications of the user to the front when it closes.
        // NOLINTNEXTLINE(concurrency-mt-unsafe): nothing here writes the environment.
        const char* const platform = std::getenv("QT_QPA_PLATFORM");
        REQUIRE(platform != nullptr);
        CHECK(std::string_view(platform) == "offscreen");
    }
}
