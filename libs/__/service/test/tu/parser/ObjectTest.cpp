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

#include "ObjectTest.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/String.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::parser::ut::ObjectTest);

namespace sight::service::parser::ut
{

//------------------------------------------------------------------------------

void ObjectTest::setUp()
{
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_service");
    CPPUNIT_ASSERT(std::filesystem::exists(location));
    core::runtime::add_modules(location);

    core::runtime::load_module("sight::module::service");
}

//------------------------------------------------------------------------------

void ObjectTest::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(objectParser.destroyConfig());
}

//------------------------------------------------------------------------------

void ObjectTest::basicTest()
{
    using namespace std::literals::string_literals;

    boost::property_tree::ptree ptree;
    ptree.put("item.<xmlattr>.key", "data");
    ptree.put("item.object.<xmlattr>.uid", "hello_world");
    ptree.put("item.object.<xmlattr>.type", "sight::data::String");
    ptree.put("item.object.value", "Hello world");
    auto object = std::make_shared<data::String>();
    objectParser.setObjectConfig(ptree);
    objectParser.createConfig(object);
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::String>(object->getField("data")) != nullptr);
    CPPUNIT_ASSERT_EQUAL("Hello world"s, std::dynamic_pointer_cast<data::String>(object->getField("data"))->getValue());
}

} // namespace sight::service::parser::ut
