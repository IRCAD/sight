/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/cstdint.hpp>
#include <fwData/Mesh.hpp>
#include "MeshTest.hpp"


using namespace boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MeshTest );

namespace fwData
{
namespace ut
{

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

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointsArray()->getNumberOfComponents());
    size_t pointAllocatedSize = pointSize * mesh->getPointsArray()->getNumberOfComponents() * mesh->getPointsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize, mesh->getPointsArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(1), mesh->getCellTypesArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellTypesArray()->getNumberOfComponents());
    size_t cellTypeAllocatedSize = cellSize * mesh->getCellTypesArray()->getNumberOfComponents() * mesh->getCellTypesArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellSize, mesh->getCellTypesArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(8), mesh->getCellDataOffsetsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellDataOffsetsArray()->getNumberOfComponents());
    size_t cellDataOffsetsAllocatedSize = cellSize * mesh->getCellDataOffsetsArray()->getNumberOfComponents() * mesh->getCellDataOffsetsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellDataOffsetsAllocatedSize, mesh->getCellDataOffsetsArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(8), mesh->getCellDataArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellDataArray()->getNumberOfComponents());
    size_t cellDataAllocatedSize = cellDataSize * mesh->getCellDataArray()->getNumberOfComponents() * mesh->getCellDataArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellDataAllocatedSize, mesh->getCellDataArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize + cellSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize , mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfCells());

    mesh->insertNextPoint(10, 20, 30);
    mesh->insertNextPoint(10, 10, 10);
    mesh->insertNextPoint(20, 20, 10);

    mesh->insertNextCell(1, 2, 3);
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)3, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)1, mesh->getNumberOfCells());

    mesh->allocatePointNormals();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointNormalsArray()->getSize()[0]);
    mesh->allocatePointColors(::fwData::Mesh::RGB);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointColorsArray()->getSize()[0]);

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointNormalsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointNormalsArray()->getNumberOfComponents());
    size_t pointNormalsAllocatedSize = mesh->getNumberOfPoints() * mesh->getPointNormalsArray()->getNumberOfComponents() * mesh->getPointNormalsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointNormalsAllocatedSize, mesh->getPointNormalsArray()->getSizeInBytes());

    size_t pointColorsAllocatedSize = mesh->getNumberOfPoints() * mesh->getPointColorsArray()->getNumberOfComponents() * mesh->getPointColorsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize + cellSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize + pointNormalsAllocatedSize + pointColorsAllocatedSize, mesh->getAllocatedSizeInBytes());

    mesh->allocateCellNormals();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellNormalsArray()->getSize()[0]);
    mesh->allocateCellColors(::fwData::Mesh::RGBA);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellColorsArray()->getSize()[0]);

    size_t oldsize = mesh->getAllocatedSizeInBytes();
    bool adjusted = mesh->adjustAllocatedMemory();
    size_t newSize = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > newSize);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()) , mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getCellDataSize()  ), mesh->getCellDataArray()->getSize()[0]);

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, mesh->getDataSizeInBytes());

    ::fwData::Mesh::Id nbPoints = 300;
    ::fwData::Mesh::Id nbCells = 200;
    ::fwData::Mesh::Id newCellDataSize = 500;
    mesh->setNumberOfPoints(nbPoints);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(newCellDataSize);
    oldsize = mesh->getAllocatedSizeInBytes();
    adjusted = mesh->adjustAllocatedMemory();
    newSize = mesh->getAllocatedSizeInBytes();
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

void MeshTest::insertion()
{
    ::fwData::Mesh::NewSptr mesh;
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

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    mesh->insertNextCell(::fwData::Mesh::POLY, p3, 5);

    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)8, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)6, mesh->getNumberOfCells());


    ::fwData::Mesh::PointsMultiArrayType pointArray = mesh->getPoints();
    CPPUNIT_ASSERT_EQUAL((float) 10, pointArray[0][0]);
    CPPUNIT_ASSERT_EQUAL((float) 20, pointArray[2][1]);
    CPPUNIT_ASSERT_EQUAL((float) 52, pointArray[7][2]);

    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsetArray = mesh->getCellDataOffsets();
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)0, cellDataOffsetArray[0]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)2, cellDataOffsetArray[1]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)9, cellDataOffsetArray[3]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)13, cellDataOffsetArray[4]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)19, cellDataOffsetArray[5]);

    ::fwData::Mesh::CellDataMultiArrayType cellDataArray = mesh->getCellData();
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)1, cellDataArray[cellDataOffsetArray[0]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)1, cellDataArray[cellDataOffsetArray[1]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)3, cellDataArray[cellDataOffsetArray[3]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)1, cellDataArray[cellDataOffsetArray[4]]);
    CPPUNIT_ASSERT_EQUAL( (::boost::uint64_t)7, cellDataArray[cellDataOffsetArray[5]]);

    ::fwData::Mesh::CellTypesMultiArrayType cellTypesArray = mesh->getCellTypes();
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::EDGE, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[0]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::TRIANGLE, static_cast < ::fwData::Mesh::CellTypesEnum >(cellTypesArray[1]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::QUAD, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[3]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::POLY, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[4]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::POLY, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[5]));

    mesh->setPoint(4, 45, 59, 45);
    CPPUNIT_ASSERT_EQUAL((float) 45, pointArray[4][0]);
    CPPUNIT_ASSERT_EQUAL((float) 59, pointArray[4][1]);
    CPPUNIT_ASSERT_EQUAL((float) 45, pointArray[4][2]);


    ::fwData::Array::sptr array = mesh->getPointsArray();
    CPPUNIT_ASSERT_EQUAL(*(array->getItem< ::fwData::Mesh::PointValueType >(list_of(4),0)), pointArray[4][0]);
    CPPUNIT_ASSERT_EQUAL(*(array->getItem< ::fwData::Mesh::PointValueType >(list_of(4),1)), pointArray[4][1]);
    CPPUNIT_ASSERT_EQUAL(*(array->getItem< ::fwData::Mesh::PointValueType >(list_of(4),2)), pointArray[4][2]);
}

//-----------------------------------------------------------------------------

void MeshTest::colorsNormals()
{
    ::fwData::Mesh::NewSptr mesh;
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

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    mesh->insertNextCell(::fwData::Mesh::POLY, p3, 5);

    mesh->allocatePointColors(::fwData::Mesh::RGBA);
    ::fwData::Mesh::PointColorsMultiArrayType pointColorArray = mesh->getPointColors();

    ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
    for (int id=0 ; id< nbPoints ; id++)
    {
        for (int c=0 ; c < 4 ; c++)
        {
            pointColorArray[id][c] = id * 10 + c;
        }
    }
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >( 0), pointColorArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(21), pointColorArray[2][1]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(50), pointColorArray[5][0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(73), pointColorArray[7][3]);

    ::fwData::Mesh::ColorValueType color[4] = {5, 8, 6, 10};
    mesh->setPointColor(5, color);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(8), pointColorArray[5][1]);

    mesh->allocateCellNormals();
    ::fwData::Mesh::CellNormalsMultiArrayType cellNormalArray = mesh->getCellNormals();

    ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
    for (int id=0 ; id< nbCells ; id++)
    {
        for (int n=0 ; n < 3 ; n++)
        {
            cellNormalArray[id][n] = id + n/10.f;
        }
    }
    CPPUNIT_ASSERT_EQUAL(0.0f, cellNormalArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(2.1f, cellNormalArray[2][1]);
    CPPUNIT_ASSERT_EQUAL(4.2f, cellNormalArray[4][2]);
    CPPUNIT_ASSERT_EQUAL(5.0f, cellNormalArray[5][0]);

    ::fwData::Mesh::NormalValueType normal[3] = {0.9f, 0.4f, 0.2f};
    mesh->setCellNormal(4, normal);
    CPPUNIT_ASSERT_EQUAL(0.2f, cellNormalArray[4][2]);
}

//-----------------------------------------------------------------------------

void MeshTest::addingArray()
{
    ::fwData::Mesh::NewSptr mesh;

    ::fwData::Mesh::Id nbPoints = 60;
    ::fwData::Mesh::Id nbCells = 20;

    // Add point array
    ::fwData::Array::NewSptr pointArray;

    pointArray->resize(::fwTools::Type::create< ::fwData::Mesh::PointValueType >() , list_of(nbPoints), 3, true);

    ::fwData::Mesh::PointValueType count = 0;
    ::fwData::Mesh::PointValueType *iter = pointArray->begin< ::fwData::Mesh::PointValueType >();
    for (; iter != pointArray->end< ::fwData::Mesh::PointValueType >() ; ++iter)
    {
        for (int i=0 ; i<3 ; i++)
        {
            *iter = count + i/10.0f;
        }
        count++;
    }

    mesh->setPointsArray(pointArray);
    mesh->setNumberOfPoints(nbPoints);

    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());

    ::fwData::Mesh::Id id = mesh->insertNextPoint(50, 50, 50);
    CPPUNIT_ASSERT_EQUAL(nbPoints, id);
    nbPoints++;
    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());

    ::fwData::Mesh::PointsMultiArrayType pointMultiArray = mesh->getPoints();
    CPPUNIT_ASSERT_EQUAL(*(pointArray->getItem< ::fwData::Mesh::PointValueType >(list_of(0),0)), pointMultiArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(*(pointArray->getItem< ::fwData::Mesh::PointValueType >(list_of(12),2)), pointMultiArray[12][2]);
    CPPUNIT_ASSERT_EQUAL(*(pointArray->getItem< ::fwData::Mesh::PointValueType >(list_of(45),1)), pointMultiArray[45][1]);


    // add cells arrays
    ::fwData::Array::NewSptr cellTypeArray;
    ::fwData::Array::NewSptr cellDataOffsetArray;
    ::fwData::Array::NewSptr cellDataArray;


    cellTypeArray->resize(::fwTools::Type::create< ::fwData::Mesh::CellTypes >() , list_of(nbCells), 1, true);
    cellDataOffsetArray->resize(::fwTools::Type::create< ::fwData::Mesh::CellDataOffsetType >() , list_of(nbCells), 1, true);
    cellDataArray->resize(::fwTools::Type::create< ::fwData::Mesh::CellValueType >() , list_of(nbCells*3), 1, true);

    ::fwData::Mesh::CellValueType counter = 0;
    for (int id = 0 ; id<nbCells ; id++)
    {
        ::fwData::Mesh::CellTypes type = static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::TRIANGLE);
        cellTypeArray->setItem(list_of(id), &type);

        ::fwData::Mesh::CellDataOffsetType offset = id*3;
        cellDataOffsetArray->setItem(list_of(id), &offset);

        ::fwData::Mesh::CellValueType cell[3] = { counter, counter+1, counter+2 };
        counter += 3;
        cellDataArray->setItem(list_of(offset), &cell);
    }
    mesh->setCellTypesArray(cellTypeArray);
    mesh->setCellDataOffsetsArray(cellDataOffsetArray);
    mesh->setCellDataArray(cellDataArray);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(nbCells*3);

    mesh->insertNextCell(1, 6, 25, 12 );

    nbCells++;
    CPPUNIT_ASSERT_EQUAL(nbCells, mesh->getNumberOfCells());

    ::fwData::Mesh::CellTypesMultiArrayType cellTypesMultiArray = mesh->getCellTypes();
    CPPUNIT_ASSERT_EQUAL(*(cellTypeArray->getItem< ::fwData::Mesh::CellTypes >(list_of(0))) , cellTypesMultiArray[0]);
    CPPUNIT_ASSERT_EQUAL(*(cellTypeArray->getItem< ::fwData::Mesh::CellTypes >(list_of(12))), cellTypesMultiArray[12]);
    CPPUNIT_ASSERT_EQUAL(*(cellTypeArray->getItem< ::fwData::Mesh::CellTypes >(list_of(18))), cellTypesMultiArray[18]);

    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsetsMultiArray = mesh->getCellDataOffsets();
    CPPUNIT_ASSERT_EQUAL(*(cellDataOffsetArray->getItem< ::fwData::Mesh::CellDataOffsetType >(list_of(0))) , cellDataOffsetsMultiArray[0]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataOffsetArray->getItem< ::fwData::Mesh::CellDataOffsetType >(list_of(12))), cellDataOffsetsMultiArray[12]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataOffsetArray->getItem< ::fwData::Mesh::CellDataOffsetType >(list_of(18))), cellDataOffsetsMultiArray[18]);


    ::fwData::Mesh::CellDataOffsetType offset0  = cellTypesMultiArray[0] ;
    ::fwData::Mesh::CellDataOffsetType offset12 = cellTypesMultiArray[12];
    ::fwData::Mesh::CellDataOffsetType offset18 = cellTypesMultiArray[18];

    ::fwData::Mesh::CellDataMultiArrayType cellDataMultiArray = mesh->getCellData();
    CPPUNIT_ASSERT_EQUAL(*(cellDataArray->getItem< ::fwData::Mesh::CellValueType >(list_of(offset0))) , cellDataMultiArray[offset0]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataArray->getItem< ::fwData::Mesh::CellValueType >(list_of(offset12))), cellDataMultiArray[offset12]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataArray->getItem< ::fwData::Mesh::CellValueType >(list_of(offset18))), cellDataMultiArray[offset18]);
}

//-----------------------------------------------------------------------------

void MeshTest::copy()
{
    ::fwData::Mesh::NewSptr mesh;
    ::fwData::Mesh::NewSptr deepCopyMesh;
    ::fwData::Mesh::NewSptr shallowCopyMesh;

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

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    mesh->insertNextCell(::fwData::Mesh::POLY, p3, 5);

    mesh->allocatePointColors(::fwData::Mesh::RGBA);
    ::fwData::Mesh::PointColorsMultiArrayType pointColorArray = mesh->getPointColors();

    // check deep copy
    deepCopyMesh->deepCopy(mesh);
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), deepCopyMesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells() , deepCopyMesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataSize()  , deepCopyMesh->getCellDataSize());

    CPPUNIT_ASSERT(mesh->getPointsArray()->getSize()          == deepCopyMesh->getPointsArray()->getSize());
    CPPUNIT_ASSERT(mesh->getCellTypesArray()->getSize()       == deepCopyMesh->getCellTypesArray()->getSize());
    CPPUNIT_ASSERT(mesh->getCellDataOffsetsArray()->getSize() == deepCopyMesh->getCellDataOffsetsArray()->getSize());
    CPPUNIT_ASSERT(mesh->getCellDataArray()->getSize()        == deepCopyMesh->getCellDataArray()->getSize());
    CPPUNIT_ASSERT(mesh->getPointColorsArray()->getSize()     == deepCopyMesh->getPointColorsArray()->getSize());

    ::fwData::Mesh::PointsMultiArrayType meshPointArray = mesh->getPoints();
    ::fwData::Mesh::PointsMultiArrayType deepCopyMeshPointArray = deepCopyMesh->getPoints();

    for (int i=0 ; i< mesh->getNumberOfPoints() ; i++)
    {
        for (int j=0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL(meshPointArray[i][j], deepCopyMeshPointArray[i][j]);
        }
    }

    //check shallow copy
    shallowCopyMesh->shallowCopy(mesh);
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), shallowCopyMesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells() , shallowCopyMesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataSize()  , shallowCopyMesh->getCellDataSize());

    CPPUNIT_ASSERT_EQUAL(mesh->getPointsArray()         , shallowCopyMesh->getPointsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellTypesArray()      , shallowCopyMesh->getCellTypesArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataOffsetsArray(), shallowCopyMesh->getCellDataOffsetsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataArray()       , shallowCopyMesh->getCellDataArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getPointColorsArray()    , shallowCopyMesh->getPointColorsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getPointNormalsArray()   , shallowCopyMesh->getPointNormalsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellColorsArray()     , shallowCopyMesh->getCellColorsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellNormalsArray()    , shallowCopyMesh->getCellNormalsArray());
}

} //namespace ut
} //namespace fwData

