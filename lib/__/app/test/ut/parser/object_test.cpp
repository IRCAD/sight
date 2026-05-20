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

#include <data/string.hpp>

#include <app/parser/object.hpp>

#include <boost/property_tree/ptree.hpp>

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
TEST_SUITE("sight::app::parser::Object")
{
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "basic_test")
    {
        using namespace std::literals::string_literals;

        boost::property_tree::ptree ptree;
        ptree.put("item.<xmlattr>.key", "data");
        ptree.put("item.object.<xmlattr>.uid", "hello_world");
        ptree.put("item.object.<xmlattr>.type", "sight::data::string");
        ptree.put("item.object.value", "Hello world");
        auto object = std::make_shared<sight::data::string>();

        sight::app::parser::object object_parser;
        CHECK(object_parser.is_a("sight::app::parser::object"));

        sight::service::object_parser::objects_t sub_objects;
        object_parser.parse(ptree, object, sub_objects);
        CHECK(std::dynamic_pointer_cast<sight::data::string>(object->get_field("data")) != nullptr);
        CHECK_EQ(
            "Hello world"s,
            std::dynamic_pointer_cast<sight::data::string>(object->get_field("data"))->get_value()
        );
    }
//------------------------------------------------------------------------------
} // TEST_SUITE
