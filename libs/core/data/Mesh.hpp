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
#include "data/iterator.hpp"

#include <core/macros.hpp>
#include <core/memory/IBuffered.hpp>

#include <boost/range/combine.hpp>
#include <boost/range/iterator_range_core.hpp>

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
 * - A list of arrays (m_points) which contains point attributes, such as position, normal, color and textures
 * coordinates.
 * - A list of arrays (m_cells) which contains cell indexes, and other cell attributes such as normal, color and
 *  textures coordinates.
 * *
 * The arrays store attributes as array of structures, i.e. for the normals :
 *  -normals = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ]
 *
 * @section Usage Usage
 *
 * @subsection Allocation Allocation
 *
 * The two methods reserve() and resize() allocate the mesh arrays. The difference between the two methods is
 * that resize() modifies the number of points and cells.
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
        const data::Mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
        const size_t value                                   = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const data::Mesh::color_t val             = static_cast< data::Mesh::color_t >(i);
        const data::Mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }
   @endcode
 *
 * Example with reserve(), pushPoint() and pushCell()
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();

    mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                               = static_cast<uint8_t>(i);
        const data::Mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
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

        const data::Mesh::color_t val             = static_cast< data::Mesh::color_t >(i);
        const data::Mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const data::Mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const data::Mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(id, color);
        mesh->setCellNormal(id, normal);
        mesh->setCellTexCoord(id, texCoords);
    }
   @endcode
 *
 * @subsection Iterators Iterators
 *
 * To access the mesh points and cells, it is recommended to use iterators because they are the most efficient.
 * Our iterators can loop over a single array or multiple arrays, thanks to boost::zip_iterator.
 *
 * Example to iterate over points:
 * @code{.cpp}
   data::Mesh::sptr mesh = data::Mesh::New();
   mesh->resize(25, 33, data::Mesh::CellType::TRIANGLE);

   float p[3] = {12.f, 16.f, 18.f};

   for (auto& pt = mesh->range<data::iterator::point::xyz>())
   {
       pt.x = p[0];
       pt.y = p[1];
       pt.z = p[2];
   }
   @endcode
 *
 * Example to iterate over cells:
 *
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::CellType::TRIANGLE);

    auto itrPt = mesh->begin<data::iterator::point::xyz>();
    float p[3];

    for(const auto& cell : mesh->range<data::iterator::cell::triangle>())
    {
        for(size_t i = 0 ; i < 3 ; ++i)
        {
            const auto pIdx = cell.pt[i];

            auto& pointItr(itrPt + pIdx);
            p[0] = pointItr->x;
            p[1] = pointItr->y;
            p[2] = pointItr->z;
        }
    }
   @endcode
 *
 * pushCell() and setCell() are not very efficient, you can use CellIterator to define the cells if speed is a concern.
 *
 * Example of defining cells using iterators
 *
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::CellType::QUAD);
    const auto cellType = data::Mesh::CellType::QUAD;
    const size_t nbPointPerCell = 4;

    size_t count = 0;
    for(const auto& cell : mesh->range<data::iterator::cell::quad>())
    {
        // define the point indices
        for (size_t i = 0; i < 4; ++i)
        {
            cell.pt[i] = count++;
        }
    }
   @endcode
 *
 * The iterators are compatible with all STL algorithm functions, for example std::copy.
 *
 * @code{.cpp}
    void copyPoints(const data::Mesh& origin, const data::Mesh& dest)
    {
        SIGHT_ASSERT("Meshes must have the same number of points",
                   origin.getNumberOfPoints() == dest.getNumberOfPoints());

        auto origIt = origin.begin< data::Mesh::iterator::xyz >();
        auto origEnd = origin.end< data::Mesh::iterator::xyz >();
        auto destIt = dest.begin< data::Mesh::iterator::xyz >();
        std::copy(origIt, origEnd, dest);
    }
   @endcode
 *
 * Last but not least, it is also possible to get an iterator over multiple attributes using the zip_range()
 * function. Coupled with C++17 structured bindings, this makes such loops fairly elegant.
 *
 * @code{.cpp}
    uint32_t count = 0;
    for(auto&& [p, n, c, uv] : mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>())
    {
        p.x = static_cast<float>(3 * count);
        p.y = static_cast<float>(3 * count + 1);
        p.z = static_cast<float>(3 * count + 2);

        n.nx = static_cast<float>(3 * count + 1);
        n.ny = static_cast<float>(3 * count + 2);
        n.nz = static_cast<float>(3 * count + 3);

        c.r = static_cast<std::uint8_t>(4 * count);
        c.g = static_cast<std::uint8_t>(4 * count + 1);
        c.b = static_cast<std::uint8_t>(4 * count + 2);
        c.a = static_cast<std::uint8_t>(4 * count + 3);

        uv.u = static_cast<float>(2 * count);
        uv.v = static_cast<float>(2 * count + 1);
 ++count;
    }
 * @endcode
 */
class DATA_CLASS_API Mesh : public data::Object,
                            public core::memory::IBuffered
{
public:

    SIGHT_DECLARE_CLASS(Mesh, data::Object, data::factory::New<Mesh>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Mesh))

    /// Stores a cell type.
    enum class CellType : std::uint8_t
    {
        POINT = 0,
        LINE,
        TRIANGLE,
        QUAD,
        TETRA,
        _SIZE
    };

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

    using position_t = float;
    using color_t    = std::uint8_t;
    using normal_t   = float;
    using texcoord_t = float;

    using cell_t  = data::iterator::cell_t;
    using point_t = data::iterator::point_t;
    using size_t  = data::iterator::Size;

    using locks_t = std::vector<core::memory::BufferObject::Lock>;

    /**
     * @name Signals
     * @{
     */
    using signal_t = core::com::Signal<void ()>;

    /// Key in m_signals map of signal m_sigVertexModified
    DATA_API static const core::com::Signals::SignalKeyType s_VERTEX_MODIFIED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_COLORS_MODIFIED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_CELL_COLORS_MODIFIED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_NORMALS_MODIFIED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_CELL_NORMALS_MODIFIED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_TEX_COORDS_MODIFIED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_CELL_TEX_COORDS_MODIFIED_SIG;
    /// @}

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
    DATA_API std::size_t reserve(
        Mesh::size_t nbPts,
        Mesh::size_t nbCells,
        CellType cellType    = CellType::TRIANGLE,
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
    DATA_API std::size_t resize(
        Mesh::size_t nbPts,
        Mesh::size_t nbCells,
        CellType cellType    = CellType::TRIANGLE,
        Attributes arrayMask = Attributes::NONE
    );

    /**
     * @brief Adjust mesh memory usage
     *
     * The arrays (points, cell-types, cell-data, cell-data-offsets, and if they exists point-colors/normals and
     * cell-colors/normals) will be resized according to the number of points and cells of the mesh (they may have
     * been defined before adjusting by setNumberOfPoints or setNumberOfCells).
     *
     * @return true if memory usage changed
     *
     * @throw Raise data::Exception if the memory can not be re-allocated.
     */
    DATA_API bool shrinkToFit();

    /**
     * @brief Truncate the number of points and cells of a mesh.
     *
     * Use this method to downsize the number of elements without reallocating any array.
     * This is particularly useful with dynamic meshes to avoid an allocation every frame, which would occur if you
     * use shrinkToFit() for instance.
     *
     * @throw Raise data::Exception if the number of elements if higher than the allocated size.
     */
    DATA_API void truncate(Mesh::size_t nbPts, Mesh::size_t nbCells);

    /**
     * @brief Remove all data contained in the mesh. Memory is freed.
     */
    DATA_API void clear();

    /// Clear corresponding array, memory is freed.
    template<Mesh::Attributes A>
    void clear();

    /// Return true if the mesh has a given point attribute set
    template<Mesh::Attributes A>
    bool has() const;

    /// Get number of points.
    size_t getNumberOfPoints() const;

    /// Get number of cells.
    size_t getNumberOfCells() const;

    /// Get the cell type of this mesh.
    CellType getCellType() const;

    /// Get the size of a cell given its type
    Mesh::size_t getCellSize() const;

    /// Get the mask type of point and cell attributes enabled
    Mesh::Attributes getAttributes() const;

    /**
     * @brief Get the mesh data size in bytes.
     *
     * @return Mesh data size in bytes
     * @note The allocated memory may be greater than the data size in bytes.
     */
    DATA_API std::size_t getDataSizeInBytes() const;

    /**
     * @brief Get the amount of memory allocated in this mesh. May be bigger than getDataSizeInBytes().
     *
     * @return Mesh data size in bytes
     * @note You can call shrinkToFit() to free extra memory.
     */
    DATA_API std::size_t getAllocatedSizeInBytes() const;

    /**
     *  @{
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @return The id of the new point
     *
     * @throw data::Exception if the allocation failed
     */
    DATA_API point_t pushPoint(const position_t p[3]);
    DATA_API point_t pushPoint(position_t x, position_t y, position_t z);
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
    DATA_API cell_t pushCell(point_t idPt);
    DATA_API cell_t pushCell(point_t idP1, point_t idP2);
    DATA_API cell_t pushCell(point_t idP1, point_t idP2, point_t idP3);
    DATA_API cell_t pushCell(point_t idP1, point_t idP2, point_t idP3, point_t idP4);
    DATA_API cell_t pushCell(const std::vector<point_t> pointIds);
    DATA_API cell_t pushCell(const point_t* pointIds, size_t nbPoints);
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
    DATA_API void setPoint(point_t id, const data::Mesh::position_t p[3]);

    /**
     * @brief Set a point coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @see setPoint
     * @throw Raise data::Exception if the id is out of bounds
     */
    DATA_API void setPoint(point_t id, position_t x, position_t y, position_t z);

    /**
     * @{
     * @brief Set a cell into the mesh.
     *
     * @warning Use this method carefully, the cell should be properly allocated. If the current cell does not contain
     * as many points as the previous one the following cells will be corrupted.
     *
     * @throw data::Exception if the mesh is not correctly allocated (ie. the id is out of bounds)
     */
    DATA_API void setCell(cell_t id, point_t idPt);
    DATA_API void setCell(cell_t id, point_t idP1, point_t idP2);
    DATA_API void setCell(cell_t id, point_t idP1, point_t idP2, point_t idP3);
    DATA_API void setCell(cell_t id, point_t idP1, point_t idP2, point_t idP3, point_t idP4);
    DATA_API void setCell(cell_t id, const std::vector<point_t>& pointIds);
    DATA_API void setCell(cell_t id, const point_t* pointIds, size_t nbPoints);
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
    DATA_API void setPointColor(point_t id, const std::array<color_t, 4>& c);
    DATA_API void setPointColor(point_t id, color_t r, color_t g, color_t b, color_t a);
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
    DATA_API void setCellColor(cell_t id, const std::array<color_t, 4>& c);
    DATA_API void setCellColor(cell_t id, color_t r, color_t g, color_t b, color_t a);
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
    DATA_API void setPointNormal(point_t id, const std::array<normal_t, 3>& n);
    DATA_API void setPointNormal(point_t id, normal_t nx, normal_t ny, normal_t nz);
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
    DATA_API void setCellNormal(cell_t id, const std::array<normal_t, 3>& n);
    DATA_API void setCellNormal(cell_t id, normal_t nx, normal_t ny, normal_t nz);
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
    DATA_API void setPointTexCoord(point_t id, const std::array<texcoord_t, 2>& t);
    DATA_API void setPointTexCoord(point_t id, texcoord_t u, texcoord_t v);
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
    DATA_API void setCellTexCoord(cell_t id, const std::array<texcoord_t, 2>& t);
    DATA_API void setCellTexCoord(cell_t id, texcoord_t u, texcoord_t v);
    /// @}

    /**
     * @brief Returns the begin/end iterators to the mesh buffers
     */
    template<typename T>
    array_iterator<T> begin();
    template<typename T>
    array_iterator<T> end();
    template<typename T>
    const_array_iterator<T> begin() const;
    template<typename T>
    const_array_iterator<T> end() const;
    template<typename T>
    const_array_iterator<T> cbegin() const;
    template<typename T>
    const_array_iterator<T> cend() const;

    template<typename T>
    auto range();
    template<typename ... ATTRS>
    auto zip_range();

    template<typename T>
    auto range() const;
    template<typename ... ATTRS>
    auto zip_range() const;

    template<typename T>
    auto crange() const;
    template<typename ... ATTRS>
    auto czip_range() const;
    /// @}

    /**
     * @brief Return a lock on the mesh to prevent from dumping the buffers on the disk
     *
     * The buffer cannot be accessed if the mesh is not locked
     *
     * @warning You must allocate all the mesh's arrays before calling lock()
     */
    [[nodiscard]] DATA_API locks_t lock() const;

private:

    template<class DATATYPE>
    friend class data::mt::locked_ptr;

    /**
     * @brief Add a lock on the mesh in the given vector to prevent from dumping the buffer on the disk
     *
     * This is needed for IBuffered interface implementation
     */
    DATA_API void lockBuffer(std::vector<core::memory::BufferObject::Lock>& locks) const override;

    /// Helper function used to get the array given a point or cell attribute type
    template<class ATTR>
    data::Array::sptr getArray();

    /// Helper function used to get the array given a point or cell attribute type
    template<class ATTR>
    data::Array::csptr getArray() const;

    /// Helper function used to get the number of points or cells given a point or cell attribute type
    template<class ATTR>
    std::size_t getNumElements() const;

    enum class PointAttribute : std::uint8_t
    {
        POSITION = 0,
        COLORS,
        NORMALS,
        TEX_COORDS,
        _SIZE
    };
    enum class CellAttribute : std::uint8_t
    {
        INDEX = 0,
        COLORS,
        NORMALS,
        TEX_COORDS,
        _SIZE
    };

    /// Number of points defined for the mesh
    size_t m_nbPoints {0};

    /// Number of cells defined for the mesh
    size_t m_nbCells {0};

    /// Type of primitive
    CellType m_cellType {CellType::_SIZE};

    /**
     * @brief Points arrays
     *
     * Position : 3-components 1-dimension float array, size = m_nbPoints x 3.
     * Contains points : [ x1 y1 z1 x2 y2 z2 ... xn yn zn ]
     *
     * Point colors array : 3 or 4-components 1-dimension float array, size = m_nbPoints.
     * Contains point colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     *
     * Mesh point array : 3-components 1-dimension uint8_t array, size = m_nbPoints.
     * Contains point normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     *
     * Mesh texCoord array : 2-components 1-dimension float array, size = m_nbPoints.
     * Contains point texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    std::array<data::Array::sptr, static_cast<size_t>(PointAttribute::_SIZE)> m_points;

    /**
     * @brief Cell data arrays
     *
     * Cell index array : 1-components 1-dimension uint64 array, size = m_cellsDataSize.
     * Contains cell data : cell points ids are contiguously stored regardless
     * of the cell type. Size depends of cell type. If we have only TRIANGLE type, size = m_nbCells x 3.
     * This array contains point indexes (index in m_points) : [ TRIAN_ID1, TRIAN_ID2, TRIAN_ID3, ... ]
     *
     * Cell colors array : 3 or 4-components 1-dimension uint8_t array, size = m_nbCells.
     * Contains cell colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     *
     * Cell normal array : 3-components 1-dimension float array, size = m_nbCells.
     * Contains cell normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     *
     * Cell texCoord array : 2-components 1-dimension float array, size = m_nbCells.
     * Contains cell texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    std::array<data::Array::sptr, static_cast<size_t>(CellAttribute::_SIZE)> m_cells;

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

inline Mesh::size_t Mesh::getNumberOfPoints() const
{
    return m_nbPoints;
}

//------------------------------------------------------------------------------

inline Mesh::size_t Mesh::getNumberOfCells() const
{
    return m_nbCells;
}

//------------------------------------------------------------------------------

inline Mesh::CellType Mesh::getCellType() const
{
    return m_cellType;
}

//------------------------------------------------------------------------------

inline Mesh::size_t Mesh::getCellSize() const
{
    static const std::array<Mesh::size_t, static_cast<std::size_t>(Mesh::CellType::_SIZE)> s_cellTypeToSize =
    {1, 2, 3, 4, 4};
    return s_cellTypeToSize[static_cast<std::size_t>(m_cellType)];
}

//------------------------------------------------------------------------------

inline Mesh::Attributes Mesh::getAttributes() const
{
    return m_attributes;
}

//------------------------------------------------------------------------------

template<Mesh::Attributes A>
void Mesh::clear()
{
    m_attributes = m_attributes & ~A;
    m_points[static_cast<size_t>(A)]->clear();
}

//------------------------------------------------------------------------------

template<Mesh::Attributes A>
inline bool Mesh::has() const
{
    return static_cast<bool>(m_attributes & A);
}

//------------------------------------------------------------------------------

template<typename T>
inline array_iterator<T> Mesh::begin()
{
    auto array = getArray<T>();
    return array_iterator<T>(static_cast<typename array_iterator<T>::pointer>(array->getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline array_iterator<T> Mesh::end()
{
    auto itr = begin<T>();
    itr += static_cast<typename array_iterator<T>::difference_type>(getNumElements<T>());
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> Mesh::begin() const
{
    auto array = getArray<T>();
    return const_array_iterator<T>(static_cast<typename const_array_iterator<T>::pointer>(array->getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> Mesh::end() const
{
    auto itr = cbegin<T>();
    itr += static_cast<typename const_array_iterator<T>::difference_type>(numElements<T>());
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> Mesh::cbegin() const
{
    auto array = getArray<T>();
    return const_array_iterator<T>(static_cast<typename const_array_iterator<T>::pointer>(array->getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> Mesh::cend() const
{
    auto itr = cbegin<T>();
    itr += static_cast<typename const_array_iterator<T>::difference_type>(getNumElements<T>());
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
auto Mesh::range()
{
    auto b = begin<T>();
    auto e = end<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename ... ATTRS>
auto Mesh::zip_range()
{
    return boost::combine(range<ATTRS>() ...);
}

//------------------------------------------------------------------------------

template<typename T>
auto Mesh::range() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename ... ATTRS>
auto Mesh::zip_range() const
{
    return boost::combine(crange<ATTRS>() ...);
}

//------------------------------------------------------------------------------

template<typename T>
auto Mesh::crange() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename ... ATTRS>
auto Mesh::czip_range() const
{
    return boost::combine(crange<ATTRS>() ...);
}

//------------------------------------------------------------------------------

} // namespace sight::data
