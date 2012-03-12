/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/TransfertFunction.hpp>

#include <vtkIO/helper/TransfertFunction.hpp>

#include "TransfertFunctionTest.hpp"

using vtkIO::helper::TransfertFunction;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwVtkIO::ut::TransfertFunctionTest );

namespace fwVtkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void TransfertFunctionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TransfertFunctionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

::fwData::TransfertFunction_VERSION_II::sptr TransfertFunctionTest::createTFColor()
{
    ::fwData::TransfertFunction_VERSION_II::NewSptr tf;

    tf->setBackgroundColor( ::fwData::TransfertFunction_VERSION_II::TFColor( 1.0, 0.3, 0.6, 0.1) );
    tf->setInterpolationMode( ::fwData::TransfertFunction_VERSION_II::LINEAR );
    tf->setIsClamped( false );
    tf->setLevel( 0.0 );
    tf->setName( "TFColor" );
    tf->setWindow( 400.0 );

    tf->addTFColor( -200,   ::fwData::TransfertFunction_VERSION_II::TFColor( 1.0, 0.0, 0.0, 0.0) );
    tf->addTFColor( 0,      ::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 1.0, 0.0, 0.0) );
    tf->addTFColor( 100,     ::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 1.0, 0.5) );
    tf->addTFColor( 200,    ::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 1.0, 1.0, 1.0) );

    return tf;
}

//------------------------------------------------------------------------------

void TransfertFunctionTest::toVtkLookupTableTest()
{
    ::fwData::TransfertFunction_VERSION_II::sptr tf = this->createTFColor();
    vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();

    double color[3];
    double opacity;
    double colorTolerance = 1.0/255.0;

    ::vtkIO::helper::TransfertFunction::toVtkLookupTable( tf, lt, true, 4096 );
    lt->GetColor( 0, color);
    opacity = lt->GetOpacity(0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, opacity  , colorTolerance );

    lt->GetColor(200,color);
    opacity = lt->GetOpacity(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity  , colorTolerance );

    lt->GetColor(150,color);
    opacity = lt->GetOpacity(150);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75, opacity  , colorTolerance );

    tf->setInterpolationMode( ::fwData::TransfertFunction_VERSION_II::NEAREST );
    ::vtkIO::helper::TransfertFunction::toVtkLookupTable( tf, lt, true, 4096 );
    lt->GetColor(120,color);
    opacity = lt->GetOpacity(120);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, opacity  , colorTolerance );

//    tf->setLevel(200);
//    ::vtkIO::helper::TransfertFunction::toVtkLookupTable( tf, lt, true, 4096 );
//    lt->GetColor(200,color);
//    opacity = lt->GetOpacity(200);
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[2] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, opacity  , colorTolerance );
//
//    tf->setLevel(0);
//    tf->setWindow(800);
//    ::vtkIO::helper::TransfertFunction::toVtkLookupTable( tf, lt, true, 4096 );
//    lt->GetColor(200,color);
//    opacity = lt->GetOpacity(200);
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[1] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, opacity  , colorTolerance );

}

//------------------------------------------------------------------------------

void TransfertFunctionTest::toBWVtkLookupTableTest()
{
    vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();
    ::vtkIO::helper::TransfertFunction::toBWVtkLookupTable( 0.0, 100.0, lt, 4096 );

    double color[3];
    double opacity;
    double colorTolerance = 1.0/255.0;

    lt->GetColor(0,color);
    opacity = lt->GetOpacity(0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity  , colorTolerance );

    lt->GetColor(100,color);
    opacity = lt->GetOpacity(100);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity  , colorTolerance );

    lt->GetColor(50,color);
    opacity = lt->GetOpacity(50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[0] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[1] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[2] , colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity  , colorTolerance );
}

//------------------------------------------------------------------------------

}
}
