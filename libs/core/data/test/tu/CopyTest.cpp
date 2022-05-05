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

namespace sight::data
{

namespace ut
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

void CopyTest::fieldCopyTest()
{
    __FWDATA_UT_FIELD_DEEP_COPY_MACRO(data::Array);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Boolean);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Color);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Composite);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Float);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Graph);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Image);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Integer);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Line);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Material);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Mesh);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Plane);
    __FWDATA_UT_FIELD_COPY_MACRO(data::PlaneList);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Point);
    __FWDATA_UT_FIELD_COPY_MACRO(data::PointList);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Reconstruction);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Resection);
    __FWDATA_UT_FIELD_COPY_MACRO(data::ResectionDB);
    __FWDATA_UT_FIELD_COPY_MACRO(data::String);
    __FWDATA_UT_FIELD_COPY_MACRO(data::TransferFunction);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Matrix4);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Vector);

    __FWDATA_UT_FIELD_COPY_MACRO(data::Edge);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Node);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Port);
    __FWDATA_UT_FIELD_COPY_MACRO(data::ProcessObject);
    __FWDATA_UT_FIELD_COPY_MACRO(data::ReconstructionTraits);
    __FWDATA_UT_FIELD_COPY_MACRO(data::ROITraits);
    __FWDATA_UT_FIELD_COPY_MACRO(data::StructureTraits);
    __FWDATA_UT_FIELD_COPY_MACRO(data::StructureTraitsDictionary);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Tag);
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

} //namespace ut

} //namespace sight::data
