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
#include <data/vector.hpp>

#include <app/parser/vector.hpp>

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

TEST_SUITE("sight::app::parser::Vector")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "basic_test")
    {
        using namespace std::literals::string_literals;

        boost::property_tree::ptree root;
        boost::property_tree::ptree first;
        first.put("object.<xmlattr>.uid", "first");
        first.put("object.<xmlattr>.type", "sight::data::string");
        first.put("object.value", "First");
        root.add_child("item", first);
        boost::property_tree::ptree second;
        second.put("object.<xmlattr>.uid", "second");
        second.put("object.<xmlattr>.type", "sight::data::string");
        second.put("object.value", "Second");
        root.add_child("item", second);
        auto vector = std::make_shared<sight::data::vector>();
        sight::app::parser::vector vector_parser;
        CHECK(vector_parser.is_a("sight::app::parser::vector"));
        sight::service::object_parser::objects_t sub_objects;
        vector_parser.parse(root, vector, sub_objects);
        CHECK_EQ(std::size_t(2), vector->size());
        CHECK_EQ("First"s, std::dynamic_pointer_cast<sight::data::string>((*vector)[0])->get_value());
        CHECK_EQ("Second"s, std::dynamic_pointer_cast<sight::data::string>((*vector)[1])->get_value());
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
