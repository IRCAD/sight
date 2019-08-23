/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "MeshTest.hpp"

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/MeshReader.hpp>
#include <fwVtkIO/MeshWriter.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/Mesh.hpp>

#include <fwTest/generator/Mesh.hpp>

#include <fwTools/NumericRoundCast.hxx>
#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedSphereSource.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwVtkIO::ut::MeshTest );

namespace fwVtkIO
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.

    std::srand(::fwTools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToVtk()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkTexturedSphereSource > source = vtkSmartPointer< vtkTexturedSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    ::fwDataTools::Mesh::generatePointNormals(mesh1);
    ::fwDataTools::Mesh::generateCellNormals(mesh1);
    ::fwDataTools::Mesh::colorizeMeshPoints(mesh1);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );
    CPPUNIT_ASSERT( mesh1->getPointNormalsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getCellNormalsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getPointColorsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getCellColorsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getPointTexCoordsArray() );
    CPPUNIT_ASSERT( mesh1->getPointTexCoordsArray()->getSize()[0] );

    const vtkSmartPointer< vtkPolyData > vtkMesh = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, vtkMesh);
    CPPUNIT_ASSERT( vtkMesh );

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfVerts(), vtkMesh->GetNumberOfVerts());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfLines(), vtkMesh->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPolys(), vtkMesh->GetNumberOfPolys());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfStrips(), vtkMesh->GetNumberOfStrips());

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh2 );
    ::fwVtkIO::helper::Mesh::fromVTKMesh(vtkMesh, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToGrid()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkTexturedSphereSource > source = vtkSmartPointer< vtkTexturedSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    ::fwDataTools::Mesh::generatePointNormals(mesh1);
    ::fwDataTools::Mesh::generateCellNormals(mesh1);
    ::fwDataTools::Mesh::colorizeMeshPoints(mesh1);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );
    CPPUNIT_ASSERT( mesh1->getPointNormalsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getCellNormalsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getPointColorsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getCellColorsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getPointTexCoordsArray() );
    CPPUNIT_ASSERT( mesh1->getPointTexCoordsArray()->getSize()[0] );

    const vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPoints(), vtkGrid->GetNumberOfPoints());

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh2 );
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testSyntheticMesh()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::Mesh::shakePoint(mesh1);
    mesh1->adjustAllocatedMemory();

    const vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
    CPPUNIT_ASSERT( poly );

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testExportImportSyntheticMesh()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::Mesh::shakePoint(mesh1);
    ::fwDataTools::Mesh::colorizeMeshPoints(mesh1);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh1);
    ::fwDataTools::Mesh::generatePointNormals(mesh1);
    ::fwDataTools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const ::boost::filesystem::path testFile = ::fwTools::System::getTemporaryFolder() /
                                               "testExportImportSyntheticMesh.vtk";

    const ::fwVtkIO::MeshWriter::sptr writer = ::fwVtkIO::MeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    writer->write();
    CPPUNIT_ASSERT(::boost::filesystem::exists(testFile));

    const ::fwData::Mesh::sptr mesh2         = ::fwData::Mesh::New();
    const ::fwVtkIO::MeshReader::sptr reader = ::fwVtkIO::MeshReader::New();
    reader->setObject(mesh2);
    reader->setFile(testFile);
    reader->read();

    compare(mesh1, mesh2);

    const bool suppr = ::boost::filesystem::remove(testFile);
    CPPUNIT_ASSERT(suppr);
}

//------------------------------------------------------------------------------

void MeshTest::testPointCloud()
{
    const std::uint64_t NB_POINTS = (100 + rand()%1000);

    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();

    mesh1->allocate(NB_POINTS, NB_POINTS, NB_POINTS);

    ::fwDataTools::helper::Mesh helper(mesh1);

    for (std::uint64_t i = 0; i < NB_POINTS; ++i)
    {
        ::fwData::Mesh::PointValueType point[3];
        point[0] = (rand()%1000 - 500.f) / 3.f;
        point[1] = (rand()%1000 - 500.f) / 3.f;
        point[2] = (rand()%1000 - 500.f) / 3.f;
        helper.insertNextPoint(point);
        helper.insertNextCell(i);
    }
    mesh1->adjustAllocatedMemory();

    const vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
    CPPUNIT_ASSERT( poly );

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->getCellDataSize());
    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshUpdatePoints()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    // modify mesh vertex position
    ::fwDataTools::Mesh::shakePoint(mesh1);

    ::fwVtkIO::helper::Mesh::updatePolyDataPoints(poly_source, mesh1);

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshUpdateColors()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    ::fwDataTools::Mesh::colorizeMeshPoints(mesh1);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh1);

    ::fwVtkIO::helper::Mesh::updatePolyDataPointColor(poly_source, mesh1);
    ::fwVtkIO::helper::Mesh::updatePolyDataCellColor(poly_source, mesh1);

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshUpdateNormals()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    ::fwDataTools::Mesh::generatePointNormals(mesh1);
    ::fwDataTools::Mesh::generateCellNormals(mesh1);
    ::fwDataTools::Mesh::shakeNormals(mesh1->getPointNormalsArray());
    ::fwDataTools::Mesh::shakeNormals(mesh1->getCellNormalsArray());

    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(poly_source, mesh1);
    ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(poly_source, mesh1);

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testGridUpdatePoints()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    // modify mesh vertex position
    ::fwDataTools::Mesh::shakePoint(mesh1);
    ::fwVtkIO::helper::Mesh::updateGridPoints(vtkGrid, mesh1);

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testGridUpdateColors()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    ::fwDataTools::Mesh::colorizeMeshPoints(mesh1);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh1);

    ::fwVtkIO::helper::Mesh::updateGridPointColor(vtkGrid, mesh1);
    ::fwVtkIO::helper::Mesh::updateGridCellColor(vtkGrid, mesh1);

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testGridUpdateNormals()
{
    const ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    ::fwDataTools::Mesh::generatePointNormals(mesh1);
    ::fwDataTools::Mesh::generateCellNormals(mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );
    CPPUNIT_ASSERT( mesh1->getPointNormalsArray()->getSize()[0] );
    CPPUNIT_ASSERT( mesh1->getCellNormalsArray()->getSize()[0] );

    vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    ::fwVtkIO::helper::Mesh::updateGridPoints(vtkGrid, mesh1);
    ::fwVtkIO::helper::Mesh::updateGridPointNormals(vtkGrid, mesh1);
    ::fwVtkIO::helper::Mesh::updateGridCellNormals(vtkGrid, mesh1);

    const ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwVtkIO
