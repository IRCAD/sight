/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/Mesh.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

#include <cstdlib>
#include <functional>
#include <numeric>

namespace sight::data
{

constexpr int POINT_REALLOC_STEP    = 1000;
constexpr int CELLDATA_REALLOC_STEP = 1000;

SIGHT_REGISTER_DATA(sight::data::Mesh);

static constexpr auto cellSizeToType = [](std::size_t size)
                                       {
                                           static constexpr std::array<Mesh::CellType, 5> s_cellSizeToType =
                                           {
                                               Mesh::CellType::_SIZE, // 0 point, unused
                                               Mesh::CellType::POINT,
                                               Mesh::CellType::LINE,
                                               Mesh::CellType::TRIANGLE,
                                               Mesh::CellType::QUAD // Assume 4 points means a quad. Pushing tetras
                                                                    // would require a reserve() first
                                           };
                                           return s_cellSizeToType[size];
                                       };

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType Mesh::s_VERTEX_MODIFIED_SIG           = "vertexModified";
const core::com::Signals::SignalKeyType Mesh::s_POINT_COLORS_MODIFIED_SIG     = "pointColorsModified";
const core::com::Signals::SignalKeyType Mesh::s_CELL_COLORS_MODIFIED_SIG      = "cellColorsModified";
const core::com::Signals::SignalKeyType Mesh::s_POINT_NORMALS_MODIFIED_SIG    = "pointNormalsModified";
const core::com::Signals::SignalKeyType Mesh::s_CELL_NORMALS_MODIFIED_SIG     = "cellNormalsModified";
const core::com::Signals::SignalKeyType Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG = "pointTexCoordsModified";
const core::com::Signals::SignalKeyType Mesh::s_CELL_TEX_COORDS_MODIFIED_SIG  = "cellTexCoordsModified";

//------------------------------------------------------------------------------

Mesh::Mesh(data::Object::Key /*unused*/)
{
    newSignal<signal_t>(s_VERTEX_MODIFIED_SIG);
    newSignal<signal_t>(s_POINT_COLORS_MODIFIED_SIG);
    newSignal<signal_t>(s_CELL_COLORS_MODIFIED_SIG);
    newSignal<signal_t>(s_POINT_NORMALS_MODIFIED_SIG);
    newSignal<signal_t>(s_CELL_NORMALS_MODIFIED_SIG);
    newSignal<signal_t>(s_POINT_TEX_COORDS_MODIFIED_SIG);
    newSignal<signal_t>(s_CELL_TEX_COORDS_MODIFIED_SIG);

    std::ranges::for_each(m_points, [](auto& array){array = data::Array::New();});
    std::ranges::for_each(m_cells, [](auto& array){array = data::Array::New();});
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
= default;

//------------------------------------------------------------------------------

void Mesh::shallowCopy(const Object::csptr& _source)
{
    Mesh::csptr other = Mesh::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);

    m_numPoints  = other->m_numPoints;
    m_attributes = other->m_attributes;
    m_numCells   = other->m_numCells;
    m_cellType   = other->m_cellType;

    std::copy(other->m_points.begin(), other->m_points.end(), m_points.begin());
    std::copy(other->m_cells.begin(), other->m_cells.end(), m_cells.begin());
}

//------------------------------------------------------------------------------

void Mesh::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Mesh::csptr other = Mesh::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !other
    );
    this->fieldDeepCopy(_source, cache);

    m_numPoints  = other->m_numPoints;
    m_attributes = other->m_attributes;
    m_numCells   = other->m_numCells;
    m_cellType   = other->m_cellType;

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(PointAttribute::_SIZE) ; ++i)
    {
        m_points[i] = data::Object::copy(other->m_points[i], cache);
    }

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(CellAttribute::_SIZE) ; ++i)
    {
        m_cells[i] = data::Object::copy(other->m_cells[i], cache);
    }
}

//------------------------------------------------------------------------------

std::size_t Mesh::reserve(Mesh::size_t nbPts, Mesh::size_t nbCells, CellType cellType, Attributes arrayMask)
{
    m_cellType = cellType;

    SIGHT_THROW_EXCEPTION_IF(data::Exception("Cannot not allocate empty size"), nbPts == 0 || nbCells == 0);

    m_points[static_cast<std::size_t>(PointAttribute::POSITION)]->resize(
        {3, nbPts},
        core::Type::get<position_t>()
    );

    m_attributes = m_attributes | arrayMask;

    // TODO sight 22.0: Add a second dimension on the array to replace the deprecated component

    // Test attributes mask, if present resize corresponding array, if not check if array needs to be cleared.
    if(static_cast<int>(arrayMask & Attributes::POINT_COLORS) != 0)
    {
        m_points[static_cast<std::size_t>(PointAttribute::COLORS)]->resize(
            {4, nbPts},
            core::Type(core::Type::UINT8)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::POINT_NORMALS) != 0)
    {
        m_points[static_cast<std::size_t>(PointAttribute::NORMALS)]->resize(
            {3, nbPts},
            core::Type(core::Type::FLOAT)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::POINT_TEX_COORDS) != 0)
    {
        m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)]->resize(
            {2, nbPts},
            core::Type(core::Type::FLOAT)
        );
    }

    m_cells[static_cast<std::size_t>(CellAttribute::INDEX)]->resize(
        {getCellSize(), nbCells},
        core::Type::get<cell_t>()
    );

    if(static_cast<int>(arrayMask & Attributes::CELL_COLORS) != 0)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::COLORS)]->resize(
            {4, nbCells},
            core::Type(core::Type::UINT8)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::CELL_NORMALS) != 0)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)]->resize(
            {3, nbCells},
            core::Type(core::Type::FLOAT)
        );
    }

    if(static_cast<int>(arrayMask & Attributes::CELL_TEX_COORDS) != 0)
    {
        m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)]->resize(
            {2, nbCells},
            core::Type(core::Type::FLOAT)
        );
    }

    return this->getAllocatedSizeInBytes();
}

//------------------------------------------------------------------------------
std::size_t Mesh::resize(Mesh::size_t nbPts, Mesh::size_t nbCells, CellType cellType, Attributes arrayMask)
{
    m_cellType = cellType;

    const auto size = this->reserve(nbPts, nbCells, cellType, arrayMask);
    m_numPoints = nbPts;
    m_numCells  = nbCells;
    return size;
}

//------------------------------------------------------------------------------

bool Mesh::shrinkToFit()
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

void Mesh::truncate(Mesh::size_t nbPts, Mesh::size_t nbCells)
{
    SIGHT_THROW_IF(
        "Can not truncate with a number of points higher than the allocated size",
        nbPts > m_points[0]->getSize().at(1)
    );
    SIGHT_THROW_IF(
        "Can not truncate with a number of cells higher than the allocated size",
        nbCells > m_cells[0]->getSize().at(1)
    );
    m_numPoints = nbPts;
    m_numCells  = nbCells;
}

//------------------------------------------------------------------------------

void Mesh::clear()
{
    std::ranges::for_each(m_points, [](auto& array){array->clear();});
    std::ranges::for_each(m_cells, [](auto& array){array->clear();});

    // Reset nbPoints, nbCells & cellsDataSize.
    m_numPoints = 0;
    m_numCells  = 0;

    m_attributes = Attributes::NONE;
}

//------------------------------------------------------------------------------

std::size_t Mesh::getDataSizeInBytes() const
{
    std::size_t size = 0;

    std::ranges::for_each(
        m_points,
        [&](auto& array)
        {
            const auto numComponents = array->empty() ? 1 : array->getSize()[0];
            size                    += array->getElementSizeInBytes() * numComponents
                                       * static_cast<std::size_t>(m_numPoints);
        });

    std::ranges::for_each(
        m_cells,
        [&](auto& array)
        {
            const auto numComponents = array->empty() ? 1 : array->getSize()[0];
            size                    += array->getElementSizeInBytes() * numComponents
                                       * static_cast<std::size_t>(m_numCells);
        });

    return size;
}

//------------------------------------------------------------------------------

std::size_t Mesh::getAllocatedSizeInBytes() const
{
    std::size_t size = 0;

    std::ranges::for_each(m_points, [&](auto& array){size += array->getSizeInBytes();});
    std::ranges::for_each(m_cells, [&](auto& array){size += array->getSizeInBytes();});

    return size;
}

//------------------------------------------------------------------------------

Mesh::point_t Mesh::pushPoint(const std::array<position_t, 3>& p)
{
    const auto nbPoints = m_numPoints;

    auto positions          = m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
    const auto allocatedPts = positions->empty() ? 0 : positions->getSize().at(1);

    if(allocatedPts <= nbPoints)
    {
        positions->resize({3, allocatedPts + POINT_REALLOC_STEP}, core::Type::get<position_t>(), true);
        if(this->has<data::Mesh::Attributes::POINT_COLORS>())
        {
            m_points[static_cast<std::size_t>(PointAttribute::COLORS)]->resize(
                {4, allocatedPts + POINT_REALLOC_STEP},
                core::Type::get<color_t>(),
                true
            );
        }

        if(this->has<data::Mesh::Attributes::POINT_NORMALS>())
        {
            m_points[static_cast<std::size_t>(PointAttribute::NORMALS)]->resize(
                {3, allocatedPts + POINT_REALLOC_STEP},
                core::Type::get<normal_t>(),
                true
            );
        }

        if(this->has<data::Mesh::Attributes::POINT_TEX_COORDS>())
        {
            m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)]->resize(
                {2, allocatedPts + POINT_REALLOC_STEP},
                core::Type::get<texcoord_t>(),
                true
            );
        }
    }

    this->setPoint(nbPoints, p);

    ++m_numPoints;
    return nbPoints;
}

//------------------------------------------------------------------------------

Mesh::point_t Mesh::pushPoint(position_t x, position_t y, position_t z)
{
    return this->pushPoint({x, y, z});
}

//------------------------------------------------------------------------------

Mesh::cell_t Mesh::pushCell(point_t idPt)
{
    std::vector pointIds = {idPt};
    return this->pushCell(pointIds);
}

//------------------------------------------------------------------------------

Mesh::cell_t Mesh::pushCell(point_t idP1, point_t idP2)
{
    return this->pushCell({idP1, idP2});
}

//------------------------------------------------------------------------------

Mesh::cell_t Mesh::pushCell(point_t idP1, point_t idP2, point_t idP3)
{
    return this->pushCell({idP1, idP2, idP3});
}

//------------------------------------------------------------------------------

Mesh::cell_t Mesh::pushCell(point_t idP1, point_t idP2, point_t idP3, point_t idP4)
{
    return this->pushCell({idP1, idP2, idP3, idP4});
}

//------------------------------------------------------------------------------

Mesh::cell_t Mesh::pushCell(const std::vector<point_t> pointIds)
{
    return this->pushCell(pointIds.data(), static_cast<point_t>(pointIds.size()));
}

//------------------------------------------------------------------------------

Mesh::cell_t Mesh::pushCell(const point_t* pointIds, std::size_t nbPoints)
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
    const auto allocatedCellData = cells->empty() ? 0 : cells->getSize().at(1);
    const auto cellSize          = getCellSize();
    if(allocatedCellData <= m_numCells)
    {
        cells->resize({cellSize, allocatedCellData + CELLDATA_REALLOC_STEP}, core::Type::get<cell_t>());
        if(this->has<data::Mesh::Attributes::CELL_COLORS>())
        {
            m_cells[static_cast<std::size_t>(CellAttribute::COLORS)]->resize(
                {4, allocatedCellData + CELLDATA_REALLOC_STEP},
                core::Type::get<color_t>(),
                true
            );
        }

        if(this->has<data::Mesh::Attributes::CELL_NORMALS>())
        {
            m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)]->resize(
                {3, allocatedCellData + CELLDATA_REALLOC_STEP},
                core::Type::get<normal_t>(),
                true
            );
        }

        if(this->has<data::Mesh::Attributes::CELL_TEX_COORDS>())
        {
            m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)]->resize(
                {2, allocatedCellData + CELLDATA_REALLOC_STEP},
                core::Type::get<texcoord_t>(),
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

void Mesh::setPoint(point_t id, const std::array<position_t, 3>& p)
{
    auto& points = m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
    points->at<position_t>(3LL * id)     = p[0];
    points->at<position_t>(3LL * id + 1) = p[1];
    points->at<position_t>(3LL * id + 2) = p[2];
}

//------------------------------------------------------------------------------

void Mesh::setPoint(
    point_t id,
    data::Mesh::position_t x,
    data::Mesh::position_t y,
    data::Mesh::position_t z
)
{
    this->setPoint(id, {x, y, z});
}

//------------------------------------------------------------------------------

void Mesh::setCell(cell_t id, point_t idPt)
{
    std::vector pointIds = {idPt};
    this->setCell(id, pointIds);
}

//------------------------------------------------------------------------------

void Mesh::setCell(cell_t id, point_t idP1, point_t idP2)
{
    this->setCell(id, {idP1, idP2});
}

//------------------------------------------------------------------------------

void Mesh::setCell(cell_t id, point_t idP1, point_t idP2, point_t idP3)
{
    this->setCell(id, {idP1, idP2, idP3});
}

//------------------------------------------------------------------------------

void Mesh::setCell(
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

void Mesh::setCell(cell_t id, const std::vector<point_t>& pointIds)
{
    this->setCell(id, pointIds.data(), static_cast<std::size_t>(pointIds.size()));
}

//------------------------------------------------------------------------------

void Mesh::setCell(cell_t id, const point_t* pointIds, std::size_t nbPoints)
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

void Mesh::setPointColor(point_t id, const std::array<data::Mesh::color_t, 4>& c)
{
    this->setPointColor(id, c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(
    point_t id,
    data::Mesh::color_t r,
    data::Mesh::color_t g,
    data::Mesh::color_t b,
    data::Mesh::color_t a
)
{
    auto& colors = m_points[static_cast<std::size_t>(PointAttribute::COLORS)];
    colors->at<color_t>(4LL * id)     = r;
    colors->at<color_t>(4LL * id + 1) = g;
    colors->at<color_t>(4LL * id + 2) = b;
    colors->at<color_t>(4LL * id + 3) = a;
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(cell_t id, const std::array<data::Mesh::color_t, 4>& c)
{
    this->setCellColor(id, c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(
    cell_t id,
    data::Mesh::color_t r,
    data::Mesh::color_t g,
    data::Mesh::color_t b,
    data::Mesh::color_t a
)
{
    auto& colors = m_cells[static_cast<std::size_t>(CellAttribute::COLORS)];
    colors->at<color_t>(4LL * id)     = r;
    colors->at<color_t>(4LL * id + 1) = g;
    colors->at<color_t>(4LL * id + 2) = b;
    colors->at<color_t>(4LL * id + 3) = a;
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(point_t id, const std::array<data::Mesh::normal_t, 3>& n)
{
    this->setPointNormal(id, n[0], n[1], n[2]);
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(
    point_t id,
    data::Mesh::normal_t nx,
    data::Mesh::normal_t ny,
    data::Mesh::normal_t nz
)
{
    auto& normals = m_points[static_cast<std::size_t>(PointAttribute::NORMALS)];
    normals->at<normal_t>(3LL * id)     = nx;
    normals->at<normal_t>(3LL * id + 1) = ny;
    normals->at<normal_t>(3LL * id + 2) = nz;
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(cell_t id, const std::array<data::Mesh::normal_t, 3>& n)
{
    this->setCellNormal(id, n[0], n[1], n[2]);
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(
    cell_t id,
    data::Mesh::normal_t nx,
    data::Mesh::normal_t ny,
    data::Mesh::normal_t nz
)
{
    auto& normals = m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)];
    normals->at<normal_t>(3LL * id)     = nx;
    normals->at<normal_t>(3LL * id + 1) = ny;
    normals->at<normal_t>(3LL * id + 2) = nz;
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(point_t id, const std::array<data::Mesh::texcoord_t, 2>& t)
{
    this->setPointTexCoord(id, t[0], t[1]);
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(
    point_t id,
    data::Mesh::texcoord_t u,
    data::Mesh::texcoord_t v
)
{
    auto& texCoords = m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)];
    texCoords->at<texcoord_t>(2LL * id)     = u;
    texCoords->at<texcoord_t>(2LL * id + 1) = v;
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(cell_t id, const std::array<data::Mesh::texcoord_t, 2>& t)
{
    this->setCellTexCoord(id, t[0], t[1]);
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(
    cell_t id,
    data::Mesh::texcoord_t u,
    data::Mesh::texcoord_t v
)
{
    auto& texCoords = m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)];
    texCoords->at<texcoord_t>(2LL * id)     = u;
    texCoords->at<texcoord_t>(2LL * id + 1) = v;
}

//------------------------------------------------------------------------------

void Mesh::dump_lock_impl(std::vector<core::memory::BufferObject::Lock>& locks) const
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

bool Mesh::operator==(const Mesh& other) const noexcept
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
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Mesh::operator!=(const Mesh& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::point::xyz>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::point::nxyz>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::point::uv>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::point::rgba>()
{
    return m_points[static_cast<std::size_t>(PointAttribute::COLORS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::point>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::line>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::triangle>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::quad>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::tetra>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::nxyz>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::uv>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::sptr Mesh::getArray<iterator::cell::rgba>()
{
    return m_cells[static_cast<std::size_t>(CellAttribute::COLORS)];
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::point::xyz>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::POSITION)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::point::nxyz>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::point::uv>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::point::rgba>() const
{
    return m_points[static_cast<std::size_t>(PointAttribute::COLORS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::point>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::line>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::triangle>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::quad>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::tetra>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::INDEX)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::nxyz>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::NORMALS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::uv>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::TEX_COORDS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API data::Array::csptr Mesh::getArray<iterator::cell::rgba>() const
{
    return m_cells[static_cast<std::size_t>(CellAttribute::COLORS)];
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::point::xyz>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::point::nxyz>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::point::uv>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::point::rgba>() const
{
    return numPoints();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::point>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::line>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::triangle>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::quad>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::tetra>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::nxyz>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::uv>() const
{
    return numCells();
}

//------------------------------------------------------------------------------

template<>
DATA_API std::size_t Mesh::numElements<iterator::cell::rgba>() const
{
    return numCells();
}

} //namespace sight::data
