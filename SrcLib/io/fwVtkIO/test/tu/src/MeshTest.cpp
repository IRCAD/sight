/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>

#include <fwTools/System.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwComEd/helper/Array.hpp>

#include <fwTest/generator/Mesh.hpp>
#include <fwDataTools/Mesh.hpp>

#include <vtkIO/MeshWriter.hpp>
#include <vtkIO/MeshReader.hpp>
#include <vtkIO/helper/Mesh.hpp>

#include "MeshTest.hpp"

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
    BOOST_FOREACH( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal" , props->size() == 0 );
}

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToVtk()
{
    ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();


    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)0);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)0);

    ::vtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );

    vtkSmartPointer< vtkPolyData > vtkMesh = vtkSmartPointer< vtkPolyData >::New();
    ::vtkIO::helper::Mesh::toVTKMesh( mesh1, vtkMesh);
    CPPUNIT_ASSERT( vtkMesh );

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfVerts(), vtkMesh->GetNumberOfVerts());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfLines(), vtkMesh->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPolys(), vtkMesh->GetNumberOfPolys());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfStrips(), vtkMesh->GetNumberOfStrips());

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT( mesh2 );
    ::vtkIO::helper::Mesh::fromVTKMesh(vtkMesh, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testSyntheticMesh()
{
    ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::Mesh::shakePoint(mesh1);
    mesh1->adjustAllocatedMemory();

    vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
    ::vtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
    CPPUNIT_ASSERT( poly );

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::vtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testExportImportSyntheticMesh()
{
    ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::Mesh::shakePoint(mesh1);
    ::fwDataTools::Mesh::colorizeMeshPoints(mesh1);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh1);
    ::fwDataTools::Mesh::generatePointNormals(mesh1);
    ::fwDataTools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    ::boost::filesystem::path testFile = ::fwTools::System::getTemporaryFolder() / "testExportImportSyntheticMesh.vtk";

    ::vtkIO::MeshWriter::sptr writer = ::vtkIO::MeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    writer->write();
    CPPUNIT_ASSERT(::boost::filesystem::exists(testFile));

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::vtkIO::MeshReader::sptr reader = ::vtkIO::MeshReader::New();
    reader->setObject(mesh2);
    reader->setFile(testFile);
    reader->read();

    compare(mesh1, mesh2);

    bool suppr = ::boost::filesystem::remove(testFile);
    CPPUNIT_ASSERT(suppr);
}

} // namespace ut
} // namespace fwVtkIO
