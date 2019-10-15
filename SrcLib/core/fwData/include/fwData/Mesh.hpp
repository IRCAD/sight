/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
 */
class FWDATA_CLASS_API Mesh : public ::fwData::Object
{

public:

    fwCoreClassMacro(Mesh, ::fwData::Object, ::fwData::factory::New< Mesh >)

    fwCampMakeFriendDataMacro((fwData)(Mesh))

    typedef std::map< std::string, ::fwData::Array::sptr > ArrayMapType;
    typedef std::uint64_t Id;

    typedef enum
    {
        NO_CELL = 0,
        POINT,
        EDGE,
        TRIANGLE,
        QUAD,
        POLY,
        TETRA
    } CellTypesEnum;

    typedef enum
    {
        RGB  = 3,
        RGBA = 4
    } ColorArrayTypes;

    enum class ExtraArrayType : int
    {
        NO_ARRAY         = 0,
        POINT_COLORS     = 1 << 1,
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
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays
     * with informations given by the parameters. if nbCellData is zero, it
     * will be initialized to 3*nbCells.
     * It does not modify the number of points and cells
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param cellType type of cell to allocate, it define the number of points by cell to allocate. If you want to
     *        use different types of cells in the same mesh, use
     *        resize(size_t nbPts, size_t nbCells, ExtraArrayType arrayMask, size_t nbCellsData)
     * @param arrayMask extra Array to allocate (ex: ExtraArrayType::POINT_COLORS | ExtraArrayType::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t reserve(size_t nbPts, size_t nbCells, CellTypesEnum cellType = TRIANGLE,
                              ExtraArrayType arrayMask = ExtraArrayType::NO_ARRAY);

    /**
     * @brief Allocate Mesh memory
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays
     * with informations given by the parameters. if nbCellData is zero, it
     * will be initialized to 3*nbCells.
     * It does not modify the number of points and cells
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param nbCellsData sum of the number of points of each cell, il allows to allocate the cells array.
     * @param arrayMask extra Array to allocate (ex: ExtraArrayType::POINT_COLORS | ExtraArrayType::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t reserve(size_t nbPts, size_t nbCells, size_t nbCellsData,
                              ExtraArrayType arrayMask = ExtraArrayType::NO_ARRAY);

    /**
     * @brief Allocate Mesh memory and initialize the number of points and cells
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays
     * with informations given by the parameters. if nbCellData is zero, it
     * will be initialized to 3*nbCells
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param cellType type of cell to allocate, it define the number of points by cell to allocate. If you want to
     *        use different types of cells in the same mesh, use
     *        resize(size_t nbPts, size_t nbCells, ExtraArrayType arrayMask, size_t nbCellsData)
     * @param arrayMask extra Array to allocate (ex: ExtraArrayType::POINT_COLORS | ExtraArrayType::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t resize(size_t nbPts, size_t nbCells, CellTypesEnum cellType = TRIANGLE,
                             ExtraArrayType arrayMask = ExtraArrayType::NO_ARRAY);

    /**
     * @brief Allocate Mesh memory and initialize the number of points and cells
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays
     * with informations given by the parameters. if nbCellData is zero, it
     * will be initialized to 3*nbCells
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param nbCellsData sum of the number of points of each cell, il allows to allocate the cells array.
     * @param arrayMask extra Array to allocate (ex: ExtraArrayType::POINT_COLORS | ExtraArrayType::POINT_NORMALS)
     *
     * @return Return the allocated memory
     *
     * @throw Raise ::fwData::Exception if the memory can not be allocated.
     */
    FWDATA_API size_t resize(size_t nbPts, size_t nbCells, size_t nbCellsData,
                             ExtraArrayType arrayMask = ExtraArrayType::NO_ARRAY);

    /**
     * @brief Adjust mesh memory usage
     *
     * The array (points, cell-types, cell-data, cell-data-offsets, and if they
     * exists point-colors/normals and cell-colors/normals) will be resized
     * according to the number of points and cells of the mesh (they may have
     * been defined before adjusting by setNumberOfPoints or setNumberOfCells).
     * Arrays in array-map are not impacted by this method.
     *
     * @return true if memory usage changed
     *
     * @throw Raise ::fwData::Exception if the memory can not be re-allocated.
     */
    FWDATA_API bool adjustAllocatedMemory();

    /**
     * @brief Clear mesh points.
     * Calling this method don't impact memory allocation.
     */
    FWDATA_API void clearPoints();

    /**
     * @brief Clear mesh cells.
     * Calling this method don't impact memory allocation.
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
     */
    FWDATA_API size_t getDataSizeInBytes() const;

    /**
     * @brief Get the amount of memory allocated in this mesh. Mey be bigger
     * than getDataSizeInBytes().
     *
     * @return Mesh data size in bytes
     */
    FWDATA_API size_t getAllocatedSizeInBytes() const;

    /***
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

    /**
       @{
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @param p point coordinates
     *
     * @return The id of the new point
     *
     * @throw ::fwData::Exception if the allocation failed
     */
    Id pushPoint(const PointValueType p[3]);
    Id pushPoint(PointValueType x,
                 PointValueType y,
                 PointValueType z);
    /// @}
    /**
     * @{
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw ::fwData::Exception if the allocation failed
     */
    Id pushCell(CellValueType idPt);
    Id pushCell(CellValueType idP1, CellValueType idP2);
    Id pushCell(CellValueType idP1, CellValueType idP2, CellValueType idP3);
    Id pushCell(CellValueType idP1, CellValueType idP2, CellValueType idP3, CellValueType idP4,
                CellTypesEnum type = QUAD);
    Id pushCell(CellTypesEnum type,
                const std::vector<CellValueType> pointIds);
    Id pushCell(CellTypesEnum type,
                const CellValueType* pointIds,
                size_t nbPoints );
    /// @}

    /**
     * @brief Set a point coordinates.
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p point coordinates
     */
    FWDATA_API void setPoint(::fwData::Mesh::Id id, const ::fwData::Mesh::PointValueType p[3]);

    /**
     * @brief Set a point coordinates.
     * @see setPoint
     */
    FWDATA_API void setPoint(::fwData::Mesh::Id id,
                             ::fwData::Mesh::PointValueType x,
                             ::fwData::Mesh::PointValueType y,
                             ::fwData::Mesh::PointValueType z);
    /**
     * @brief Set a point color.
     *
     * Depending on the type of allocated point-colors array (RGB or RGBA), 3
     * or 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id point index
     * @param c color
     */
    FWDATA_API void setPointColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4]);

    /**
     * @brief Set a cell color.
     *
     * Depending on the type of allocated cell-colors array (RGB or RGBA), 3 or
     * 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id cell index
     * @param c color
     */
    FWDATA_API void setCellColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4]);

    /**
     * @brief Set a point normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id point index
     * @param n normal
     */
    FWDATA_API void setPointNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3]);

    /**
     * @brief Set a cell normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id cell index
     * @param n normal
     */
    FWDATA_API void setCellNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3]);

    /**
     * @brief Set a point texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id point index
     * @param t texCoord
     */
    FWDATA_API void setPointTexCoord(::fwData::Mesh::Id id, const ::fwData::Mesh::TexCoordValueType t[2]);

    /**
     * @brief Set a cell texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id cell index
     * @param t texCoord
     */
    FWDATA_API void setCellTexCoord(::fwData::Mesh::Id id, const ::fwData::Mesh::TexCoordValueType t[2]);

    /**
     * @brief Returns the begin/end iterators to the mesh buffers
     *
     * @note These functions lock the buffer
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
     */
    FWDATA_API LocksType lock() const;

    /**
     * @}
     */

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
     * @return
     *
     * @throw ::fwData::Exception
     */
    [[deprecated]] FWDATA_API size_t allocate(size_t nbPts, size_t nbCells, size_t nbCellsData = 0);

    /// Allocates normals array according to the number of points of the mesh.
    [[deprecated]] FWDATA_API size_t allocatePointNormals();

    /// Allocates colors array according to the number of points of the mesh.
    [[deprecated]] FWDATA_API size_t allocatePointColors(ColorArrayTypes t);

    /// Allocates texCoords array according to the number of points of the mesh.
    [[deprecated]] FWDATA_API size_t allocatePointTexCoords();

    /// Allocates normals array according to the number of cells of the mesh.
    [[deprecated]] FWDATA_API size_t allocateCellNormals();

    /// Allocates colors array according to the number of cells of the mesh.
    [[deprecated]] FWDATA_API size_t allocateCellColors(ColorArrayTypes t);

    /// Allocates texCoords array according to the number of cells of the mesh.
    [[deprecated]] FWDATA_API size_t allocateCellTexCoords();

    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setPointsArray           (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setCellTypesArray        (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setCellDataArray         (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setCellDataOffsetsArray  (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setPointColorsArray      (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setCellColorsArray       (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setPointNormalsArray     (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setCellNormalsArray      (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setPointTexCoordsArray   (const ::fwData::Array::sptr& array);
    /// Sets the internal corresponding array
    [[deprecated]] FWDATA_API void setCellTexCoordsArray    (const ::fwData::Array::sptr& array);

    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getPointsArray           () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getCellTypesArray        () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getCellDataArray         () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getCellDataOffsetsArray  () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getPointColorsArray      () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getCellColorsArray       () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getPointNormalsArray     () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getCellNormalsArray      () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getPointTexCoordsArray   () const;
    /// Returns the internal corresponding array
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getCellTexCoordsArray    () const;

    /// Add an array in the mesh array-map
    [[deprecated]] FWDATA_API void addDataArray(const std::string& name, ::fwData::Array::sptr array);

    /// Get an array in the mesh array-map
    [[deprecated]] FWDATA_API ::fwData::Array::sptr getDataArray(const std::string& name) const;

    /// Remove an array in the mesh array-map
    [[deprecated]] FWDATA_API void removeDataArray(const std::string& name);

    /// Return all array names stock in the mesh array-map
    [[deprecated]] FWDATA_API std::vector<std::string> getDataArrayNames() const;

protected:
    friend class ::fwData::iterator::PointIterator;
    friend class ::fwData::iterator::ConstPointIterator;

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
    ArrayMapType m_arrayMap;
};

//------------------------------------------------------------------------------

inline Mesh::ExtraArrayType operator|(const Mesh::ExtraArrayType& lhs, const Mesh::ExtraArrayType& rhs)
{
    return static_cast<Mesh::ExtraArrayType> (
        static_cast<std::underlying_type<Mesh::ExtraArrayType>::type>(lhs) |
        static_cast<std::underlying_type<Mesh::ExtraArrayType>::type>(rhs)
        );
}

//------------------------------------------------------------------------------

inline Mesh::ExtraArrayType operator&(const Mesh::ExtraArrayType& lhs, const Mesh::ExtraArrayType& rhs)
{
    return static_cast<Mesh::ExtraArrayType> (
        static_cast<std::underlying_type<Mesh::ExtraArrayType>::type>(lhs) &
        static_cast<std::underlying_type<Mesh::ExtraArrayType>::type>(rhs)
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
    itr += static_cast< typename ITERATOR::difference_type>(this->getNumberOfPoints());
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
    itr += static_cast< typename ITERATOR::difference_type>(this->getNumberOfPoints());
    return itr;
}

} // namespace fwData
