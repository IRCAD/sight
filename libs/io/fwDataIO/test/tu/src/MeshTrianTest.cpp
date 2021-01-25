/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "MeshTrianTest.hpp"

#include <fwDataIO/reader/MeshReader.hpp>
#include <fwDataIO/writer/MeshWriter.hpp>

#include <core/tools/System.hpp>

#include <fwDataTools/Mesh.hpp>

#include <utestData/generator/Mesh.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataIO::ut::MeshTrianTest );

namespace fwDataIO
{
namespace ut
{

//------------------------------------------------------------------------------

void MeshTrianTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshTrianTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTrianTest::testSimpleMesh()
{
    std::filesystem::path trianPath = core::tools::System::getTemporaryFolder() / "test.trian";

    data::Mesh::sptr mesh  = data::Mesh::New();
    data::Mesh::sptr mesh2 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh);
    ::fwDataTools::Mesh::shakePoint(mesh);

    mesh->adjustAllocatedMemory();

    ::fwDataIO::writer::MeshWriter::sptr writer = ::fwDataIO::writer::MeshWriter::New();
    writer->setObject(mesh);
    writer->setFile(trianPath);
    writer->write();

    ::fwDataIO::reader::MeshReader::sptr reader = ::fwDataIO::reader::MeshReader::New();

    reader->setObject(mesh2);
    reader->setFile(trianPath);
    reader->read();

    bool suppr = std::filesystem::remove(trianPath);
    CPPUNIT_ASSERT(suppr);

    this->compareMesh(mesh, mesh2);
}

//------------------------------------------------------------------------------

void MeshTrianTest::testMeshWithCellNormals()
{
    std::filesystem::path trianPath = core::tools::System::getTemporaryFolder() / "test.trian";

    data::Mesh::sptr mesh  = data::Mesh::New();
    data::Mesh::sptr mesh2 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh);
    ::fwDataTools::Mesh::shakePoint(mesh);
    ::fwDataTools::Mesh::generateCellNormals(mesh);

    mesh->adjustAllocatedMemory();

    ::fwDataIO::writer::MeshWriter::sptr writer = ::fwDataIO::writer::MeshWriter::New();
    writer->setObject(mesh);
    writer->setFile(trianPath);
    writer->write();

    ::fwDataIO::reader::MeshReader::sptr reader = ::fwDataIO::reader::MeshReader::New();

    reader->setObject(mesh2);
    reader->setFile(trianPath);
    reader->read();

    bool suppr = std::filesystem::remove(trianPath);
    CPPUNIT_ASSERT(suppr);

    this->compareMesh(mesh, mesh2);
}

//------------------------------------------------------------------------------

void MeshTrianTest::compareMesh(data::Mesh::sptr mesh1, data::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize(), mesh2->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL(mesh1->hasCellNormals(), mesh2->hasCellNormals());

    const auto dumpLock1 = mesh1->lock();
    const auto dumpLock2 = mesh2->lock();

    auto pointItr1      = mesh1->begin< data::iterator::ConstPointIterator >();
    auto pointItr2      = mesh2->begin< data::iterator::ConstPointIterator >();
    const auto pointEnd = mesh1->end< data::iterator::ConstPointIterator >();

    for (; pointItr1 != pointEnd; ++pointItr1, ++pointItr2)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr1->point->x, pointItr2->point->x, 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr1->point->y, pointItr2->point->y, 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr1->point->z, pointItr2->point->z, 0.00001);
    }

    auto cellItr1      = mesh1->begin< data::iterator::ConstCellIterator >();
    auto cellItr2      = mesh2->begin< data::iterator::ConstCellIterator >();
    const auto cellEnd = mesh1->end< data::iterator::ConstCellIterator >();

    for(; cellItr1 != cellEnd; ++cellItr1, ++cellItr2)
    {
        CPPUNIT_ASSERT_EQUAL(*cellItr1->type, *cellItr2->type);
        CPPUNIT_ASSERT_EQUAL(*cellItr1->offset, *cellItr2->offset);
        CPPUNIT_ASSERT_EQUAL(cellItr1->nbPoints, cellItr2->nbPoints);
        CPPUNIT_ASSERT_EQUAL(cellItr1->pointIdx[0], cellItr2->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL(cellItr1->pointIdx[1], cellItr2->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL(cellItr1->pointIdx[2], cellItr2->pointIdx[2]);
        if (mesh1->hasCellNormals())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellItr1->normal->nx, cellItr2->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellItr1->normal->ny, cellItr2->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellItr1->normal->nz, cellItr2->normal->nz, 0.00001);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataIO
