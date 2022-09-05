/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <data/Edge.hpp>
#include <data/Float.hpp>
#include <data/Graph.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Line.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>
#include <data/Node.hpp>
#include <data/Plane.hpp>
#include <data/PlaneList.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Port.hpp>
#include <data/ProcessObject.hpp>
#include <data/Reconstruction.hpp>
#include <data/ReconstructionTraits.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/ROITraits.hpp>
#include <data/String.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/Tag.hpp>
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
    SIGHT_WARN("Testing : " << core::Demangler(typeid(T)).getClassname());
    typename T::sptr object = T::New();
    typename T::sptr deepCopyObject;

    typename T::sptr field1 = T::New();
    typename T::sptr field2 = T::New();
    typename T::sptr field3 = T::New();

    object->setField("F1", field1);
    object->setField("F2", field2);
    object->setField("F3", field3);

    deepCopyObject = data::Object::copy(object);
    CPPUNIT_ASSERT_EQUAL(object->getClassname(), deepCopyObject->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getFields().size(), deepCopyObject->getFields().size());
    CPPUNIT_ASSERT(object->getField("F1") != deepCopyObject->getField("F1"));
    CPPUNIT_ASSERT(object->getField("F2") != deepCopyObject->getField("F2"));
    CPPUNIT_ASSERT(object->getField("F3") != deepCopyObject->getField("F3"));

    CPPUNIT_ASSERT_EQUAL(object->getField("F1")->getClassname(), deepCopyObject->getField("F1")->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F2")->getClassname(), deepCopyObject->getField("F2")->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F3")->getClassname(), deepCopyObject->getField("F3")->getClassname());
}

//------------------------------------------------------------------------------

template<typename T>
void fieldCopy()
{
    SIGHT_WARN("Testing : " << core::Demangler(typeid(T)).getClassname());
    typename T::sptr object            = T::New();
    typename T::sptr deepCopyObject    = T::New();
    typename T::sptr shallowCopyObject = T::New();

    typename T::sptr field1 = T::New();
    typename T::sptr field2 = T::New();
    typename T::sptr field3 = T::New();

    object->setField("F1", field1);
    object->setField("F2", field2);
    object->setField("F3", field3);

    deepCopyObject = data::Object::copy(object);
    CPPUNIT_ASSERT_EQUAL(object->getClassname(), deepCopyObject->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getFields().size(), deepCopyObject->getFields().size());
    CPPUNIT_ASSERT(object->getField("F1") != deepCopyObject->getField("F1"));
    CPPUNIT_ASSERT(object->getField("F2") != deepCopyObject->getField("F2"));
    CPPUNIT_ASSERT(object->getField("F3") != deepCopyObject->getField("F3"));

    CPPUNIT_ASSERT_EQUAL(object->getField("F1")->getClassname(), deepCopyObject->getField("F1")->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F2")->getClassname(), deepCopyObject->getField("F2")->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getField("F3")->getClassname(), deepCopyObject->getField("F3")->getClassname());

    shallowCopyObject->shallowCopy(object);
    CPPUNIT_ASSERT_EQUAL(object->getClassname(), shallowCopyObject->getClassname());
    CPPUNIT_ASSERT_EQUAL(object->getFields().size(), shallowCopyObject->getFields().size());
    CPPUNIT_ASSERT_EQUAL(object->getField("F1"), shallowCopyObject->getField("F1"));
    CPPUNIT_ASSERT_EQUAL(object->getField("F2"), shallowCopyObject->getField("F2"));
    CPPUNIT_ASSERT_EQUAL(object->getField("F3"), shallowCopyObject->getField("F3"));
}

//------------------------------------------------------------------------------

void CopyTest::fieldCopyTest()
{
    fieldDeepCopy<data::Array>();
    fieldCopy<data::Boolean>();
    fieldCopy<data::Color>();
    fieldCopy<data::Composite>();
    fieldCopy<data::Float>();
    fieldCopy<data::Graph>();
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

    fieldCopy<data::Edge>();
    fieldCopy<data::Node>();
    fieldCopy<data::Port>();
    fieldCopy<data::ProcessObject>();
    fieldCopy<data::ReconstructionTraits>();
    fieldCopy<data::ROITraits>();
    fieldCopy<data::StructureTraits>();
    fieldCopy<data::StructureTraitsDictionary>();
    fieldCopy<data::Tag>();
}

//-----------------------------------------------------------------------------

void CopyTest::severalReferencesCopyTest()
{
    const std::int64_t value        = 42;
    data::Integer::sptr integer     = data::Integer::New(value);
    data::Composite::sptr composite = data::Composite::New();

    (*composite)["A"] = integer;
    (*composite)["B"] = integer;
    composite->setField("F1", integer);
    composite->setField("F2", integer);

    data::Composite::sptr compositeCopy = data::Object::copy(composite);

    CPPUNIT_ASSERT(integer != data::Integer::dynamicCast((*compositeCopy)["A"]));
    CPPUNIT_ASSERT_EQUAL(value, data::Integer::dynamicCast((*compositeCopy)["A"])->getValue());
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    data::Vector::sptr vector = data::Vector::New();

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
