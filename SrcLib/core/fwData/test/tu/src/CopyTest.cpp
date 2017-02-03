/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "CopyTest.hpp"

#include <fwData/Array.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Edge.hpp>
#include <fwData/Float.hpp>
#include <fwData/Graph.hpp>
#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Line.hpp>
#include <fwData/List.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Node.hpp>
#include <fwData/Plane.hpp>
#include <fwData/PlaneList.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Port.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/Spline.hpp>
#include <fwData/String.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/Tag.hpp>
#include <fwData/TransferFunction.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::CopyTest);

namespace fwData
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
    __FWDATA_UT_FIELD_DEEP_COPY_MACRO(::fwData::Array);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Boolean);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Color);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Composite);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Float);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Graph);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Histogram);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Image);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Integer);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Line);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::List);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::location::Folder);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::location::SingleFile);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Material);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Mesh);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Plane);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::PlaneList);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Point);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::PointList);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Reconstruction);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Resection);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ResectionDB);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::String);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::TransferFunction);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::TransformationMatrix3D);
    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Vector);

//Not implemented !?
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Edge);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Node);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Object);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Port);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ProcessObject);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ReconstructionTraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::ROITraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Spline);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::StructureTraits);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::StructureTraitsDictionary);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::Tag);
//    __FWDATA_UT_FIELD_COPY_MACRO(::fwData::location::MultiFiles);

}

//-----------------------------------------------------------------------------

void CopyTest::severalReferencesCopyTest()
{
    ::fwData::Integer::sptr integer     = ::fwData::Integer::New(42);
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    (*composite)["A"] = integer;
    (*composite)["B"] = integer;
    composite->setField("F1", integer);
    composite->setField("F2", integer);

    ::fwData::Composite::sptr compositeCopy = ::fwData::Object::copy(composite);

    CPPUNIT_ASSERT( integer != ::fwData::Integer::dynamicCast((*compositeCopy)["A"]) );
    CPPUNIT_ASSERT_EQUAL(42, ::fwData::Integer::dynamicCast((*compositeCopy)["A"])->getValue());
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    ::fwData::Vector::sptr vector = ::fwData::Vector::New();

    vector->getContainer().push_back(composite);
    vector->getContainer().push_back(composite);
    vector->setField("F1", composite);
    vector->setField("F2", composite);

    ::fwData::Vector::sptr vectorCopy = ::fwData::Object::copy(vector);
    CPPUNIT_ASSERT( composite != (*vectorCopy)[0] );
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], (*vectorCopy)[1]);

    ::fwData::List::sptr list = ::fwData::List::New();

    list->getContainer().push_back(vector);
    list->getContainer().push_back(vector);
    list->setField("F1", vector);
    list->setField("F2", vector);

    ::fwData::List::sptr listCopy = ::fwData::Object::copy(list);
    CPPUNIT_ASSERT( vector != (*listCopy).front() );
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), (*listCopy).back());

}

//-----------------------------------------------------------------------------

void CopyTest::recursiveCopyTest()
{
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    ::fwData::Vector::sptr vector       = ::fwData::Vector::New();
    ::fwData::List::sptr list           = ::fwData::List::New();

    ::fwData::Composite::sptr compositeCopy;
    ::fwData::Vector::sptr vectorCopy;
    ::fwData::List::sptr listCopy;

    (*composite)["A"] = composite;
    (*composite)["B"] = composite;

    composite->setField("F1", composite);
    composite->setField("F2", composite);

    compositeCopy = ::fwData::Object::copy(composite);

    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], compositeCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*compositeCopy)["A"], (*compositeCopy)["B"]);

    vector->getContainer().push_back(vector);
    vector->getContainer().push_back(vector);
    vector->setField("F1", vector);
    vector->setField("F2", vector);

    vectorCopy = ::fwData::Object::copy(vector);
    CPPUNIT_ASSERT( vector != (*vectorCopy)[0] );
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], vectorCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*vectorCopy)[0], (*vectorCopy)[1]);

    list->getContainer().push_back(list);
    list->getContainer().push_back(list);
    list->setField("F1", list);
    list->setField("F2", list);

    listCopy = ::fwData::Object::copy(list);
    CPPUNIT_ASSERT( list != (*listCopy).front() );
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F1"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), listCopy->getField("F2"));
    CPPUNIT_ASSERT_EQUAL((*listCopy).front(), (*listCopy).back());

    ::fwData::Object::FieldMapType zeroFields;
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

    compositeCopy = ::fwData::Object::copy(composite);
    vectorCopy    = ::fwData::Object::copy(vector);
    listCopy      = ::fwData::Object::copy(list);

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

    #define C(obj) ::fwData::Composite::dynamicCast(obj)
    #define L(obj) ::fwData::List::dynamicCast(obj)
    #define V(obj) ::fwData::Vector::dynamicCast(obj)

    //composite->list->vector->composite
    {
        ::fwData::List::sptr insideList           = L((*compositeCopy)["A"]);
        ::fwData::Vector::sptr insideVector       = V(insideList->front());
        ::fwData::Composite::sptr insideComposite = C(insideVector->front());

        CPPUNIT_ASSERT_EQUAL(compositeCopy, insideComposite );
        CPPUNIT_ASSERT_EQUAL(insideList, L((*insideComposite)["A"]) );
        CPPUNIT_ASSERT_EQUAL(compositeCopy, C(compositeCopy->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideList, L(insideList->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideVector->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideComposite, C(insideComposite->getField("F1")->getField("F1")->getField("F1")));
    }

    //list->vector->composite->list
    {
        ::fwData::Vector::sptr insideVector       = V(listCopy->front());
        ::fwData::Composite::sptr insideComposite = C(insideVector->front());
        ::fwData::List::sptr insideList           = L((*insideComposite)["A"]);

        CPPUNIT_ASSERT_EQUAL(listCopy, insideList );
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideList->front()) );
        CPPUNIT_ASSERT_EQUAL(listCopy, L(listCopy->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideList, L(insideList->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideVector, V(insideVector->getField("F1")->getField("F1")->getField("F1")));
        CPPUNIT_ASSERT_EQUAL(insideComposite, C(insideComposite->getField("F1")->getField("F1")->getField("F1")));
    }

    //vector->composite->list->vector
    {
        ::fwData::Composite::sptr insideComposite = C(vectorCopy->front());
        ::fwData::List::sptr insideList           = L((*insideComposite)["A"]);
        ::fwData::Vector::sptr insideVector       = V(insideList->front());

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

    compositeCopy = ::fwData::Object::copy(composite);

    {
        ::fwData::List::sptr insideList    = L((*compositeCopy)["A"]);
        ::fwData::Vector::sptr fieldVector = V(compositeCopy->getField("F1"));

        ::fwData::Composite::sptr insideComposite = C(fieldVector->front());
        ::fwData::List::sptr fieldList            = L(fieldVector->getField("F1"));

        ::fwData::Vector::sptr insideVector      = V(fieldList->front());
        ::fwData::Composite::sptr fieldComposite = C(fieldList->getField("F1"));

        CPPUNIT_ASSERT_EQUAL(compositeCopy, insideComposite );
        CPPUNIT_ASSERT_EQUAL(insideComposite, fieldComposite );
        CPPUNIT_ASSERT_EQUAL(fieldVector, insideVector );
        CPPUNIT_ASSERT_EQUAL(fieldList, insideList );
    }

    //get rid of circular references
    composite->getContainer().clear();
    vector->getContainer().clear();
    list->getContainer().clear();
    composite->setFields(zeroFields);
    vector->setFields(zeroFields);
    list->setFields(zeroFields);

}

} //namespace ut
} //namespace fwData
