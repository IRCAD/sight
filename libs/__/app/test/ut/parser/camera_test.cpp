/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "camera_test.hpp"

#include <app/parser/camera.hpp>

#include <core/runtime/path.hpp>

#include <data/camera.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::camera_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void camera_test::resource_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("resource", "sight::io/aruco_tag.m4v");
    auto object = std::make_shared<data::camera>();

    parser::camera parser;
    CPPUNIT_ASSERT(parser.is_a("sight::app::parser::camera"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
    service::object_parser::objects_t sub_objects;
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(
        sight::core::runtime::get_resource_file_path("sight::io/aruco_tag.m4v").string(),
        object->get_video_file().string()
    );
    CPPUNIT_ASSERT_EQUAL(data::camera::source_t::file, object->get_camera_source());
}

//------------------------------------------------------------------------------

void camera_test::file_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("file", "/this/file/path/is/totally/real/and/wont/fail.avi");
    auto object = std::make_shared<data::camera>();

    parser::camera parser;
    service::object_parser::objects_t sub_objects;
    // As a correct parsing has already been tested, we can use a failing path and ensure it failed.
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(
        std::string("/this/file/path/is/totally/real/and/wont/fail.avi"),
        object->get_video_file().string()
    );
    CPPUNIT_ASSERT_EQUAL(data::camera::source_t::file, object->get_camera_source());
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser::ut
