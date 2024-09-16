/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "map_test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/map.hpp>
#include <data/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::map_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void map_test::setUp()
{
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));
    core::runtime::add_modules(location);

    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void map_test::tearDown()
{
}

//------------------------------------------------------------------------------

void map_test::basic_test()
{
    using namespace std::literals::string_literals;
    parser::map map_parser;
    CPPUNIT_ASSERT(map_parser.is_a("sight::app::parser::map"));
    {
        std::stringstream xml_config;
        xml_config << ""
                      "  <item key=\"first\">"
                      "    <object type=\"sight::data::string\" value=\"First\" />"
                      "  </item>"
                      "  <item key=\"second\">"
                      "    <object type=\"sight::data::string\" value=\"Second\" />"
                      "  </item>"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        auto map = std::make_shared<data::map>();
        service::object_parser::objects_t sub_objects;
        map_parser.parse(config, map, sub_objects);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), map->size());
        CPPUNIT_ASSERT_EQUAL("First"s, std::dynamic_pointer_cast<data::string>((*map)["first"])->get_value());
        CPPUNIT_ASSERT_EQUAL("Second"s, std::dynamic_pointer_cast<data::string>((*map)["second"])->get_value());
    }
    {
        std::stringstream xml_config;
        xml_config << ""
                      "  <item key=\"first\" type=\"sight::data::string\" value=\"First\" />"
                      "  <item key=\"second\" type=\"sight::data::string\" value=\"Second\" />"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        auto map = std::make_shared<data::map>();
        service::object_parser::objects_t sub_objects;
        map_parser.parse(config, map, sub_objects);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), map->size());
        CPPUNIT_ASSERT_EQUAL("First"s, std::dynamic_pointer_cast<data::string>((*map)["first"])->get_value());
        CPPUNIT_ASSERT_EQUAL("Second"s, std::dynamic_pointer_cast<data::string>((*map)["second"])->get_value());
    }
}

} // namespace sight::app::parser::ut
