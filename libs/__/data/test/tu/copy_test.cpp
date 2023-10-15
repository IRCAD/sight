/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/line.hpp>
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
void fieldDeepCopy()
{
    SIGHT_WARN("Testing : " << core::demangler(typeid(T)).get_classname());
    typename T::sptr object = std::make_shared<T>();
    typename T::sptr deep_copyObject;

    typename T::sptr field1 = std::make_shared<T>();
    typename T::sptr field2 = std::make_shared<T>();
    typename T::sptr field3 = std::make_shared<T>();

    object->setField("F1", field1);
    object->setField("F2", field2);
    object->setField("F3", field3);

    deep_copyObject = data::object::copy(object);
    CPPUNIT_ASSERT_EQUAL(object->get_classname(), deep_copyObject->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getFields().size(), deep_copyObject->getFields().size());
    CPPUNIT_ASSERT(object->getField("F1") != deep_copyObject->getField("F1"));
    CPPUNIT_ASSERT(object->getField("F2") != deep_copyObject->getField("F2"));
    CPPUNIT_ASSERT(object->getField("F3") != deep_copyObject->getField("F3"));

    CPPUNIT_ASSERT_EQUAL(object->getField("F1")->get_classname(), deep_copyObject->getField("F1")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F2")->get_classname(), deep_copyObject->getField("F2")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F3")->get_classname(), deep_copyObject->getField("F3")->get_classname());
}

//------------------------------------------------------------------------------

template<typename T>
void fieldCopy()
{
    SIGHT_WARN("Testing : " << core::demangler(typeid(T)).get_classname());
    typename T::sptr object             = std::make_shared<T>();
    typename T::sptr deep_copyObject    = std::make_shared<T>();
    typename T::sptr shallow_copyObject = std::make_shared<T>();

    typename T::sptr field1 = std::make_shared<T>();
    typename T::sptr field2 = std::make_shared<T>();
    typename T::sptr field3 = std::make_shared<T>();

    object->setField("F1", field1);
    object->setField("F2", field2);
    object->setField("F3", field3);

    deep_copyObject = data::object::copy(object);
    CPPUNIT_ASSERT_EQUAL(object->get_classname(), deep_copyObject->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getFields().size(), deep_copyObject->getFields().size());
    CPPUNIT_ASSERT(object->getField("F1") != deep_copyObject->getField("F1"));
    CPPUNIT_ASSERT(object->getField("F2") != deep_copyObject->getField("F2"));
    CPPUNIT_ASSERT(object->getField("F3") != deep_copyObject->getField("F3"));

    CPPUNIT_ASSERT_EQUAL(object->getField("F1")->get_classname(), deep_copyObject->getField("F1")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F2")->get_classname(), deep_copyObject->getField("F2")->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F3")->get_classname(), deep_copyObject->getField("F3")->get_classname());

    shallow_copyObject->shallow_copy(object);
    CPPUNIT_ASSERT_EQUAL(object->get_classname(), shallow_copyObject->get_classname());
    CPPUNIT_ASSERT_EQUAL(object->getFields().size(), shallow_copyObject->getFields().size());
    CPPUNIT_ASSERT_EQUAL(object->getField("F1"), shallow_copyObject->getField("F1"));
    CPPUNIT_ASSERT_EQUAL(object->getField("F2"), shallow_copyObject->getField("F2"));
    CPPUNIT_ASSERT_EQUAL(object->getField("F3"), shallow_copyObject->getField("F3"));
}

//------------------------------------------------------------------------------

void copy_test::fieldCopyTest()
{
    fieldDeepCopy<data::array>();
    fieldCopy<data::boolean>();
    fieldCopy<data::color>();
    fieldCopy<data::composite>();
    fieldCopy<data::real>();
    fieldCopy<data::image>();
    fieldCopy<data::integer>();
    fieldCopy<data::line>();
    fieldCopy<data::material>();
    fieldCopy<data::mesh>();
    fieldCopy<data::plane>();
    fieldCopy<data::plane_list>();
    fieldCopy<data::point>();
    fieldCopy<data::point_list>();
    fieldCopy<data::reconstruction>();
    fieldCopy<data::resection>();
    fieldCopy<data::resection_db>();
    fieldCopy<data::string>();
    fieldCopy<data::transfer_function>();
    fieldCopy<data::matrix4>();
    fieldCopy<data::vector>();
    fieldCopy<data::structure_traits>();
    fieldCopy<data::structure_traits_dictionary>();
}

//-----------------------------------------------------------------------------

void copy_test::severalReferencesCopyTest()
{
    const std::int64_t value        = 42;
    data::integer::sptr integer     = std::make_shared<data::integer>(value);
    data::composite::sptr composite = std::make_shared<data::composite>();

    (*composite)["A"] = integer;
    (*composite)["B"] = integer;
    composite->setField("F1", integer);
    composite->setField("F2", integer);

    data::composite::sptr compositeCopy = data::object::copy(composite);

    CPPUNIT_ASSERT(integer != std::dynamic_pointer_cast<data::integer>((*compositeCopy)["A"]));
    CPPUNIT_ASSERT_EQUAL(value, std::dynamic_pointer_cast<data::integer>((*compositeCopy)["A"])->getValue());
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    data::vector::sptr vector = std::make_shared<data::vector>();

    vector->push_back(composite);
    vector->push_back(composite);
    vector->setField("F1", composite);
    vector->setField("F2", composite);

    data::vector::sptr vectorCopy = data::object::copy(vector);
    CPPUNIT_ASSERT(composite != (*vectorCopy)[0]);
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], (*vectorCopy)[1]);
}

} // namespace sight::data::ut
