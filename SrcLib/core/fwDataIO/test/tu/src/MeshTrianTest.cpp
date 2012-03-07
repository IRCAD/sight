/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <iomanip>
#include <fstream>

#include <fwTools/System.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <fwDataIO/reader/MeshReader.hpp>
#include <fwDataIO/writer/MeshWriter.hpp>

#include "MeshTrianTest.hpp"

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
    ::boost::filesystem::path trianPath = ::fwTools::System::getTemporaryFolder() / "test.trian";

    ::fwData::Mesh::NewSptr mesh;
    ::fwData::Mesh::NewSptr mesh2;
    ::fwDataTools::MeshGenerator::generateTriangleMesh(mesh);
    ::fwDataTools::MeshGenerator::shakePoint(mesh);

    mesh->adjustAllocatedMemory();

    ::fwDataIO::writer::MeshWriter::NewSptr writer;
    writer->setObject(mesh);
    writer->setFile(trianPath);
    writer->write();

    ::fwDataIO::reader::MeshReader::NewSptr reader;

    reader->setObject(mesh2);
    reader->setFile(trianPath);
    reader->read();

    bool suppr = ::boost::filesystem::remove(trianPath);
    CPPUNIT_ASSERT(suppr);

    this->compareMesh(mesh, mesh2);
}

//------------------------------------------------------------------------------

void MeshTrianTest::testMeshWithCellNormals()
{
    ::boost::filesystem::path trianPath = ::fwTools::System::getTemporaryFolder() / "test.trian";

    ::fwData::Mesh::NewSptr mesh;
    ::fwData::Mesh::NewSptr mesh2;
    ::fwDataTools::MeshGenerator::generateTriangleMesh(mesh);
    ::fwDataTools::MeshGenerator::shakePoint(mesh);
    ::fwDataTools::MeshGenerator::generateCellNormals(mesh);

    mesh->adjustAllocatedMemory();

    ::fwDataIO::writer::MeshWriter::NewSptr writer;
    writer->setObject(mesh);
    writer->setFile(trianPath);
    writer->write();

    ::fwDataIO::reader::MeshReader::NewSptr reader;

    reader->setObject(mesh2);
    reader->setFile(trianPath);
    reader->read();

    bool suppr = ::boost::filesystem::remove(trianPath);
    CPPUNIT_ASSERT(suppr);

    this->compareMesh(mesh, mesh2);
}

//------------------------------------------------------------------------------

#define COMPAREBUFFER(type, buff1, buff2)                            \
{                                                                    \
    CPPUNIT_ASSERT( (!buff1 && !buff2) || (buff1 && buff2));         \
    if(buff1)                                                        \
    {                                                                \
        CPPUNIT_ASSERT(buff1->getSize() == buff2->getSize());        \
                                                                     \
        type *iter1 = buff1->begin<type>();                          \
        type *iter2 = buff2->begin<type>();                          \
                                                                     \
        for (; iter1 != buff1->end<type>() ; ++iter1, ++iter2)       \
        {                                                            \
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*iter1, *iter2, 0.000001);  \
        }                                                            \
    }                                                                \
}

//------------------------------------------------------------------------------

void MeshTrianTest::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells() , mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize()  , mesh2->getCellDataSize());

    COMPAREBUFFER(::fwData::Mesh::PointValueType, mesh1->getPointsArray(), mesh2->getPointsArray());
    COMPAREBUFFER(::fwData::Mesh::CellTypes, mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    COMPAREBUFFER(::fwData::Mesh::CellDataOffsetType, mesh1->getCellDataOffsetsArray(), mesh2->getCellDataOffsetsArray());
    COMPAREBUFFER(::fwData::Mesh::CellValueType, mesh1->getCellDataArray(), mesh2->getCellDataArray());
    COMPAREBUFFER(::fwData::Mesh::ColorValueType, mesh1->getPointColorsArray(), mesh2->getPointColorsArray());
    COMPAREBUFFER(::fwData::Mesh::ColorValueType, mesh1->getCellColorsArray(), mesh2->getCellColorsArray());
    COMPAREBUFFER(::fwData::Mesh::NormalValueType, mesh1->getPointNormalsArray(), mesh2->getPointNormalsArray());
    COMPAREBUFFER(::fwData::Mesh::NormalValueType, mesh1->getCellNormalsArray(), mesh2->getCellNormalsArray());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataIO
