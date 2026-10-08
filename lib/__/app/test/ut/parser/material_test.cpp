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

#include <data/material.hpp>

#include <app/parser/material.hpp>

#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::app::parser::Material")
{
//-----------------------------------------------------------------------------

    TEST_CASE("type")
    {
        sight::app::parser::material parser;

        CHECK(parser.is_a("sight::app::parser::material"));
        CHECK(parser.is_type_of("sight::service::object_parser"));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("complete_configuration")
    {
        boost::property_tree::ptree config;
        config.put("ambient", "#808080FF");
        config.put("diffuse", "#3366CC80");
        config.put("shading", "FLAT");
        config.put("representation", "WireFrame");
        config.put("options", "cells_normals");

        auto object = std::make_shared<sight::data::material>();
        sight::app::parser::material parser;
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(config, object, sub_objects);

        CHECK_EQ(sight::data::color(0.5019608F, 0.5019608F, 0.5019608F, 1.F), *object->ambient());
        CHECK_EQ(sight::data::color(0.2F, 0.4F, 0.8F, 0.5019608F), *object->diffuse());
        CHECK_EQ(sight::data::material::shading_t::flat, object->get_shading_mode());
        CHECK_EQ(sight::data::material::representation_t::wireframe, object->get_representation_mode());
        CHECK_EQ(sight::data::material::options_t::cells_normals, object->get_options_mode());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("missing_properties_keep_defaults")
    {
        boost::property_tree::ptree config;
        auto object         = std::make_shared<sight::data::material>();
        const auto expected = std::make_shared<sight::data::material>();

        sight::app::parser::material parser;
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(config, object, sub_objects);

        CHECK(*object == *expected);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("invalid_modes_use_defaults")
    {
        boost::property_tree::ptree config;
        config.put("shading", "invalid");
        config.put("representation", "invalid");
        config.put("options", "invalid");

        auto object = std::make_shared<sight::data::material>();
        sight::app::parser::material parser;
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(config, object, sub_objects);

        CHECK_EQ(sight::data::material::shading_t::phong, object->get_shading_mode());
        CHECK_EQ(sight::data::material::representation_t::surface, object->get_representation_mode());
        CHECK_EQ(sight::data::material::options_t::standard, object->get_options_mode());
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE
