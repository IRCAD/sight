/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include <data/Histogram.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Line.hpp>
#include <data/List.hpp>
#include <data/location/Folder.hpp>
#include <data/location/MultiFiles.hpp>
#include <data/location/SingleFile.hpp>
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
CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::ut::CopyTest);

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
    __FWDATA_UT_FIELD_COPY_MACRO(data::Histogram);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Image);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Integer);
    __FWDATA_UT_FIELD_COPY_MACRO(data::Line);
    __FWDATA_UT_FIELD_COPY_MACRO(data::List);
    __FWDATA_UT_FIELD_COPY_MACRO(data::location::Folder);
    __FWDATA_UT_FIELD_COPY_MACRO(data::location::SingleFile);
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

//Not implemented !?
//    __FWDATA_UT_FIELD_COPY_MACRO(data::Edge);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::Node);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::Object);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::Port);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::ProcessObject);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::ReconstructionTraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::ROITraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::StructureTraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::StructureTraitsDictionary);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::Tag);
//    __FWDATA_UT_FIELD_COPY_MACRO(data::location::MultiFiles);

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

    CPPUNIT_ASSERT( integer != data::Integer::dynamicCast((*compositeCopy)["A"]) );
    CPPUNIT_ASSERT_EQUAL(value, data::Integer::dynamicCast((*compositeCopy)["A"])->getValue());
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    data::Vector::sptr vector = data::Vector::New();

    vector->getContainer().push_back(composite);
    vector->getContainer().push_back(composite);
    vector->setField("F1", composite);
    vector->setField("F2", composite);

    data::Vector::sptr vectorCopy = data::Object::copy(vector);
    CPPUNIT_ASSERT( composite != (*vectorCopy)[0] );
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], (*vectorCopy)[1]);

    data::List::sptr list = data::List::New();

    list->getContainer().push_back(vector);
    list->getContainer().push_back(vector);
    list->setField("F1", vector);
    list->setField("F2", vector);

    data::List::sptr listCopy = data::Object::copy(list);
    CPPUNIT_ASSERT( vector != (*listCopy).front() );
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), (*listCopy).back());

}

//-----------------------------------------------------------------------------

void CopyTest::recursiveCopyTest()
{
    data::Composite::sptr composite = data::Composite::New();
    data::Vector::sptr vector       = data::Vector::New();
    data::List::sptr list           = data::List::New();

    data::Composite::sptr compositeCopy;
    data::Vector::sptr vectorCopy;
    data::List::sptr listCopy;

    (*composite)["A"] = composite;
    (*composite)["B"] = composite;

    composite->setField("F1", composite);
    composite->setField("F2", composite);

    compositeCopy = data::Object::copy(composite);

    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    vector->getContainer().push_back(vector);
    vector->getContainer().push_back(vector);
    vector->setField("F1", vector);
    vector->setField("F2", vector);

    vectorCopy = data::Object::copy(vector);
    CPPUNIT_ASSERT( vector != (*vectorCopy)[0] );
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], (*vectorCopy)[1]);

    list->getContainer().push_back(list);
    list->getContainer().push_back(list);
    list->setField("F1", list);
    list->setField("F2", list);

    listCopy = data::Object::copy(list);
    CPPUNIT_ASSERT( list != (*listCopy).front() );
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), (*listCopy).back());

    data::Object::FieldMapType zeroFields;
    composite->getContainer().clear();
    vector->getContainer().clear();
    list->getContainer().clear();
    composite->setFields(zeroFields);
    vector->setFields(zeroFields);
    list->setFields(zeroFields);

    (*composite)["A"] = list;
    composite->setField("F1", list);

    vector->getContainer().push_back(composite);
    vector->setField("F1", composite);

    list->getContainer().push_back(vector);
    list->setField("F1", vector);

    // Break cyclic references to avoid memory leaks
    compositeCopy->getContainer().clear();
    compositeCopy->setFields(zeroFields);
    vectorCopy->getContainer().clear();
    vectorCopy->setFields(zeroFields);
    listCopy->getContainer().clear();
    listCopy->setFields(zeroFields);
    compositeCopy = data::Object::copy(composite);
    vectorCopy    = data::Object::copy(vector);
    listCopy      = data::Object::copy(list);

    CPPUNIT_ASSERT( list != compositeCopy->getField("F1") );
    CPPUNIT_ASSERT( list != (*compositeCopy)["A"] );
    CPPUNIT_ASSERT( vector != listCopy->getField("F1") );
    CPPUNIT_ASSERT( vector != (*listCopy).front() );
    CPPUNIT_ASSERT( composite != vectorCopy->getField("F1") );
    CPPUNIT_ASSERT( composite != (*vectorCopy)[0] );

    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));

    //ensures copy cache is not persistant
    CPPUNIT_ASSERT( listCopy != compositeCopy->getField("F1") );
    CPPUNIT_ASSERT( listCopy != (*composite)["A"] );
    CPPUNIT_ASSERT( vectorCopy != listCopy->getField("F1") );
    CPPUNIT_ASSERT( vectorCopy != (*listCopy).front() );
    CPPUNIT_ASSERT( compositeCopy != vectorCopy->getField("F1") );
    CPPUNIT_ASSERT( compositeCopy != (*vectorCopy)[0] );

    #define C(obj) data::Composite::dynamicCast(obj)
    #define L(obj) data::List::dynamicCast(obj)
    #define V(obj) data::Vector::dynamicCast(obj)

    //composite->list->vector->composite
    {
        data::List::sptr insideList           = L((*compositeCopy)["A"]);
        data::Vector::sptr insideVector       = V(insideList->front());
        data::Composite::sptr insideComposite = C(insideVector->front());

        CPPUNIT_ASSERT_EQUAL(compositeCopy, insideComposite );
        CPPUNIT_ASSERT_EQUAL(insideList, L((*insideComposite)["A"]) );
        CPPUNIT_ASSERT_EQUAL(compositeCopy, C(compositeCopy->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideList, L(insideList->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideVector->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideComposite, C(insideComposite->getField("F1")->getField("F1")->getField("F1")));
    }

    //list->vector->composite->list
    {
        data::Vector::sptr insideVector       = V(listCopy->front());
        data::Composite::sptr insideComposite = C(insideVector->front());
        data::List::sptr insideList           = L((*insideComposite)["A"]);

        CPPUNIT_ASSERT_EQUAL(listCopy, insideList );
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideList->front()) );
        CPPUNIT_ASSERT_EQUAL(listCopy, L(listCopy->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideList, L(insideList->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideVector->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideComposite, C(insideComposite->getField("F1")->getField("F1")->getField("F1")));
    }

    //vector->composite->list->vector
    {
        data::Composite::sptr insideComposite = C(vectorCopy->front());
        data::List::sptr insideList           = L((*insideComposite)["A"]);
        data::Vector::sptr insideVector       = V(insideList->front());

        CPPUNIT_ASSERT_EQUAL(vectorCopy, insideVector );
        CPPUNIT_ASSERT_EQUAL(insideComposite, C(insideVector->front()) );
        CPPUNIT_ASSERT_EQUAL(vectorCopy, V(vectorCopy->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideList, L(insideList->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideVector->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideComposite, C(insideComposite->getField("F1")->getField("F1")->getField("F1")));
    }

    composite->getContainer().clear();
    vector->getContainer().clear();
    list->getContainer().clear();
    composite->setFields(zeroFields);
    vector->setFields(zeroFields);
    list->setFields(zeroFields);

    (*composite)["A"] = list;
    composite->setField("F1", vector);

    vector->getContainer().push_back(composite);
    vector->setField("F1", list);

    list->getContainer().push_back(vector);
    list->setField("F1", composite);

    compositeCopy->getContainer().clear();
    compositeCopy->setFields(zeroFields);
    compositeCopy = data::Object::copy(composite);

    {
        data::List::sptr insideList    = L((*compositeCopy)["A"]);
        data::Vector::sptr fieldVector = V(compositeCopy->getField("F1"));

        data::Composite::sptr insideComposite = C(fieldVector->front());
        data::List::sptr fieldList            = L(fieldVector->getField("F1"));

        data::Vector::sptr insideVector      = V(fieldList->front());
        data::Composite::sptr fieldComposite = C(fieldList->getField("F1"));

        CPPUNIT_ASSERT_EQUAL(compositeCopy, insideComposite );
        CPPUNIT_ASSERT_EQUAL(insideComposite, fieldComposite );
        CPPUNIT_ASSERT_EQUAL(fieldVector, insideVector );
        CPPUNIT_ASSERT_EQUAL(fieldList, insideList );
    }

    //get rid of circular references
    data::List::sptr insideList = L((*compositeCopy)["A"]);
    insideList->getContainer().clear();
    insideList->setFields(zeroFields);
    compositeCopy->getContainer().clear();
    compositeCopy->setFields(zeroFields);
    vectorCopy->getContainer().clear();
    vectorCopy->setFields(zeroFields);
    listCopy->getContainer().clear();
    listCopy->setFields(zeroFields);

    composite->getContainer().clear();
    vector->getContainer().clear();
    list->getContainer().clear();
    composite->setFields(zeroFields);
    vector->setFields(zeroFields);
    list->setFields(zeroFields);
}

} //namespace ut
} //namespace sight::data
