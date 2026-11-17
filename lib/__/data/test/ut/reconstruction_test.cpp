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

#include <data/image.hpp>
#include <data/reconstruction.hpp>

#include <doctest/doctest.h>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

TEST_SUITE("sight::data::reconstruction")
{
//------------------------------------------------------------------------------

    TEST_CASE("accessors") // testing setters et getters
    {
        const bool is_visible            = true;
        const std::string organ_name     = "OrganName";
        const std::string structure_type = "structure_t";
        const std::uint32_t label        = 42;

        // process
        auto p1 = std::make_shared<sight::data::reconstruction>();

        p1->set_is_visible(is_visible);
        p1->set_organ_name(organ_name);
        p1->set_structure_type(structure_type);
        p1->set_label(label);

        // check
        CHECK_EQ(p1->get_is_visible(), is_visible);
        CHECK_EQ(p1->get_organ_name(), organ_name);
        CHECK_EQ(p1->get_structure_type(), structure_type);

        auto label_opt = p1->get_label();
        CHECK(label_opt);
        CHECK_EQ(*label_opt, label);

        auto p2 = std::make_shared<sight::data::reconstruction>();
        CHECK(*p1 != *p2);

        p2->set_is_visible(is_visible);
        p2->set_organ_name(organ_name);
        p2->set_structure_type(structure_type);
        p2->set_label(label);

        CHECK(*p1 == *p2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("image")
    {
        sight::data::reconstruction::sptr p1 = std::make_shared<sight::data::reconstruction>();
        sight::data::image::sptr i1(std::make_shared<sight::data::image>());

        p1->set_image(i1);
        CHECK_EQ(p1->get_image(), i1);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto reconstruction1 = std::make_shared<sight::data::reconstruction>();
        auto reconstruction2 = std::make_shared<sight::data::reconstruction>();

        CHECK(*reconstruction1 == *reconstruction2);
        CHECK(!(*reconstruction1 != *reconstruction2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            reconstruction1->op; \
            CHECK_MESSAGE( \
                *reconstruction1 != *reconstruction2, \
                "Reconstructions should be different when using " #op " on the first one" \
            ); \
            CHECK_MESSAGE( \
                !(*reconstruction1 == *reconstruction2), \
                "Reconstructions should be different when using " #op " on the first one" \
            ); \
            reconstruction2->op; \
            CHECK_MESSAGE( \
                *reconstruction1 == *reconstruction2, \
                "Reconstructions should be equal when using " #op " on both" \
            ); \
            CHECK_MESSAGE( \
                !(*reconstruction1 != *reconstruction2), \
                "Reconstructions should be equal when using " #op " on both" \
            );

        TEST(set_is_visible(true));
        TEST(set_organ_name("1"));
        TEST(set_structure_type("2"));
        TEST(set_image(std::make_shared<sight::data::image>()));
        TEST(set_mesh(std::make_shared<sight::data::mesh>()));
        TEST(set_label(234));
        auto material = std::make_shared<sight::data::material>();
        material->set_ambient(std::make_shared<sight::data::color>(3.F, 4.F, 5.F));
        material->set_diffuse(std::make_shared<sight::data::color>(6.F, 7.F, 8.F));
        material->set_shading_mode(sight::data::material::shading_t::ambient);
        material->set_representation_mode(sight::data::material::point);
        material->set_options_mode(sight::data::material::normals);
        material->set_diffuse_texture_filtering(sight::data::material::linear);
        material->set_diffuse_texture_wrapping(sight::data::material::clamp);
        TEST(set_material(material));
        TEST(set_computed_mask_volume(9));

    #undef TEST
    }
} // TEST_SUITE("sight::data::reconstruction")
