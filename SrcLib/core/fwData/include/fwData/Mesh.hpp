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

#pragma once

#include "fwData/Array.hpp"
#include "fwData/config.hpp"
#include "fwData/Exception.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/iterator/MeshIterators.hpp"

#include <fwCore/macros.hpp>

#include <boost/multi_array.hpp>

fwCampAutoDeclareDataMacro((fwData)(Mesh), FWDATA_API);

namespace fwData
{

/**
 * @brief   Data holding a geometric structure composed of points, lines, triangles, quads or polygons.
 *
 * It is the new structure that represent mesh in sight. For the moment, this new structure is available
 * to register mesh with triangle cells, with quad cells or with triangle and quad cells. Peharps, in future
 * work other cell type will be available (ex: POLYGON)
 *
 * This structure contains some information stock in ::fwData::Array \n
 *
 * An array (m_points) which contains point coord (x,y,z) \n
 * An array (m_cellTypes) which contains cell type (TRIAN or QUAD for the moment) \n
 * An array (m_cellData) which contains point indexes in m_points used to create cells, 3 indexes are necessary to
 * create a triangle cell, 4 for quad cell. \n
 * An array (m_cellDataOffsets) which contains indexes relative to m_cellData, to retrieve the first point necessary
 * to the cell creation. \n
 *
 * Example : \n
 * m_points.size = number of mesh points  * 3 \n
 * m_points = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ] \n
 * m_cellTypes.size = number of mesh cells \n
 * m_cellTypes = [TRIANGLE, TRIANGLE, QUAD, QUAD, TRIANGLE ... ] \n
 * m_cellDataOffsets.size = number of mesh cells \n
 * m_cellDataOffsets = [0, 3, 6, 10, 14, ... ] (offset shifting in  m_cellData = +3 if triangle cell rr +4 if quad cell)
 * \n
 * number of mesh cells * 3 (if only triangle cell) < m_cellData.size < number of mesh cells * 4 (if only quad cell) \n
 * m_cellData = [0, 1, 2, 0, 1, 3, 0, 1, 3, 5... ] ( correspond to point id ) \n
 *
 * Get the points coordinates of the third cell \n
 * m_cellTypes[2] => cell type = QUAD \n
 * m_cellDataOffsets[2] => index in m_cellData of cell definition = 6 \n
 * index of p1 = m_cellData[6] = 0 \n
 * index of p2 = m_cellData[6+1] = 1 \n
 * index of p3 = m_cellData[6+2] = 3 \n
 * index of p4 = m_cellData[6+3] = 5 \n
 * p1 = [ x0=m_points[0]  y0 z0 ] ( 0 * 3 = 0 ) \n
 * p2 = [ x1=m_points[3]  y1 z1 ] ( 1 * 3 = 3 ) \n
 * p3 = [ x3=m_points[9]  y3 z3 ] ( 3 * 3 = 9 ) \n
 * p4 = [ x5=m_points[15] y5 z5 ] ( 5 * 3 = 15 ) \n
 *
 * We have another array to stock normal by points, normal by edges, color by points or color by cells, to short : \n
 * Normal arrays contains normal vector (x,y,z) \n
 * normals.size = number of mesh points (respc cells)  * 3 \n
 * normals = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ] \n
 * Color arrays contains RGB or RGBA colors ( check the number of components 3 or 4 of array ) \n
 * colors.size = number of mesh points (respc cells) * 3 (respc 4) \n
 * colors = [ r0, g0, b0, r1, g1, b1, ... ] \n
 * or colors = [ r0, g0, b0, a0, r1, g1, b1, a1, ... ] \n
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
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                               = static_cast<uint8_t>(i);
        const ::fwData::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const ::fwData::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const ::fwData::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        const size_t value                                   = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const ::fwData::Mesh::ColorValueType val             = static_cast< ::fwData::Mesh::ColorValueType >(i);
        const ::fwData::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const ::fwData::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const ::fwData::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }
   @endcode
 *
 * Example with reseve(), pushPoint() and pushCell()
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                               = static_cast<uint8_t>(i);
        const ::fwData::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const ::fwData::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const ::fwData::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
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

        const ::fwData::Mesh::ColorValueType val             = static_cast< ::fwData::Mesh::ColorValueType >(i);
        const ::fwData::Mesh::ColorValueType color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const ::fwData::Mesh::NormalValueType normal[3]      = {floatVal, floatVal, floatVal};
        const ::fwData::Mesh::TexCoordValueType texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(id, color);
        mesh->setCellNormal(id, normal);
        mesh->setCellTexCoord(id, texCoords);
    }
   @endcode
 *
 * @subsection Iterators Iterators
 *
 * To access the mesh points and cells, you should use the following iterators:
 *  - ::fwData::iterator::PointIterator: to iterate through mesh points
 *  - ::fwData::iterator::ConstPointIterator: to iterate through mesh points read-only
 *  - ::fwData::iterator::CellIterator: to iterate through mesh cells
 *  - ::fwData::iterator::ConstCellIterator: to iterate through mesh cells read-only
 *
 * Example to iterate through points:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter    = mesh->begin< ::fwData::iterator::PointIterator >();
    const auto iterEnd = mesh->end< ::fwData::iterator::PointIterator >();
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
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter         = mesh->begin< ::fwData::iterator::ConstCellIterator >();
    const auto endItr = mesh->end< ::fwData::iterator::ConstCellIterator >();

    auto itrPt = mesh->begin< ::fwData::iterator::ConstPointIterator >();
    float p[3];

    for(; iter != endItr; ++iter)
    {
        const auto nbPoints = iter->nbPoints;

        for(size_t i = 0 ; i < nbPoints ; ++i)
        {
            auto pIdx = static_cast< ::fwData::iterator::ConstCellIterator::difference_type >(iter->pointIdx[i]);

            ::fwData::iterator::ConstPointIterator pointItr(itrPt + pIdx);
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
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::QUAD);
    auto it          = mesh->begin< ::fwData::iterator::CellIterator >();
    const auto itEnd = mesh->end< ::fwData::iterator::CellIterator >();

    const auto cellType = ::fwData::Mesh::QUAD;
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
            ::fwData::Mesh::CellValueType ptIdx = val;
            it->pointIdx[i] = ptIdx;
        }
    }
   @endcode
 */
class FWDATA_CLASS_API Mesh : public ::fwData::Object
{

public:

    fwCoreClassMacro(Mesh, ::fwData::Object, ::fwData::factory::New< Mesh >)

    fwCampMakeFriendDataMacro((fwData)(Mesh))

    typedef std::map< std::string, ::fwData::Array::sptr > ArrayMapType;
    typedef std::uint64_t Id;

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

    enum class CellType : std::uint8_t
    {
        NO_CELL = 0,
        POINT,
        EDGE,
        TRIANGLE,
        QUAD,
        POLY,
        TETRA
    };

    [[deprecated("replaced by CellType (sight 22.0)")]] typedef enum
    {
        RGB  = 3,
        RGBA = 4
    } ColorArrayTypes;

    enum class Attributes : std::uint8_t
    {
        NONE                 = 0,
        POINT_COLORS         = 1 << 1,
            POINT_NORMALS    = 1 << 2,
            CELL_COLORS      = 1 << 3,
            CELL_NORMALS     = 1 << 4,
            POINT_TEX_COORDS = 1 << 5,
            CELL_TEX_COORDS  = 1 << 6
    };

    typedef float PointValueType;
    typedef std::uint8_t ColorValueType;
    typedef float NormalValueType;
    typedef float TexCoordValueType;
    typedef Id CellValueType;
    typedef Id CellDataOffsetType;
    typedef std::uint8_t CellTypes;

    typedef std::vector< ::fwMemory::BufferObject::Lock > LocksType;
    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Mesh(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Mesh() override;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

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
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t reserve(size_t nbPts, size_t nbCells, CellType cellType = CellType::TRIANGLE,
                              Attributes arrayMask = Attributes::NONE);

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
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t reserve(size_t nbPts, size_t nbCells, size_t nbCellsData,
                              Attributes arrayMask = Attributes::NONE);

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
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t resize(size_t nbPts, size_t nbCells, CellType cellType = CellType::TRIANGLE,
                             Attributes arrayMask = Attributes::NONE);

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
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t resize(size_t nbPts, size_t nbCells, size_t nbCellsData,
                             Attributes arrayMask = Attributes::NONE);

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
     * @throw Raise ::fwData::Exception if the memory can not be re-allocated.
     */
    FWDATA_API bool adjustAllocatedMemory();

    /**
     * @brief Clear mesh points.
     *
     * Reset the number of points to 0. It doesn't impact memory allocation.
     */
    FWDATA_API void clearPoints();

    /**
     * @brief Clear mesh cells.
     *
     * Reset the number of cells and celldata size to 0. It doesn't impact memory allocation.
     */
    FWDATA_API void clearCells();

    /**
     * @brief Remove all data contained in the mesh. Memory is freed.
     */
    FWDATA_API void clear();

    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearPointNormals();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearPointColors();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearPointTexCoords();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearCellNormals();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearCellColors();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearCellTexCoords();

    /// Set number of points.
    FWDATA_API void setNumberOfPoints(Id nb);

    /// Get number of points.
    FWDATA_API Id getNumberOfPoints() const;

    /// Set number of cells.
    FWDATA_API void setNumberOfCells(Id nb);

    /// Get number of cells.
    FWDATA_API Id getNumberOfCells() const;

    /// Set cell data size.
    FWDATA_API void setCellDataSize(Id nb);

    /// Get cell data size.
    FWDATA_API Id getCellDataSize() const;

    /**
     * @brief Get the mesh data size in bytes.
     *
     * @return Mesh data size in bytes
     * @note The allocated memory may be greater than the data size in bytes.
     */
    FWDATA_API size_t getDataSizeInBytes() const;

    /**
     * @brief Get the amount of memory allocated in this mesh. Mey be bigger
     * than getDataSizeInBytes().
     *
     * @return Mesh data size in bytes
     * @note You can call adjustAllocatedMemory() to free extra memory.
     */
    FWDATA_API size_t getAllocatedSizeInBytes() const;

    /**
     * @name Signals
     * @{
     */

    /// Type of signal when vertex are modified
    typedef ::fwCom::Signal< void () > VertexModifiedSignalType;

    /// Key in m_signals map of signal m_sigVertexModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_VERTEX_MODIFIED_SIG;

    /// Type of signal when point colors are modified
    typedef ::fwCom::Signal< void () > PointColorsModifiedSignalType;

    /// Key in m_signals map of signal m_sigPointColorsModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_COLORS_MODIFIED_SIG;

    /// Type of signal when cell colors are modified
    typedef ::fwCom::Signal< void () > CellColorsModifiedSignalType;

    /// Key in m_signals map of signal m_sigCellColorsModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_CELL_COLORS_MODIFIED_SIG;

    /// Type of signal when point normals are modified
    typedef ::fwCom::Signal< void () > PointNormalsModifiedSignalType;

    /// Key in m_signals map of signal m_sigPointNormalsModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_NORMALS_MODIFIED_SIG;

    /// Type of signal when cell normals are modified
    typedef ::fwCom::Signal< void () > CellNormalsModifiedSignalType;

    /// Key in m_signals map of signal m_sigCellNormalsModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_CELL_NORMALS_MODIFIED_SIG;

    /// Type of signal when point tex coords are modified
    typedef ::fwCom::Signal< void () > PointTexCoordsModifiedSignalType;

    /// Key in m_signals map of signal m_sigPointTexCoorddModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_TEX_COORDS_MODIFIED_SIG;

    /// Type of signal when cell tex coords are modified
    typedef ::fwCom::Signal< void () > CellTexCoordsModifiedSignalType;

    /// Key in m_signals map of signal m_sigCellTexCoorddModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_CELL_TEX_COORDS_MODIFIED_SIG;
    /// @}

    /**
     *  @{
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @return The id of the new point
     *
     * @throw ::fwData::Exception if the allocation failed
     */
    FWDATA_API Id pushPoint(const PointValueType p[3]);
    FWDATA_API Id pushPoint(PointValueType x,
                            PointValueType y,
                            PointValueType z);
    /// @}
    /**
     * @{
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh's concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw ::fwData::Exception if the allocation failed
     */
    FWDATA_API Id pushCell(CellValueType idPt);
    FWDATA_API Id pushCell(CellValueType idP1, CellValueType idP2);
    FWDATA_API Id pushCell(CellValueType idP1, CellValueType idP2, CellValueType idP3);
    FWDATA_API Id pushCell(CellValueType idP1, CellValueType idP2, CellValueType idP3, CellValueType idP4,
                           CellType type = CellType::QUAD);
    FWDATA_API Id pushCell(CellType type,
                           const std::vector<CellValueType> pointIds);
    FWDATA_API Id pushCell(CellType type,
                           const CellValueType* pointIds,
                           size_t nbPoints );
    /// @}

    /**
     * @brief Set a point's coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p point coordinates
     * @throw Raise ::fwData::Exception if the id is out of bounds
     */
    FWDATA_API void setPoint(::fwData::Mesh::Id id, const ::fwData::Mesh::PointValueType p[3]);

    /**
     * @brief Set a point coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @see setPoint
     * @throw Raise ::fwData::Exception if the id is out of bounds
     */
    FWDATA_API void setPoint(::fwData::Mesh::Id id,
                             ::fwData::Mesh::PointValueType x,
                             ::fwData::Mesh::PointValueType y,
                             ::fwData::Mesh::PointValueType z);

    /**
     * @{
     * @brief Set a cell into the mesh.
     *
     * @warning Use this method carefully, the cell should be properly allocated. If the current cell does not contain
     * as many points as the previous one the following cells will be corrupted.
     *
     * @throw ::fwData::Exception if the mesh is not correctly allocated (ie. the id is out of bounds)
     */
    FWDATA_API void setCell(::fwData::Mesh::Id id, CellValueType idPt);
    FWDATA_API void setCell(::fwData::Mesh::Id id, CellValueType idP1, CellValueType idP2);
    FWDATA_API void setCell(::fwData::Mesh::Id id, CellValueType idP1, CellValueType idP2, CellValueType idP3);
    FWDATA_API void setCell(::fwData::Mesh::Id id, CellValueType idP1, CellValueType idP2, CellValueType idP3,
                            CellValueType idP4,
                            CellType type = CellType::QUAD);
    FWDATA_API void setCell(::fwData::Mesh::Id id, CellType type, const std::vector<CellValueType>& pointIds);
    FWDATA_API void setCell(::fwData::Mesh::Id id, CellType type, const CellValueType* pointIds, size_t nbPoints );
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
    FWDATA_API void setPointColor(::fwData::Mesh::Id id, const std::array< ::fwData::Mesh::ColorValueType, 4>& c);
    FWDATA_API void setPointColor(::fwData::Mesh::Id id, ::fwData::Mesh::ColorValueType r,
                                  ::fwData::Mesh::ColorValueType g, ::fwData::Mesh::ColorValueType b,
                                  ::fwData::Mesh::ColorValueType a);
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
    FWDATA_API void setCellColor(::fwData::Mesh::Id id, const std::array< ::fwData::Mesh::ColorValueType, 4>& c);
    FWDATA_API void setCellColor(::fwData::Mesh::Id id, ::fwData::Mesh::ColorValueType r,
                                 ::fwData::Mesh::ColorValueType g, ::fwData::Mesh::ColorValueType b,
                                 ::fwData::Mesh::ColorValueType a);
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
    FWDATA_API void setPointNormal(::fwData::Mesh::Id id, const std::array< ::fwData::Mesh::NormalValueType, 3>& n);
    FWDATA_API void setPointNormal(::fwData::Mesh::Id id, ::fwData::Mesh::NormalValueType nx,
                                   ::fwData::Mesh::NormalValueType ny, ::fwData::Mesh::NormalValueType nz);
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
    FWDATA_API void setCellNormal(::fwData::Mesh::Id id, const std::array< ::fwData::Mesh::NormalValueType, 3>& n);
    FWDATA_API void setCellNormal(::fwData::Mesh::Id id, ::fwData::Mesh::NormalValueType nx,
                                  ::fwData::Mesh::NormalValueType ny, ::fwData::Mesh::NormalValueType nz);
    /// @}
    /**
     * @{
     * @brief Set a point's texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id point index
     * @param t texCoord
     * /// @}
     */
    FWDATA_API void setPointTexCoord(::fwData::Mesh::Id id, const std::array< ::fwData::Mesh::TexCoordValueType, 2>& t);
    FWDATA_API void setPointTexCoord(::fwData::Mesh::Id id, ::fwData::Mesh::TexCoordValueType u,
                                     ::fwData::Mesh::TexCoordValueType v);

    /**
     * @{
     * @brief Set a cell's texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id cell index
     * @param t texCoord
     */
    FWDATA_API void setCellTexCoord(::fwData::Mesh::Id id, const std::array< ::fwData::Mesh::TexCoordValueType, 2>& t);
    FWDATA_API void setCellTexCoord(::fwData::Mesh::Id id, ::fwData::Mesh::TexCoordValueType u,
                                    ::fwData::Mesh::TexCoordValueType v);
    /// @}

    /**
     * @brief Returns the begin/end iterators to the mesh buffers
     *
     * @tparam ITERATOR iterator type:
     *  - ::fwData::iterator::PointIterator: to iterate through mesh points
     *  - ::fwData::iterator::ConstPointIterator: to iterate through mesh points read-only
     *  - ::fwData::iterator::CellIterator: to iterate through mesh cells
     *  - ::fwData::iterator::ConstCellIterator: to iterate through mesh cells read-only
     *
     * @note These iterators lock the buffer
     * @{
     */
    template< typename ITERATOR > ITERATOR begin();
    template< typename ITERATOR > ITERATOR end();
    template< typename ITERATOR > ITERATOR begin() const;
    template< typename ITERATOR > ITERATOR end() const;
    /// @}

    /**
     * @brief Return a lock on the mesh to prevent from dumping the buffers on the disk
     *
     * The buffer cannot be accessed if the mesh is not locked
     *
     * @warning You must allocate all the mesh's arrays before calling lock()
     */
    [[nodiscard]] FWDATA_API LocksType lock() const;

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

    // ---------------------------------------
    // Deprecated API for sight 22.0
    // ---------------------------------------

    [[deprecated]] typedef boost::multi_array_ref<PointValueType, 2> PointsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<CellTypes, 1> CellTypesMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<CellValueType, 1> CellDataMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<CellDataOffsetType, 1> CellDataOffsetsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<ColorValueType, 2> PointColorsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<ColorValueType, 2> CellColorsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<NormalValueType, 2> PointNormalsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<NormalValueType, 2> CellNormalsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<TexCoordValueType, 2> PointTexCoordsMultiArrayType;
    [[deprecated]] typedef boost::multi_array_ref<TexCoordValueType, 2> CellTexCoordsMultiArrayType;

    [[deprecated]] typedef boost::const_multi_array_ref<PointValueType, 2> ConstPointsMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<CellTypes, 1> ConstCellTypesMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<CellValueType, 1> ConstCellDataMultiArrayType;
    [[deprecated]] typedef boost::const_multi_array_ref<CellDataOffsetType, 1> ConstCellDataOffsetsMultiArrayType;
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
     * @throw ::fwData::Exception
     */
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocate(size_t nbPts, size_t nbCells, size_t nbCellsData = 0);

    /// Allocates normals array according to the number of points of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocatePointNormals();

    /// Allocates colors array according to the number of points of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocatePointColors(ColorArrayTypes t);

    /// Allocates texCoords array according to the number of points of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocatePointTexCoords();

    /// Allocates normals array according to the number of cells of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocateCellNormals();

    /// Allocates colors array according to the number of cells of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocateCellColors(ColorArrayTypes t);

    /// Allocates texCoords array according to the number of cells of the mesh.
    [[deprecated("call reserve(nbPoints, nbCells, cellType, extraArrays).(sight 22.0)")]]
    FWDATA_API size_t allocateCellTexCoords();

    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setPointsArray (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setCellTypesArray        (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setCellDataArray         (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setCellDataOffsetsArray  (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setPointColorsArray      (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setCellColorsArray       (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setPointNormalsArray     (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setCellNormalsArray      (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setPointTexCoordsArray   (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void setCellTexCoordsArray    (const ::fwData::Array::sptr& array);

    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getPointsArray           () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getCellTypesArray        () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getCellDataArray         () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getCellDataOffsetsArray  () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getPointColorsArray      () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getCellColorsArray       () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getPointNormalsArray     () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getCellNormalsArray      () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getPointTexCoordsArray   () const;
    /// Returns the internal corresponding array
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getCellTexCoordsArray    () const;

    /// Add an array in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void addDataArray(const std::string& name, ::fwData::Array::sptr array);

    /// Get an array in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API ::fwData::Array::sptr getDataArray(const std::string& name) const;

    /// Remove an array in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API void removeDataArray(const std::string& name);

    /// Return all array names stock in the mesh array-map
    [[deprecated("The mesh arrays can no longer be accessed(sight 22.0)")]]
    FWDATA_API std::vector<std::string> getDataArrayNames() const;

protected:

    // make iterator friend in order to access mesh arrays
    friend class ::fwData::iterator::PointIterator;
    friend class ::fwData::iterator::ConstPointIterator;
    friend class ::fwData::iterator::CellIterator;
    friend class ::fwData::iterator::ConstCellIterator;

    /**
     * @brief Initializes points, cell-types, cell-data, and cell-data-offsets arrays.
     */
    FWDATA_API void initArrays();

    /// Number of points defined for the mesh
    Id m_nbPoints;

    /// Number of cells defined for the mesh
    Id m_nbCells;

    /// Number of point index defined for mesh (size of m_cellData)
    Id m_cellsDataSize;

    /**
     * @brief Mesh point array : 3-components 1-dimension float array, size = m_nbPoints x 3.
     *
     * This array contains points : [ x1 y1 z1 x2 y2 z2 ... xn yn zn ]
     */
    ::fwData::Array::sptr m_points;

    /**
     * @brief Cell types array : 1-components 1-dimension uint8 array, size = m_nbCells.
     *
     * This array each cell type : [ TRIANGLE QUAD QUAD ... TRIANGLE TRIANGLE QUAD ]
     */
    ::fwData::Array::sptr m_cellTypes;

    /**
     * @brief Cell data array : 1-components 1-dimension uint64 array, size = m_cellsDataSize.
     *
     * Contains cell data : cell points ids are contiguously stored regardless
     * of the cell type. Size depends of cell type. If we have only TRIANGLE type, size = m_nbCells x 3.
     *
     * This array contains point indexes (index in m_points) : [ TRIAN_ID1, TRIAN_ID2, TRIAN_ID3, QUAD_ID1, QUAD_ID2,
     * QUAD_ID3, QUAD_ID4, ... ]
     */
    ::fwData::Array::sptr m_cellData;

    /**
     * @brief Cell data offsets array : 1-components 1-dimension uint64 array, size = m_nbCells.
     *
     * Contains cell data offsets : for each cell, gives the offset of
     * the first item of this cell in m_cellData.
     *
     * This array contains cell indexes m_cellData : [ INDEX_TRIAN_1, INDEX_QUAD_1, INDEX_QUAD_2 ...  ]
     */
    ::fwData::Array::sptr m_cellDataOffsets;

    /**
     * @brief point colors array : 3 or 4-components 1-dimension float array, size = m_nbPoints.
     *
     * This array contains point colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     */
    ::fwData::Array::sptr m_pointColors;

    /**
     * @brief Mesh point array : 3 or 4-components 1-dimension uint8_t array, size = m_nbCells.
     *
     * This array contains cell colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     */
    ::fwData::Array::sptr m_cellColors;

    /**
     * @brief Mesh point array : 3-components 1-dimension uint8_t array, size = m_nbPoints.
     *
     * This array contains point normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     */
    ::fwData::Array::sptr m_pointNormals;

    /**
     * @brief Mesh point array : 3-components 1-dimension float array, size = m_nbCells.
     *
     * This array contains cell normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     */
    ::fwData::Array::sptr m_cellNormals;

    /**
     * @brief Mesh texCoord array : 2-components 1-dimension float array, size = m_nbPoints.
     *
     * This array contains point texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    ::fwData::Array::sptr m_pointTexCoords;

    /**
     * @brief Mesh texCoord array : 2-components 1-dimension float array, size = m_nbCells.
     *
     * This array contains cell texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    ::fwData::Array::sptr m_cellTexCoords;

    /// Array map where you can add few additional arrays registered thanks to a key to perform/conserve some specific
    /// analysis.
    /// @deprecated Will be removed in sight 22.0
    [[deprecated("sight 22.0")]] ArrayMapType m_arrayMap;
};

//------------------------------------------------------------------------------

inline Mesh::Attributes operator|(const Mesh::Attributes& lhs, const Mesh::Attributes& rhs)
{
    return static_cast<Mesh::Attributes> (
        static_cast<std::underlying_type<Mesh::Attributes>::type>(lhs) |
        static_cast<std::underlying_type<Mesh::Attributes>::type>(rhs)
        );
}

//------------------------------------------------------------------------------

inline Mesh::Attributes operator&(const Mesh::Attributes& lhs, const Mesh::Attributes& rhs)
{
    return static_cast<Mesh::Attributes> (
        static_cast<std::underlying_type<Mesh::Attributes>::type>(lhs) &
        static_cast<std::underlying_type<Mesh::Attributes>::type>(rhs)
        );
}

//------------------------------------------------------------------------------

template< typename ITERATOR >
inline ITERATOR Mesh::begin()
{
    return ITERATOR(this);
}

//------------------------------------------------------------------------------

template< typename ITERATOR >
inline ITERATOR Mesh::end()
{
    auto itr = ITERATOR(this);
    itr += static_cast< typename ITERATOR::difference_type>(itr.m_numberOfElements);
    return itr;
}

//------------------------------------------------------------------------------

template< typename ITERATOR >
inline ITERATOR Mesh::begin() const
{
    return ITERATOR(this);
}

//------------------------------------------------------------------------------

template< typename ITERATOR >
inline ITERATOR Mesh::end() const
{
    auto itr = ITERATOR(this);
    itr += static_cast< typename ITERATOR::difference_type>(itr.m_numberOfElements);
    return itr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasPointColors() const
{
    return m_pointColors != nullptr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasCellColors() const
{
    return m_cellColors != nullptr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasPointNormals() const
{
    return m_pointNormals != nullptr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasCellNormals() const
{
    return m_cellNormals != nullptr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasPointTexCoords() const
{
    return m_pointTexCoords != nullptr;
}

//------------------------------------------------------------------------------

inline bool Mesh::hasCellTexCoords() const
{
    return m_cellTexCoords != nullptr;
}
//------------------------------------------------------------------------------

} // namespace fwData
