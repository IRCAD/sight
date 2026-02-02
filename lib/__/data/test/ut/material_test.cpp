/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/color.hpp>
#include <data/material.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::material")
{
//------------------------------------------------------------------------------

    TEST_CASE("methode1")
    {
        //-----------test values
        sight::data::color::sptr ambient_color = std::make_shared<sight::data::color>();
        ambient_color->set_rgba(0.5F, 0.5F, 0.5F, 0.5F);

        sight::data::color::sptr diffuse_color = std::make_shared<sight::data::color>();
        diffuse_color->set_rgba(0.8F, 0.2F, 0.5F, 0.4F);

        auto material = std::make_shared<sight::data::material>();

        material->set_ambient(sight::data::object::copy(ambient_color));
        material->set_diffuse(sight::data::object::copy(diffuse_color));

        CHECK_EQ(material->ambient()->rgba()[0], ambient_color->rgba()[0]);
        CHECK_EQ(material->diffuse()->rgba()[0], diffuse_color->rgba()[0]);

        auto material2 = std::make_shared<sight::data::material>();
        CHECK(*material != *material2);

        material2->set_ambient(sight::data::object::copy(ambient_color));
        material2->set_diffuse(sight::data::object::copy(diffuse_color));
        CHECK(*material == *material2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto material1 = std::make_shared<sight::data::material>();
        auto material2 = std::make_shared<sight::data::material>();

        CHECK(*material1 == *material2);
        CHECK(!(*material1 != *material2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            material1->op; \
            CHECK_MESSAGE( \
                *material1 != *material2, \
                "Materials should be different when using " #op " on the first one" \
            ); \
            CHECK_MESSAGE( \
                !(*material1 == *material2), \
                "Materials should be different when using " #op " on the first one" \
            ); \
            material2->op; \
            CHECK_MESSAGE( \
                *material1 == *material2, \
                "Materials should be equal when using " #op " on both" \
            ); \
            CHECK_MESSAGE( \
                !(*material1 != *material2), \
                "Materials should be equal when using " #op " on both" \
            );

        TEST(set_ambient(std::make_shared<sight::data::color>(1.F, 0.F, 0.F)));
        TEST(set_ambient(std::make_shared<sight::data::color>(0.F, 1.F, 0.F)));
        TEST(set_ambient(std::make_shared<sight::data::color>(0.F, 0.F, 1.F)));
        TEST(set_diffuse(std::make_shared<sight::data::color>(1.F, 0.F, 0.F)));
        TEST(set_diffuse(std::make_shared<sight::data::color>(0.F, 1.F, 0.F)));
        TEST(set_diffuse(std::make_shared<sight::data::color>(0.F, 0.F, 1.F)));
        TEST(set_diffuse_texture(std::make_shared<sight::data::image>()));
        TEST(set_shading_mode(sight::data::material::shading_t::ambient));
        TEST(set_representation_mode(sight::data::material::point));
        TEST(set_options_mode(sight::data::material::normals));
        TEST(set_diffuse_texture_filtering(sight::data::material::linear));
        TEST(set_diffuse_texture_wrapping(sight::data::material::clamp));

    #undef TEST
    }

//------------------------------------------------------------------------------

    TEST_CASE("string_to_representation_mode")
    {
        // Valid values
        CHECK_EQ(
            sight::data::material::string_to_representation_mode("Point"),
            sight::data::material::representation_t::point
        );
        CHECK_EQ(
            sight::data::material::string_to_representation_mode("WireFrame"),
            sight::data::material::representation_t::wireframe
        );
        CHECK_EQ(
            sight::data::material::string_to_representation_mode("edge"),
            sight::data::material::representation_t::edge
        );
        CHECK_EQ(
            sight::data::material::string_to_representation_mode("SURFACE"),
            sight::data::material::representation_t::surface
        );

        // Invalid values
        CHECK_EQ(
            sight::data::material::string_to_representation_mode("SURFACES"),
            sight::data::material::representation_t::surface
        );
        CHECK_EQ(
            sight::data::material::string_to_representation_mode("Points"),
            sight::data::material::representation_t::surface
        );
    }

    TEST_CASE("string_to_options_mode")
    {
        // Valid values
        CHECK_EQ(
            sight::data::material::string_to_options_mode("Standard"),
            sight::data::material::options_t::standard
        );
        CHECK_EQ(
            sight::data::material::string_to_options_mode("standard"),
            sight::data::material::options_t::standard
        );
        CHECK_EQ(
            sight::data::material::string_to_options_mode("Normals"),
            sight::data::material::options_t::normals
        );
        CHECK_EQ(
            sight::data::material::string_to_options_mode("NORMALS"),
            sight::data::material::options_t::normals
        );
        CHECK_EQ(
            sight::data::material::string_to_options_mode("cells_normals"),
            sight::data::material::options_t::cells_normals
        );
        CHECK_EQ(
            sight::data::material::string_to_options_mode("selected"),
            sight::data::material::options_t::selected
        );

        // Invalid values
        CHECK_EQ(
            sight::data::material::string_to_options_mode("Normal"),
            sight::data::material::options_t::standard
        );
        CHECK_EQ(
            sight::data::material::string_to_options_mode("WireframesOnSurface"),
            sight::data::material::options_t::standard
        );
    }
} // TEST_SUITE("sight::data::material")
