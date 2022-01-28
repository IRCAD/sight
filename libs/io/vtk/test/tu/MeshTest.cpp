/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/random/Generator.hpp>
#include <core/tools/System.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/vtk/helper/Mesh.hpp>
#include <io/vtk/MeshReader.hpp>
#include <io/vtk/MeshWriter.hpp>
#include <io/vtk/ObjMeshReader.hpp>
#include <io/vtk/ObjMeshWriter.hpp>
#include <io/vtk/PlyMeshReader.hpp>
#include <io/vtk/PlyMeshWriter.hpp>
#include <io/vtk/StlMeshReader.hpp>
#include <io/vtk/StlMeshWriter.hpp>
#include <io/vtk/VtpMeshReader.hpp>
#include <io/vtk/VtpMeshWriter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Mesh.hpp>

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedSphereSource.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::MeshTest);

namespace sight::io::vtk
{

namespace ut
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToVtk()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);

    const vtkSmartPointer<vtkTexturedSphereSource> source = vtkSmartPointer<vtkTexturedSphereSource>::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer<vtkPolyData> poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh1->numCells());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh1->numPoints());

    io::vtk::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);

    CPPUNIT_ASSERT(mesh1->numCells());
    CPPUNIT_ASSERT(mesh1->numPoints());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::POINT_NORMALS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::CELL_NORMALS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::POINT_COLORS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::CELL_COLORS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::POINT_TEX_COORDS>());

    const vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh1, vtkMesh);
    CPPUNIT_ASSERT(vtkMesh);

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfVerts(), vtkMesh->GetNumberOfVerts());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfLines(), vtkMesh->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPolys(), vtkMesh->GetNumberOfPolys());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfStrips(), vtkMesh->GetNumberOfStrips());

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    io::vtk::helper::Mesh::fromVTKMesh(vtkMesh, mesh2);

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToGrid()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);

    const vtkSmartPointer<vtkTexturedSphereSource> source = vtkSmartPointer<vtkTexturedSphereSource>::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer<vtkPolyData> poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh1->numCells());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh1->numPoints());

    io::vtk::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);

    CPPUNIT_ASSERT(mesh1->numCells());
    CPPUNIT_ASSERT(mesh1->numPoints());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::POINT_NORMALS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::CELL_NORMALS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::POINT_COLORS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::CELL_COLORS>());
    CPPUNIT_ASSERT(mesh1->has<data::Mesh::Attributes::POINT_TEX_COORDS>());
    const vtkSmartPointer<vtkUnstructuredGrid> vtkGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    io::vtk::helper::Mesh::toVTKGrid(mesh1, vtkGrid);
    CPPUNIT_ASSERT(vtkGrid);

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPoints(), vtkGrid->GetNumberOfPoints());

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    io::vtk::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testSyntheticMesh()
{
    {
        const data::Mesh::sptr mesh1 = data::Mesh::New();
        utestData::generator::Mesh::generateTriangleQuadMesh(mesh1);
        geometry::data::Mesh::shakePoint(mesh1);
        mesh1->shrinkToFit();

        const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        io::vtk::helper::Mesh::toVTKMesh(mesh1, poly);
        CPPUNIT_ASSERT(poly);

        data::Mesh::sptr mesh2 = data::Mesh::New();
        io::vtk::helper::Mesh::fromVTKMesh(poly, mesh2);

        CPPUNIT_ASSERT(*mesh1 == *mesh2);
    }
    {
        const data::Mesh::sptr mesh1 = data::Mesh::New();
        utestData::generator::Mesh::generateTriangleQuadMesh(mesh1);
        geometry::data::Mesh::shakePoint(mesh1);
        geometry::data::Mesh::colorizeMeshPoints(mesh1);
        geometry::data::Mesh::colorizeMeshCells(mesh1);
        geometry::data::Mesh::generatePointNormals(mesh1);
        geometry::data::Mesh::generateCellNormals(mesh1);
        mesh1->shrinkToFit();

        const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        io::vtk::helper::Mesh::toVTKMesh(mesh1, poly);
        CPPUNIT_ASSERT(poly);

        data::Mesh::sptr mesh2 = data::Mesh::New();
        io::vtk::helper::Mesh::fromVTKMesh(poly, mesh2);

        CPPUNIT_ASSERT(*mesh1 == *mesh2);
    }
}

//------------------------------------------------------------------------------

void MeshTest::testExportImportSyntheticMesh()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleQuadMesh(mesh1);
    geometry::data::Mesh::shakePoint(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);
    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);

    mesh1->shrinkToFit();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder()
                                           / "testExportImportSyntheticMesh.vtk";

    const io::vtk::MeshWriter::sptr writer = io::vtk::MeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    writer->write();
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));

    const data::Mesh::sptr mesh2           = data::Mesh::New();
    const io::vtk::MeshReader::sptr reader = io::vtk::MeshReader::New();
    reader->setObject(mesh2);
    reader->setFile(testFile);
    reader->read();

    CPPUNIT_ASSERT(*mesh1 == *mesh2);

    const bool removed = std::filesystem::remove(testFile);
    CPPUNIT_ASSERT(removed);
}

//------------------------------------------------------------------------------

void MeshTest::testPointCloud()
{
    const data::Mesh::size_t NB_POINTS = static_cast<data::Mesh::size_t>(100 + safeRand() % 1000);

    const data::Mesh::sptr mesh1 = data::Mesh::New();

    mesh1->reserve(NB_POINTS, NB_POINTS, data::Mesh::CellType::POINT);

    const auto dumpLock = mesh1->lock();

    for(data::Mesh::size_t i = 0 ; i < NB_POINTS ; ++i)
    {
        data::Mesh::position_t point[3];
        point[0] = (static_cast<float>(safeRand() % 1000) - 500.f) / 3.f;
        point[1] = (static_cast<float>(safeRand() % 1000) - 500.f) / 3.f;
        point[2] = (static_cast<float>(safeRand() % 1000) - 500.f) / 3.f;
        mesh1->pushPoint(point);
        mesh1->pushCell(i);
    }

    const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh1, poly);
    CPPUNIT_ASSERT(poly);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    io::vtk::helper::Mesh::fromVTKMesh(poly, mesh2);

    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->numPoints());
    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->numCells());
    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testReadVtkFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/vtk/sphere.vtk"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::Mesh::sptr mesh = data::Mesh::New();

    io::vtk::MeshReader::sptr vtk_reader = io::vtk::MeshReader::New();

    vtk_reader->setObject(mesh);
    vtk_reader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->numCells() == 720);
    CPPUNIT_ASSERT(mesh->numPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadVtpFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/vtp/sphere.vtp"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::Mesh::sptr mesh = data::Mesh::New();

    io::vtk::VtpMeshReader::sptr vtk_reader = io::vtk::VtpMeshReader::New();

    vtk_reader->setObject(mesh);
    vtk_reader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->numCells() == 720);
    CPPUNIT_ASSERT(mesh->numPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadObjFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/obj/sphere.obj"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::Mesh::sptr mesh = data::Mesh::New();

    io::vtk::ObjMeshReader::sptr vtk_reader = io::vtk::ObjMeshReader::New();

    vtk_reader->setObject(mesh);
    vtk_reader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->numCells() == 720);
    CPPUNIT_ASSERT(mesh->numPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadPlyFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/ply/sphere.ply"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::Mesh::sptr mesh = data::Mesh::New();

    io::vtk::PlyMeshReader::sptr vtk_reader = io::vtk::PlyMeshReader::New();

    vtk_reader->setObject(mesh);
    vtk_reader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->numCells() == 720);
    CPPUNIT_ASSERT(mesh->numPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadStlFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/stl/sphere.stl"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::Mesh::sptr mesh = data::Mesh::New();

    io::vtk::StlMeshReader::sptr vtk_reader = io::vtk::StlMeshReader::New();

    vtk_reader->setObject(mesh);
    vtk_reader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->numCells() == 720);
    CPPUNIT_ASSERT(mesh->numPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testWriteVtkFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    geometry::data::Mesh::shakePoint(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);
    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);

    mesh1->shrinkToFit();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder()
                                           / "vtkTestMesh.vtk";

    const io::vtk::MeshWriter::sptr writer = io::vtk::MeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWriteVtpFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    geometry::data::Mesh::shakePoint(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);
    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);

    mesh1->shrinkToFit();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder()
                                           / "vtpTestMesh.vtp";

    const io::vtk::VtpMeshWriter::sptr writer = io::vtk::VtpMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWriteObjFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    geometry::data::Mesh::shakePoint(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);
    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);

    mesh1->shrinkToFit();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder()
                                           / "objTestMesh.obj";

    const io::vtk::ObjMeshWriter::sptr writer = io::vtk::ObjMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWritePlyFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    geometry::data::Mesh::shakePoint(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);
    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);

    mesh1->shrinkToFit();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder()
                                           / "plyTestMesh.ply";

    const io::vtk::PlyMeshWriter::sptr writer = io::vtk::PlyMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWriteStlFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    geometry::data::Mesh::shakePoint(mesh1);
    geometry::data::Mesh::colorizeMeshPoints(mesh1);
    geometry::data::Mesh::colorizeMeshCells(mesh1);
    geometry::data::Mesh::generatePointNormals(mesh1);
    geometry::data::Mesh::generateCellNormals(mesh1);

    mesh1->shrinkToFit();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder()
                                           / "stlTestMesh.stl";

    const io::vtk::StlMeshWriter::sptr writer = io::vtk::StlMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::io::vtk
