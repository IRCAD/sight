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

#include "fwGdcmIO/container/DicomSurface.hpp"

#include "fwGdcmIO/exception/Failed.hpp"

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
                          !::fwDataTools::Mesh::hasUniqueCellType(mesh, ::fwData::Mesh::CellType::TRIANGLE));
    auto itr       = mesh->begin< ::fwData::iterator::ConstPointIterator >();
    const auto end = mesh->end< ::fwData::iterator::ConstPointIterator >();
    // Coordinates
    {
        // Retrieve & copy data
        m_pointBuffer.reserve(mesh->getNumberOfPoints() * 3);
        m_pointBuffer.assign(reinterpret_cast<const float*>(itr->point), reinterpret_cast<const float*>(end->point));
    }

    // Cells
    {
        // Retrieve & copy data
        m_cellBuffer.resize(mesh->getNumberOfCells() * 3);
        auto cellIt        = mesh->begin< ::fwData::iterator::ConstCellIterator >();
        const auto cellEnd = mesh->end< ::fwData::iterator::ConstCellIterator >();

        std::size_t index = 0;
        for(; cellIt != cellEnd; ++cellIt)
        {
            // Index shall start at 1 in DICOM
            m_cellBuffer[index++] = static_cast< DicomCellValueType >(cellIt->pointIdx[0]) + 1;
            m_cellBuffer[index++] = static_cast< DicomCellValueType >(cellIt->pointIdx[1]) + 1;
            m_cellBuffer[index++] = static_cast< DicomCellValueType >(cellIt->pointIdx[2]) + 1;
        }
    }

    // Normals
    if (mesh->hasPointNormals())
    {
        // Retrieve & copy data
        m_normalBuffer.reserve(mesh->getNumberOfPoints() * 3);
        m_normalBuffer.assign(reinterpret_cast<const float*>(itr->normal), reinterpret_cast<const float*>(end->normal));
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
    ::fwData::Mesh::Attributes attribute = ::fwData::Mesh::Attributes::NONE;
    if( !m_normalBuffer.empty())
    {
        attribute = ::fwData::Mesh::Attributes::POINT_NORMALS;
    }

    mesh->resize(m_pointBuffer.size() / 3, m_cellBuffer.size() / 3, ::fwData::Mesh::CellType::TRIANGLE, attribute);

    // Coordinates
    {
        auto itr = mesh->begin< ::fwData::iterator::PointIterator >();

        std::copy(m_pointBuffer.begin(),
                  m_pointBuffer.end(),
                  reinterpret_cast<float*>(itr->point));
    }

    // Cells
    {
        auto itr       = mesh->begin< ::fwData::iterator::CellIterator >();
        const auto end = mesh->end< ::fwData::iterator::CellIterator >();

        // Cells types
        std::fill(itr->type,
                  end->type,
                  static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::CellType::TRIANGLE));

        // Cell data offset
        CellDataOffsetGenerator cellDataOffsetGenerator;
        std::generate(itr->offset,
                      end->offset,
                      cellDataOffsetGenerator);

        for(size_t index = 0; index != m_cellBuffer.size(); ++index)
        {
            // Index shall start at 0 in Sight
            itr->pointIdx[index] = static_cast< ::fwData::Mesh::CellValueType >(m_cellBuffer[index]) - 1;
        }
    }

    // Normals
    if(!m_normalBuffer.empty())
    {
        auto itr = mesh->begin< ::fwData::iterator::PointIterator >();

        std::copy(m_normalBuffer.begin(),
                  m_normalBuffer.end(),
                  reinterpret_cast<float*>(itr->normal));
    }

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
