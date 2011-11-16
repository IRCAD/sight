/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>

#include <fwTools/System.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <vtkIO/MeshWriter.hpp>
#include <vtkIO/MeshReader.hpp>
#include <vtkIO/helper/Mesh.hpp>

#include "MeshTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MeshTest );

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
    ::fwData::Mesh::NewSptr mesh1;
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

    vtkSmartPointer< vtkPolyData > mesh = vtkSmartPointer< vtkPolyData >::New();
    ::vtkIO::helper::Mesh::toVTKMesh( mesh1, mesh);
    CPPUNIT_ASSERT( mesh );

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfVerts(), mesh->GetNumberOfVerts());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfLines(), mesh->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPolys(), mesh->GetNumberOfPolys());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfStrips(), mesh->GetNumberOfStrips());

    ::fwData::Mesh::NewSptr mesh2;
    CPPUNIT_ASSERT( mesh2 );
    ::vtkIO::helper::Mesh::fromVTKMesh(mesh, mesh2);

    this->compareMesh(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testSyntheticMesh()
{
    ::fwData::Mesh::NewSptr mesh1;
    ::fwDataTools::MeshGenerator::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::MeshGenerator::shakePoint(mesh1);
    mesh1->adjustAllocatedMemory();

    vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
    ::vtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
    CPPUNIT_ASSERT( poly );

    ::fwData::Mesh::NewSptr mesh2;
    ::vtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

    this->compareMesh(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testExportImportSyntheticMesh()
{
    ::fwData::Mesh::NewSptr mesh1;
    ::fwDataTools::MeshGenerator::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::MeshGenerator::shakePoint(mesh1);
    mesh1->adjustAllocatedMemory();

    ::boost::filesystem::path testFile = ::fwTools::System::getTemporaryFolder() / "testExportImportSyntheticMesh.trian";

    ::vtkIO::MeshWriter::NewSptr writer;
    writer->setObject(mesh1);
    writer->setFile(testFile);
    writer->write();
    CPPUNIT_ASSERT(::boost::filesystem::exists(testFile));

    ::fwData::Mesh::NewSptr mesh2;
    ::vtkIO::MeshReader::NewSptr reader;
    reader->setObject(mesh2);
    reader->setFile(testFile);
    reader->read();

    this->compareMesh(mesh1, mesh2);

    bool suppr = ::boost::filesystem::remove(testFile);
    CPPUNIT_ASSERT(suppr);
}

//------------------------------------------------------------------------------

void MeshTest::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells() , mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize()  , mesh2->getCellDataSize());

    this->compareBuffer(mesh1->getPointsArray(), mesh2->getPointsArray());
    this->compareBuffer(mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    this->compareBuffer(mesh1->getCellDataOffsetsArray(), mesh2->getCellDataOffsetsArray());
    this->compareBuffer(mesh1->getCellDataArray(), mesh2->getCellDataArray());
    this->compareBuffer(mesh1->getPointColorsArray(), mesh2->getPointColorsArray());
    this->compareBuffer(mesh1->getCellColorsArray(), mesh2->getCellColorsArray());
    this->compareBuffer(mesh1->getPointNormalsArray(), mesh2->getPointNormalsArray());
    this->compareBuffer(mesh1->getCellNormalsArray(), mesh2->getCellNormalsArray());
}

//------------------------------------------------------------------------------

void MeshTest::compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2)
{
    CPPUNIT_ASSERT( (!buff1 && !buff2) || (buff1 && buff2));
    if(buff1)
    {
        CPPUNIT_ASSERT(buff1->getSize() == buff2->getSize());

        char *iter1 = buff1->begin<char>();
        char *iter2 = buff2->begin<char>();

        for (; iter1 != buff1->end<char>() ; ++iter1, ++iter2)
        {
            CPPUNIT_ASSERT_EQUAL(*iter1, *iter2);
        }
    }
}
