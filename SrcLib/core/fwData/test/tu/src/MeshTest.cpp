/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/cstdint.hpp>
#include "fwData/Mesh.hpp"
#include "MeshTest.hpp"


using namespace boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MeshTest );

void MeshTest::setUp()
{
    // Set up context before running a test.

}
void MeshTest::tearDown()
{
    // Clean up after the test run.
}

void MeshTest::allocation()
{
    ::fwData::Mesh::NewSptr mesh;
    size_t pointSize = 3000;
    size_t cellSize = 2000;
    size_t cellDataSize = 8000;
    mesh->allocate(pointSize, cellSize, cellDataSize);
    CPPUNIT_ASSERT_EQUAL(pointSize, mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(cellSize, mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(cellDataSize, mesh->getCellDataArray()->getSize()[0]);

    mesh->allocatePointNormals();
}

void MeshTest::insertion()
{
    ::fwData::Mesh::NewSptr mesh;
    mesh->allocate(2, 2);
    mesh->insertNextPoint(10, 20, 30);
    mesh->insertNextPoint(10, 10, 10);
    mesh->insertNextPoint(20, 20, 10);
    mesh->insertNextPoint(30, 30, 10);
    mesh->insertNextPoint(15, 20, 35);
    mesh->insertNextPoint(20, 20, 10);
    mesh->insertNextPoint(20, 63, 17);
    mesh->insertNextPoint(27, 83, 52);

    mesh->insertNextCell(1, 2);
    mesh->insertNextCell(1, 3, 4);
    mesh->insertNextCell(1, 2, 5, 4);

    ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
    mesh->insertNextCell(::fwData::Mesh::QUAD, p, 4);

    ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
    mesh->insertNextCell(::fwData::Mesh::POLY, p2, 6);

    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)8, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)5, mesh->getNumberOfCells());

    ::fwData::Mesh::PointMultiArrayType pointArray = mesh->getPoints();
    CPPUNIT_ASSERT_EQUAL((float) 10, pointArray[0][0]);
    CPPUNIT_ASSERT_EQUAL((float) 20, pointArray[2][1]);
    CPPUNIT_ASSERT_EQUAL((float) 52, pointArray[7][2]);

    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsetArray = mesh->getCellDataOffsets();
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)0, cellDataOffsetArray[0]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)2, cellDataOffsetArray[1]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)9, cellDataOffsetArray[3]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)13, cellDataOffsetArray[4]);

    ::fwData::Mesh::CellDataMultiArrayType cellDataArray = mesh->getCellData();
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)1, cellDataArray[cellDataOffsetArray[0]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)1, cellDataArray[cellDataOffsetArray[1]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)3, cellDataArray[cellDataOffsetArray[3]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)1, cellDataArray[cellDataOffsetArray[4]]);

    ::fwData::Mesh::CellTypesMultiArrayType cellTypesArray = mesh->getCellTypes();
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::EDGE, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[0]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::TRIANGLE, static_cast < ::fwData::Mesh::CellTypesEnum >(cellTypesArray[1]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::QUAD, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[3]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::POLY, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[4]));
}

