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

#include "vector_test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/string.hpp>
#include <data/vector.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::vector_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void vector_test::setUp()
{
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_app");
    CPPUNIT_ASSERT(std::filesystem::exists(location));
    core::runtime::add_modules(location);

    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void vector_test::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(vectorParser.destroyConfig());
}

//------------------------------------------------------------------------------

void vector_test::basicTest()
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
    auto vector = std::make_shared<data::vector>();
    vectorParser.setObjectConfig(root);
    vectorParser.createConfig(vector);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), vector->size());
    CPPUNIT_ASSERT_EQUAL("First"s, std::dynamic_pointer_cast<data::string>((*vector)[0])->getValue());
    CPPUNIT_ASSERT_EQUAL("Second"s, std::dynamic_pointer_cast<data::string>((*vector)[1])->getValue());
}

} // namespace sight::app::parser::ut
