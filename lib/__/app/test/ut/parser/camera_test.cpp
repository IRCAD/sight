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

#include <core/runtime/path.hpp>

#include <data/camera.hpp>

#include <app/parser/camera.hpp>

#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::app::parser::Camera")
{
//------------------------------------------------------------------------------

    TEST_CASE("resource")
    {
        boost::property_tree::ptree ptree;
        ptree.put("resource", "sight::io/aruco_tag.m4v");
        auto object = std::make_shared<sight::data::camera>();

        sight::app::parser::camera parser;
        CHECK(parser.is_a("sight::app::parser::camera"));
        CHECK(parser.is_type_of("sight::service::object_parser"));
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(
            sight::core::runtime::get_resource_file_path("sight::io/aruco_tag.m4v").string(),
            object->get_video_file().string()
        );
        CHECK_EQ(sight::data::camera::source_t::file, object->get_camera_source());
    }

//------------------------------------------------------------------------------

    TEST_CASE("file")
    {
        boost::property_tree::ptree ptree;
        ptree.put("file", "/this/file/path/is/totally/real/and/wont/fail.avi");
        auto object = std::make_shared<sight::data::camera>();

        sight::app::parser::camera parser;
        sight::service::object_parser::objects_t sub_objects;
        // As a correct parsing has already been tested, we can use a failing path and ensure it failed.
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(
            std::string("/this/file/path/is/totally/real/and/wont/fail.avi"),
            object->get_video_file().string()
        );
        CHECK_EQ(sight::data::camera::source_t::file, object->get_camera_source());
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
