/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataTools/MeshGenerator.hpp>

#include "MeshGeneratorTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MeshGeneratorTest );

//------------------------------------------------------------------------------

void MeshGeneratorTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshGeneratorTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshGeneratorTest::testMeshToTriangularMesh()
{
    ::fwData::Mesh::NewSptr mesh1;
    ::fwDataTools::MeshGenerator::generateTriangleMesh(mesh1);
    ::fwDataTools::MeshGenerator::shakePoint(mesh1);
    mesh1->adjustAllocatedMemory();

    ::fwData::TriangularMesh::NewSptr trian;
    ::fwDataTools::MeshGenerator::toTriangularMesh(mesh1, trian);

    ::fwData::Mesh::NewSptr mesh2;
    ::fwDataTools::MeshGenerator::fromTriangularMesh(trian, mesh2);

    this->compareMesh(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshGeneratorTest::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells() , mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize()  , mesh2->getCellDataSize());

    this->compareBuffer(mesh1->getPointsArray(), mesh2->getPointsArray());
    this->compareBuffer(mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    this->compareBuffer(mesh1->getCellDataOffsetsArray(), mesh2->getCellDataOffsetsArray());
    this->compareBuffer(mesh1->getCellDataArray(), mesh2->getCellDataArray());
}

//------------------------------------------------------------------------------

void MeshGeneratorTest::compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2)
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

//------------------------------------------------------------------------------
