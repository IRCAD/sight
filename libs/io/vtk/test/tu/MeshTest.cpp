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

#include <core/tools/random/Generator.hpp>
#include <core/tools/System.hpp>

#include <data/iterator.hpp>

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

#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedSphereSource.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::MeshTest);

namespace sight::io::vtk::ut
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
    const auto NB_POINTS = static_cast<data::Mesh::size_t>(100 + safeRand() % 1000);

    const data::Mesh::sptr mesh1 = data::Mesh::New();

    mesh1->reserve(NB_POINTS, NB_POINTS, data::Mesh::CellType::POINT);

    const auto dumpLock = mesh1->dump_lock();

    for(data::Mesh::size_t i = 0 ; i < NB_POINTS ; ++i)
    {
        std::array<data::Mesh::position_t, 3> point {};
        point[0] = (static_cast<float>(safeRand() % 1000) - 500.F) / 3.F;
        point[1] = (static_cast<float>(safeRand() % 1000) - 500.F) / 3.F;
        point[2] = (static_cast<float>(safeRand() % 1000) - 500.F) / 3.F;
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

void MeshTest::toVtkMeshWithLinesTest()
{
    auto mesh     = data::Mesh::New();
    auto meshLock = mesh->dump_lock();
    mesh->reserve(4, 3, data::Mesh::CellType::LINE);
    mesh->pushPoint(0, 1, 2);
    mesh->pushPoint(3, 4, 5);
    mesh->pushPoint(6, 7, 8);
    mesh->pushPoint(9, 10, 11);
    mesh->pushCell(0U, 1U);
    mesh->pushCell(1U, 2U);
    mesh->pushCell(2U, 3U);

    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh, vtkMesh);

    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "component " + std::to_string(j) + " of point " + std::to_string(i),
                i * 3. + j,
                vtkMesh->GetPoints()->GetPoint(i)[j]
            );
        }
    }

    auto* pointIdList = vtkIdList::New();
    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        vtkMesh->GetCellPoints(i, pointIdList);
        for(std::uint8_t j = 0 ; j < 2 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "point " + std::to_string(j) + " of cell " + std::to_string(i),
                vtkIdType(i) + j,
                pointIdList->GetId(j)
            );
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::toVtkMeshWithTetrasTest()
{
    auto mesh     = data::Mesh::New();
    auto meshLock = mesh->dump_lock();
    mesh->reserve(5, 3, data::Mesh::CellType::TETRA);
    mesh->pushPoint(0, 1, 2);
    mesh->pushPoint(3, 4, 5);
    mesh->pushPoint(6, 7, 8);
    mesh->pushPoint(9, 10, 11);
    mesh->pushPoint(12, 13, 14);
    mesh->pushCell(0U, 1U, 2U, 3U);
    mesh->pushCell(1U, 2U, 3U, 4U);
    mesh->pushCell(2U, 3U, 4U, 0U);

    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh, vtkMesh);

    for(std::uint8_t i = 0 ; i < 5 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "component " + std::to_string(j) + " of point " + std::to_string(i),
                i * 3. + j,
                vtkMesh->GetPoints()->GetPoint(i)[j]
            );
        }
    }

    auto* pointIdList = vtkIdList::New();
    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        vtkMesh->GetCellPoints(i, pointIdList);
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            // TODO: fix crash. With CellType = TETRA, the pointIdList is always empty.
            // CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(j) + " of cell " + std::to_string(i),
            // (vtkIdType(i) + j)%4, pointIdList->GetId(j));
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::toVtkMeshWithCellTexCoordsTest()
{
    auto mesh     = data::Mesh::New();
    auto meshLock = mesh->dump_lock();
    mesh->reserve(3, 3, data::Mesh::CellType::POINT, data::Mesh::Attributes::CELL_TEX_COORDS);
    mesh->pushPoint(0, 1, 2);
    mesh->pushPoint(3, 4, 5);
    mesh->pushPoint(6, 7, 8);
    mesh->pushCell(0U);
    mesh->pushCell(1U);
    mesh->pushCell(2U);
    mesh->setCellTexCoord(0, 0, 1);
    mesh->setCellTexCoord(1, 1, 2);
    mesh->setCellTexCoord(2, 2, 3);

    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh, vtkMesh);

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "component " + std::to_string(j) + " of point " + std::to_string(i),
                i * 3. + j,
                vtkMesh->GetPoints()->GetPoint(i)[j]
            );
        }
    }

    auto* pointIdList = vtkIdList::New();
    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        vtkMesh->GetCellPoints(i, pointIdList);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), vtkIdType(i), pointIdList->GetId(0));
    }

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 2 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "cell " + std::to_string(i),
                double(i) + j,
                vtkMesh->GetCellData()->GetTCoords()->GetTuple(i)[j]
            );
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::fromVtkMeshWithLinesTest()
{
    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    auto points  = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0, 1, 2);
    points->InsertNextPoint(3, 4, 5);
    points->InsertNextPoint(6, 7, 8);
    points->InsertNextPoint(9, 10, 11);
    vtkMesh->SetPoints(points);
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell({0, 1});
    cells->InsertNextCell({1, 2});
    cells->InsertNextCell({2, 3});
    vtkMesh->SetLines(cells);

    auto mesh = data::Mesh::New();
    io::vtk::helper::Mesh::fromVTKMesh(vtkMesh, mesh);

    auto meshLock  = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    auto* pointIdList = vtkIdList::New();
    i = 0;
    for(auto c : mesh->crange<data::iterator::cell::line>())
    {
        vtkMesh->GetCellPoints(i, pointIdList);
        for(std::uint8_t j = 0 ; j < 2 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "point " + std::to_string(j) + " of cell " + std::to_string(i),
                data::iterator::cell_t(i) + j,
                c.pt[j]
            );
        }

        i++;
    }
}

//------------------------------------------------------------------------------

void MeshTest::fromVtkMeshWithQuadsTest()
{
    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    auto points  = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0, 1, 2);
    points->InsertNextPoint(3, 4, 5);
    points->InsertNextPoint(6, 7, 8);
    points->InsertNextPoint(9, 10, 11);
    points->InsertNextPoint(12, 13, 14);
    vtkMesh->SetPoints(points);
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell({0, 1, 2, 3});
    cells->InsertNextCell({1, 2, 3, 4});
    cells->InsertNextCell({2, 3, 4, 0});
    vtkMesh->SetPolys(cells);

    auto mesh = data::Mesh::New();
    io::vtk::helper::Mesh::fromVTKMesh(vtkMesh, mesh);

    auto meshLock  = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    i = 0;
    for(auto c : mesh->crange<data::iterator::cell::quad>())
    {
        std::uint8_t j = 0;
        for(auto pt : c.pt)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "point " + std::to_string(j) + " of cell " + std::to_string(i),
                (i + j) % 5U,
                pt
            );
            j++;
        }

        i++;
    }
}

//------------------------------------------------------------------------------

void MeshTest::fromVtkMeshWithCellTexCoordsTest()
{
    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    auto points  = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0, 1, 2);
    points->InsertNextPoint(3, 4, 5);
    points->InsertNextPoint(6, 7, 8);
    vtkMesh->SetPoints(points);
    auto cells                    = vtkSmartPointer<vtkCellArray>::New();
    std::vector<vtkIdType> idList = {0};
    cells->InsertNextCell(static_cast<vtkIdType>(idList.size()), idList.data());
    idList = {1};
    cells->InsertNextCell(static_cast<vtkIdType>(idList.size()), idList.data());
    idList = {2};
    cells->InsertNextCell(static_cast<vtkIdType>(idList.size()), idList.data());
    vtkMesh->SetVerts(cells);
    auto* texCoordsArray = new float [6] {0, 1, 1, 2, 2, 3};
    auto texCoords       = vtkSmartPointer<vtkFloatArray>::New();
    texCoords->SetNumberOfComponents(2);
    texCoords->SetArray(texCoordsArray, 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
    vtkMesh->GetCellData()->SetTCoords(texCoords);

    auto mesh = data::Mesh::New();
    io::vtk::helper::Mesh::fromVTKMesh(vtkMesh, mesh);

    auto meshLock  = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    i = 0;
    for(auto [c, uv] : mesh->czip_range<data::iterator::cell::point, data::iterator::cell::uv>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), unsigned(i), c.pt);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), float(i), uv.u);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), float(i) + 1, uv.v);
        i++;
    }
}

//------------------------------------------------------------------------------

void MeshTest::fromVtkMeshWith3ComponentsCellColorsTest()
{
    auto vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    auto points  = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0, 1, 2);
    points->InsertNextPoint(3, 4, 5);
    points->InsertNextPoint(6, 7, 8);
    vtkMesh->SetPoints(points);
    auto cells                    = vtkSmartPointer<vtkCellArray>::New();
    std::vector<vtkIdType> idList = {0};
    cells->InsertNextCell(static_cast<vtkIdType>(idList.size()), idList.data());
    idList = {1};
    cells->InsertNextCell(static_cast<vtkIdType>(idList.size()), idList.data());
    idList = {2};
    cells->InsertNextCell(static_cast<vtkIdType>(idList.size()), idList.data());
    vtkMesh->SetVerts(cells);
    auto* colorsArray = new std::uint8_t[9] {0, 1, 2, 3, 4, 5, 6, 7, 8};
    auto colors       = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    colors->SetArray(colorsArray, 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
    vtkMesh->GetCellData()->SetScalars(colors);

    auto mesh = data::Mesh::New();
    io::vtk::helper::Mesh::fromVTKMesh(vtkMesh, mesh);

    auto meshLock  = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    i = 0;
    for(auto [c, rgba] : mesh->czip_range<data::iterator::cell::point, data::iterator::cell::rgba>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), unsigned(i), c.pt);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(3 * i), rgba.r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(3 * i + 1), rgba.g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(3 * i + 2), rgba.b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(255), rgba.a);
        i++;
    }
}

} // namespace sight::io::vtk::ut
