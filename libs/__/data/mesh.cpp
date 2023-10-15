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

static constexpr auto cellSizeToType = [](std::size_t size)
                                       {
                                           static constexpr std::array<mesh::CellType, 5> s_cellSizeToType =
                                           {
                                               mesh::CellType::_SIZE, // 0 point, unused
                                               mesh::CellType::POINT,
                                               mesh::CellType::LINE,
                                               mesh::CellType::TRIANGLE,
                                               mesh::CellType::QUAD // Assume 4 points means a quad. Pushing tetras
                                                                    // would require a reserve() first
                                           };
                                           return s_cellSizeToType[size];
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

    std::ranges::for_each(m_points, [](auto& array){array = std::make_shared<data::array>();});
    std::ranges::for_each(m_cells, [](auto& array){array = std::make_shared<data::array>();});
}

//------------------------------------------------------------------------------

void mesh::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const mesh>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_numPoints  = other->m_numPoints;
    m_attributes = other->m_attributes;
    m_numCells   = other->m_numCells;
    m_cellType   = other->m_cellType;

    std::copy(other->m_points.cbegin(), other->m_points.cend(), m_points.begin());
    std::copy(other->m_cells.cbegin(), other->m_cells.cend(), m_cells.begin());

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void mesh::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const mesh>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    m_numPoints  = other->m_numPoints;
    m_attributes = other->m_attributes;
    m_numCells   = other->m_numCells;
    m_cellType   = other->m_cellType;

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(PointAttribute::_SIZE) ; ++i)
    {
        m_points[i] = data::object::copy(other->m_points[i], cache);
    }

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(CellAttribute::_SIZE) ; ++i)
    {
        m_cells[i] = data::object::copy(other->m_cells[i], cache);
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

std::size_t mesh::reserve(mesh::size_t nbPts, mesh::size_t nbCells, CellType cellType, Attributes arrayMask)
{
    m_cellType = cellType;

    SIGHT_THROW_EXCEPTION_IF(data::exception("Cannot not allocate empty size"), nbPts == 0 || nbCells == 0);

    m_points[static_cast<std::size_t>(PointAttribute::POSITION)]->resize(
        {3, nbPts},
        core::type::get<position_t>()
    );

    m_attributes = m_attributes | arrayMask;

    // Test attributes mask, if present resize corresponding array, if not check if array needs to be cleared.
    if(static_cast<int>(arrayMask & Attributes::POINT_COLORS) != 0)
    {
        m_points[static_cast<std::size_t>(PointAttribute::COLORS)]->resize(
            {4, nbPts},
            core::type(core::type::UINT8)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::POINT_NORMALS) != 0)
    {
        m_points[static_cast<std::size_t>(PointAttribute::NORMALS)]->resize(
            {3, nbPts},
            core::type(core::type::FLOAT)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::POINT_TEX_COORDS) != 0)
    {
        m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)]->resize(
            {2, nbPts},
            core::type(core::type::FLOAT)
        );
    }

    m_cells[static_cast<std::size_t>(CellAttribute::INDEX)]->resize(
        {getCellSize(), nbCells},
        core::type::get<cell_t>()
    );

    if(static_cast<int>(arrayMask & Attributes::CELL_COLORS) != 0)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::COLORS)]->resize(
            {4, nbCells},
            core::type(core::type::UINT8)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::CELL_NORMALS) != 0)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)]->resize(
            {3, nbCells},
            core::type(core::type::FLOAT)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::CELL_TEX_COORDS) != 0)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)]->resize(
            {2, nbCells},
            core::type(core::type::FLOAT)
        );
    }

    return this->getAllocatedSizeInBytes();
}

//------------------------------------------------------------------------------
std::size_t mesh::resize(mesh::size_t nbPts, mesh::size_t nbCells, CellType cellType, Attributes arrayMask)
{
    m_cellType = cellType;

    const auto size = this->reserve(nbPts, nbCells, cellType, arrayMask);
    m_numPoints = nbPts;
    m_numCells  = nbCells;
    return size;
}

//------------------------------------------------------------------------------

bool mesh::shrinkToFit()
{
    const auto oldAllocatedSize = this->getAllocatedSizeInBytes();

    m_points[static_cast<std::size_t>(PointAttribute::POSITION)]->resize({3, m_numPoints});
    m_cells[static_cast<std::size_t>(CellAttribute::INDEX)]->resize({getCellSize(), m_numCells});

    if(static_cast<std::uint8_t>(m_attributes & Attributes::POINT_COLORS) != 0U)
    {
        m_points[static_cast<std::size_t>(PointAttribute::COLORS)]->resize({4, std::size_t(m_numPoints)});
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::POINT_NORMALS) != 0U)
    {
        m_points[static_cast<std::size_t>(PointAttribute::NORMALS)]->resize({3, std::size_t(m_numPoints)});
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::POINT_TEX_COORDS) != 0U)
    {
        m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)]->resize({2, std::size_t(m_numPoints)});
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::CELL_COLORS) != 0U)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::COLORS)]->resize({4, std::size_t(m_numCells)});
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::CELL_NORMALS) != 0U)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)]->resize({3, std::size_t(m_numCells)});
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::CELL_TEX_COORDS) != 0U)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)]->resize({2, std::size_t(m_numCells)});
    }

    const auto newAllocatedSize = this->getAllocatedSizeInBytes();
    SIGHT_ASSERT(
        "Error adjusting memory : allocated size: " << newAllocatedSize
        << " != data size : " << this->getDataSizeInBytes(),
        newAllocatedSize == this->getDataSizeInBytes()
    );
    return oldAllocatedSize != newAllocatedSize;
}

//------------------------------------------------------------------------------

void mesh::truncate(mesh::size_t nbPts, mesh::size_t nbCells)
{
    SIGHT_THROW_IF(
        "Can not truncate with a number of points higher than the allocated size",
        nbPts > m_points[0]->size().at(1)
    );
    SIGHT_THROW_IF(
        "Can not truncate with a number of cells higher than the allocated size",
        nbCells > m_cells[0]->size().at(1)
    );
    m_numPoints = nbPts;
    m_numCells  = nbCells;
}

//------------------------------------------------------------------------------

void mesh::clear()
{
    std::ranges::for_each(m_points, [](auto& array){array->clear();});
    std::ranges::for_each(m_cells, [](auto& array){array->clear();});

    // Reset nbPoints, nbCells & cellsDataSize.
    m_numPoints = 0;
    m_numCells  = 0;

    m_attributes = Attributes::NONE;
}

//------------------------------------------------------------------------------

std::size_t mesh::getDataSizeInBytes() const
{
    std::size_t size = 0;

    std::ranges::for_each(
        m_points,
        [&](auto& array)
        {
            const auto numComponents = array->empty() ? 1 : array->size()[0];
            size                    += array->getElementSizeInBytes() * numComponents
                                       * static_cast<std::size_t>(m_numPoints);
        });

    std::ranges::for_each(
        m_cells,
        [&](auto& array)
        {
            const auto numComponents = array->empty() ? 1 : array->size()[0];
            size                    += array->getElementSizeInBytes() * numComponents
                                       * static_cast<std::size_t>(m_numCells);
        });

    return size;
}

//------------------------------------------------------------------------------

std::size_t mesh::getAllocatedSizeInBytes() const
{
    std::size_t size = 0;

    std::ranges::for_each(m_points, [&](auto& array){size += array->getSizeInBytes();});
    std::ranges::for_each(m_cells, [&](auto& array){size += array->getSizeInBytes();});

    return size;
}

//------------------------------------------------------------------------------

mesh::point_t mesh::pushPoint(const std::array<position_t, 3>& p)
{
    const auto nbPoints = m_numPoints;

    auto positions          = m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
    const auto allocatedPts = positions->empty() ? 0 : positions->size().at(1);

    if(allocatedPts <= nbPoints)
    {
        positions->resize({3, allocatedPts + POINT_REALLOC_STEP}, core::type::get<position_t>(), true);
        if(this->has<data::mesh::Attributes::POINT_COLORS>())
        {
            m_points[static_cast<std::size_t>(PointAttribute::COLORS)]->resize(
                {4, allocatedPts + POINT_REALLOC_STEP},
                core::type::get<color_t>(),
                true
            );
        }

        if(this->has<data::mesh::Attributes::POINT_NORMALS>())
        {
            m_points[static_cast<std::size_t>(PointAttribute::NORMALS)]->resize(
                {3, allocatedPts + POINT_REALLOC_STEP},
                core::type::get<normal_t>(),
                true
            );
        }

        if(this->has<data::mesh::Attributes::POINT_TEX_COORDS>())
        {
            m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)]->resize(
                {2, allocatedPts + POINT_REALLOC_STEP},
                core::type::get<texcoord_t>(),
                true
            );
        }
    }

    this->setPoint(nbPoints, p);

    ++m_numPoints;
    return nbPoints;
}

//------------------------------------------------------------------------------

mesh::point_t mesh::pushPoint(position_t x, position_t y, position_t z)
{
    return this->pushPoint({x, y, z});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::pushCell(point_t idPt)
{
    std::vector pointIds = {idPt};
    return this->pushCell(pointIds);
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::pushCell(point_t idP1, point_t idP2)
{
    return this->pushCell({idP1, idP2});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::pushCell(point_t idP1, point_t idP2, point_t idP3)
{
    return this->pushCell({idP1, idP2, idP3});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::pushCell(point_t idP1, point_t idP2, point_t idP3, point_t idP4)
{
    return this->pushCell({idP1, idP2, idP3, idP4});
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::pushCell(const std::vector<point_t> pointIds)
{
    return this->pushCell(pointIds.data(), static_cast<point_t>(pointIds.size()));
}

//------------------------------------------------------------------------------

mesh::cell_t mesh::pushCell(const point_t* pointIds, std::size_t nbPoints)
{
    m_cellType = (m_cellType == CellType::_SIZE) ? cellSizeToType(nbPoints) : m_cellType;

    SIGHT_THROW_IF("Can not push a point cell before setting the cell type", m_cellType == CellType::_SIZE);
    SIGHT_THROW_IF("Can not push a point cell with this cell type", m_cellType == CellType::POINT && nbPoints != 1);
    SIGHT_THROW_IF("Can not push a line cell with this cell type", m_cellType == CellType::LINE && nbPoints != 2);
    SIGHT_THROW_IF(
        "Can not push a triangle cell with this cell type",
        m_cellType == CellType::TRIANGLE && nbPoints != 3
    );
    SIGHT_THROW_IF(
        "Can not push a quad or tetra cell with this cell type",
        (m_cellType == CellType::QUAD || m_cellType == CellType::TETRA) && nbPoints != 4
    );

    const auto nbCells = m_numCells;

    auto& cells                  = m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
    const auto allocatedCellData = cells->empty() ? 0 : cells->size().at(1);
    const auto cellSize          = getCellSize();
    if(allocatedCellData <= m_numCells)
    {
        cells->resize({cellSize, allocatedCellData + CELLDATA_REALLOC_STEP}, core::type::get<cell_t>());
        if(this->has<data::mesh::Attributes::CELL_COLORS>())
        {
            m_cells[static_cast<std::size_t>(CellAttribute::COLORS)]->resize(
                {4, allocatedCellData + CELLDATA_REALLOC_STEP},
                core::type::get<color_t>(),
                true
            );
        }

        if(this->has<data::mesh::Attributes::CELL_NORMALS>())
        {
            m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)]->resize(
                {3, allocatedCellData + CELLDATA_REALLOC_STEP},
                core::type::get<normal_t>(),
                true
            );
        }

        if(this->has<data::mesh::Attributes::CELL_TEX_COORDS>())
        {
            m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)]->resize(
                {2, allocatedCellData + CELLDATA_REALLOC_STEP},
                core::type::get<texcoord_t>(),
                true
            );
        }
    }

    for(std::size_t i = 0 ; i < nbPoints ; ++i)
    {
        const point_t cellValue = pointIds[i];
        cells->at<cell_t>(static_cast<std::size_t>(m_numCells) * cellSize + i) = cellValue;
    }

    ++m_numCells;
    return nbCells;
}

//------------------------------------------------------------------------------

void mesh::setPoint(point_t id, const std::array<position_t, 3>& p)
{
    auto& points = m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
    points->at<position_t>(3LL * id)     = p[0];
    points->at<position_t>(3LL * id + 1) = p[1];
    points->at<position_t>(3LL * id + 2) = p[2];
}

//------------------------------------------------------------------------------

void mesh::setPoint(
    point_t id,
    data::mesh::position_t x,
    data::mesh::position_t y,
    data::mesh::position_t z
)
{
    this->setPoint(id, {x, y, z});
}

//------------------------------------------------------------------------------

void mesh::setCell(cell_t id, point_t idPt)
{
    std::vector pointIds = {idPt};
    this->setCell(id, pointIds);
}

//------------------------------------------------------------------------------

void mesh::setCell(cell_t id, point_t idP1, point_t idP2)
{
    this->setCell(id, {idP1, idP2});
}

//------------------------------------------------------------------------------

void mesh::setCell(cell_t id, point_t idP1, point_t idP2, point_t idP3)
{
    this->setCell(id, {idP1, idP2, idP3});
}

//------------------------------------------------------------------------------

void mesh::setCell(
    cell_t id,
    point_t idP1,
    point_t idP2,
    point_t idP3,
    point_t idP4
)
{
    this->setCell(id, {idP1, idP2, idP3, idP4});
}

//------------------------------------------------------------------------------

void mesh::setCell(cell_t id, const std::vector<point_t>& pointIds)
{
    this->setCell(id, pointIds.data(), static_cast<std::size_t>(pointIds.size()));
}

//------------------------------------------------------------------------------

void mesh::setCell(cell_t id, const point_t* pointIds, std::size_t nbPoints)
{
    SIGHT_THROW_IF("Can not push a point cell before setting the cell type", m_cellType == CellType::_SIZE);
    SIGHT_THROW_IF("Can not push a point cell with this cell type", m_cellType == CellType::POINT && nbPoints != 1);
    SIGHT_THROW_IF("Can not push a line cell with this cell type", m_cellType == CellType::LINE && nbPoints != 2);
    SIGHT_THROW_IF(
        "Can not push a triangle cell with this cell type",
        m_cellType == CellType::TRIANGLE && nbPoints != 3
    );
    SIGHT_THROW_IF(
        "Can not push a quad or tetra cell with this cell type",
        (m_cellType == CellType::QUAD || m_cellType == CellType::TETRA) && nbPoints != 4
    );

    auto& cells = m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
    for(std::size_t i = 0 ; i < nbPoints ; ++i)
    {
        const cell_t cellValue = pointIds[i];
        cells->at<cell_t>(static_cast<std::size_t>(getCellSize()) * id + i) = cellValue;
    }
}

//------------------------------------------------------------------------------

void mesh::setPointColor(point_t id, const std::array<data::mesh::color_t, 4>& c)
{
    this->setPointColor(id, c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------

void mesh::setPointColor(
    point_t id,
    data::mesh::color_t r,
    data::mesh::color_t g,
    data::mesh::color_t b,
    data::mesh::color_t a
)
{
    auto& colors = m_points[static_cast<std::size_t>(PointAttribute::COLORS)];
    colors->at<color_t>(4LL * id)     = r;
    colors->at<color_t>(4LL * id + 1) = g;
    colors->at<color_t>(4LL * id + 2) = b;
    colors->at<color_t>(4LL * id + 3) = a;
}

//------------------------------------------------------------------------------

void mesh::setCellColor(cell_t id, const std::array<data::mesh::color_t, 4>& c)
{
    this->setCellColor(id, c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------

void mesh::setCellColor(
    cell_t id,
    data::mesh::color_t r,
    data::mesh::color_t g,
    data::mesh::color_t b,
    data::mesh::color_t a
)
{
    auto& colors = m_cells[static_cast<std::size_t>(CellAttribute::COLORS)];
    colors->at<color_t>(4LL * id)     = r;
    colors->at<color_t>(4LL * id + 1) = g;
    colors->at<color_t>(4LL * id + 2) = b;
    colors->at<color_t>(4LL * id + 3) = a;
}

//------------------------------------------------------------------------------

void mesh::setPointNormal(point_t id, const std::array<data::mesh::normal_t, 3>& n)
{
    this->setPointNormal(id, n[0], n[1], n[2]);
}

//------------------------------------------------------------------------------

void mesh::setPointNormal(
    point_t id,
    data::mesh::normal_t nx,
    data::mesh::normal_t ny,
    data::mesh::normal_t nz
)
{
    auto& normals = m_points[static_cast<std::size_t>(PointAttribute::NORMALS)];
    normals->at<normal_t>(3LL * id)     = nx;
    normals->at<normal_t>(3LL * id + 1) = ny;
    normals->at<normal_t>(3LL * id + 2) = nz;
}

//------------------------------------------------------------------------------

void mesh::setCellNormal(cell_t id, const std::array<data::mesh::normal_t, 3>& n)
{
    this->setCellNormal(id, n[0], n[1], n[2]);
}

//------------------------------------------------------------------------------

void mesh::setCellNormal(
    cell_t id,
    data::mesh::normal_t nx,
    data::mesh::normal_t ny,
    data::mesh::normal_t nz
)
{
    auto& normals = m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)];
    normals->at<normal_t>(3LL * id)     = nx;
    normals->at<normal_t>(3LL * id + 1) = ny;
    normals->at<normal_t>(3LL * id + 2) = nz;
}

//------------------------------------------------------------------------------

void mesh::setPointTexCoord(point_t id, const std::array<data::mesh::texcoord_t, 2>& t)
{
    this->setPointTexCoord(id, t[0], t[1]);
}

//------------------------------------------------------------------------------

void mesh::setPointTexCoord(
    point_t id,
    data::mesh::texcoord_t u,
    data::mesh::texcoord_t v
)
{
    auto& texCoords = m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)];
    texCoords->at<texcoord_t>(2LL * id)     = u;
    texCoords->at<texcoord_t>(2LL * id + 1) = v;
}

//------------------------------------------------------------------------------

void mesh::setCellTexCoord(cell_t id, const std::array<data::mesh::texcoord_t, 2>& t)
{
    this->setCellTexCoord(id, t[0], t[1]);
}

//------------------------------------------------------------------------------

void mesh::setCellTexCoord(
    cell_t id,
    data::mesh::texcoord_t u,
    data::mesh::texcoord_t v
)
{
    auto& texCoords = m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)];
    texCoords->at<texcoord_t>(2LL * id)     = u;
    texCoords->at<texcoord_t>(2LL * id + 1) = v;
}

//------------------------------------------------------------------------------

void mesh::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& locks) const
{
    for(const auto& point : m_points)
    {
        const auto& point_locks = point->dump_lock();
        locks.insert(locks.end(), point_locks.cbegin(), point_locks.cend());
    }

    for(const auto& cell : m_cells)
    {
        const auto& cell_locks = cell->dump_lock();
        locks.insert(locks.end(), cell_locks.cbegin(), cell_locks.cend());
    }
}

//------------------------------------------------------------------------------

bool mesh::operator==(const mesh& other) const noexcept
{
    if(m_numPoints != other.m_numPoints
       || m_numCells != other.m_numCells
       || m_cellType != other.m_cellType
       || m_attributes != other.m_attributes
       || !core::tools::is_equal(m_points, other.m_points)
       || !core::tools::is_equal(m_cells, other.m_cells))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool mesh::operator!=(const mesh& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::point::xyz>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::point::nxyz>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::point::uv>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::point::rgba>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::COLORS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::point>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::line>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::triangle>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::quad>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::tetra>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::nxyz>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::uv>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::sptr mesh::getArray<iterator::cell::rgba>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::COLORS)];
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::point::xyz>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::point::nxyz>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::point::uv>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::point::rgba>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::COLORS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::point>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::line>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::triangle>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::quad>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::tetra>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::nxyz>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::uv>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::array::csptr mesh::getArray<iterator::cell::rgba>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::COLORS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::point::xyz>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::point::nxyz>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::point::uv>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::point::rgba>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::point>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::line>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::triangle>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::quad>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::tetra>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::nxyz>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::uv>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t mesh::numElements<iterator::cell::rgba>() const
{
    return numCells();
}

} //namespace sight::data
