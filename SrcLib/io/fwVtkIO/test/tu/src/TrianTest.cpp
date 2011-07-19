/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>

#include <fwData/TriangularMesh.hpp>

#include <vtkIO/vtk.hpp>

#include "TrianTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TrianTest );

//------------------------------------------------------------------------------

void TrianTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TrianTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TrianTest::testTrianToVtk()
{
    ::fwData::TriangularMesh::NewSptr trian1;
    CPPUNIT_ASSERT( trian1 );

    vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT( trian1->cells().empty() );
    CPPUNIT_ASSERT( trian1->points().empty() );

    ::vtkIO::fromVTKMesh(poly_source, trian1);

    CPPUNIT_ASSERT( !trian1->cells().empty() );
    CPPUNIT_ASSERT( !trian1->points().empty() );

    vtkPolyData *mesh = ::vtkIO::toVTKMesh( trian1 );
    CPPUNIT_ASSERT( mesh );

    CPPUNIT_ASSERT(poly_source->GetNumberOfVerts()  == mesh->GetNumberOfVerts());
    CPPUNIT_ASSERT(poly_source->GetNumberOfLines()  == mesh->GetNumberOfLines());
    CPPUNIT_ASSERT(poly_source->GetNumberOfPolys()  == mesh->GetNumberOfPolys());
    CPPUNIT_ASSERT(poly_source->GetNumberOfStrips() == mesh->GetNumberOfStrips());


    ::fwData::TriangularMesh::NewSptr trian2;
    CPPUNIT_ASSERT( trian2 );
    ::vtkIO::fromVTKMesh(mesh, trian2);

    CPPUNIT_ASSERT(trian1->getNumCells()  == trian2->getNumCells());
    CPPUNIT_ASSERT(trian1->getNumPoints() == trian2->getNumPoints());
    CPPUNIT_ASSERT(trian1->points() == trian2->points());
    CPPUNIT_ASSERT(trian1->cells()  == trian2->cells());

    mesh->Delete();
}

//------------------------------------------------------------------------------
