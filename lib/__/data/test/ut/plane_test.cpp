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

#include <data/plane.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::plane")
{
//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto plane1 = std::make_shared<sight::data::plane>();
        auto plane2 = std::make_shared<sight::data::plane>();

        CHECK(*plane1 == *plane2);
        CHECK(!(*plane1 != *plane2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            plane1->op; \
            CHECK_MESSAGE( \
                *plane1 != *plane2, \
                "Planes should be different when " #op " of the first plane is different" \
            ); \
            CHECK_MESSAGE( \
                !(*plane1 == *plane2), \
                "Planes should be different when " #op " of the first plane is different" \
            ); \
            plane2->op; \
            CHECK_MESSAGE( \
                *plane1 == *plane2, \
                "Planes should be equal when " #op " of both plane are equal" \
            ); \
            CHECK_MESSAGE( \
                !(*plane1 != *plane2), \
                "Planes should be equal when " #op " of both plane are equal" \
            );

        TEST(
            set_points(
                {std::make_shared<sight::data::point>(1., 2., 3.), std::make_shared<sight::data::point>(4., 5., 6.),
                 std::make_shared<sight::data::point>(7., 8., 9.)
                })
        );
        TEST(set_is_intersection(false));

    #undef TEST
    }
} // TEST_SUITE("sight::data::plane")
