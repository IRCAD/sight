/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <data/reconstruction.hpp>
#include <data/resection.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::resection")
{
//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto resection1 = std::make_shared<sight::data::resection>();
        auto resection2 = std::make_shared<sight::data::resection>();

        CHECK(*resection1 == *resection2);
        CHECK(!(*resection1 != *resection2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            resection1->op; \
            CHECK_MESSAGE( \
                *resection1 != *resection2, \
                "Resections should be different when using " #op " on the first one" \
            ); \
            CHECK_MESSAGE( \
                !(*resection1 == *resection2), \
                "Resections should be different when using " #op " on the first one" \
            ); \
            resection2->op; \
            CHECK_MESSAGE( \
                *resection1 == *resection2, \
                "Resections should be equal when using " #op " on both" \
            ); \
            CHECK_MESSAGE( \
                !(*resection1 != *resection2), \
                "Resections should be equal when using " #op " on both" \
            );

        auto plane_list = std::make_shared<sight::data::plane_list>();
        plane_list->set_planes({std::make_shared<sight::data::plane>()});
        TEST(set_plane_list(plane_list));
        TEST(set_inputs({std::make_shared<sight::data::reconstruction>()}));
        TEST(set_outputs({std::make_shared<sight::data::reconstruction>()}));
        TEST(set_is_safe_part(false));
        TEST(set_name("1"));
        TEST(set_is_visible(false));
        TEST(set_is_valid(true));

    #undef TEST
    }
} // TEST_SUITE("sight::data::resection")
