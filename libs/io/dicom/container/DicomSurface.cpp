/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/container/DicomSurface.hpp"

#include "io/dicom/exception/Failed.hpp"

#include <geometry/data/mesh.hpp>

namespace sight::io::dicom::container
{

//------------------------------------------------------------------------------

struct CellDataOffsetGenerator
{
    data::mesh::cell_t current {0};
    CellDataOffsetGenerator()
    = default;

    //------------------------------------------------------------------------------

    data::mesh::cell_t operator()()
    {
        data::mesh::cell_t res = current;
        current += 3;
        return res;
    }
};

//------------------------------------------------------------------------------
DicomSurface::DicomSurface(const data::reconstruction::csptr& _reconstruction)
{
    // Get mesh
    data::mesh::csptr mesh = _reconstruction->getMesh();

    // Coordinates
    {
        const auto begin = mesh->cbegin<data::iterator::point::xyz>();
        const auto end   = mesh->cend<data::iterator::point::xyz>();

        // Retrieve & copy data
        m_pointBuffer.reserve(mesh->numPoints() * std::size_t(3));
        m_pointBuffer.assign(reinterpret_cast<const float*>(&*begin), reinterpret_cast<const float*>(&*end));
    }

    // Cells
    {
        // Retrieve & copy data
        m_cellBuffer.resize(mesh->numCells() * std::size_t(3));

        std::size_t index = 0;
        for(const auto& cell : mesh->crange<data::iterator::cell::triangle>())
        {
            // Index shall start at 1 in DICOM
            m_cellBuffer[index++] = static_cast<dicom_cell_value_t>(cell.pt[0]) + 1;
            m_cellBuffer[index++] = static_cast<dicom_cell_value_t>(cell.pt[1]) + 1;
            m_cellBuffer[index++] = static_cast<dicom_cell_value_t>(cell.pt[2]) + 1;
        }
    }

    // Normals
    if(mesh->has<data::mesh::Attributes::POINT_NORMALS>())
    {
        const auto begin = mesh->cbegin<data::iterator::point::nxyz>();
        const auto end   = mesh->cend<data::iterator::point::nxyz>();

        // Retrieve & copy data
        m_normalBuffer.reserve(mesh->numPoints() * std::size_t(3));
        m_normalBuffer.assign(reinterpret_cast<const float*>(&*begin), reinterpret_cast<const float*>(&*end));
    }
}

//------------------------------------------------------------------------------

DicomSurface::DicomSurface(
    const data::mesh::position_t* _point_buffer,
    const data::mesh::size_t _point_buffer_size,
    const dicom_cell_value_t* _cell_buffer,
    const data::mesh::size_t _cell_buffer_size,
    const data::mesh::normal_t* _normal_buffer
)
{
    // Coordinates
    m_pointBuffer.reserve(_point_buffer_size);
    m_pointBuffer.assign(_point_buffer, _point_buffer + _point_buffer_size);

    // Cells
    m_cellBuffer.reserve(_cell_buffer_size);
    m_cellBuffer.assign(_cell_buffer, _cell_buffer + _cell_buffer_size);

    // Normals
    if(_normal_buffer != nullptr)
    {
        m_normalBuffer.reserve(_point_buffer_size);
        m_normalBuffer.assign(_normal_buffer, _normal_buffer + _point_buffer_size);
    }
}

//------------------------------------------------------------------------------

DicomSurface::~DicomSurface()
= default;

//------------------------------------------------------------------------------

data::mesh::sptr DicomSurface::convertToData()
{
    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    const auto lock       = mesh->dump_lock();

    // Initialize number of points
    data::mesh::Attributes attribute = data::mesh::Attributes::NONE;
    if(!m_normalBuffer.empty())
    {
        attribute = data::mesh::Attributes::POINT_NORMALS;
    }

    mesh->resize(
        data::mesh::size_t(m_pointBuffer.size() / 3),
        data::mesh::size_t(m_cellBuffer.size() / 3),
        data::mesh::cell_type_t::TRIANGLE,
        attribute
    );

    // Coordinates
    {
        auto itr = mesh->begin<data::iterator::point::xyz>();
        std::copy(m_pointBuffer.begin(), m_pointBuffer.end(), reinterpret_cast<float*>(&*itr));
    }

    // Cells
    {
        std::size_t index = 0;

        for(auto& cell : mesh->range<data::iterator::cell::triangle>())
        {
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                // Index shall start at 0 in Sight
                cell.pt[i] = static_cast<data::mesh::point_t>(m_cellBuffer[index]) - 1;
                ++index;
            }
        }
    }

    // Normals
    if(!m_normalBuffer.empty())
    {
        const auto itr = mesh->begin<data::iterator::point::nxyz>();
        std::copy(m_normalBuffer.begin(), m_normalBuffer.end(), reinterpret_cast<float*>(&*itr));
    }

    return mesh;
}

//------------------------------------------------------------------------------

const DicomSurface::dicom_point_buffer_t& DicomSurface::getPointBuffer() const
{
    return m_pointBuffer;
}

//------------------------------------------------------------------------------

const DicomSurface::dicom_cell_buffer_t& DicomSurface::getCellBuffer() const
{
    return m_cellBuffer;
}

//------------------------------------------------------------------------------

const DicomSurface::dicom_normal_buffer_t& DicomSurface::getNormalBuffer() const
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

} // namespace sight::io::dicom::container
