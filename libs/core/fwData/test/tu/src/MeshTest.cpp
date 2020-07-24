/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include <fwData/Mesh.hpp>
#include <fwData/ObjectLock.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MeshTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void MeshTest::deprecatedAllocation()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    size_t pointSize    = 3000;
    size_t cellSize     = 2000;
    size_t cellDataSize = 8000;
    mesh->allocate(pointSize, cellSize, cellDataSize);
    CPPUNIT_ASSERT_EQUAL(pointSize, mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(cellSize, mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(cellDataSize, mesh->getCellDataArray()->getSize()[0]);

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointsArray()->getNumberOfComponents());
    size_t pointAllocatedSize = pointSize * mesh->getPointsArray()->getNumberOfComponents() *
                                mesh->getPointsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize, mesh->getPointsArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(1), mesh->getCellTypesArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellTypesArray()->getNumberOfComponents());
    size_t cellTypeAllocatedSize = cellSize * mesh->getCellTypesArray()->getNumberOfComponents() *
                                   mesh->getCellTypesArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellTypeAllocatedSize, mesh->getCellTypesArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(8), mesh->getCellDataOffsetsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellDataOffsetsArray()->getNumberOfComponents());
    size_t cellDataOffsetsAllocatedSize = cellSize * mesh->getCellDataOffsetsArray()->getNumberOfComponents() *
                                          mesh->getCellDataOffsetsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellDataOffsetsAllocatedSize, mesh->getCellDataOffsetsArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(8), mesh->getCellDataArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellDataArray()->getNumberOfComponents());
    size_t cellDataAllocatedSize = cellDataSize * mesh->getCellDataArray()->getNumberOfComponents() *
                                   mesh->getCellDataArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellDataAllocatedSize, mesh->getCellDataArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize + cellSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize,
                         mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfCells());

    const auto dumpLock = mesh->lock();
    mesh->pushPoint(10, 20, 30);
    mesh->pushPoint(10, 10, 10);
    mesh->pushPoint(20, 20, 10);

    mesh->pushCell(1, 2, 3);
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)3, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)1, mesh->getNumberOfCells());

    mesh->allocatePointNormals();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointNormalsArray()->getSize()[0]);
    mesh->allocatePointColors(::fwData::Mesh::RGB);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointColorsArray()->getSize()[0]);
    mesh->allocatePointTexCoords();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointTexCoordsArray()->getSize()[0]);

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointNormalsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointNormalsArray()->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(1), mesh->getPointColorsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointColorsArray()->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointTexCoordsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), mesh->getPointTexCoordsArray()->getNumberOfComponents());

    size_t pointNormalsAllocatedSize = mesh->getNumberOfPoints() *
                                       mesh->getPointNormalsArray()->getNumberOfComponents() *
                                       mesh->getPointNormalsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointNormalsAllocatedSize, mesh->getPointNormalsArray()->getSizeInBytes());
    size_t pointColorsAllocatedSize = mesh->getNumberOfPoints() * mesh->getPointColorsArray()->getNumberOfComponents() *
                                      mesh->getPointColorsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointColorsAllocatedSize, mesh->getPointColorsArray()->getSizeInBytes());
    size_t pointTexCoordsAllocatedSize = mesh->getNumberOfPoints() *
                                         mesh->getPointTexCoordsArray()->getNumberOfComponents() *
                                         mesh->getPointTexCoordsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointTexCoordsAllocatedSize, mesh->getPointTexCoordsArray()->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize + pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize,
        mesh->getAllocatedSizeInBytes());

    mesh->allocateCellNormals();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellNormalsArray()->getSize()[0]);
    mesh->allocateCellColors(::fwData::Mesh::RGBA);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellColorsArray()->getSize()[0]);
    mesh->allocateCellTexCoords();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellTexCoordsArray()->getSize()[0]);

    size_t oldsize = mesh->getAllocatedSizeInBytes();
    bool adjusted  = mesh->adjustAllocatedMemory();
    size_t newSize = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > newSize);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getCellDataSize()), mesh->getCellDataArray()->getSize()[0]);

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, mesh->getDataSizeInBytes());

    ::fwData::Mesh::Id nbPoints        = 300;
    ::fwData::Mesh::Id nbCells         = 200;
    ::fwData::Mesh::Id newCellDataSize = 500;
    mesh->setNumberOfPoints(nbPoints);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(newCellDataSize);
    oldsize  = mesh->getAllocatedSizeInBytes();
    adjusted = mesh->adjustAllocatedMemory();
    newSize  = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize < newSize);

    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(nbCells, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(newCellDataSize, mesh->getCellDataSize());

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(nbPoints), mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(nbCells), mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(newCellDataSize), mesh->getCellDataArray()->getSize()[0]);
}

//-----------------------------------------------------------------------------

void MeshTest::copy()
{
    ::fwData::Mesh::sptr mesh            = ::fwData::Mesh::New();
    ::fwData::Mesh::sptr shallowCopyMesh = ::fwData::Mesh::New();

    const auto dumpLock = mesh->lock();

    mesh->pushPoint(10, 20, 30);
    mesh->pushPoint(10, 10, 10);
    mesh->pushPoint(20, 20, 10);
    mesh->pushPoint(30, 30, 10);
    mesh->pushPoint(15, 20, 35);
    mesh->pushPoint(20, 20, 10);
    mesh->pushPoint(20, 63, 17);
    mesh->pushPoint(27, 83, 52);

    mesh->pushCell(1, 2);
    mesh->pushCell(1, 3, 4);
    mesh->pushCell(1, 2, 5, 4);

    ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
    mesh->pushCell(::fwData::Mesh::CellType::QUAD, p, 4);

    ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
    mesh->pushCell(::fwData::Mesh::CellType::POLY, p2, 6);

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    mesh->pushCell(::fwData::Mesh::CellType::POLY, p3, 5);

    mesh->resize(mesh->getNumberOfPoints(), mesh->getNumberOfCells(), mesh->getCellDataSize(),
                 ::fwData::Mesh::Attributes::POINT_COLORS | ::fwData::Mesh::Attributes::POINT_TEX_COORDS);

    // check deep copy
    {
        ::fwData::Mesh::sptr deepCopyMesh;
        deepCopyMesh = ::fwData::Object::copy(mesh);
        const auto copyDumpLock = deepCopyMesh->lock();

        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), deepCopyMesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), deepCopyMesh->getNumberOfCells());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellDataSize(), deepCopyMesh->getCellDataSize());

        auto pointItr         = mesh->begin< ::fwData::iterator::ConstPointIterator >();
        const auto pointEnd   = mesh->end< ::fwData::iterator::ConstPointIterator >();
        auto pointItrCopyMesh = deepCopyMesh->begin< ::fwData::iterator::ConstPointIterator >();

        for (; pointItr != pointEnd; ++pointItr, ++pointItrCopyMesh)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr->point->x, pointItrCopyMesh->point->x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr->point->y, pointItrCopyMesh->point->y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr->point->z, pointItrCopyMesh->point->z, EPSILON);
        }

        auto cellItr         = mesh->begin< ::fwData::iterator::ConstCellIterator >();
        const auto cellEnd   = mesh->end< ::fwData::iterator::ConstCellIterator >();
        auto cellItrCopyMesh = deepCopyMesh->begin< ::fwData::iterator::ConstCellIterator >();

        for (; cellItr != cellEnd; ++cellItr, ++cellItrCopyMesh)
        {
            CPPUNIT_ASSERT_EQUAL(*cellItr->type, *cellItrCopyMesh->type);
            CPPUNIT_ASSERT_EQUAL(*cellItr->offset, *cellItrCopyMesh->offset);
            CPPUNIT_ASSERT_EQUAL(cellItr->nbPoints, cellItrCopyMesh->nbPoints);
            for (size_t i = 0; i < cellItr->nbPoints; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(cellItr->pointIdx[i], cellItrCopyMesh->pointIdx[i]);
            }
        }
    }

    //check shallow copy
    {
        shallowCopyMesh->shallowCopy(mesh);
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), shallowCopyMesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), shallowCopyMesh->getNumberOfCells());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellDataSize(), shallowCopyMesh->getCellDataSize());

        const auto pointItr         = mesh->begin< ::fwData::iterator::ConstPointIterator >();
        const auto pointItrCopyMesh = shallowCopyMesh->begin< ::fwData::iterator::ConstPointIterator >();

        CPPUNIT_ASSERT_EQUAL(pointItr->point, pointItrCopyMesh->point);
        CPPUNIT_ASSERT_EQUAL(pointItr->normal, pointItrCopyMesh->normal);
        CPPUNIT_ASSERT_EQUAL(pointItr->rgb, pointItrCopyMesh->rgb);
        CPPUNIT_ASSERT_EQUAL(pointItr->rgba, pointItrCopyMesh->rgba);
        CPPUNIT_ASSERT_EQUAL(pointItr->tex, pointItrCopyMesh->tex);

        const auto cellItr         = mesh->begin< ::fwData::iterator::ConstCellIterator >();
        const auto cellItrCopyMesh = shallowCopyMesh->begin< ::fwData::iterator::ConstCellIterator >();

        CPPUNIT_ASSERT_EQUAL(cellItr->type, cellItrCopyMesh->type);
        CPPUNIT_ASSERT_EQUAL(cellItr->offset, cellItrCopyMesh->offset);
        CPPUNIT_ASSERT_EQUAL(cellItr->pointIdx, cellItrCopyMesh->pointIdx);
        CPPUNIT_ASSERT_EQUAL(cellItr->rgb, cellItrCopyMesh->rgb);
        CPPUNIT_ASSERT_EQUAL(cellItr->rgba, cellItrCopyMesh->rgba);
        CPPUNIT_ASSERT_EQUAL(cellItr->tex, cellItrCopyMesh->tex);

        //Deprecated
        CPPUNIT_ASSERT_EQUAL(mesh->getPointsArray(), shallowCopyMesh->getPointsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellTypesArray(), shallowCopyMesh->getCellTypesArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellDataOffsetsArray(), shallowCopyMesh->getCellDataOffsetsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellDataArray(), shallowCopyMesh->getCellDataArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getPointColorsArray(), shallowCopyMesh->getPointColorsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getPointNormalsArray(), shallowCopyMesh->getPointNormalsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getPointTexCoordsArray(), shallowCopyMesh->getPointTexCoordsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellColorsArray(), shallowCopyMesh->getCellColorsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellNormalsArray(), shallowCopyMesh->getCellNormalsArray());
        CPPUNIT_ASSERT_EQUAL(mesh->getCellTexCoordsArray(), shallowCopyMesh->getCellTexCoordsArray());
    }
}

//------------------------------------------------------------------------------

void MeshTest::allocationTest()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    size_t pointSize    = 3000;
    size_t cellSize     = 2000;
    size_t cellDataSize = 8000;

    const auto lock = mesh->lock();

    mesh->pushPoint(10, 20, 30);
    mesh->pushPoint(10, 10, 10);
    mesh->pushPoint(20, 20, 10);

    mesh->pushCell(0, 1, 2);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(3), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(1), mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(3), mesh->getCellDataSize());

    size_t pointAllocatedSize           = mesh->getNumberOfPoints() * 3 * sizeof (::fwData::Mesh::PointValueType);
    size_t cellTypeAllocatedSize        = mesh->getNumberOfCells() * sizeof (::fwData::Mesh::CellTypes);
    size_t cellDataOffsetsAllocatedSize = mesh->getNumberOfCells() * sizeof (::fwData::Mesh::CellDataOffsetType);
    size_t cellDataAllocatedSize        = mesh->getCellDataSize() * sizeof(::fwData::Mesh::CellValueType);

    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellTypeAllocatedSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize,
        mesh->getDataSizeInBytes());

    mesh->reserve(pointSize, cellSize, cellDataSize);

    pointAllocatedSize           = pointSize * 3 * sizeof (::fwData::Mesh::PointValueType);
    cellTypeAllocatedSize        = cellSize * sizeof (::fwData::Mesh::CellTypes);
    cellDataOffsetsAllocatedSize = cellSize * sizeof (::fwData::Mesh::CellDataOffsetType);
    cellDataAllocatedSize        = cellDataSize * sizeof(::fwData::Mesh::CellValueType);

    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellTypeAllocatedSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize,
        mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(3), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(1), mesh->getNumberOfCells());

    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellTexCoords());

    mesh->reserve(pointSize, cellSize, cellDataSize, ::fwData::Mesh::Attributes::POINT_NORMALS |
                  ::fwData::Mesh::Attributes::POINT_COLORS | ::fwData::Mesh::Attributes::POINT_TEX_COORDS);

    size_t pointNormalsAllocatedSize   = pointSize * 3 * sizeof (::fwData::Mesh::NormalValueType);
    size_t pointColorsAllocatedSize    = pointSize * 4 * sizeof (::fwData::Mesh::ColorValueType);
    size_t pointTexCoordsAllocatedSize = pointSize * 2 * sizeof (::fwData::Mesh::TexCoordValueType);
    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellTypeAllocatedSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize +
        pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize,
        mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellTexCoords());

    mesh->reserve(pointSize, cellSize, cellDataSize,
                  ::fwData::Mesh::Attributes::POINT_NORMALS |
                  ::fwData::Mesh::Attributes::POINT_COLORS |
                  ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
                  ::fwData::Mesh::Attributes::CELL_NORMALS |
                  ::fwData::Mesh::Attributes::CELL_COLORS |
                  ::fwData::Mesh::Attributes::CELL_TEX_COORDS);

    size_t oldsize = mesh->getAllocatedSizeInBytes();
    bool adjusted  = mesh->adjustAllocatedMemory();
    size_t newSize = mesh->getAllocatedSizeInBytes();

    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > newSize);

    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasCellTexCoords());

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(0), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(0), mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(0), mesh->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), mesh->getDataSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellTexCoords());

    ::fwData::Mesh::Id nbPoints        = 300;
    ::fwData::Mesh::Id nbCells         = 200;
    ::fwData::Mesh::Id newCellDataSize = 500;
    mesh->setNumberOfPoints(nbPoints);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(newCellDataSize);
    oldsize  = mesh->getAllocatedSizeInBytes();
    adjusted = mesh->adjustAllocatedMemory();
    newSize  = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize < newSize);

    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(nbCells, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(newCellDataSize, mesh->getCellDataSize());
}

//------------------------------------------------------------------------------

void MeshTest::insertion()
{
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        const auto lock = mesh->lock();
        mesh->pushPoint(10, 20, 30);
        mesh->pushPoint(10, 10, 10);
        mesh->pushPoint(20, 21, 10);
        mesh->pushPoint(30, 30, 10);
        mesh->pushPoint(15, 20, 35);
        mesh->pushPoint(20, 20, 10);
        mesh->pushPoint(20, 63, 17);
        mesh->pushPoint(27, 83, 52);

        mesh->pushCell(1, 2);
        mesh->pushCell(1, 3, 4);
        mesh->pushCell(1, 2, 5, 4);

        ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
        mesh->pushCell(::fwData::Mesh::CellType::QUAD, p, 4);

        ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
        mesh->pushCell(::fwData::Mesh::CellType::POLY, p2, 6);

        ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
        mesh->pushCell(::fwData::Mesh::CellType::POLY, p3, 5);

        CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(8), mesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(6), mesh->getNumberOfCells());

        auto it = mesh->begin< ::fwData::iterator::PointIterator >();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(10), (*it).point->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(20), (*it).point->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(30), (*it).point->z, EPSILON);
        CPPUNIT_ASSERT( nullptr == it->rgba);
        CPPUNIT_ASSERT( nullptr == it->normal);
        CPPUNIT_ASSERT( nullptr == it->tex);
        it += 2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(20), it->point->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(21), it->point->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(10), it->point->z, EPSILON);
        CPPUNIT_ASSERT( nullptr == it->rgba);
        CPPUNIT_ASSERT( nullptr == it->normal);
        CPPUNIT_ASSERT( nullptr == it->tex);
        it += 5;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(27), it->point->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(83), it->point->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(52), it->point->z, EPSILON);
        --it;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(20), it->point->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(63), it->point->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(17), it->point->z, EPSILON);
        it -= 2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(15), it->point->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(20), it->point->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(35), it->point->z, EPSILON);

        auto cellit = mesh->begin< ::fwData::iterator::CellIterator >();
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(0), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::EDGE, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(2), cellit.nbPoints());
        CPPUNIT_ASSERT( nullptr == cellit->rgba);
        CPPUNIT_ASSERT( nullptr == cellit->normal);
        CPPUNIT_ASSERT( nullptr == cellit->tex);
        ++cellit;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::TRIANGLE,
                              static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), cellit.nbPoints());
        cellit += 2;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(9), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::QUAD, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), cellit.nbPoints());
        cellit++;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(13), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[5]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[5]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::POLY, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(6), cellit.nbPoints());
        cellit += 1;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(19), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[4]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::POLY, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(5), cellit->nbPoints);
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(5), cellit.nbPoints());
        CPPUNIT_ASSERT( nullptr == cellit->rgba);
        CPPUNIT_ASSERT( nullptr == cellit->normal);
        CPPUNIT_ASSERT( nullptr == cellit->tex);
        --cellit;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(13), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[5]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[5]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::POLY, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(6), cellit.nbPoints());
        cellit--;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(9), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::QUAD, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), cellit->nbPoints);
        cellit -= 2;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::TRIANGLE,
                              static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), cellit->nbPoints);

        mesh->setPoint(4, 45, 59, 48);
        auto itPt = mesh->begin< ::fwData::iterator::PointIterator >();
        itPt += 4;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(45), itPt->point->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(59), itPt->point->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(48), itPt->point->z, EPSILON);
    }

    {
        const ::fwData::Mesh::Id NB_POINTS           = 60;
        const ::fwData::Mesh::Id NB_CELLS            = 59;
        const ::fwData::Mesh::CellType CELL_TYPE     = ::fwData::Mesh::CellType::TRIANGLE;
        const ::fwData::Mesh::Attributes EXTRA_ARRAY =
            ::fwData::Mesh::Attributes::POINT_NORMALS |
            ::fwData::Mesh::Attributes::POINT_COLORS |
            ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
            ::fwData::Mesh::Attributes::CELL_NORMALS |
            ::fwData::Mesh::Attributes::CELL_COLORS |
            ::fwData::Mesh::Attributes::CELL_TEX_COORDS;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

        mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
        const auto lock = mesh->lock();

        for (size_t i = 0; i < NB_POINTS; ++i)
        {
            const std::uint8_t val                                            = static_cast<uint8_t>(i);
            const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
            const float floatVal                                              = static_cast<float>(i);
            const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
            const size_t value                                                = 3*i;
            const auto id                                                     = mesh->pushPoint(
                static_cast<float>(value),
                static_cast<float>(value
                                   +1),
                static_cast<float>(value
                                   +2));
            mesh->setPointColor(id, color);
            mesh->setPointNormal(id, normal);
            mesh->setPointTexCoord(id, texCoords);
        }

        for (size_t i = 0; i < NB_CELLS; ++i)
        {
            const auto id = mesh->pushCell(i, i+1, i+2);

            const ::fwData::Mesh::ColorValueType val =
                static_cast< ::fwData::Mesh::ColorValueType >(i);
            const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
            const float floatVal                                              = static_cast<float>(i);
            const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
            mesh->setCellColor(id, color);
            mesh->setCellNormal(id, normal);
            mesh->setCellTexCoord(id, texCoords);
        }
        CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(NB_CELLS, mesh->getNumberOfCells());
        CPPUNIT_ASSERT_EQUAL(NB_CELLS*3, mesh->getCellDataSize());
        CPPUNIT_ASSERT_EQUAL(mesh->getAllocatedSizeInBytes(), mesh->getDataSizeInBytes());
        const bool resizeMemory = mesh->adjustAllocatedMemory();
        CPPUNIT_ASSERT_EQUAL(false, resizeMemory);
    }
}

//------------------------------------------------------------------------------

void MeshTest::iteratorTest()
{
    const ::fwData::Mesh::Id NB_POINTS           = 60;
    const ::fwData::Mesh::Id NB_CELLS            = 59;
    const ::fwData::Mesh::CellType CELL_TYPE     = ::fwData::Mesh::CellType::TRIANGLE;
    const ::fwData::Mesh::Attributes EXTRA_ARRAY =
        ::fwData::Mesh::Attributes::POINT_NORMALS |
        ::fwData::Mesh::Attributes::POINT_COLORS |
        ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
        ::fwData::Mesh::Attributes::CELL_NORMALS |
        ::fwData::Mesh::Attributes::CELL_COLORS |
        ::fwData::Mesh::Attributes::CELL_TEX_COORDS;

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                                            = static_cast<uint8_t>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const ::fwData::Mesh::ColorValueType val =
            static_cast< ::fwData::Mesh::ColorValueType >(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }

    {
        auto it          = mesh->begin< ::fwData::iterator::PointIterator >();
        const auto itEnd = mesh->end< ::fwData::iterator::PointIterator >();
        ::fwData::iterator::PointIterator pointIt;

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            ::fwData::iterator::Point p = *it->point;
            pointIt                     = it;
            const float fValue = static_cast<float>(3*count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue+1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue+2, p.z, EPSILON);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal = static_cast< ::fwData::Mesh::NormalValueType >(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v, EPSILON);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), count);

        ::fwData::iterator::Point p = *pointIt->point;
        const float fValue = static_cast<float>( 3*(count-1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue, p.x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue+1, p.y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue+2, p.z, EPSILON);
    }

    {
        auto it            = mesh->end< ::fwData::iterator::PointIterator >()-1;
        const auto itBegin = mesh->begin< ::fwData::iterator::PointIterator >();

        size_t count = NB_POINTS-1;
        for (; it != itBegin; --it)
        {
            ::fwData::iterator::Point p = *it->point;
            const float fValue = static_cast<float>(3*count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fValue, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fValue+1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fValue+2, p.z, EPSILON);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal = static_cast< ::fwData::Mesh::NormalValueType >(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v, EPSILON);

            --count;
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), count);
    }

    {
        auto it          = mesh->begin< ::fwData::iterator::CellIterator >();
        const auto itEnd = mesh->end< ::fwData::iterator::CellIterator >();
        ::fwData::iterator::CellIterator cellIt;

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            cellIt = it;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count), it[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+2), it->pointIdx[2]);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal =
                static_cast< ::fwData::Mesh::NormalValueType >(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v, EPSILON);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it->nbPoints);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), count);

        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint64_t>(count-1), cellIt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint64_t>(count), cellIt->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint64_t>(count+1), cellIt->pointIdx[2]);
    }

    {
        auto it            = mesh->end< ::fwData::iterator::CellIterator >()-1;
        const auto itBegin = mesh->begin< ::fwData::iterator::CellIterator >();

        size_t count = 0;
        for (; it != itBegin; --it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count), it[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+2), it->pointIdx[2]);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal =
                static_cast< ::fwData::Mesh::NormalValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v, EPSILON);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it->nbPoints);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells()-1, count);
    }

    ::fwData::Mesh::csptr mesh2 = ::fwData::Mesh::copy(mesh);
    {
        auto it          = mesh2->begin< ::fwData::iterator::ConstPointIterator >();
        const auto itEnd = mesh2->end< ::fwData::iterator::ConstPointIterator >();

        float count = 0;
        for (; it != itEnd; ++it)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, it->point->x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, it->point->y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, it->point->z, EPSILON);
        }
    }

    {
        auto it            = mesh2->end< ::fwData::iterator::ConstCellIterator >()-1;
        const auto itBegin = mesh2->begin< ::fwData::iterator::ConstCellIterator >();

        size_t count = 0;
        for (; it != itBegin; --it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count), it[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+2), it->pointIdx[2]);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal =
                static_cast< ::fwData::Mesh::NormalValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v, EPSILON);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it->nbPoints);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells()-1, count);
    }

    ::fwData::Mesh::sptr mesh3 = ::fwData::Mesh::New();
    mesh3->resize(NB_POINTS, NB_CELLS, ::fwData::Mesh::CellType::QUAD, EXTRA_ARRAY);

    {
        auto it          = mesh3->begin< ::fwData::iterator::PointIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::PointIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            ::fwData::iterator::Point* p = it->point;
            p->x                         = static_cast<float>(3*count);
            p->y                         = static_cast<float>(3*count+1);
            p->z                         = static_cast<float>(3*count+2);

            ::fwData::iterator::RGBA* c = it->rgba;
            c->r                        = static_cast<std::uint8_t>(4*count);
            c->g                        = static_cast<std::uint8_t>(4*count+1);
            c->b                        = static_cast<std::uint8_t>(4*count+2);
            c->a                        = static_cast<std::uint8_t>(4*count+3);

            ::fwData::iterator::Normal* n = it->normal;
            n->nx                         = static_cast<float>(3*count+1);
            n->ny                         = static_cast<float>(3*count+2);
            n->nz                         = static_cast<float>(3*count+3);

            ::fwData::iterator::TexCoords* uv = it->tex;
            uv->u                             = static_cast<float>(2*count);
            uv->v                             = static_cast<float>(2*count+1);
            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), count);
    }

    {
        auto it          = mesh3->begin< ::fwData::iterator::CellIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::CellIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            *it->type   = ::fwData::Mesh::CellType::QUAD;
            *it->offset = 4*count;
            if (it != itEnd-1)
            {
                *(it+1)->offset = 4*(count+1);
            }

            for (size_t i = 0; i < 4; ++i)
            {
                it->pointIdx[i] = count + i;
            }

            ::fwData::iterator::RGBA& c = *it->rgba;
            c.r                         = static_cast<std::uint8_t>(4*count);
            c.g                         = static_cast<std::uint8_t>(4*count+1);
            c.b                         = static_cast<std::uint8_t>(4*count+2);
            c.a                         = static_cast<std::uint8_t>(4*count+3);

            ::fwData::iterator::Normal& n = *it->normal;
            n.nx                          = static_cast<float>(3*count+1);
            n.ny                          = static_cast<float>(3*count+2);
            n.nz                          = static_cast<float>(3*count+3);

            ::fwData::iterator::TexCoords& uv = *it->tex;
            uv.u                              = static_cast<float>(2*count);
            uv.v                              = static_cast<float>(2*count+1);
            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), count);
    }

    {
        auto it          = mesh3->begin< ::fwData::iterator::ConstPointIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::ConstPointIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            ::fwData::iterator::Point p = *it->point;
            const float fValue = static_cast<float>(3*count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue+1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue+2, p.z, EPSILON);

            ::fwData::iterator::RGBA c = *it->rgba;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count, static_cast<size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+1, static_cast<size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+2, static_cast<size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+3, static_cast<size_t>(c.a));

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType nVal = static_cast< ::fwData::Mesh::NormalValueType >(3*count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+1, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+2, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+3, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            const ::fwData::Mesh::NormalValueType uvVal = static_cast< ::fwData::Mesh::NormalValueType >(2*count);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal+1, uv.v, EPSILON);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), count);
    }

    {
        auto it          = mesh3->begin< ::fwData::iterator::ConstCellIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::ConstCellIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(4), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count), it->pointIdx[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+2), it->pointIdx[2]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+3), it->pointIdx[3]);

            ::fwData::iterator::RGBA c = *it->rgba;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count, static_cast<size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+1, static_cast<size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+2, static_cast<size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+3, static_cast<size_t>(c.a));
            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType nVal =
                static_cast< ::fwData::Mesh::NormalValueType >(3*count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+1, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+2, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+3, n.nz, EPSILON);

            ::fwData::iterator::TexCoords uv = *it->tex;
            const ::fwData::Mesh::NormalValueType uvVal =
                static_cast< ::fwData::Mesh::NormalValueType >(2*count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal+1, uv.v, EPSILON);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(4), it->nbPoints);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), count);
    }
}

//------------------------------------------------------------------------------

void MeshTest::iteratorCopyTest()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(10, 10, ::fwData::Mesh::CellType::POINT,
                 ::fwData::Mesh::Attributes::POINT_NORMALS |
                 ::fwData::Mesh::Attributes::POINT_COLORS |
                 ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
                 ::fwData::Mesh::Attributes::CELL_NORMALS |
                 ::fwData::Mesh::Attributes::CELL_COLORS |
                 ::fwData::Mesh::Attributes::CELL_TEX_COORDS);

    ::fwData::Mesh::sptr copiedMesh = ::fwData::Mesh::New();
    copiedMesh->resize(10, 10, ::fwData::Mesh::CellType::POINT,
                       ::fwData::Mesh::Attributes::POINT_NORMALS |
                       ::fwData::Mesh::Attributes::POINT_COLORS |
                       ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
                       ::fwData::Mesh::Attributes::CELL_NORMALS |
                       ::fwData::Mesh::Attributes::CELL_COLORS |
                       ::fwData::Mesh::Attributes::CELL_TEX_COORDS);

    {
        //fill the mesh
        auto it        = mesh->begin< ::fwData::Mesh::PointIterator >();
        const auto end = mesh->end< ::fwData::Mesh::PointIterator >();

        auto itCell = mesh->begin< ::fwData::Mesh::CellIterator >();

        size_t count = 0;
        for (; it != end; ++it, ++itCell)
        {
            // point
            it->point->x = static_cast<float>(count);
            it->point->y = static_cast<float>(count) + 0.6f;
            it->point->z = static_cast<float>(count) + 0.8f;

            it->normal->nx = static_cast<float>(count) + 0.1f;
            it->normal->ny = static_cast<float>(count) + 0.2f;
            it->normal->nz = static_cast<float>(count) + 0.3f;

            it->rgba->r = static_cast<std::uint8_t>(4*count);
            it->rgba->g = static_cast<std::uint8_t>(4*count+1);
            it->rgba->b = static_cast<std::uint8_t>(4*count+2);
            it->rgba->a = static_cast<std::uint8_t>(4*count+3);

            it->tex->u = static_cast<float>(count) + 0.7f;
            it->tex->v = static_cast<float>(count) + 0.9f;

            // cell
            *itCell->type     = ::fwData::Mesh::CellType::POINT;
            *itCell->offset   = count;
            *itCell->pointIdx = count;

            itCell->normal->nx = static_cast<float>(count) + 0.11f;
            itCell->normal->ny = static_cast<float>(count) + 0.22f;
            itCell->normal->nz = static_cast<float>(count) + 0.33f;

            itCell->rgba->r = static_cast<std::uint8_t>(4*count+1);
            itCell->rgba->g = static_cast<std::uint8_t>(4*count+2);
            itCell->rgba->b = static_cast<std::uint8_t>(4*count+3);
            itCell->rgba->a = static_cast<std::uint8_t>(4*count+4);

            itCell->tex->u = static_cast<float>(count) + 0.77f;
            itCell->tex->v = static_cast<float>(count) + 0.99f;

            if (it+1 != end)
            {
                *(itCell+1)->offset = count+1;
            }

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL( mesh->getNumberOfPoints(), count);
    }

    {
        // check the mesh points
        auto it        = mesh->begin< ::fwData::Mesh::ConstPointIterator >();
        const auto end = mesh->end< ::fwData::Mesh::ConstPointIterator >();
        auto itCell    = mesh->begin< ::fwData::Mesh::ConstCellIterator >();

        size_t count = 0;
        for (; it != end; ++it, ++itCell)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count), it->point->x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.6f, it->point->y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.8f, it->point->z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.1f, it->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.2f, it->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.3f, it->normal->nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count), it->rgba->r);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+1), it->rgba->g);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+2), it->rgba->b);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+3), it->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.7, it->tex->u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.9, it->tex->v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(::fwData::Mesh::CellType::POINT, *itCell->type);
            CPPUNIT_ASSERT_EQUAL(count, *itCell->offset);
            CPPUNIT_ASSERT_EQUAL(count, *itCell->pointIdx);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.11f, itCell->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.22f, itCell->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.33f, itCell->normal->nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+1), itCell->rgba->r);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+2), itCell->rgba->g);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+3), itCell->rgba->b);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4*count+4), itCell->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.77, itCell->tex->u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.99, itCell->tex->v, 0.00001);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL( mesh->getNumberOfPoints(), count);
    }

    {
        // copy the mesh points with the iterator
        auto it        = mesh->begin< ::fwData::Mesh::ConstPointIterator >();
        const auto end = mesh->end< ::fwData::Mesh::ConstPointIterator >();
        auto copiedIt  = copiedMesh->begin< ::fwData::Mesh::PointIterator >();

        std::copy(it, end, copiedIt);

        // copy the mesh cells with the iterator
        auto itCell        = mesh->begin< ::fwData::Mesh::ConstCellIterator >();
        const auto endCell = mesh->end< ::fwData::Mesh::ConstCellIterator >();
        auto copiedItCell  = copiedMesh->begin< ::fwData::Mesh::CellIterator >();

        std::copy(itCell, endCell, copiedItCell);
    }

    {
        // check the copied mesh
        auto it        = mesh->begin< ::fwData::iterator::ConstPointIterator >();
        const auto end = mesh->end< ::fwData::iterator::ConstPointIterator >();
        auto copiedIt  = copiedMesh->begin< ::fwData::iterator::ConstPointIterator >();

        CPPUNIT_ASSERT(std::equal(it, end, copiedIt));

        auto itCell        = mesh->begin< ::fwData::iterator::ConstCellIterator >();
        const auto endCell = mesh->end< ::fwData::iterator::ConstCellIterator >();
        auto copiedItCell  = copiedMesh->begin< ::fwData::iterator::ConstCellIterator >();

        CPPUNIT_ASSERT(std::equal(itCell, endCell, copiedItCell));

        size_t count = 0;
        for (; it != end; ++it, ++copiedIt, ++itCell, ++copiedItCell, ++count)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->point->x, copiedIt->point->x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->point->y, copiedIt->point->y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->point->z, copiedIt->point->z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->normal->nx, copiedIt->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->normal->ny, copiedIt->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->normal->nz, copiedIt->normal->nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), it->rgba->r, copiedIt->rgba->r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), it->rgba->g, copiedIt->rgba->g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), it->rgba->b, copiedIt->rgba->b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), it->rgba->a, copiedIt->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->tex->u, copiedIt->tex->u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), it->tex->v, copiedIt->tex->v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(*itCell->type, *copiedItCell->type);
            CPPUNIT_ASSERT_EQUAL(*itCell->offset, *copiedItCell->offset);
            CPPUNIT_ASSERT_EQUAL(*itCell->pointIdx, *copiedItCell->pointIdx);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(itCell->normal->nx, copiedItCell->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(itCell->normal->ny, copiedItCell->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(itCell->normal->nz, copiedItCell->normal->nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(itCell->rgba->r, copiedItCell->rgba->r);
            CPPUNIT_ASSERT_EQUAL(itCell->rgba->g, copiedItCell->rgba->g);
            CPPUNIT_ASSERT_EQUAL(itCell->rgba->b, copiedItCell->rgba->b);
            CPPUNIT_ASSERT_EQUAL(itCell->rgba->a, copiedItCell->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(itCell->tex->u, copiedItCell->tex->u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(itCell->tex->v, copiedItCell->tex->v, 0.00001);
        }
        CPPUNIT_ASSERT_EQUAL( mesh->getNumberOfPoints(), count);
    }

    {
        // check the mesh points are deep copied and not shallow copied

        auto it        = mesh->begin< ::fwData::Mesh::PointIterator >();
        const auto end = mesh->end< ::fwData::Mesh::PointIterator >();

        auto itCell    = mesh->begin< ::fwData::Mesh::CellIterator >();
        auto itCellEnd = mesh->end< ::fwData::Mesh::CellIterator >();

        ::fwData::iterator::PointIterator::PointInfo pointInfo;
        pointInfo.point  = new ::fwData::iterator::Point({1.0f, 1.1f, 1.2f});
        pointInfo.normal = new ::fwData::iterator::Normal({1.f, 0.f, 0.f});
        pointInfo.rgba   = new ::fwData::iterator::RGBA({25, 15, 18, 32});
        pointInfo.tex    = new ::fwData::iterator::TexCoords({0.5f, 1.f});
        std::fill(it, end, pointInfo);

        ::fwData::iterator::CellIterator::CellInfo cellInfo;
        cellInfo.type     = new ::fwData::Mesh::CellType{::fwData::Mesh::CellType::NO_CELL};
        cellInfo.offset   = new ::fwData::Mesh::CellDataOffsetType{0};
        cellInfo.pointIdx = new ::fwData::Mesh::CellValueType{0};
        cellInfo.normal   = new ::fwData::iterator::Normal({0.f, 1.f, 0.f});
        cellInfo.rgba     = new ::fwData::iterator::RGBA({20, 13, 10, 37});
        cellInfo.tex      = new ::fwData::iterator::TexCoords({0.2f, 0.8f});
        std::fill(itCell, itCellEnd, cellInfo);

        // check the mesh points are filled
        size_t count = 0;
        for (; it != end; ++it, ++itCell)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.point->x, it->point->x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.point->y, it->point->y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.point->z, it->point->z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.normal->nx, it->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.normal->ny, it->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.normal->nz, it->normal->nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(pointInfo.rgba->r, it->rgba->r);
            CPPUNIT_ASSERT_EQUAL(pointInfo.rgba->g, it->rgba->g);
            CPPUNIT_ASSERT_EQUAL(pointInfo.rgba->b, it->rgba->b);
            CPPUNIT_ASSERT_EQUAL(pointInfo.rgba->a, it->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.tex->u, it->tex->u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointInfo.tex->v, it->tex->v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(*cellInfo.type, *itCell->type);
            CPPUNIT_ASSERT_EQUAL(*cellInfo.offset, *itCell->offset);
            CPPUNIT_ASSERT_EQUAL(*cellInfo.pointIdx, *itCell->pointIdx);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellInfo.normal->nx, itCell->normal->nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellInfo.normal->ny, itCell->normal->ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellInfo.normal->nz, itCell->normal->nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(cellInfo.rgba->r, itCell->rgba->r);
            CPPUNIT_ASSERT_EQUAL(cellInfo.rgba->g, itCell->rgba->g);
            CPPUNIT_ASSERT_EQUAL(cellInfo.rgba->b, itCell->rgba->b);
            CPPUNIT_ASSERT_EQUAL(cellInfo.rgba->a, itCell->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellInfo.tex->u, itCell->tex->u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cellInfo.tex->v, itCell->tex->v, 0.00001);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL( mesh->getNumberOfPoints(), count);

        auto copiedIt        = copiedMesh->begin< ::fwData::Mesh::ConstPointIterator >();
        const auto copiedEnd = copiedMesh->end< ::fwData::Mesh::ConstPointIterator >();
        auto copiedItCell    = copiedMesh->begin< ::fwData::Mesh::ConstCellIterator >();

        auto it2        = mesh->begin< ::fwData::Mesh::ConstPointIterator >();
        const auto end2 = mesh->end< ::fwData::Mesh::ConstPointIterator >();

        CPPUNIT_ASSERT(!std::equal(it2, end2, copiedIt));
        auto itCell2    = mesh->begin< ::fwData::Mesh::ConstCellIterator >();
        auto itCell2End = mesh->end< ::fwData::Mesh::ConstCellIterator >();
        CPPUNIT_ASSERT(!std::equal(itCell2, itCell2End, copiedItCell));

        count = 0;
        for (; copiedIt != copiedEnd; ++copiedIt, ++copiedItCell)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count), copiedIt->point->x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.6f, copiedIt->point->y,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.8f, copiedIt->point->z,
                                                 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.1f, copiedIt->normal->nx,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.2f, copiedIt->normal->ny,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.3f, copiedIt->normal->nz,
                                                 0.00001);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count), copiedIt->rgba->r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+1), copiedIt->rgba->g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+2), copiedIt->rgba->b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+3), copiedIt->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.7, copiedIt->tex->u,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.9, copiedIt->tex->v,
                                                 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), ::fwData::Mesh::CellType::POINT, *copiedItCell->type);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), count, *copiedItCell->offset);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), count, *copiedItCell->pointIdx);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.11f, copiedItCell->normal->nx,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.22f, copiedItCell->normal->ny,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.33f, copiedItCell->normal->nz,
                                                 0.00001);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+1), copiedItCell->rgba->r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+2), copiedItCell->rgba->g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+3), copiedItCell->rgba->b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint8_t>(4*count+4), copiedItCell->rgba->a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.77, copiedItCell->tex->u,
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("iteration: " + std::to_string(
                                                     count), static_cast<float>(count) + 0.99, copiedItCell->tex->v,
                                                 0.00001);

            ++count;
        }
        CPPUNIT_ASSERT_EQUAL( mesh->getNumberOfPoints(), count);

        delete pointInfo.point;
        delete pointInfo.normal;
        delete pointInfo.rgba;
        delete pointInfo.tex;

        delete cellInfo.type;
        delete cellInfo.offset;
        delete cellInfo.pointIdx;
        delete cellInfo.normal;
        delete cellInfo.rgba;
        delete cellInfo.tex;
    }
}

} //namespace ut
} //namespace fwData
