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

struct cell_data_offset_generator
{
    data::mesh::cell_t current {0};
    cell_data_offset_generator()
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
dicom_surface::dicom_surface(const data::reconstruction::csptr& _reconstruction)
{
    // Get mesh
    data::mesh::csptr mesh = _reconstruction->get_mesh();

    // Coordinates
    {
        const auto begin = mesh->cbegin<data::iterator::point::xyz>();
        const auto end   = mesh->cend<data::iterator::point::xyz>();

        // Retrieve & copy data
        m_point_buffer.reserve(mesh->num_points() * std::size_t(3));
        m_point_buffer.assign(reinterpret_cast<const float*>(&*begin), reinterpret_cast<const float*>(&*end));
    }

    // Cells
    {
        // Retrieve & copy data
        m_cell_buffer.resize(mesh->num_cells() * std::size_t(3));

        std::size_t index = 0;
        for(const auto& cell : mesh->crange<data::iterator::cell::triangle>())
        {
            // Index shall start at 1 in DICOM
            m_cell_buffer[index++] = static_cast<dicom_cell_value_t>(cell.pt[0]) + 1;
            m_cell_buffer[index++] = static_cast<dicom_cell_value_t>(cell.pt[1]) + 1;
            m_cell_buffer[index++] = static_cast<dicom_cell_value_t>(cell.pt[2]) + 1;
        }
    }

    // Normals
    if(mesh->has<data::mesh::attribute::point_normals>())
    {
        const auto begin = mesh->cbegin<data::iterator::point::nxyz>();
        const auto end   = mesh->cend<data::iterator::point::nxyz>();

        // Retrieve & copy data
        m_normal_buffer.reserve(mesh->num_points() * std::size_t(3));
        m_normal_buffer.assign(reinterpret_cast<const float*>(&*begin), reinterpret_cast<const float*>(&*end));
    }
}

//------------------------------------------------------------------------------

dicom_surface::dicom_surface(
    const data::mesh::position_t* _point_buffer,
    const data::mesh::size_t _point_buffer_size,
    const dicom_cell_value_t* _cell_buffer,
    const data::mesh::size_t _cell_buffer_size,
    const data::mesh::normal_t* _normal_buffer
)
{
    // Coordinates
    m_point_buffer.reserve(_point_buffer_size);
    m_point_buffer.assign(_point_buffer, _point_buffer + _point_buffer_size);

    // Cells
    m_cell_buffer.reserve(_cell_buffer_size);
    m_cell_buffer.assign(_cell_buffer, _cell_buffer + _cell_buffer_size);

    // Normals
    if(_normal_buffer != nullptr)
    {
        m_normal_buffer.reserve(_point_buffer_size);
        m_normal_buffer.assign(_normal_buffer, _normal_buffer + _point_buffer_size);
    }
}

//------------------------------------------------------------------------------

dicom_surface::~dicom_surface()
= default;

//------------------------------------------------------------------------------

data::mesh::sptr dicom_surface::convert_to_data()
{
    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    const auto lock       = mesh->dump_lock();

    // Initialize number of points
    data::mesh::attribute attribute = data::mesh::attribute::none;
    if(!m_normal_buffer.empty())
    {
        attribute = data::mesh::attribute::point_normals;
    }

    mesh->resize(
        data::mesh::size_t(m_point_buffer.size() / 3),
        data::mesh::size_t(m_cell_buffer.size() / 3),
        data::mesh::cell_type_t::triangle,
        attribute
    );

    // Coordinates
    {
        auto itr = mesh->begin<data::iterator::point::xyz>();
        std::copy(m_point_buffer.begin(), m_point_buffer.end(), reinterpret_cast<float*>(&*itr));
    }

    // Cells
    {
        std::size_t index = 0;

        for(auto& cell : mesh->range<data::iterator::cell::triangle>())
        {
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                // Index shall start at 0 in Sight
                cell.pt[i] = static_cast<data::mesh::point_t>(m_cell_buffer[index]) - 1;
                ++index;
            }
        }
    }

    // Normals
    if(!m_normal_buffer.empty())
    {
        const auto itr = mesh->begin<data::iterator::point::nxyz>();
        std::copy(m_normal_buffer.begin(), m_normal_buffer.end(), reinterpret_cast<float*>(&*itr));
    }

    return mesh;
}

//------------------------------------------------------------------------------

const dicom_surface::dicom_point_buffer_t& dicom_surface::get_point_buffer() const
{
    return m_point_buffer;
}

//------------------------------------------------------------------------------

const dicom_surface::dicom_cell_buffer_t& dicom_surface::get_cell_buffer() const
{
    return m_cell_buffer;
}

//------------------------------------------------------------------------------

const dicom_surface::dicom_normal_buffer_t& dicom_surface::get_normal_buffer() const
{
    return m_normal_buffer;
}

//------------------------------------------------------------------------------

std::size_t dicom_surface::get_point_buffer_size() const
{
    return m_point_buffer.size();
}

//------------------------------------------------------------------------------

std::size_t dicom_surface::get_cell_buffer_size() const
{
    return m_cell_buffer.size();
}

//------------------------------------------------------------------------------

std::size_t dicom_surface::get_normal_buffer_size() const
{
    return m_normal_buffer.size();
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container
