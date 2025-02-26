/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "data/mesh.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

#include <cstdlib>
#include <functional>
#include <numeric>

namespace sight::data
{

constexpr int POINT_REALLOC_STEP    = 1000;
constexpr int CELLDATA_REALLOC_STEP = 1000;

SIGHT_REGISTER_DATA(sight::data::mesh);

static constexpr auto CELL_SIZE_TO_TYPE = [](std::size_t _size)
                                          {
                                              static constexpr std::array<mesh::cell_type_t, 5> s_CELL_SIZE_TO_TYPE =
                                              {
                                                  mesh::cell_type_t::size, // 0 point, unused
                                                  mesh::cell_type_t::point,
                                                  mesh::cell_type_t::line,
                                                  mesh::cell_type_t::triangle,
                                                  mesh::cell_type_t::quad // Assume 4 points means a quad. Pushing
                                                                          // tetras
                                                                          // would require a reserve() first
                                              };
                                              return s_CELL_SIZE_TO_TYPE[_size];
                                          };

//------------------------------------------------------------------------------

const core::com::signals::key_t mesh::VERTEX_MODIFIED_SIG           = "vertexModified";
const core::com::signals::key_t mesh::POINT_COLORS_MODIFIED_SIG     = "pointColorsModified";
const core::com::signals::key_t mesh::CELL_COLORS_MODIFIED_SIG      = "cellColorsModified";
const core::com::signals::key_t mesh::POINT_NORMALS_MODIFIED_SIG    = "pointNormalsModified";
const core::com::signals::key_t mesh::CELL_NORMALS_MODIFIED_SIG     = "cellNormalsModified";
const core::com::signals::key_t mesh::POINT_TEX_COORDS_MODIFIED_SIG = "pointTexCoordsModified";
const core::com::signals::key_t mesh::CELL_TEX_COORDS_MODIFIED_SIG  = "cellTexCoordsModified";

//------------------------------------------------------------------------------

mesh::mesh()
{
    new_signal<signal_t>(VERTEX_MODIFIED_SIG);
    new_signal<signal_t>(POINT_COLORS_MODIFIED_SIG);
    new_signal<signal_t>(CELL_COLORS_MODIFIED_SIG);
    new_signal<signal_t>(POINT_NORMALS_MODIFIED_SIG);
    new_signal<signal_t>(CELL_NORMALS_MODIFIED_SIG);
    new_signal<signal_t>(POINT_TEX_COORDS_MODIFIED_SIG);
    new_signal<signal_t>(CELL_TEX_COORDS_MODIFIED_SIG);

    std::ranges::for_each(m_points, [](auto& _array){_array = std::make_shared<data::array>();});
    std::ranges::for_each(m_cells, [](auto& _array){_array = std::make_shared<data::array>();});
}

//------------------------------------------------------------------------------

void mesh::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const mesh>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_num_points = other->m_num_points;
    m_attributes = other->m_attributes;
    m_num_cells  = other->m_num_cells;
    m_cell_type  = other->m_cell_type;

    std::copy(other->m_points.cbegin(), other->m_points.cend(), m_points.begin());
    std::copy(other->m_cells.cbegin(), other->m_cells.cend(), m_cells.begin());

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void mesh::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const mesh>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    m_num_points = other->m_num_points;
    m_attributes = other->m_attributes;
    m_num_cells  = other->m_num_cells;
    m_cell_type  = other->m_cell_type;

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(point_attribute::size) ; ++i)
    {
        m_points[i] = data::object::copy(other->m_points[i], _cache);
    }

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(cell_attribute::size) ; ++i)
    {
        m_cells[i] = data::object::copy(other->m_cells[i], _cache);
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

std::size_t mesh::reserve(mesh::size_t _nb_pts, mesh::size_t _nb_cells, cell_type_t _cell_type, attribute _array_mask)
{
    m_cell_type = _cell_type;

    SIGHT_THROW_EXCEPTION_IF(data::exception("Cannot not allocate empty size"), _nb_pts == 0 || _nb_cells == 0);

    m_points[static_cast<std::size_t>(point_attribute::position)]->resize(
        {3, _nb_pts},
        core::type::get<position_t>()
    );

    m_attributes = m_attributes | _array_mask;

    // Test attributes mask, if present resize corresponding array, if not check if array needs to be cleared.
    if(static_cast<int>(_array_mask & attribute::point_colors) != 0)
    {
        m_points[static_cast<std::size_t>(point_attribute::colors)]->resize(
            {4, _nb_pts},
            core::type(core::type::UINT8)
        );
    }

    if(static_cast<int>(_array_mask & attribute::point_normals) != 0)
    {
        m_points[static_cast<std::size_t>(point_attribute::normals)]->resize(
            {3, _nb_pts},
            core::type(core::type::FLOAT)
        );
    }

    if(static_cast<int>(_array_mask & attribute::point_tex_coords) != 0)
    {
        m_points[static_cast<std::size_t>(point_attribute::tex_coords)]->resize(
            {2, _nb_pts},
            core::type(core::type::FLOAT)
        );
    }

    m_cells[static_cast<std::size_t>(cell_attribute::index)]->resize(
        {cell_size(), _nb_cells},
        core::type::get<cell_t>()
    );

    if(static_cast<int>(_array_mask & attribute::cell_colors) != 0)
    {
        m_cells[static_cast<std::size_t>(cell_attribute::colors)]->resize(
            {4, _nb_cells},
            core::type(core::type::UINT8)
        );
    }

    if(static_cast<int>(_array_mask & attribute::cell_normals) != 0)
    {
        m_cells[static_cast<std::size_t>(cell_attribute::normals)]->resize(
            {3, _nb_cells},
            core::type(core::type::FLOAT)
        );
    }

    if(static_cast<int>(_array_mask & attribute::cell_tex_coords) != 0)
    {
        m_cells[static_cast<std::size_t>(cell_attribute::tex_coords)]->resize(
            {2, _nb_cells},
            core::type(core::type::FLOAT)
        );
    }

    return this->allocated_size_in_bytes();
}

//------------------------------------------------------------------------------
std::size_t mesh::resize(mesh::size_t _nb_pts, mesh::size_t _nb_cells, cell_type_t _cell_type, attribute _array_mask)
{
    m_cell_type = _cell_type;

    const auto size = this->reserve(_nb_pts, _nb_cells, _cell_type, _array_mask);
    m_num_points = _nb_pts;
    m_num_cells  = _nb_cells;
    return size;
}

//------------------------------------------------------------------------------

bool mesh::shrink_to_fit()
{
    const auto old_allocated_size = this->allocated_size_in_bytes();

    m_points[static_cast<std::size_t>(point_attribute::position)]->resize({3, m_num_points});
    m_cells[static_cast<std::size_t>(cell_attribute::index)]->resize({cell_size(), m_num_cells});

    if(static_cast<std::uint8_t>(m_attributes & attribute::point_colors) != 0U)
    {
        m_points[static_cast<std::size_t>(point_attribute::colors)]->resize({4, std::size_t(m_num_points)});
    }

    if(static_cast<std::uint8_t>(m_attributes & attribute::point_normals) != 0U)
    {
        m_points[static_cast<std::size_t>(point_attribute::normals)]->resize({3, std::size_t(m_num_points)});
    }

    if(static_cast<std::uint8_t>(m_attributes & attribute::point_tex_coords) != 0U)
    {
        m_points[static_cast<std::size_t>(point_attribute::tex_coords)]->resize({2, std::size_t(m_num_points)});
    }

    if(static_cast<std::uint8_t>(m_attributes & attribute::cell_colors) != 0U)
    {
        m_cells[static_cast<std::size_t>(cell_attribute::colors)]->resize({4, std::size_t(m_num_cells)});
    }

    if(static_cast<std::uint8_t>(m_attributes & attribute::cell_normals) != 0U)
    {
        m_cells[static_cast<std::size_t>(cell_attribute::normals)]->resize({3, std::size_t(m_num_cells)});
    }

    if(static_cast<std::uint8_t>(m_attributes & attribute::cell_tex_coords) != 0U)
    {
        m_cells[static_cast<std::size_t>(cell_attribute::tex_coords)]->resize({2, std::size_t(m_num_cells)});
    }

    const auto new_allocated_size = this->allocated_size_in_bytes();
    SIGHT_ASSERT(
        "Error adjusting memory : allocated size: " << new_allocated_size
        << " != data size : " << this->size_in_bytes(),
        new_allocated_size == this->size_in_bytes()
    );
    return old_allocated_size != new_allocated_size;
}

//------------------------------------------------------------------------------

void mesh::truncate(mesh::size_t _nb_pts, mesh::size_t _nb_cells)
{
    SIGHT_THROW_IF(
        "Can not truncate with a number of points higher than the allocated size",
        _nb_pts > m_points[0]->size().at(1)
    );
    SIGHT_THROW_IF(
        "Can not truncate with a number of cells higher than the allocated size",
        _nb_cells > m_cells[0]->size().at(1)
    );
    m_num_points = _nb_pts;
    m_num_cells  = _nb_cells;
}

//------------------------------------------------------------------------------

void mesh::clear()
{
    std::ranges::for_each(m_points, [](auto& _array){_array->clear();});
    std::ranges::for_each(m_cells, [](auto& _array){_array->clear();});

    // Reset nbPoints, nbCells & cellsDataSize.
    m_num_points = 0;
    m_num_cells  = 0;

    m_attributes = attribute::none;
}

//------------------------------------------------------------------------------

std::size_t mesh::size_in_bytes() const
{
    std::size_t size = 0;

    std::ranges::for_each(
        m_points,
        [&](auto& _array)
        {
            const auto num_components = _array->empty() ? 1 : _array->size()[0];
            size                     += _array->element_size_in_bytes() * num_components
                                        * static_cast<std::size_t>(m_num_points);
        });

    std::ranges::for_each(
        m_cells,
        [&](auto& _array)
        {
            const auto num_components = _array->empty() ? 1 : _array->size()[0];
            size                     += _array->element_size_in_bytes() * num_components
                                        * static_cast<std::size_t>(m_num_cells);
        });

    return size;
}

//------------------------------------------------------------------------------

std::size_t mesh::allocated_size_in_bytes() const
{
    std::size_t size = 0;

    std::ranges::for_each(m_points, [&](auto& _array){size += _array->size_in_bytes();});
    std::ranges::for_each(m_cells, [&](auto& _array){size += _array->size_in_bytes();});

    return size;
}

//------------------------------------------------------------------------------

mesh::point_t mesh::push_point(const std::array<position_t, 3>& _p)
{
    const auto nb_points = m_num_points;

    auto positions           = m_points[static_cast<std::size_t>(point_attribute::position)];
    const auto allocated_pts = positions->empty() ? 0 : positions->size().at(1);

    if(allocated_pts <= nb_points)
    {
        positions->resize({3, allocated_pts + POINT_REALLOC_STEP}, core::type::get<position_t>(), true);
        if(this->has<data::mesh::attribute::point_colors>())
        {
            m_points[static_cast<std::size_t>(point_attribute::colors)]->resize(
                {4, allocated_pts + POINT_REALLOC_STEP},
                core::type::get<color_t>(),
                true
            );
        }

        if(this->has<data::mesh::attribute::point_normals>())
        {
            m_points[static_cast<std::size_t>(point_attribute::normals)]->resize(
                {3, allocated_pts + POINT_REALLOC_STEP},
                core::type::get<normal_t>(),
                true
            );
        }

        if(this->has<data::mesh::attribute::point_tex_coords>())
        {
            m_points[static_cast<std::size_t>(point_attribute::tex_coords)]->resize(
                {2, allocated_pts + POINT_REALLOC_STEP},
                core::type::get<texcoord_t>(),
                true
            );
        }
    }

    this->set_point(nb_points, _p);

    ++m_num_points;
    return nb_points;
}

//------------------------------------------------------------------------------

mesh::point_t mesh::push_point(position_t _x, position_t _y, position_t _z)
{
    return this->push_point({_x, _y, _z});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::push_cell(point_t _id_pt)
{
    std::vector point_ids = {_id_pt};
    return this->push_cell(point_ids);
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::push_cell(point_t _id_p1, point_t _id_p2)
{
    return this->push_cell({_id_p1, _id_p2});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::push_cell(point_t _id_p1, point_t _id_p2, point_t _id_p3)
{
    return this->push_cell({_id_p1, _id_p2, _id_p3});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::push_cell(point_t _id_p1, point_t _id_p2, point_t _id_p3, point_t _id_p4)
{
    return this->push_cell({_id_p1, _id_p2, _id_p3, _id_p4});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::push_cell(const std::vector<point_t> _point_ids)
{
    return this->push_cell(_point_ids.data(), static_cast<point_t>(_point_ids.size()));
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::push_cell(const point_t* _point_ids, std::size_t _nb_points)
{
    m_cell_type = (m_cell_type == cell_type_t::size) ? CELL_SIZE_TO_TYPE(_nb_points) : m_cell_type;

    SIGHT_THROW_IF("Can not push a point cell before setting the cell type", m_cell_type == cell_type_t::size);
    SIGHT_THROW_IF(
        "Can not push a point cell with this cell type",
        m_cell_type == cell_type_t::point && _nb_points != 1
    );
    SIGHT_THROW_IF("Can not push a line cell with this cell type", m_cell_type == cell_type_t::line && _nb_points != 2);
    SIGHT_THROW_IF(
        "Can not push a triangle cell with this cell type",
        m_cell_type == cell_type_t::triangle && _nb_points != 3
    );
    SIGHT_THROW_IF(
        "Can not push a quad or tetra cell with this cell type",
        (m_cell_type == cell_type_t::quad || m_cell_type == cell_type_t::tetra) && _nb_points != 4
    );

    const auto nb_cells = m_num_cells;

    auto& cells                    = m_cells[static_cast<std::size_t>(cell_attribute::index)];
    const auto allocated_cell_data = cells->empty() ? 0 : cells->size().at(1);
    const auto cell_size           = this->cell_size();
    if(allocated_cell_data <= m_num_cells)
    {
        cells->resize({cell_size, allocated_cell_data + CELLDATA_REALLOC_STEP}, core::type::get<cell_t>());
        if(this->has<data::mesh::attribute::cell_colors>())
        {
            m_cells[static_cast<std::size_t>(cell_attribute::colors)]->resize(
                {4, allocated_cell_data + CELLDATA_REALLOC_STEP},
                core::type::get<color_t>(),
                true
            );
        }

        if(this->has<data::mesh::attribute::cell_normals>())
        {
            m_cells[static_cast<std::size_t>(cell_attribute::normals)]->resize(
                {3, allocated_cell_data + CELLDATA_REALLOC_STEP},
                core::type::get<normal_t>(),
                true
            );
        }

        if(this->has<data::mesh::attribute::cell_tex_coords>())
        {
            m_cells[static_cast<std::size_t>(cell_attribute::tex_coords)]->resize(
                {2, allocated_cell_data + CELLDATA_REALLOC_STEP},
                core::type::get<texcoord_t>(),
                true
            );
        }
    }

    for(std::size_t i = 0 ; i < _nb_points ; ++i)
    {
        const point_t cell_value = _point_ids[i];
        cells->at<cell_t>(static_cast<std::size_t>(m_num_cells) * cell_size + i) = cell_value;
    }

    ++m_num_cells;
    return nb_cells;
}

//------------------------------------------------------------------------------

void mesh::set_point(point_t _id, const std::array<position_t, 3>& _p)
{
    auto& points = m_points[static_cast<std::size_t>(point_attribute::position)];
    points->at<position_t>(3LL * _id)     = _p[0];
    points->at<position_t>(3LL * _id + 1) = _p[1];
    points->at<position_t>(3LL * _id + 2) = _p[2];
}

//------------------------------------------------------------------------------

void mesh::set_point(
    point_t _id,
    data::mesh::position_t _x,
    data::mesh::position_t _y,
    data::mesh::position_t _z
)
{
    this->set_point(_id, {_x, _y, _z});
}

//------------------------------------------------------------------------------

void mesh::set_cell(cell_t _id, point_t _id_pt)
{
    std::vector point_ids = {_id_pt};
    this->set_cell(_id, point_ids);
}

//------------------------------------------------------------------------------

void mesh::set_cell(cell_t _id, point_t _id_p1, point_t _id_p2)
{
    this->set_cell(_id, {_id_p1, _id_p2});
}

//------------------------------------------------------------------------------

void mesh::set_cell(cell_t _id, point_t _id_p1, point_t _id_p2, point_t _id_p3)
{
    this->set_cell(_id, {_id_p1, _id_p2, _id_p3});
}

//------------------------------------------------------------------------------

void mesh::set_cell(
    cell_t _id,
    point_t _id_p1,
    point_t _id_p2,
    point_t _id_p3,
    point_t _id_p4
)
{
    this->set_cell(_id, {_id_p1, _id_p2, _id_p3, _id_p4});
}

//------------------------------------------------------------------------------

void mesh::set_cell(cell_t _id, const std::vector<point_t>& _point_ids)
{
    this->set_cell(_id, _point_ids.data(), static_cast<std::size_t>(_point_ids.size()));
}

//------------------------------------------------------------------------------

void mesh::set_cell(cell_t _id, const point_t* _point_ids, std::size_t _nb_points)
{
    SIGHT_THROW_IF("Can not push a point cell before setting the cell type", m_cell_type == cell_type_t::size);
    SIGHT_THROW_IF(
        "Can not push a point cell with this cell type",
        m_cell_type == cell_type_t::point && _nb_points != 1
    );
    SIGHT_THROW_IF("Can not push a line cell with this cell type", m_cell_type == cell_type_t::line && _nb_points != 2);
    SIGHT_THROW_IF(
        "Can not push a triangle cell with this cell type",
        m_cell_type == cell_type_t::triangle && _nb_points != 3
    );
    SIGHT_THROW_IF(
        "Can not push a quad or tetra cell with this cell type",
        (m_cell_type == cell_type_t::quad || m_cell_type == cell_type_t::tetra) && _nb_points != 4
    );

    auto& cells = m_cells[static_cast<std::size_t>(cell_attribute::index)];
    for(std::size_t i = 0 ; i < _nb_points ; ++i)
    {
        const cell_t cell_value = _point_ids[i];
        cells->at<cell_t>(static_cast<std::size_t>(cell_size()) * _id + i) = cell_value;
    }
}

//------------------------------------------------------------------------------

void mesh::set_point_color(point_t _id, const std::array<data::mesh::color_t, 4>& _c)
{
    this->set_point_color(_id, _c[0], _c[1], _c[2], _c[3]);
}

//------------------------------------------------------------------------------

void mesh::set_point_color(
    point_t _id,
    data::mesh::color_t _r,
    data::mesh::color_t _g,
    data::mesh::color_t _b,
    data::mesh::color_t _a
)
{
    auto& colors = m_points[static_cast<std::size_t>(point_attribute::colors)];
    colors->at<color_t>(4LL * _id)     = _r;
    colors->at<color_t>(4LL * _id + 1) = _g;
    colors->at<color_t>(4LL * _id + 2) = _b;
    colors->at<color_t>(4LL * _id + 3) = _a;
}

//------------------------------------------------------------------------------

void mesh::set_cell_color(cell_t _id, const std::array<data::mesh::color_t, 4>& _c)
{
    this->set_cell_color(_id, _c[0], _c[1], _c[2], _c[3]);
}

//------------------------------------------------------------------------------

void mesh::set_cell_color(
    cell_t _id,
    data::mesh::color_t _r,
    data::mesh::color_t _g,
    data::mesh::color_t _b,
    data::mesh::color_t _a
)
{
    auto& colors = m_cells[static_cast<std::size_t>(cell_attribute::colors)];
    colors->at<color_t>(4LL * _id)     = _r;
    colors->at<color_t>(4LL * _id + 1) = _g;
    colors->at<color_t>(4LL * _id + 2) = _b;
    colors->at<color_t>(4LL * _id + 3) = _a;
}

//------------------------------------------------------------------------------

void mesh::set_point_normal(point_t _id, const std::array<data::mesh::normal_t, 3>& _n)
{
    this->set_point_normal(_id, _n[0], _n[1], _n[2]);
}

//------------------------------------------------------------------------------

void mesh::set_point_normal(
    point_t _id,
    data::mesh::normal_t _nx,
    data::mesh::normal_t _ny,
    data::mesh::normal_t _nz
)
{
    auto& normals = m_points[static_cast<std::size_t>(point_attribute::normals)];
    normals->at<normal_t>(3LL * _id)     = _nx;
    normals->at<normal_t>(3LL * _id + 1) = _ny;
    normals->at<normal_t>(3LL * _id + 2) = _nz;
}

//------------------------------------------------------------------------------

void mesh::set_cell_normal(cell_t _id, const std::array<data::mesh::normal_t, 3>& _n)
{
    this->set_cell_normal(_id, _n[0], _n[1], _n[2]);
}

//------------------------------------------------------------------------------

void mesh::set_cell_normal(
    cell_t _id,
    data::mesh::normal_t _nx,
    data::mesh::normal_t _ny,
    data::mesh::normal_t _nz
)
{
    auto& normals = m_cells[static_cast<std::size_t>(cell_attribute::normals)];
    normals->at<normal_t>(3LL * _id)     = _nx;
    normals->at<normal_t>(3LL * _id + 1) = _ny;
    normals->at<normal_t>(3LL * _id + 2) = _nz;
}

//------------------------------------------------------------------------------

void mesh::set_point_tex_coord(point_t _id, const std::array<data::mesh::texcoord_t, 2>& _t)
{
    this->set_point_tex_coord(_id, _t[0], _t[1]);
}

//------------------------------------------------------------------------------

void mesh::set_point_tex_coord(
    point_t _id,
    data::mesh::texcoord_t _u,
    data::mesh::texcoord_t _v
)
{
    auto& tex_coords = m_points[static_cast<std::size_t>(point_attribute::tex_coords)];
    tex_coords->at<texcoord_t>(2LL * _id)     = _u;
    tex_coords->at<texcoord_t>(2LL * _id + 1) = _v;
}

//------------------------------------------------------------------------------

void mesh::set_cell_tex_coord(cell_t _id, const std::array<data::mesh::texcoord_t, 2>& _t)
{
    this->set_cell_tex_coord(_id, _t[0], _t[1]);
}

//------------------------------------------------------------------------------

void mesh::set_cell_tex_coord(
    cell_t _id,
    data::mesh::texcoord_t _u,
    data::mesh::texcoord_t _v
)
{
    auto& tex_coords = m_cells[static_cast<std::size_t>(cell_attribute::tex_coords)];
    tex_coords->at<texcoord_t>(2LL * _id)     = _u;
    tex_coords->at<texcoord_t>(2LL * _id + 1) = _v;
}

//------------------------------------------------------------------------------

void mesh::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const
{
    for(const auto& point : m_points)
    {
        const auto& point_locks = point->dump_lock();
        _locks.insert(_locks.end(), point_locks.cbegin(), point_locks.cend());
    }

    for(const auto& cell : m_cells)
    {
        const auto& cell_locks = cell->dump_lock();
        _locks.insert(_locks.end(), cell_locks.cbegin(), cell_locks.cend());
    }
}

//------------------------------------------------------------------------------

bool mesh::operator==(const mesh& _other) const noexcept
{
    if(m_num_points != _other.m_num_points
       || m_num_cells != _other.m_num_cells
       || m_cell_type != _other.m_cell_type
       || m_attributes != _other.m_attributes
       || !core::is_equal(m_points, _other.m_points)
       || !core::is_equal(m_cells, _other.m_cells))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool mesh::operator!=(const mesh& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::point::xyz>()
{
    return m_points[static_cast<std::size_t>(point_attribute::position)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::point::nxyz>()
{
    return m_points[static_cast<std::size_t>(point_attribute::normals)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::point::uv>()
{
    return m_points[static_cast<std::size_t>(point_attribute::tex_coords)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::point::rgba>()
{
    return m_points[static_cast<std::size_t>(point_attribute::colors)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::point>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::line>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::triangle>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::quad>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::tetra>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::nxyz>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::normals)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::uv>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::tex_coords)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::sptr mesh::get_array<iterator::cell::rgba>()
{
    return m_cells[static_cast<std::size_t>(cell_attribute::colors)];
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::point::xyz>() const
{
    return m_points[static_cast<std::size_t>(point_attribute::position)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::point::nxyz>() const
{
    return m_points[static_cast<std::size_t>(point_attribute::normals)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::point::uv>() const
{
    return m_points[static_cast<std::size_t>(point_attribute::tex_coords)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::point::rgba>() const
{
    return m_points[static_cast<std::size_t>(point_attribute::colors)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::point>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::line>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::triangle>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::quad>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::tetra>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::index)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::nxyz>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::normals)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::uv>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::tex_coords)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API data::array::csptr mesh::get_array<iterator::cell::rgba>() const
{
    return m_cells[static_cast<std::size_t>(cell_attribute::colors)];
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::point::xyz>() const
{
    return num_points();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::point::nxyz>() const
{
    return num_points();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::point::uv>() const
{
    return num_points();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::point::rgba>() const
{
    return num_points();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::point>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::line>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::triangle>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::quad>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::tetra>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::nxyz>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::uv>() const
{
    return num_cells();
}

//------------------------------------------------------------------------------

template<>
SIGHT_DATA_API std::size_t mesh::num_elements<iterator::cell::rgba>() const
{
    return num_cells();
}

} //namespace sight::data
