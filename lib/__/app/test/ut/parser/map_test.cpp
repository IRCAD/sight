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

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/map.hpp>
#include <data/string.hpp>

#include <app/parser/map.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

namespace
{

struct fixture
{
    fixture()
    {
        sight::core::runtime::init();

        std::filesystem::path location = sight::core::runtime::get_resource_file_path("app_ut");
        CHECK(std::filesystem::exists(location));
        sight::core::runtime::add_modules(location);

        sight::core::runtime::load_module("sight::module::app");
    }
};

} // namespace
TEST_SUITE("sight::app::parser::map")
{
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "basic_test")
    {
        using namespace std::literals::string_literals;
        sight::app::parser::map map_parser;
        CHECK(map_parser.is_a("sight::app::parser::map"));
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

            auto map = std::make_shared<sight::data::map>();
            sight::service::object_parser::objects_t sub_objects;
            map_parser.parse(config, map, sub_objects);
            CHECK_EQ(std::size_t(2), map->size());
            CHECK_EQ("First"s, std::dynamic_pointer_cast<sight::data::string>((*map)["first"])->get_value());
            CHECK_EQ("Second"s, std::dynamic_pointer_cast<sight::data::string>((*map)["second"])->get_value());
        }
        {
            std::stringstream xml_config;
            xml_config << ""
                          "  <item key=\"first\" type=\"sight::data::string\" value=\"First\" />"
                          "  <item key=\"second\" type=\"sight::data::string\" value=\"Second\" />"
                          "";
            boost::property_tree::ptree config;
            boost::property_tree::read_xml(xml_config, config);

            auto map = std::make_shared<sight::data::map>();
            sight::service::object_parser::objects_t sub_objects;
            map_parser.parse(config, map, sub_objects);
            CHECK_EQ(std::size_t(2), map->size());
            CHECK_EQ("First"s, std::dynamic_pointer_cast<sight::data::string>((*map)["first"])->get_value());
            CHECK_EQ("Second"s, std::dynamic_pointer_cast<sight::data::string>((*map)["second"])->get_value());
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
