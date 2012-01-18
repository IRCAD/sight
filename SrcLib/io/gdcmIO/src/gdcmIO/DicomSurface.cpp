/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataTools/MeshGenerator.hpp>

#include "gdcmIO/DicomSurface.hpp"

namespace gdcmIO
{

//------------------------------------------------------------------------------

DicomSurface::DicomSurface():
        m_pointCoordSize(0),
        m_pointIndexSize(0)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSurface::~DicomSurface()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

const ::boost::shared_ptr<float> DicomSurface::getPointCoordData() const
{
    return this->m_pointCoordData;
}

//------------------------------------------------------------------------------

unsigned long DicomSurface::getPointCoordSize() const
{
    return this->m_pointCoordSize;
}

//------------------------------------------------------------------------------

const ::boost::shared_ptr<uint32_t> DicomSurface::getPointIndexList() const
{
    return this->m_pointIndexList;
}

//------------------------------------------------------------------------------

unsigned long DicomSurface::getPointIndexSize() const
{
    return this->m_pointIndexSize;
}

//------------------------------------------------------------------------------

const ::boost::shared_ptr<float> DicomSurface::getNormalCoordData() const
{
    return this->m_normalCoordData;
}

//------------------------------------------------------------------------------

unsigned long DicomSurface::getNormalCoordSize() const
{
    return this->m_normalCoordSize;
}

//------------------------------------------------------------------------------

void DicomSurface::setPointCoordData(const ::boost::shared_ptr<float> a_array)
{
    this->m_pointCoordData = a_array;
}

//------------------------------------------------------------------------------

void DicomSurface::setPointCoordSize(const unsigned long a_size)
{
    this->m_pointCoordSize = a_size;
}

//------------------------------------------------------------------------------

void DicomSurface::setPointIndexList(const ::boost::shared_ptr<uint32_t> a_array)
{
    this->m_pointIndexList = a_array;
}

//------------------------------------------------------------------------------

void DicomSurface::setPointIndexSize(const unsigned long a_size)
{
    this->m_pointIndexSize = a_size;
}

//------------------------------------------------------------------------------

void DicomSurface::setNormalCoordData(const ::boost::shared_ptr<float> a_array)
{
    this->m_normalCoordData = a_array;
}

//------------------------------------------------------------------------------

void DicomSurface::setNormalCoordSize(const unsigned long a_size)
{
    this->m_normalCoordSize = a_size;
}

//------------------------------------------------------------------------------

void DicomSurface::setFromData(fwData::Reconstruction::csptr a_reconstruction)
{
    // Get mesh
    ::fwData::Mesh::csptr mesh = a_reconstruction->getMesh();
    FW_RAISE_IF("Can't save this Mesh. It must contain only triangle!", !::fwDataTools::MeshGenerator::hasUniqueCellType(mesh, ::fwData::Mesh::TRIANGLE));

    ::fwData::Array::sptr pointArray = mesh->getPointsArray();
    ::fwData::Mesh::PointValueType * points = pointArray->begin< ::fwData::Mesh::PointValueType >();
    ::fwData::Array::sptr cellData = mesh->getCellDataArray();
    ::fwData::Mesh::Id cellDataSize = mesh->getCellDataSize();
    ::fwData::Mesh::CellValueType* cells = cellData->begin< ::fwData::Mesh::CellValueType >();

    // Initialize members
    m_pointCoordSize    = mesh->getNumberOfPoints();
    m_pointCoordData    = ::boost::shared_ptr<float>( new float[ 3 * m_pointCoordSize ] );

    m_pointIndexSize    = mesh->getNumberOfCells();
    m_pointIndexList    = ::boost::shared_ptr<uint32_t>( new uint32_t[ 3 * m_pointIndexSize ] );

    m_normalCoordSize   = 0;

    // Traverse Mesh components
    float *     pointCoordData = m_pointCoordData.get();
    uint32_t *  pointIndexList = m_pointIndexList.get();

    memcpy(pointCoordData, points, pointArray->getSizeInBytes());

    for (unsigned long id = 0 ; id < mesh->getCellDataSize() ; ++id)
    {
        *pointIndexList = static_cast< uint32_t >(*cells);
        ++cells;
        ++pointIndexList;
    }

    if (mesh->getPointNormalsArray())
    {
        ::fwData::Array::sptr normalArray = mesh->getPointNormalsArray();
        ::fwData::Mesh::NormalValueType * normals = normalArray->begin< ::fwData::Mesh::NormalValueType >();

        m_normalCoordSize   = mesh->getNumberOfPoints();
        m_normalCoordData   = ::boost::shared_ptr<float>( new float[ 3 * m_normalCoordSize ] );

        float *normalCoordData = m_normalCoordData.get();

        memcpy(normalCoordData, normals, normalArray->getSizeInBytes());
    }
}

//------------------------------------------------------------------------------

struct cell_data_offset_generator {
        ::fwData::Mesh::CellDataOffsetType current;
        cell_data_offset_generator() {current=0;}
        int operator()() {
            ::fwData::Mesh::CellDataOffsetType res = current;
            current += 3;
            return res;
        }
} ;

//------------------------------------------------------------------------------

void DicomSurface::convertToData(fwData::Reconstruction::sptr a_reconstruction)
{
    // Get mesh
    ::fwData::Mesh::NewSptr mesh;

    // Initialize members
    const unsigned long sizePoints   = m_pointCoordSize / 3;
    const unsigned long sizeCells    = m_pointIndexSize / 3;
    mesh->setNumberOfPoints(sizePoints);
    mesh->setNumberOfCells(sizeCells);
    mesh->setCellDataSize(sizeCells*3);
    mesh->adjustAllocatedMemory();

    ::fwData::Array::sptr pointArray = mesh->getPointsArray();
    ::fwData::Array::sptr cellData = mesh->getCellDataArray();
    ::fwData::Array::sptr cellDataOffsets = mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellTypes = mesh->getCellTypesArray();

    ::fwData::Mesh::PointValueType * points = pointArray->begin< ::fwData::Mesh::PointValueType >();
    ::fwData::Mesh::CellValueType* cells = cellData->begin< ::fwData::Mesh::CellValueType >();

    // Traverse DicomSurface components
    const float *       pointCoordData = m_pointCoordData.get();
    const uint32_t *    pointIndexList = m_pointIndexList.get();

    memcpy(points, pointCoordData, pointArray->getSizeInBytes());

    for (unsigned long id = 0 ; id < sizeCells*3 ; ++id)
    {
        *cells = static_cast< ::fwData::Mesh::CellValueType >(*pointIndexList);
        ++cells;
        ++pointIndexList;
    }

    std::fill(
            cellTypes->begin< ::fwData::Mesh::CellTypes >(),
            cellTypes->end< ::fwData::Mesh::CellTypes >(),
            static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::TRIANGLE)
            );


    cell_data_offset_generator cellDataOffsetGenerator;

    std::generate(
            cellDataOffsets->begin< ::fwData::Mesh::CellDataOffsetType >(),
            cellDataOffsets->end< ::fwData::Mesh::CellDataOffsetType >(),
            cellDataOffsetGenerator
            );

    if (m_normalCoordSize > 0)
    {
        const float *normalCoordData = m_normalCoordData.get();
        mesh->allocatePointNormals();

        ::fwData::Array::sptr normalArray = mesh->getPointNormalsArray();
        ::fwData::Mesh::NormalValueType * normals = normalArray->begin< ::fwData::Mesh::NormalValueType >();

        memcpy(normals, normalCoordData, normalArray->getSizeInBytes());
    }

    a_reconstruction->setMesh(mesh);
}

//------------------------------------------------------------------------------

::fwData::Mesh::sptr DicomSurface::convertToData(const float * coord, const unsigned long coordSize,
                                                 const uint32_t * index, const unsigned long indexSize,
                                                 const float * normalCoord)
{
    ::fwData::Mesh::NewSptr mesh;

    // Initialize members
    const unsigned long sizePoints   = coordSize / 3;
    const unsigned long sizeCells    = indexSize / 3;
    mesh->setNumberOfPoints(sizePoints);
    mesh->setNumberOfCells(sizeCells);
    mesh->setCellDataSize(sizeCells*3);
    mesh->adjustAllocatedMemory();

    ::fwData::Array::sptr pointArray = mesh->getPointsArray();
    ::fwData::Array::sptr cellData = mesh->getCellDataArray();
    ::fwData::Array::sptr cellDataOffsets = mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellTypes = mesh->getCellTypesArray();

    ::fwData::Mesh::PointValueType * points = pointArray->begin< ::fwData::Mesh::PointValueType >();
    ::fwData::Mesh::CellValueType* cells = cellData->begin< ::fwData::Mesh::CellValueType >();

    memcpy(points, coord, pointArray->getSizeInBytes());

    for (unsigned long id = 0 ; id < indexSize ; ++id)
    {
        *cells = static_cast< ::fwData::Mesh::CellValueType >(*index);
        ++cells;
        ++index;
    }

    std::fill(
            cellTypes->begin< ::fwData::Mesh::CellTypes >(),
            cellTypes->end< ::fwData::Mesh::CellTypes >(),
            static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::TRIANGLE)
            );


    cell_data_offset_generator cellDataOffsetGenerator;

    std::generate(
            cellDataOffsets->begin< ::fwData::Mesh::CellDataOffsetType >(),
            cellDataOffsets->end< ::fwData::Mesh::CellDataOffsetType >(),
            cellDataOffsetGenerator
            );

    if (normalCoord)
    {
        mesh->allocatePointNormals();

        ::fwData::Array::sptr normalArray = mesh->getPointNormalsArray();
        ::fwData::Mesh::NormalValueType * normals = normalArray->begin< ::fwData::Mesh::NormalValueType >();

        memcpy(normals, normalCoord, normalArray->getSizeInBytes());
    }

    return mesh;
}

}
