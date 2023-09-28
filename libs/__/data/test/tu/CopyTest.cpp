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

#include "CopyTest.hpp"

#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Line.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>
#include <data/Plane.hpp>
#include <data/PlaneList.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/String.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::CopyTest);

namespace sight::data::ut
{

//-----------------------------------------------------------------------------

void CopyTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void CopyTest::tearDown()
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

    deep_copyObject = data::Object::copy(object);
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

    deep_copyObject = data::Object::copy(object);
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

void CopyTest::fieldCopyTest()
{
    fieldDeepCopy<data::Array>();
    fieldCopy<data::Boolean>();
    fieldCopy<data::Color>();
    fieldCopy<data::Composite>();
    fieldCopy<data::Float>();
    fieldCopy<data::Image>();
    fieldCopy<data::Integer>();
    fieldCopy<data::Line>();
    fieldCopy<data::Material>();
    fieldCopy<data::Mesh>();
    fieldCopy<data::Plane>();
    fieldCopy<data::PlaneList>();
    fieldCopy<data::Point>();
    fieldCopy<data::PointList>();
    fieldCopy<data::Reconstruction>();
    fieldCopy<data::Resection>();
    fieldCopy<data::ResectionDB>();
    fieldCopy<data::String>();
    fieldCopy<data::TransferFunction>();
    fieldCopy<data::Matrix4>();
    fieldCopy<data::Vector>();
    fieldCopy<data::StructureTraits>();
    fieldCopy<data::StructureTraitsDictionary>();
}

//-----------------------------------------------------------------------------

void CopyTest::severalReferencesCopyTest()
{
    const std::int64_t value        = 42;
    data::Integer::sptr integer     = std::make_shared<data::Integer>(value);
    data::Composite::sptr composite = std::make_shared<data::Composite>();

    (*composite)["A"] = integer;
    (*composite)["B"] = integer;
    composite->setField("F1", integer);
    composite->setField("F2", integer);

    data::Composite::sptr compositeCopy = data::Object::copy(composite);

    CPPUNIT_ASSERT(integer != std::dynamic_pointer_cast<data::Integer>((*compositeCopy)["A"]));
    CPPUNIT_ASSERT_EQUAL(value, std::dynamic_pointer_cast<data::Integer>((*compositeCopy)["A"])->getValue());
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    data::Vector::sptr vector = std::make_shared<data::Vector>();

    vector->push_back(composite);
    vector->push_back(composite);
    vector->setField("F1", composite);
    vector->setField("F2", composite);

    data::Vector::sptr vectorCopy = data::Object::copy(vector);
    CPPUNIT_ASSERT(composite != (*vectorCopy)[0]);
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], (*vectorCopy)[1]);
}

} // namespace sight::data::ut
