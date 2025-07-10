/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "copy_test.hpp"

#include <data/array.hpp>
#include <data/boolean.hpp>
#include <data/color.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/line.hpp>
#include <data/map.hpp>
#include <data/material.hpp>
#include <data/matrix4.hpp>
#include <data/mesh.hpp>
#include <data/plane.hpp>
#include <data/plane_list.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>
#include <data/reconstruction.hpp>
#include <data/resection.hpp>
#include <data/resection_db.hpp>
#include <data/string.hpp>
#include <data/structure_traits.hpp>
#include <data/structure_traits_dictionary.hpp>
#include <data/transfer_function.hpp>
#include <data/vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::copy_test);

namespace sight::data::ut
{

//-----------------------------------------------------------------------------

void copy_test::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void copy_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

template<typename T>
void field_deep_copy()
{
    SIGHT_WARN("Testing : " << core::demangler(typeid(T)).get_classname());
    typename T::sptr object = std::make_shared<T>();
    typename T::sptr deep_copy_object;

    typename T::sptr field1 = std::make_shared<T>();
    typename T::sptr field2 = std::make_shared<T>();
    typename T::sptr field3 = std::make_shared<T>();

    object->set_field("F1", field1);
    object->set_field("F2", field2);
    object->set_field("F3", field3);

    deep_copy_object = data::object::copy(object);
    CPPUNIT_ASSERT_EQUAL(object->get_classname(), deep_copy_object->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_fields().size(), deep_copy_object->get_fields().size());
    CPPUNIT_ASSERT(object->get_field("F1") != deep_copy_object->get_field("F1"));
    CPPUNIT_ASSERT(object->get_field("F2") != deep_copy_object->get_field("F2"));
    CPPUNIT_ASSERT(object->get_field("F3") != deep_copy_object->get_field("F3"));

    CPPUNIT_ASSERT_EQUAL(object->get_field("F1")->get_classname(), deep_copy_object->get_field("F1")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_field("F2")->get_classname(), deep_copy_object->get_field("F2")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_field("F3")->get_classname(), deep_copy_object->get_field("F3")->get_classname());
}

//------------------------------------------------------------------------------

template<typename T>
void field_copy()
{
    SIGHT_WARN("Testing : " << core::demangler(typeid(T)).get_classname());
    typename T::sptr object              = std::make_shared<T>();
    typename T::sptr deep_copy_object    = std::make_shared<T>();
    typename T::sptr shallow_copy_object = std::make_shared<T>();

    typename T::sptr field1 = std::make_shared<T>();
    typename T::sptr field2 = std::make_shared<T>();
    typename T::sptr field3 = std::make_shared<T>();

    object->set_field("F1", field1);
    object->set_field("F2", field2);
    object->set_field("F3", field3);

    deep_copy_object = data::object::copy(object);
    CPPUNIT_ASSERT_EQUAL(object->get_classname(), deep_copy_object->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_fields().size(), deep_copy_object->get_fields().size());
    CPPUNIT_ASSERT(object->get_field("F1") != deep_copy_object->get_field("F1"));
    CPPUNIT_ASSERT(object->get_field("F2") != deep_copy_object->get_field("F2"));
    CPPUNIT_ASSERT(object->get_field("F3") != deep_copy_object->get_field("F3"));

    CPPUNIT_ASSERT_EQUAL(object->get_field("F1")->get_classname(), deep_copy_object->get_field("F1")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_field("F2")->get_classname(), deep_copy_object->get_field("F2")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_field("F3")->get_classname(), deep_copy_object->get_field("F3")->get_classname());

    shallow_copy_object->shallow_copy(object);
    CPPUNIT_ASSERT_EQUAL(object->get_classname(), shallow_copy_object->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->get_fields().size(), shallow_copy_object->get_fields().size());
    CPPUNIT_ASSERT_EQUAL(object->get_field("F1"), shallow_copy_object->get_field("F1"));
    CPPUNIT_ASSERT_EQUAL(object->get_field("F2"), shallow_copy_object->get_field("F2"));
    CPPUNIT_ASSERT_EQUAL(object->get_field("F3"), shallow_copy_object->get_field("F3"));
}

//------------------------------------------------------------------------------

void copy_test::field_copy_test()
{
    field_deep_copy<data::array>();
    field_copy<data::boolean>();
    field_copy<data::color>();
    field_copy<data::map>();
    field_copy<data::real>();
    field_copy<data::image>();
    field_copy<data::integer>();
    field_copy<data::line>();
    field_copy<data::material>();
    field_copy<data::mesh>();
    field_copy<data::plane>();
    field_copy<data::plane_list>();
    field_copy<data::point>();
    field_copy<data::point_list>();
    field_copy<data::reconstruction>();
    field_copy<data::resection>();
    field_copy<data::resection_db>();
    field_copy<data::string>();
    field_copy<data::transfer_function>();
    field_copy<data::matrix4>();
    field_copy<data::vector>();
    field_copy<data::structure_traits>();
    field_copy<data::structure_traits_dictionary>();
}

//-----------------------------------------------------------------------------

void copy_test::several_references_copy_test()
{
    const std::int64_t value    = 42;
    data::integer::sptr integer = std::make_shared<data::integer>(value);
    data::map::sptr map         = std::make_shared<data::map>();

    (*map)["A"] = integer;
    (*map)["B"] = integer;
    map->set_field("F1", integer);
    map->set_field("F2", integer);

    data::map::sptr map_copy = data::object::copy(map);

    CPPUNIT_ASSERT(integer != std::dynamic_pointer_cast<data::integer>((*map_copy)["A"]));
    CPPUNIT_ASSERT_EQUAL(value, std::dynamic_pointer_cast<data::integer>((*map_copy)["A"])->get_value());
    CPPUNIT_ASSERT_EQUAL((*map_copy)["A"], map_copy->get_field("F1"));
    CPPUNIT_ASSERT_EQUAL((*map_copy)["A"], map_copy->get_field("F2"));
    CPPUNIT_ASSERT_EQUAL((*map_copy)["A"], (*map_copy)["B"]);

    data::vector::sptr vector = std::make_shared<data::vector>();

    vector->push_back(map);
    vector->push_back(map);
    vector->set_field("F1", map);
    vector->set_field("F2", map);

    data::vector::sptr vector_copy = data::object::copy(vector);
    CPPUNIT_ASSERT(map != (*vector_copy)[0]);
    CPPUNIT_ASSERT_EQUAL((*vector_copy)[0], vector_copy->get_field("F1"));
    CPPUNIT_ASSERT_EQUAL((*vector_copy)[0], vector_copy->get_field("F2"));
    CPPUNIT_ASSERT_EQUAL((*vector_copy)[0], (*vector_copy)[1]);
}

} // namespace sight::data::ut
