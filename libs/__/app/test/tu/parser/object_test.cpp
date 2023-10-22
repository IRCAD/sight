/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "object_test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/string.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::object_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void object_test::setUp()
{
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_app");
    CPPUNIT_ASSERT(std::filesystem::exists(location));
    core::runtime::add_modules(location);

    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void object_test::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(objectParser.destroyConfig());
}

//------------------------------------------------------------------------------

void object_test::basicTest()
{
    using namespace std::literals::string_literals;

    boost::property_tree::ptree ptree;
    ptree.put("item.<xmlattr>.key", "data");
    ptree.put("item.object.<xmlattr>.uid", "hello_world");
    ptree.put("item.object.<xmlattr>.type", "sight::data::string");
    ptree.put("item.object.value", "Hello world");
    auto object = std::make_shared<data::string>();
    objectParser.setObjectConfig(ptree);
    objectParser.createConfig(object);
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::string>(object->get_field("data")) != nullptr);
    CPPUNIT_ASSERT_EQUAL(
        "Hello world"s,
        std::dynamic_pointer_cast<data::string>(object->get_field("data"))->getValue()
    );
}

} // namespace sight::app::parser::ut
