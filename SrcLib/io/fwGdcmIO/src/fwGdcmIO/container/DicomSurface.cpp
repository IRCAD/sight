/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/container/DicomSurface.hpp"

#include "fwGdcmIO/exception/Failed.hpp"

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/Mesh.hpp>

namespace fwGdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

struct CellDataOffsetGenerator
{
    ::fwData::Mesh::CellDataOffsetType current;
    CellDataOffsetGenerator() :
        current(0)
    {
    }

    //------------------------------------------------------------------------------

    ::fwData::Mesh::CellDataOffsetType operator()()
    {
        ::fwData::Mesh::CellDataOffsetType res = current;
        current += 3;
        return res;
    }
};

//------------------------------------------------------------------------------
DicomSurface::DicomSurface(const ::fwData::Reconstruction::csptr& reconstruction)
{
    // Get mesh
    ::fwData::Mesh::csptr mesh = reconstruction->getMesh();
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Can't save this mesh. It must contain only triangles !"),
                          !::fwDataTools::Mesh::hasUniqueCellType(mesh, ::fwData::Mesh::TRIANGLE));

    // Coordinates
    {
        // Retrieve & copy data
        ::fwDataTools::helper::Array pointsArrayHelper(mesh->getPointsArray());
        m_pointBuffer.reserve(mesh->getNumberOfPoints() * 3);
        m_pointBuffer.assign(pointsArrayHelper.begin< ::fwData::Mesh::PointValueType >(),
                             pointsArrayHelper.end< ::fwData::Mesh::PointValueType >());
    }

    // Cells
    {
        // Retrieve & copy data
        ::fwDataTools::helper::Array cellDataHelper(mesh->getCellDataArray());
        m_cellBuffer.resize(mesh->getNumberOfCells() * 3);

        std::size_t index = 0;
        for(auto cellIt = cellDataHelper.begin< ::fwData::Mesh::CellValueType >();
            cellIt != cellDataHelper.end< ::fwData::Mesh::CellValueType >();
            ++cellIt)
        {
            // Index shall start at 1 in DICOM
            m_cellBuffer[index++] = static_cast< DicomCellValueType >(*cellIt) + 1;
        }
    }

    // Normals
    if (mesh->getPointNormalsArray())
    {
        // Retrieve & copy data
        ::fwDataTools::helper::Array normalsArrayHelper(mesh->getPointNormalsArray());
        m_normalBuffer.reserve(mesh->getNumberOfPoints() * 3);
        m_normalBuffer.assign(normalsArrayHelper.begin< ::fwData::Mesh::NormalValueType >(),
                              normalsArrayHelper.end< ::fwData::Mesh::NormalValueType >());
    }

}

//------------------------------------------------------------------------------

DicomSurface::DicomSurface(const ::fwData::Mesh::PointValueType* pointBuffer,
                           const ::fwData::Mesh::Id pointBufferSize,
                           const DicomCellValueType* cellBuffer,
                           const ::fwData::Mesh::Id cellBufferSize,
                           const ::fwData::Mesh::NormalValueType* normalBuffer)
{
    // Coordinates
    m_pointBuffer.reserve(pointBufferSize);
    m_pointBuffer.assign(pointBuffer, pointBuffer + pointBufferSize);

    // Cells
    m_cellBuffer.reserve(cellBufferSize);
    m_cellBuffer.assign(cellBuffer, cellBuffer + cellBufferSize);

    // Normals
    if(normalBuffer)
    {
        m_normalBuffer.reserve(pointBufferSize);
        m_normalBuffer.assign(normalBuffer, normalBuffer + pointBufferSize);
    }
}

//------------------------------------------------------------------------------

DicomSurface::~DicomSurface()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::fwData::Mesh::sptr DicomSurface::convertToData()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    // Initialize number of points
    mesh->setNumberOfPoints(m_pointBuffer.size() / 3);
    mesh->setNumberOfCells(m_cellBuffer.size() / 3);
    mesh->setCellDataSize(m_cellBuffer.size());
    mesh->adjustAllocatedMemory();

    // Coordinates
    {
        ::fwDataTools::helper::Array pointsArrayHelper(mesh->getPointsArray());
        std::copy(m_pointBuffer.begin(),
                  m_pointBuffer.end(),
                  pointsArrayHelper.begin< ::fwData::Mesh::PointValueType >());
    }

    // Cells
    {
        ::fwDataTools::helper::Array cellDataHelper(mesh->getCellDataArray());

        std::size_t index = 0;
        for(auto cellIt = cellDataHelper.begin< ::fwData::Mesh::CellValueType >();
            cellIt != cellDataHelper.end< ::fwData::Mesh::CellValueType >();
            ++cellIt)
        {
            // Index shall start at 0 in Sight
            *cellIt = static_cast< ::fwData::Mesh::CellValueType >(m_cellBuffer[index++]) - 1;
        }
    }

    // Normals
    if(!m_normalBuffer.empty())
    {
        mesh->allocatePointNormals();
        ::fwDataTools::helper::Array normalsArrayHelper(mesh->getPointNormalsArray());
        std::copy(m_normalBuffer.begin(),
                  m_normalBuffer.end(),
                  normalsArrayHelper.begin< ::fwData::Mesh::NormalValueType >());
    }

    // Cells types
    ::fwDataTools::helper::Array cellTypesHelper(mesh->getCellTypesArray());
    std::fill(cellTypesHelper.begin< ::fwData::Mesh::CellTypes >(),
              cellTypesHelper.end< ::fwData::Mesh::CellTypes >(),
              static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::TRIANGLE));

    // Cell data offset
    ::fwDataTools::helper::Array cellDataOffsetsHelper(mesh->getCellDataOffsetsArray());
    CellDataOffsetGenerator cellDataOffsetGenerator;
    std::generate(cellDataOffsetsHelper.begin< ::fwData::Mesh::CellDataOffsetType >(),
                  cellDataOffsetsHelper.end< ::fwData::Mesh::CellDataOffsetType >(),
                  cellDataOffsetGenerator);

    return mesh;

}

//------------------------------------------------------------------------------

const DicomSurface::DicomPointBufferType& DicomSurface::getPointBuffer() const
{
    return m_pointBuffer;
}

//------------------------------------------------------------------------------

const DicomSurface::DicomCellBufferType& DicomSurface::getCellBuffer() const
{
    return m_cellBuffer;
}

//------------------------------------------------------------------------------

const DicomSurface::DicomNormalBufferType& DicomSurface::getNormalBuffer() const
{
    return m_normalBuffer;
}

//------------------------------------------------------------------------------

std::size_t DicomSurface::getPointBufferSize() const
{
    return m_pointBuffer.size();
}

//------------------------------------------------------------------------------

std::size_t DicomSurface::getCellBufferSize() const
{
    return m_cellBuffer.size();
}

//------------------------------------------------------------------------------

std::size_t DicomSurface::getNormalBufferSize() const
{
    return m_normalBuffer.size();
}

//------------------------------------------------------------------------------

} //namespace container
} //namespace fwGdcmIO
