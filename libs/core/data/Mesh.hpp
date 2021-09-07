/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "data/Array.hpp"
#include "data/config.hpp"
#include "data/Exception.hpp"
#include "data/factory/new.hpp"
#include "data/iterator/MeshIterators.hpp"

#include <core/macros.hpp>
#include <core/memory/IBuffered.hpp>

#include <boost/multi_array.hpp>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Mesh));

namespace sight::data
{

/**
 * @brief   Data holding a geometric structure composed of points, lines, triangles, quads or polygons.
 *
 * @section Structure Structure
 *
 * The mesh structure contains some information stocked in data::Array
 *
 * - An array (m_points) which contains point coord (x,y,z)
 * - An array (m_cellTypes) which contains cell type (TRIAN or QUAD for the moment)
 * - An array (m_cellData) which contains point indexes in m_points used to create cells, 3 indexes are necessary to
 * create a triangle cell, 4 for quad cell.
 * - An array (m_cellDataOffsets) which contains indexes relative to m_cellData, to retrieve the first point necessary
 * to the cell creation.
 * And some additional arrays to store the mesh attributes (normals, texture coordinates and colors for points and
 * cells).
 *
 * \b Example:
 * - m_nbPoints = number of mesh points  * 3
 * - m_points = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ]
 * - m_nbCells = number of mesh cells
 * - m_cellTypes.size = m_nbCells
 * - m_cellTypes = [TRIANGLE, TRIANGLE, QUAD, QUAD, TRIANGLE ... ]
 * - m_cellDataOffsets.size = m_nbCells
 * - m_cellDataOffsets = [0, 3, 6, 10, 14, ... ] (offset shifting in  m_cellData = +3 if triangle cell rr +4 if quad
 * cell)
 *
 * - m_cellsDataSize = m_nbCells * <nb_points_per_cell> (m_nbCells * 3 if only triangle cell)
 * - m_cellData = [0, 1, 2, 0, 1, 3, 0, 1, 3, 5... ] ( correspond to point id )
 *
 * Get the points coordinates of the third cell:
 * @verbatim
    m_cellTypes[2] => cell type = QUAD
    m_cellDataOffsets[2] => index in m_cellData of cell definition = 6
    index of p1 = m_cellData[6] = 0
    index of p2 = m_cellData[6+1] = 1
    index of p3 = m_cellData[6+2] = 3
    index of p4 = m_cellData[6+3] = 5
    p1 = [ x0=m_points[0]  y0 z0 ] ( 0 * 3 = 0 )
    p2 = [ x1=m_points[3]  y1 z1 ] ( 1 * 3 = 3 )
    p3 = [ x3=m_points[9]  y3 z3 ] ( 3 * 3 = 9 )
    p4 = [ x5=m_points[15] y5 z5 ] ( 5 * 3 = 15 )
   @endverbatim
 *
 * There is other arrays to stock normal by points, normal by edges, color by points or color by cells :
 * - Normal arrays contains normal vector (x,y,z)
 * - normals.size = number of mesh points (respc cells)
 *  -normals = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ]
 * - Color arrays contains RGBA colors
 * - colors.size = number of mesh points (respc cells) * 4
 * - colors = [ r0, g0, b0, a0, r1, g1, b1, a1, ... ]
 *
 * @section Usage Usage
 *
 * @subsection Allocation Allocation
 *
 * The two methods reserve() and resize() allocate the mesh arrays. The difference between the two methods is
 * that resize modifies the number of points and cells.
 *
 * The pushPoint() and pushCell() methods add new points or cells, they increment the number of points/cells and
 * allocate more memory if needed. It is recommended to call reserve() method before it if you know the number of
 * points and cells, it avoids allocating more memory than needed.
 *
 * The setPoint() and setCell() methods change the value of a point/cell at a given index.
 *
 * Example with resize(), setPoint() and setCell()
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock(); // prevents the buffers from being dumped on the disk

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                               = static_cast<uint8_t>(i);
        const data::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        const size_t value                                   = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const data::Mesh::ColorValueType val             = static_cast< data::Mesh::ColorValueType >(i);
        const data::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }
   @endcode
 *
 * Example with reseve(), pushPoint() and pushCell()
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();

    mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                               = static_cast<uint8_t>(i);
        const data::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        const size_t value                                   = 3*i;
        const auto id =
            mesh->pushPoint(static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(id, color);
        mesh->setPointNormal(id, normal);
        mesh->setPointTexCoord(id, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        const auto id = mesh->pushCell(i, i+1, i+2);

        const data::Mesh::ColorValueType val             = static_cast< data::Mesh::ColorValueType >(i);
        const data::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(id, color);
        mesh->setCellNormal(id, normal);
        mesh->setCellTexCoord(id, texCoords);
    }
   @endcode
 *
 * @subsection Iterators Iterators
 *
 * To access the mesh points and cells, you should use the following iterators:
 *  - data::iterator::PointIterator: to iterate through mesh points
 *  - data::iterator::ConstPointIterator: to iterate through mesh points read-only
 *  - data::iterator::CellIterator: to iterate through mesh cells
 *  - data::iterator::ConstCellIterator: to iterate through mesh cells read-only
 *
 * Example to iterate through points:
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::CellType::TRIANGLE);
    auto iter    = mesh->begin< data::iterator::PointIterator >();
    const auto iterEnd = mesh->end< data::iterator::PointIterator >();
    float p[3] = {12.f, 16.f, 18.f};

   for (; iter != iterEnd; ++iter)
   {
       iter->point->x = p[0];
       iter->point->y = p[1];
       iter->point->z = p[2];
   }
   @endcode
 *
 * Example to iterate through cells:
 *
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::CellType::TRIANGLE);
    auto iter         = mesh->begin< data::iterator::ConstCellIterator >();
    const auto endItr = mesh->end< data::iterator::ConstCellIterator >();

    auto itrPt = mesh->begin< data::iterator::ConstPointIterator >();
    float p[3];

    for(; iter != endItr; ++iter)
    {
        const auto nbPoints = iter->nbPoints;

        for(size_t i = 0 ; i < nbPoints ; ++i)
        {
            auto pIdx = static_cast< data::iterator::ConstCellIterator::difference_type >(iter->pointIdx[i]);

            data::iterator::ConstPointIterator pointItr(itrPt + pIdx);
            p[0] = pointItr->point->x;
            p[1] = pointItr->point->y;
            p[2] = pointItr->point->z;
        }
    }
   @endcode
 *
 * pushCell() and setCell() may not be very efficient, you can use CellIterator to define the cell. But be careful to
 * properly define all the cell attributes.
 *
 * Example of defining cells using iterators
 *
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::CellType::QUAD);
    auto it          = mesh->begin< data::iterator::CellIterator >();
    const auto itEnd = mesh->end< data::iterator::CellIterator >();

    const auto cellType = data::Mesh::CellType::QUAD;
    const size_t nbPointPerCell = 4;

    size_t count = 0;
    for (; it != itEnd; ++it)
    {
        // define the cell type and cell offset
        (*it->type)   = cellType;
        (*it->offset) = nbPointPerCell*count;

        // /!\ define the next offset to be able to iterate through point indices
        if (it != itEnd-1)
        {
            (*(it+1)->offset) = nbPointPerCell*(count+1);
        }

        // define the point indices
        for (size_t i = 0; i < 4; ++i)
        {
            data::Mesh::CellId ptIdx = val;
            it->pointIdx[i] = ptIdx;
        }
    }
   @endcode
 *
 * It is also possible to use the iterators as a standard std iterator, for example with std::copy.
 *
 * @code{.cpp}
    void copyPoints(const data::Mesh::csptr& origin, const data::Mesh::sptr& dest)
    {
        SIGHT_ASSERT("Meshes must have the same number of points",
                   origin->getNumberOfPoints() == dest->getNumberOfPoints());

        auto origIt = origin->begin< data::Mesh::ConstPointIterator >();
        auto origEnd = origin->end< data::Mesh::ConstPointIterator >();
        auto destIt = dest->begin< data::Mesh::PointIterator >();
        std::copy(origIt, origEnd, dest);
    }
   @endcode
 */
class DATA_CLASS_API Mesh : public data::Object,
                            public core::memory::IBuffered
{
public:

    SIGHT_DECLARE_CLASS(Mesh, data::Object, data::factory::New<Mesh>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Mesh))

    typedef std::map<std::string, data::Array::sptr> ArrayMapType;

    [[deprecated("replaced by CellType (sight 22.0)")]] typedef enum
    {
        NO_CELL = 0,
        POINT,
        EDGE,
        TRIANGLE,
        QUAD,
        POLY,
        TETRA
    } CellTypesEnum;

    [[deprecated("replaced by CellType (sight 22.0)")]] typedef enum
    {
        RGB  = 3,
        RGBA = 4
    } ColorArrayTypes;

    enum class Attributes : std::uint8_t
    {
        NONE             = 0,
        POINT_COLORS     = 1 << 1,
        POINT_NORMALS    = 1 << 2,
        CELL_COLORS      = 1 << 3,
        CELL_NORMALS     = 1 << 4,
        POINT_TEX_COORDS = 1 << 5,
        CELL_TEX_COORDS  = 1 << 6
    };

    typedef data::iterator::CellType CellType;

    typedef float PointValueType;
    typedef std::uint8_t ColorValueType;
    typedef float NormalValueType;
    typedef float TexCoordValueType;
    typedef std::uint8_t CellTypes;

    typedef iterator::PointIterator PointIterator;
    typedef iterator::ConstPointIterator ConstPointIterator;
    typedef iterator::CellIterator CellIterator;
    typedef iterator::ConstCellIterator ConstCellIterator;

    typedef data::iterator::CellId CellId;
    typedef data::iterator::PointId PointId;
    typedef data::iterator::Size Size;

    typedef std::vector<core::memory::BufferObject::Lock> LocksType;
    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Mesh(data::Object::Key key);

    /// Destructor
    DATA_API ~Mesh() override;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Allocate Mesh memory
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays with the information given by the
     * parameters.
     * It does not modify the number of points and cells.
     *
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param cellType type of cell to allocate, it defines the number of points by cell to allocate. If you want to
     *        mix different types of cells in the same mesh, you should use
     *        resize(size_t nbPts, size_t nbCells, size_t nbCellsData, Attribute arrayMask)
     * @param arrayMask Mesh attribute: additional Arrays to allocate
     *        (ex: Attribute::POINT_COLORS | Attribute::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise data::Exception if the memory can not be allocated.
     */
    DATA_API size_t reserve(
        Size nbPts,
        Size nbCells,
        CellType cellType    = CellType::TRIANGLE,
        Attributes arrayMask = Attributes::NONE
    );

    /**
     * @brief Allocate Mesh memory
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays with the information given by the
     * parameters.
     * It does not modify the number of points and cells.
     *
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param nbCellsData sum of the number of points of each cell, il allows to allocate the cells array.
     * @param arrayMask Mesh attribute: additional Arrays to allocate
     *        (ex: Attribute::POINT_COLORS | Attribute::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise data::Exception if the memory can not be allocated.
     */
    DATA_API size_t reserve(
        Size nbPts,
        Size nbCells,
        Size nbCellsData,
        Attributes arrayMask = Attributes::NONE
    );

    /**
     * @brief Allocate Mesh memory and initialize the number of points and cells
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays with the information given by the
     * parameters.
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param cellType type of cell to allocate, it defines the number of points by cell to allocate. If you want to
     *        use different types of cells in the same mesh, use
     *        resize(size_t nbPts, size_t nbCells, size_t nbCellsData, Attribute arrayMask)
     * @param arrayMask Mesh attribute: additional Arrays to allocate
     *        (ex: Attribute::POINT_COLORS | Attribute::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise data::Exception if the memory can not be allocated.
     */
    DATA_API size_t resize(
        Size nbPts,
        Size nbCells,
        CellType cellType    = CellType::TRIANGLE,
        Attributes arrayMask = Attributes::NONE
    );

    /**
     * @brief Allocate Mesh memory and initialize the number of points and cells
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays with the information given by the
     * parameters.
     *
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param nbCellsData sum of the number of points of each cell, il allows to allocate the cells array.
     * @param arrayMask Mesh attribute: additional Arrays to allocate
     *        (ex: Attribute::POINT_COLORS | Attribute::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise data::Exception if the memory can not be allocated.
     */
    DATA_API size_t resize(
        Size nbPts,
        Size nbCells,
        Size nbCellsData,
        Attributes arrayMask = Attributes::NONE
    );

    /**
     * @brief Adjust mesh memory usage
     *
     * The arrays (points, cell-types, cell-data, cell-data-offsets, and if they
     * exists point-colors/normals and cell-colors/normals) will be resized
     * according to the number of points and cells of the mesh (they may have
     * been defined before adjusting by setNumberOfPoints or setNumberOfCells).
     *
     * @return true if memory usage changed
     *
     * @throw Raise data::Exception if the memory can not be re-allocated.
     */
    DATA_API bool adjustAllocatedMemory();

    /**
     * @brief Clear mesh points.
     *
     * Reset the number of points to 0. It doesn't impact memory allocation.
     */
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API void clearPoints();

    /**
     * @brief Clear mesh cells.
     *
     * Reset the number of cells and celldata size to 0. It doesn't impact memory allocation.
     */
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API void clearCells();

    /**
     * @brief Remove all data contained in the mesh. Memory is freed.
     */
    DATA_API void clear();

    /// Clear corresponding array, memory is freed.
    DATA_API void clearPointNormals();

    /// Clear corresponding array, memory is freed.
    DATA_API void clearPointColors();

    /// Clear corresponding array, memory is freed.
    DATA_API void clearPointTexCoords();

    /// Clear corresponding array, memory is freed.
    DATA_API void clearCellNormals();

    /// Clear corresponding array, memory is freed.
    DATA_API void clearCellColors();

    /// Clear corresponding array, memory is freed.
    DATA_API void clearCellTexCoords();

    /// Set number of points.
    DATA_API void setNumberOfPoints(Size nb);

    /// Get number of points.
    DATA_API Size getNumberOfPoints() const;

    /// Set number of cells.
    DATA_API void setNumberOfCells(Size nb);

    /// Get number of cells.
    DATA_API Size getNumberOfCells() const;

    /// Set cell data size.
    DATA_API void setCellDataSize(Size nb);

    /// Get cell data size.
    DATA_API Size getCellDataSize() const;

    /**
     * @brief Get the mesh data size in bytes.
     *
     * @return Mesh data size in bytes
     * @note The allocated memory may be greater than the data size in bytes.
     */
    DATA_API size_t getDataSizeInBytes() const;

    /**
     * @brief Get the amount of memory allocated in this mesh. Mey be bigger
     * than getDataSizeInBytes().
     *
     * @return Mesh data size in bytes
     * @note You can call adjustAllocatedMemory() to free extra memory.
     */
    DATA_API size_t getAllocatedSizeInBytes() const;

    /**
     * @name Signals
     * @{
     */

    /// Type of signal when vertex are modified
    typedef core::com::Signal<void ()> VertexModifiedSignalType;

    /// Key in m_signals map of signal m_sigVertexModified
    DATA_API static const core::com::Signals::SignalKeyType s_VERTEX_MODIFIED_SIG;

    /// Type of signal when point colors are modified
    typedef core::com::Signal<void ()> PointColorsModifiedSignalType;

    /// Key in m_signals map of signal m_sigPointColorsModified
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_COLORS_MODIFIED_SIG;

    /// Type of signal when cell colors are modified
    typedef core::com::Signal<void ()> CellColorsModifiedSignalType;

    /// Key in m_signals map of signal m_sigCellColorsModified
    DATA_API static const core::com::Signals::SignalKeyType s_CELL_COLORS_MODIFIED_SIG;

    /// Type of signal when point normals are modified
    typedef core::com::Signal<void ()> PointNormalsModifiedSignalType;

    /// Key in m_signals map of signal m_sigPointNormalsModified
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_NORMALS_MODIFIED_SIG;

    /// Type of signal when cell normals are modified
    typedef core::com::Signal<void ()> CellNormalsModifiedSignalType;

    /// Key in m_signals map of signal m_sigCellNormalsModified
    DATA_API static const core::com::Signals::SignalKeyType s_CELL_NORMALS_MODIFIED_SIG;

    /// Type of signal when point tex coords are modified
    typedef core::com::Signal<void ()> PointTexCoordsModifiedSignalType;

    /// Key in m_signals map of signal m_sigPointTexCoorddModified
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_TEX_COORDS_MODIFIED_SIG;

    /// Type of signal when cell tex coords are modified
    typedef core::com::Signal<void ()> CellTexCoordsModifiedSignalType;

    /// Key in m_signals map of signal m_sigCellTexCoorddModified
    DATA_API static const core::com::Signals::SignalKeyType s_CELL_TEX_COORDS_MODIFIED_SIG;
    /// @}

    /**
     *  @{
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @return The id of the new point
     *
     * @throw data::Exception if the allocation failed
     */
    DATA_API PointId pushPoint(const PointValueType p[3]);
    DATA_API PointId pushPoint(
        PointValueType x,
        PointValueType y,
        PointValueType z
    );
    /// @}
    /**
     * @{
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh's concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw data::Exception if the allocation failed
     */
    DATA_API CellId pushCell(PointId idPt);
    DATA_API CellId pushCell(PointId idP1, PointId idP2);
    DATA_API CellId pushCell(PointId idP1, PointId idP2, PointId idP3);
    DATA_API CellId pushCell(
        PointId idP1,
        PointId idP2,
        PointId idP3,
        PointId idP4,
        CellType type = CellType::QUAD
    );
    DATA_API CellId pushCell(
        CellType type,
        const std::vector<PointId> pointIds
    );
    DATA_API CellId pushCell(
        CellType type,
        const PointId* pointIds,
        Size nbPoints
    );
    /// @}

    /**
     * @brief Set a point's coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p point coordinates
     * @throw Raise data::Exception if the id is out of bounds
     */
    DATA_API void setPoint(PointId id, const data::Mesh::PointValueType p[3]);

    /**
     * @brief Set a point coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @see setPoint
     * @throw Raise data::Exception if the id is out of bounds
     */
    DATA_API void setPoint(PointId id, PointValueType x, PointValueType y, PointValueType z);

    /**
     * @{
     * @brief Set a cell into the mesh.
     *
     * @warning Use this method carefully, the cell should be properly allocated. If the current cell does not contain
     * as many points as the previous one the following cells will be corrupted.
     *
     * @throw data::Exception if the mesh is not correctly allocated (ie. the id is out of bounds)
     */
    DATA_API void setCell(CellId id, PointId idPt);
    DATA_API void setCell(CellId id, PointId idP1, PointId idP2);
    DATA_API void setCell(CellId id, PointId idP1, PointId idP2, PointId idP3);
    DATA_API void setCell(
        CellId id,
        PointId idP1,
        PointId idP2,
        PointId idP3,
        PointId idP4,
        CellType type = CellType::QUAD
    );
    DATA_API void setCell(CellId id, CellType type, const std::vector<PointId>& pointIds);
    DATA_API void setCell(CellId id, CellType type, const PointId* pointIds, Size nbPoints);
    /// @}

    /**
     * @{
     * @brief Set a point's color.
     *
     * @warning The point colors must be allocated with 4 components (RBGA)
     *
     * @param id point index
     * @param c color
     */
    DATA_API void setPointColor(PointId id, const std::array<ColorValueType, 4>& c);
    DATA_API void setPointColor(PointId id, ColorValueType r, ColorValueType g, ColorValueType b, ColorValueType a);
    /// @}
    ///
    /**
     * @{
     * @brief Set a cell's color.
     *
     * @warning The cell colors must be allocated with 4 components (RBGA)
     *
     * @param id cell index
     * @param c color
     */
    DATA_API void setCellColor(CellId id, const std::array<ColorValueType, 4>& c);
    DATA_API void setCellColor(CellId id, ColorValueType r, ColorValueType g, ColorValueType b, ColorValueType a);
    /// @}

    /**
     * @{
     * @brief Set a point's normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id point index
     * @param n normal
     */
    DATA_API void setPointNormal(PointId id, const std::array<NormalValueType, 3>& n);
    DATA_API void setPointNormal(PointId id, NormalValueType nx, NormalValueType ny, NormalValueType nz);
    ///@}
    /**
     * @{
     * @brief Set a cell's normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id cell index
     * @param n normal
     */
    DATA_API void setCellNormal(CellId id, const std::array<NormalValueType, 3>& n);
    DATA_API void setCellNormal(
        CellId id,
        NormalValueType nx,
        NormalValueType ny,
        NormalValueType nz
    );
    /// @}
    /**
     * @{
     * @brief Set a point's texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id point index
     * @param t texCoord
     */
    DATA_API void setPointTexCoord(PointId id, const std::array<TexCoordValueType, 2>& t);
    DATA_API void setPointTexCoord(
        PointId id,
        TexCoordValueType u,
        TexCoordValueType v
    );
    /// @}

    /**
     * @{
     * @brief Set a cell's texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id cell index
     * @param t texCoord
     */
    DATA_API void setCellTexCoord(CellId id, const std::array<TexCoordValueType, 2>& t);
    DATA_API void setCellTexCoord(
        CellId id,
        TexCoordValueType u,
        TexCoordValueType v
    );
    /// @}

    /**
     * @brief Returns the begin/end iterators to the mesh buffers
     *
     * @tparam ITERATOR iterator type:
     *  - data::iterator::PointIterator: to iterate through mesh points
     *  - data::iterator::ConstPointIterator: to iterate through mesh points read-only
     *  - data::iterator::CellIterator: to iterate through mesh cells
     *  - data::iterator::ConstCellIterator: to iterate through mesh cells read-only
     *
     * @warning If the Mesh is 'const', only "Const" iterators are available (data::iterator::ConstPointIterator and
     * data::iterator::ConstCellIterator)
     * @note These iterators lock the buffers for dump (see lock())
     * @{
     */
    template<typename ITERATOR>
    ITERATOR begin();
    template<typename ITERATOR>
    ITERATOR end();
    template<typename ITERATOR>
    ITERATOR begin() const;
    template<typename ITERATOR>
    ITERATOR end() const;
    /// @}

    /**
     * @brief Return a lock on the mesh to prevent from dumping the buffers on the disk
     *
     * The buffer cannot be accessed if the mesh is not locked
     *
     * @warning You must allocate all the mesh's arrays before calling lock()
     */
    [[nodiscard]] DATA_API LocksType lock() const;

    /// Return true if the mesh has point colors
    bool hasPointColors() const;

    /// Return true if the mesh has cell colors
    bool hasCellColors() const;

    /// Return true if the mesh has point normals
    bool hasPointNormals() const;

    /// Return true if the mesh has cell normals
    bool hasCellNormals() const;

    /// Return true if the mesh has point texture coordinates
    bool hasPointTexCoords() const;

    /// Return true if the mesh has cell texture coordinates
    bool hasCellTexCoords() const;

    /**
     * @brief Return true if the point color are RGB (used for temporary support of RGB colors)
     * @pre Point colors must be allocated
     */
    bool hasRGBPointColors() const;
    /**
     * @brief Return true if the cell color are RGB (used for temporary support of RGB colors)
     * @pre Cell colors must be allocated
     */
    bool hasRGBCellColors() const;

    /// Returns current Attributes of the Mesh as binary mask @see data::Mesh::Attributes
    Attributes getAttributes() const;

    /// Sets current Attributes of the Mesh as binary mask @see data::Mesh::Attributes
    /// @warning: Attributes are set automatically or using resize()/reserve methode, usually it is not recommended to
    /// call this method.
    ///  This is only used when converting from/to fwAtoms
    void setAttributes(const Attributes& _attributes);

    /**
     * @brief Return the buffer of point colors.
     *
     * It is used for efficient conversion between our mesh and external libraries mesh.
     *
     * @throw data::Exception The buffer cannot be accessed if the mesh is not locked
     * @note For temporary support of RGB colors, use hasRGBPointColors() to know the buffer size :
     * numberOfPoints * 4 for RGBA colors or numberOfPoints * 3 for RGB.
     */
    DATA_API const ColorValueType* getPointColorsBuffer() const;

    /**
     * @brief Return the buffer of cell colors.
     *
     * It is used for efficient conversion between our mesh and external libraries mesh.
     *
     * @throw data::Exception The buffer cannot be accessed if the mesh is not locked
     * @note For temporary support of RGB colors, use hasRGBPCellColors() to know the buffer size :
     * numberOfCells * 4 for RGBA colors or numberOfPCells * 3 for RGB.
     */
    DATA_API const ColorValueType* getCellColorsBuffer() const;

    // ---------------------------------------
    // Deprecated API for sight 22.0
    // ---------------------------------------

    [[deprecated]] typedef boost::multi_array_ref<PointValueType, 2> PointsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<CellTypes, 1> CellTypesMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<CellId, 1> CellDataMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<CellId, 1> CellDataOffsetsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<ColorValueType, 2> PointColorsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<ColorValueType, 2> CellColorsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<NormalValueType, 2> PointNormalsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<NormalValueType, 2> CellNormalsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<TexCoordValueType, 2> PointTexCoordsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<TexCoordValueType, 2> CellTexCoordsMultiArrayType;

    [[deprecated]] typedef boost::const_multi_array_ref<PointValueType, 2> ConstPointsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<CellTypes, 1> ConstCellTypesMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<CellId, 1> ConstCellDataMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<CellId, 1> ConstCellDataOffsetsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<ColorValueType, 2> ConstPointColorsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<ColorValueType, 2> ConstCellColorsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<NormalValueType, 2> ConstPointNormalsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<NormalValueType, 2> ConstCellNormalsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<TexCoordValueType, 2> ConstPointTexCoordsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<TexCoordValueType, 2> ConstCellTexCoordsMultiArrayType;

    /**
     * @brief Allocate Mesh memory
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays
     * with informations given by the parameters. if nbCellData is zero, it
     * will be initialized to 3*nbCells
     * @param nbPts
     * @param nbCells
     * @param nbCellsData
     *
     * @deprecated Call resize(nbPoints, nbCells, cellType, extraArrays) to allocate the arrays.
     *
     * @throw data::Exception
     */
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocate(size_t nbPts, size_t nbCells, size_t nbCellsData = 0);

    /// Allocates normals array according to the number of points of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocatePointNormals();

    /// Allocates colors array according to the number of points of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocatePointColors(ColorArrayTypes t);

    /// Allocates texCoords array according to the number of points of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocatePointTexCoords();

    /// Allocates normals array according to the number of cells of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocateCellNormals();

    /// Allocates colors array according to the number of cells of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocateCellColors(ColorArrayTypes t);

    /// Allocates texCoords array according to the number of cells of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    DATA_API size_t allocateCellTexCoords();

    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setPointsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setCellTypesArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setCellDataArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setCellDataOffsetsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setPointColorsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setCellColorsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setPointNormalsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setCellNormalsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setPointTexCoordsArray(const data::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void setCellTexCoordsArray(const data::Array::sptr& array);

    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getPointsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getCellTypesArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getCellDataArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getCellDataOffsetsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getPointColorsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getCellColorsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getPointNormalsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getCellNormalsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getPointTexCoordsArray() const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getCellTexCoordsArray() const;

    /// Add an array in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void addDataArray(const std::string& name, data::Array::sptr array);

    /// Get an array in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API data::Array::sptr getDataArray(const std::string& name) const;

    /// Remove an array in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API void removeDataArray(const std::string& name);

    /// Return all array names stock in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    DATA_API std::vector<std::string> getDataArrayNames() const;

protected:

    // make iterator friend in order to access mesh arrays
    friend class data::iterator::PointIterator;
    friend class data::iterator::ConstPointIterator;
    friend class data::iterator::CellIterator;
    friend class data::iterator::ConstCellIterator;

    template<class DATATYPE>
    friend class data::mt::locked_ptr;

    /**
     * @brief Add a lock on the mesh in the given vector to prevent from dumping the buffer on the disk
     *
     * This is needed for IBuffered interface implementation
     */
    DATA_API void lockBuffer(std::vector<core::memory::BufferObject::Lock>& locks) const override;

    /// Number of points defined for the mesh
    Size m_nbPoints;

    /// Number of cells defined for the mesh
    Size m_nbCells;

    /// Number of point index defined for mesh (size of m_cellData)
    Size m_cellsDataSize;

    /**
     * @brief Mesh point array : 3-components 1-dimension float array, size = m_nbPoints x 3.
     *
     * This array contains points : [ x1 y1 z1 x2 y2 z2 ... xn yn zn ]
     */
    data::Array::sptr m_points;

    /**
     * @brief Cell types array : 1-components 1-dimension uint8 array, size = m_nbCells.
     *
     * This array each cell type : [ TRIANGLE QUAD QUAD ... TRIANGLE TRIANGLE QUAD ]
     */
    data::Array::sptr m_cellTypes;

    /**
     * @brief Cell data array : 1-components 1-dimension uint64 array, size = m_cellsDataSize.
     *
     * Contains cell data : cell points ids are contiguously stored regardless
     * of the cell type. Size depends of cell type. If we have only TRIANGLE type, size = m_nbCells x 3.
     *
     * This array contains point indexes (index in m_points) : [ TRIAN_ID1, TRIAN_ID2, TRIAN_ID3, QUAD_ID1, QUAD_ID2,
     * QUAD_ID3, QUAD_ID4, ... ]
     */
    data::Array::sptr m_cellData;

    /**
     * @brief Cell data offsets array : 1-components 1-dimension uint64 array, size = m_nbCells.
     *
     * Contains cell data offsets : for each cell, gives the offset of
     * the first item of this cell in m_cellData.
     *
     * This array contains cell indexes m_cellData : [ INDEX_TRIAN_1, INDEX_QUAD_1, INDEX_QUAD_2 ...  ]
     */
    data::Array::sptr m_cellDataOffsets;

    /**
     * @brief point colors array : 3 or 4-components 1-dimension float array, size = m_nbPoints.
     *
     * This array contains point colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     */
    data::Array::sptr m_pointColors;

    /**
     * @brief Mesh point array : 3 or 4-components 1-dimension uint8_t array, size = m_nbCells.
     *
     * This array contains cell colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     */
    data::Array::sptr m_cellColors;

    /**
     * @brief Mesh point array : 3-components 1-dimension uint8_t array, size = m_nbPoints.
     *
     * This array contains point normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     */
    data::Array::sptr m_pointNormals;

    /**
     * @brief Mesh point array : 3-components 1-dimension float array, size = m_nbCells.
     *
     * This array contains cell normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     */
    data::Array::sptr m_cellNormals;

    /**
     * @brief Mesh texCoord array : 2-components 1-dimension float array, size = m_nbPoints.
     *
     * This array contains point texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    data::Array::sptr m_pointTexCoords;

    /**
     * @brief Mesh texCoord array : 2-components 1-dimension float array, size = m_nbCells.
     *
     * This array contains cell texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    data::Array::sptr m_cellTexCoords;

    /// Array map where you can add few additional arrays registered thanks to a key to perform/conserve some specific
    /// analysis.
    /// @deprecated Will be removed in sight 22.0
    [[deprecated("sight 22.0")]] ArrayMapType m_arrayMap;

    /// Stores current attributes.
    Attributes m_attributes {Attributes::NONE};
};

//------------------------------------------------------------------------------

inline Mesh::Attributes operator|(const Mesh::Attributes& lhs, const Mesh::Attributes& rhs)
{
    return static_cast<Mesh::Attributes>(
        static_cast<std::underlying_type<Mesh::Attributes>::type>(lhs)
        | static_cast<std::underlying_type<Mesh::Attributes>::type>(rhs)
    );
}

//------------------------------------------------------------------------------

inline Mesh::Attributes operator&(const Mesh::Attributes& lhs, const Mesh::Attributes& rhs)
{
    return static_cast<Mesh::Attributes>(
        static_cast<std::underlying_type<Mesh::Attributes>::type>(lhs)
        & static_cast<std::underlying_type<Mesh::Attributes>::type>(rhs)
    );
}

//------------------------------------------------------------------------------

inline Mesh::Attributes operator~(const Mesh::Attributes& lhs)
{
    return static_cast<Mesh::Attributes>(~static_cast<std::underlying_type<Mesh::Attributes>::type>(lhs));
}

//------------------------------------------------------------------------------

template<typename ITERATOR>
inline ITERATOR Mesh::begin()
{
    return ITERATOR(this);
}

//------------------------------------------------------------------------------

template<typename ITERATOR>
inline ITERATOR Mesh::end()
{
    auto itr = ITERATOR(this);
    itr += static_cast<typename ITERATOR::difference_type>(itr.m_numberOfElements);
    return itr;
}

//------------------------------------------------------------------------------

template<typename ITERATOR>
inline ITERATOR Mesh::begin() const
{
    return ITERATOR(this);
}

//------------------------------------------------------------------------------

template<typename ITERATOR>
inline ITERATOR Mesh::end() const
{
    auto itr = ITERATOR(this);
    itr += static_cast<typename ITERATOR::difference_type>(itr.m_numberOfElements);
    return itr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasPointColors() const
{
    return static_cast<bool>(m_attributes & Mesh::Attributes::POINT_COLORS);
}

//------------------------------------------------------------------------------

inline bool Mesh::hasCellColors() const
{
    return static_cast<bool>(m_attributes & Mesh::Attributes::CELL_COLORS);
}

//------------------------------------------------------------------------------

inline bool Mesh::hasPointNormals() const
{
    return static_cast<bool>(m_attributes & Mesh::Attributes::POINT_NORMALS);
}

//------------------------------------------------------------------------------

inline bool Mesh::hasCellNormals() const
{
    return static_cast<bool>(m_attributes & Mesh::Attributes::CELL_NORMALS);
}

//------------------------------------------------------------------------------

inline bool Mesh::hasPointTexCoords() const
{
    return static_cast<bool>(m_attributes & Mesh::Attributes::POINT_TEX_COORDS);
}

//------------------------------------------------------------------------------

inline bool Mesh::hasCellTexCoords() const
{
    return static_cast<bool>(m_attributes & Mesh::Attributes::CELL_TEX_COORDS);
}

//------------------------------------------------------------------------------

inline bool Mesh::hasRGBPointColors() const
{
    return m_pointColors->getNumberOfComponents() == 3;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasRGBCellColors() const
{
    return m_cellColors->getNumberOfComponents() == 3;
}

//------------------------------------------------------------------------------

inline Mesh::Attributes Mesh::getAttributes() const
{
    return m_attributes;
}

//------------------------------------------------------------------------------

inline void Mesh::setAttributes(const Mesh::Attributes& _attributes)
{
    m_attributes = _attributes;
}

//------------------------------------------------------------------------------

} // namespace sight::data
