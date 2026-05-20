/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>

#include <doctest/doctest.h>

#include <utility>

TEST_SUITE("sight::data::vec")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        {
            sight::data::dvec2 v2;
            CHECK(v2.is_type_of("sight::data::dvec2"));
            CHECK(v2.is_type_of("sight::data::string_serializable"));
            sight::data::dvec3 v3;
            CHECK(v3.is_type_of("sight::data::dvec3"));
            CHECK(v3.is_type_of("sight::data::string_serializable"));
            sight::data::dvec4 v4;
            CHECK(v4.is_type_of("sight::data::dvec4"));
            CHECK(v4.is_type_of("sight::data::string_serializable"));

            sight::data::ivec2 i2;
            CHECK(i2.is_type_of("sight::data::ivec2"));
            CHECK(i2.is_type_of("sight::data::string_serializable"));
            sight::data::ivec3 i3;
            CHECK(i3.is_type_of("sight::data::ivec3"));
            CHECK(i3.is_type_of("sight::data::string_serializable"));
            sight::data::ivec4 i4;
            CHECK(i4.is_type_of("sight::data::ivec4"));
            CHECK(i4.is_type_of("sight::data::string_serializable"));
        }

        sight::data::dvec2 v1;
        v1 = sight::vec2d_t({0.2, 0.8});
        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.value());

        sight::data::dvec2 v2 = sight::vec2d_t({0.2, 0.8});
        CHECK(v1 == v2);
        CHECK(!(v1 != v2));

        v2 = sight::vec2d_t({0.2, 0.1});
        CHECK(v1 != v2);

        sight::data::ivec4 v3({1, 2, 3, -4});
        CHECK_EQ(std::int64_t(1), v3[0]);
        CHECK_EQ(std::int64_t(2), v3[1]);
        CHECK_EQ(std::int64_t(3), v3[2]);
        CHECK_EQ(std::int64_t(-4), v3[3]);

        v3[1] = -12;
        CHECK_EQ(std::int64_t(-12), v3[1]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        auto p3 = std::make_shared<sight::data::ivec3>();
        auto p4 = std::make_shared<sight::data::ivec3>();

        *p3 = sight::ivec3_t({255, 0, -4});
        p4->shallow_copy(p3);

        CHECK(*p3 == *p4);
        CHECK(sight::ivec3_t({255, 0, -4}) == p4->value());

        // We start the inception
        auto p5 = std::make_shared<sight::data::integer>();
        p3->set_field("test_field", p5);

        CHECK(*p3 != *p4);
        p4->shallow_copy(p3);

        CHECK(*p3 == *p4);

        *p5 = 1234;
        CHECK(*p3 == *p4);

        p4->deep_copy(p3);
        CHECK(*p3 == *p4);

        // TODO: When two objects compare properly their content
        // *p5 = 4321;
        // CHECK(*p3 != *p4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("string_conversion")
    {
        sight::data::dvec2 v1;
        v1 = sight::vec2d_t({0.2, 0.8});
        CHECK_EQ(std::string("0.200000;0.800000"), v1.to_string());

        sight::data::dvec3 v2;
        v2.from_string("-0.76;0.2;0.8");
        CHECK(sight::vec3d_t({-0.76, 0.2, 0.8}) == v2.value());

        v2.from_string(" -0.8; 0.5; 0.2  ");
        CHECK(sight::vec3d_t({-0.8, 0.5, 0.2}) == v2.value());

        sight::data::ivec3 v3;
        CHECK_THROWS_AS(v3.from_string("-7894;-1557.2;48"), boost::bad_lexical_cast);
        CHECK_THROWS_AS(v3.from_string("-7894;1557;foo"), boost::bad_lexical_cast);
        CHECK_THROWS_AS(v3.from_string("7894;45;48;12"), sight::data::exception);
    }

//------------------------------------------------------------------------------

    TEST_CASE("reset")
    {
        sight::data::dvec2 v1;
        v1 = sight::vec2d_t({0.2, 0.8});

        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.value());
        CHECK(sight::vec2d_t({0., 0.}) == v1.default_value());

        v1.set_default_value();
        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.value());
        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.default_value());

        v1 = sight::vec2d_t({-0.76, 0.2});
        CHECK(sight::vec2d_t({-0.76, 0.2}) == v1.value());
        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.default_value());

        v1.reset();
        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.value());
        CHECK(sight::vec2d_t({0.2, 0.8}) == v1.default_value());
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::vec")
