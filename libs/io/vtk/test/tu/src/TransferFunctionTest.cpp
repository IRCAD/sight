/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "TransferFunctionTest.hpp"

#include <data/TransferFunction.hpp>

#include <io/vtk/helper/TransferFunction.hpp>

using io::vtk::helper::TransferFunction;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::vtk::ut::TransferFunctionTest );

namespace sight::io::vtk
{
namespace ut
{

//------------------------------------------------------------------------------

void TransferFunctionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TransferFunctionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunctionTest::createTFColor()
{
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setBackgroundColor( data::TransferFunction::TFColor( 1.0, 0.3, 0.6, 0.1) );
    tf->setInterpolationMode( data::TransferFunction::LINEAR );
    tf->setIsClamped( false );
    tf->setLevel( 0.0 );
    tf->setName( "TFColor" );
    tf->setWindow( 400.0 );

    tf->addTFColor( -200,   data::TransferFunction::TFColor( 1.0, 0.0, 0.0, 0.0) );
    tf->addTFColor( 0,      data::TransferFunction::TFColor( 0.0, 1.0, 0.0, 0.0) );
    tf->addTFColor( 100,     data::TransferFunction::TFColor( 0.0, 0.0, 1.0, 0.5) );
    tf->addTFColor( 200,    data::TransferFunction::TFColor( 0.0, 1.0, 1.0, 1.0) );

    return tf;
}

//------------------------------------------------------------------------------

void TransferFunctionTest::toVtkLookupTableTest()
{
    data::TransferFunction::sptr tf    = this->createTFColor();
    vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();

    double color[3];
    double opacity;
    double colorTolerance = 1.0/255.0;

    io::vtk::helper::TransferFunction::toVtkLookupTable( tf, lt, true, 4096 );
    lt->GetColor( 0, color);
    opacity = lt->GetOpacity(0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, opacity, colorTolerance );

    lt->GetColor(200, color);
    opacity = lt->GetOpacity(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity, colorTolerance );

    lt->GetColor(150, color);
    opacity = lt->GetOpacity(150);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75, opacity, colorTolerance );

    tf->setInterpolationMode( data::TransferFunction::NEAREST );
    io::vtk::helper::TransferFunction::toVtkLookupTable( tf, lt, true, 4096 );
    lt->GetColor(120, color);
    opacity = lt->GetOpacity(120);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, opacity, colorTolerance );

//    tf->setLevel(200);
//    io::vtk::helper::TransferFunction::toVtkLookupTable( tf, lt, true, 4096 );
//    lt->GetColor(200,color);
//    opacity = lt->GetOpacity(200);
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[2] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, opacity  , colorTolerance );
//
//    tf->setLevel(0);
//    tf->setWindow(800);
//    io::vtk::helper::TransferFunction::toVtkLookupTable( tf, lt, true, 4096 );
//    lt->GetColor(200,color);
//    opacity = lt->GetOpacity(200);
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[1] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2] , colorTolerance );
//    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, opacity  , colorTolerance );

}

//------------------------------------------------------------------------------

void TransferFunctionTest::toBWVtkLookupTableTest()
{
    vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();
    io::vtk::helper::TransferFunction::toBWVtkLookupTable( 0.0, 100.0, lt, 4096 );

    double color[3];
    double opacity;
    double colorTolerance = 1.0/255.0;

    lt->GetColor(0, color);
    opacity = lt->GetOpacity(0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity, colorTolerance );

    lt->GetColor(100, color);
    opacity = lt->GetOpacity(100);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity, colorTolerance );

    lt->GetColor(50, color);
    opacity = lt->GetOpacity(50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[0], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[1], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, color[2], colorTolerance );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, opacity, colorTolerance );
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace sight::io::vtk
