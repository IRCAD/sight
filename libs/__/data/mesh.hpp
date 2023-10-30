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

#pragma once

#include "data/array.hpp"
#include "data/config.hpp"
#include "data/exception.hpp"
#include "data/factory/new.hpp"
#include "data/iterator.hpp"

#include <core/macros.hpp>
#include <core/memory/buffered.hpp>

#include <boost/range/combine.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <array>

namespace sight::data
{

/**
 * @brief   Data holding a geometric structure composed of points, lines, triangles, quads or polygons.
 *
 * @section Structure Structure
 *
 * The mesh structure contains some information stocked in Array
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
 * The setPoint() and set_cell() methods change the value of a point/cell at a given index.
 *
 * Example with resize(), setPoint() and set_cell()
 * @code{.cpp}
    mesh::sptr mesh = mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock(); // prevents the buffers from being dumped on the disk

    for (std::size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val              = static_cast<uint8_t>(i);
        const mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                = static_cast<float>(i);
        const mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
        const size_t value                  = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (std::size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->set_cell(i, i, i+1, i+2);

        const mesh::color_t val             = static_cast< mesh::color_t >(i);
        const mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }
   @endcode
 *
 * Example with reserve(), pushPoint() and pushCell()
 * @code{.cpp}
    mesh::sptr mesh = mesh::New();

    mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (std::size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val              = static_cast<uint8_t>(i);
        const mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                = static_cast<float>(i);
        const mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
        const size_t value                  = 3*i;
        const auto id =
            mesh->pushPoint(static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(id, color);
        mesh->setPointNormal(id, normal);
        mesh->setPointTexCoord(id, texCoords);
    }

    for (std::size_t i = 0; i < NB_CELLS; ++i)
    {
        const auto id = mesh->pushCell(i, i+1, i+2);

        const mesh::color_t val             = static_cast< mesh::color_t >(i);
        const mesh::color_t color[4]        = {val, val, val, val};
        const float floatVal                                 = static_cast<float>(i);
        const mesh::normal_t normal[3]      = {floatVal, floatVal, floatVal};
        const mesh::texcoord_t texCoords[2] = {floatVal, floatVal};
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
   mesh::sptr mesh = mesh::New();
   mesh->resize(25, 33, mesh::cell_type_t::triangle);

   float p[3] = {12.f, 16.f, 18.f};

   for (auto& pt = mesh->range<iterator::point::xyz>())
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
    mesh::sptr mesh = mesh::New();
    mesh->resize(25, 33, mesh::cell_type_t::triangle);

    auto itrPt = mesh->begin<iterator::point::xyz>();
    float p[3];

    for(const auto& cell : mesh->range<iterator::cell::triangle>())
    {
        for(std::size_t i = 0 ; i < 3 ; ++i)
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
 * pushCell() and set_cell() are not very efficient, you can use CellIterator to define the cells if speed is a concern.
 *
 * Example of defining cells using iterators
 *
 * @code{.cpp}
    mesh::sptr mesh = mesh::New();
    mesh->resize(25, 33, mesh::cell_type_t::QUAD);
    const auto cellType = mesh::cell_type_t::QUAD;
    const std::size_t nbPointPerCell = 4;

    std::size_t count = 0;
    for(const auto& cell : mesh->range<iterator::cell::quad>())
    {
        // define the point indices
        for (std::size_t i = 0; i < 4; ++i)
        {
            cell.pt[i] = count++;
        }
    }
   @endcode
 *
 * The iterators are compatible with all STL algorithm functions, for example std::copy.
 *
 * @code{.cpp}
    void copyPoints(const mesh& origin, const mesh& dest)
    {
        SIGHT_ASSERT("Meshes must have the same number of points",
                   origin.num_points() == dest.num_points());

        auto origIt = origin.begin< mesh::iterator::xyz >();
        auto origEnd = origin.end< mesh::iterator::xyz >();
        auto destIt = dest.begin< mesh::iterator::xyz >();
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
class DATA_CLASS_API mesh final : public object,
                                  public core::memory::buffered
{
public:

    SIGHT_DECLARE_CLASS(mesh, object);

    /// Stores a cell type.
    enum class cell_type_t : std::uint8_t
    {
        point = 0,
        line,
        triangle,
        quad,
        tetra,
        size
    };

    enum class attribute : std::uint8_t
    {
        none             = 0,
        point_colors     = 1 << 1,
        point_normals    = 1 << 2,
        cell_colors      = 1 << 3,
        cell_normals     = 1 << 4,
        point_tex_coords = 1 << 5,
        cell_tex_coords  = 1 << 6
    };

    using position_t = float;
    using color_t    = std::uint8_t;
    using normal_t   = float;
    using texcoord_t = float;

    using cell_t  = iterator::cell_t;
    using point_t = iterator::point_t;
    using size_t  = iterator::size_t;

    /**
     * @name Signals
     * @{
     */
    using signal_t = core::com::signal<void ()>;

    /// Key in m_signals map of signal m_sigVertexModified
    DATA_API static const core::com::signals::key_t VERTEX_MODIFIED_SIG;
    DATA_API static const core::com::signals::key_t POINT_COLORS_MODIFIED_SIG;
    DATA_API static const core::com::signals::key_t CELL_COLORS_MODIFIED_SIG;
    DATA_API static const core::com::signals::key_t POINT_NORMALS_MODIFIED_SIG;
    DATA_API static const core::com::signals::key_t CELL_NORMALS_MODIFIED_SIG;
    DATA_API static const core::com::signals::key_t POINT_TEX_COORDS_MODIFIED_SIG;
    DATA_API static const core::com::signals::key_t CELL_TEX_COORDS_MODIFIED_SIG;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API mesh();

    /// Destructor
    DATA_API ~mesh() noexcept override = default;

    /**
     * @brief Allocate mesh memory
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays with the information given by the
     * parameters.
     * It does not modify the number of points and cells.
     *
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param cellType type of cell to allocate, it defines the number of points by cell to allocate.
     * @param arrayMask mesh attribute: additional Arrays to allocate
     *        (ex: Attribute::POINT_COLORS | Attribute::point_normals)
     *
     * @return Return the allocated memory
     *
     * @throw Raise Exception if the memory can not be allocated.
     */
    DATA_API std::size_t reserve(
        mesh::size_t _nb_pts,
        mesh::size_t _nb_cells,
        cell_type_t _cell_type = cell_type_t::triangle,
        attribute _array_mask  = attribute::none
    );

    /**
     * @brief Allocate mesh memory and initialize the number of points and cells
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays with the information given by the
     * parameters.
     * @param nbPts number of points to allocate
     * @param nbCells number of cells to allocate
     * @param cellType type of cell to allocate, it defines the number of points by cell to allocate.
     * @param arrayMask mesh attribute: additional Arrays to allocate
     *        (ex: Attribute::POINT_COLORS | Attribute::point_normals)
     *
     * @return Return the allocated memory
     *
     * @throw Raise Exception if the memory can not be allocated.
     */
    DATA_API std::size_t resize(
        mesh::size_t _nb_pts,
        mesh::size_t _nb_cells,
        cell_type_t _cell_type = cell_type_t::triangle,
        attribute _array_mask  = attribute::none
    );

    /**
     * @brief Adjust mesh memory usage
     *
     * The arrays (points, cell-types, cell-data, cell-data-offsets, and if they exists point-colors/normals and
     * cell-colors/normals) will be resized according to the number of points and cells of the mesh.
     *
     * @return true if memory usage changed
     *
     * @throw Raise Exception if the memory can not be re-allocated.
     */
    DATA_API bool shrink_to_fit();

    /**
     * @brief Truncate the number of points and cells of a mesh.
     *
     * Use this method to downsize the number of elements without reallocating any array.
     * This is particularly useful with dynamic meshes to avoid an allocation every frame, which would occur if you
     * use shrinkToFit() for instance.
     *
     * @throw Raise Exception if the number of elements if higher than the allocated size.
     */
    DATA_API void truncate(mesh::size_t _nb_pts, mesh::size_t _nb_cells);

    /**
     * @brief Remove all data contained in the mesh. Memory is freed.
     */
    DATA_API void clear();

    /// Clear corresponding array, memory is freed.
    template<mesh::attribute A>
    void clear();

    /// Return true if the mesh has a given point attribute set
    template<mesh::attribute A>
    bool has() const;

    /// Get number of points.
    mesh::size_t num_points() const;

    /// Get number of cells.
    mesh::size_t num_cells() const;

    /// Get the cell type of this mesh.
    cell_type_t cell_type() const;

    /// Get the size of a cell given its type
    mesh::size_t cell_size() const;

    /// Get the mask type of point and cell attributes enabled
    mesh::attribute attributes() const;

    /**
     * @brief Get the mesh data size in bytes.
     *
     * @return mesh data size in bytes
     * @note The allocated memory may be greater than the data size in bytes.
     */
    DATA_API std::size_t size_in_bytes() const;

    /**
     * @brief Get the amount of memory allocated in this mesh. May be bigger than getDataSizeInBytes().
     *
     * @return mesh data size in bytes
     * @note You can call shrinkToFit() to free extra memory.
     */
    DATA_API std::size_t allocated_size_in_bytes() const;

    /**
     *  @{
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @return The id of the new point
     *
     * @throw Exception if the allocation failed
     */
    DATA_API point_t push_point(const std::array<position_t, 3>& _p);
    DATA_API point_t push_point(position_t _x, position_t _y, position_t _z);
    /// @}
    /**
     * @{
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh's concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw Exception if the allocation failed
     */
    DATA_API cell_t push_cell(point_t _id_pt);
    DATA_API cell_t push_cell(point_t _id_p1, point_t _id_p2);
    DATA_API cell_t push_cell(point_t _id_p1, point_t _id_p2, point_t _id_p3);
    DATA_API cell_t push_cell(point_t _id_p1, point_t _id_p2, point_t _id_p3, point_t _id_p4);
    DATA_API cell_t push_cell(const std::vector<point_t> _point_ids);
    DATA_API cell_t push_cell(const point_t* _point_ids, std::size_t _nb_points);
    /// @}

    /**
     * @brief Set a point's coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p point coordinates
     * @throw Raise Exception if the id is out of bounds
     */
    DATA_API void set_point(point_t _id, const std::array<position_t, 3>& _p);

    /**
     * @brief Set a point coordinates.
     *
     * The mesh must be allocated before calling this method.
     *
     * @see setPoint
     * @throw Raise Exception if the id is out of bounds
     */
    DATA_API void set_point(point_t _id, position_t _x, position_t _y, position_t _z);

    /**
     * @{
     * @brief Set a cell into the mesh.
     *
     * @warning Use this method carefully, the cell should be properly allocated. If the current cell does not contain
     * as many points as the previous one the following cells will be corrupted.
     *
     * @throw Exception if the mesh is not correctly allocated (ie. the id is out of bounds)
     */
    DATA_API void set_cell(cell_t _id, point_t _id_pt);
    DATA_API void set_cell(cell_t _id, point_t _id_p1, point_t _id_p2);
    DATA_API void set_cell(cell_t _id, point_t _id_p1, point_t _id_p2, point_t _id_p3);
    DATA_API void set_cell(cell_t _id, point_t _id_p1, point_t _id_p2, point_t _id_p3, point_t _id_p4);
    DATA_API void set_cell(cell_t _id, const std::vector<point_t>& _point_ids);
    DATA_API void set_cell(cell_t _id, const point_t* _point_ids, std::size_t _nb_points);
    /// @}

    /**
     * @{
     * @brief Set a point's color.
     *
     * @warning The point colors must be allocated with 4 components (RGBA)
     *
     * @param id point index
     * @param c color
     */
    DATA_API void set_point_color(point_t _id, const std::array<color_t, 4>& _c);
    DATA_API void set_point_color(point_t _id, color_t _r, color_t _g, color_t _b, color_t _a);
    /// @}
    ///
    /**
     * @{
     * @brief Set a cell's color.
     *
     * @warning The cell colors must be allocated with 4 components (RGBA)
     *
     * @param id cell index
     * @param c color
     */
    DATA_API void set_cell_color(cell_t _id, const std::array<color_t, 4>& _c);
    DATA_API void set_cell_color(cell_t _id, color_t _r, color_t _g, color_t _b, color_t _a);
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
    DATA_API void set_point_normal(point_t _id, const std::array<normal_t, 3>& _n);
    DATA_API void set_point_normal(point_t _id, normal_t _nx, normal_t _ny, normal_t _nz);
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
    DATA_API void set_cell_normal(cell_t _id, const std::array<normal_t, 3>& _n);
    DATA_API void set_cell_normal(cell_t _id, normal_t _nx, normal_t _ny, normal_t _nz);
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
    DATA_API void set_point_tex_coord(point_t _id, const std::array<texcoord_t, 2>& _t);
    DATA_API void set_point_tex_coord(point_t _id, texcoord_t _u, texcoord_t _v);
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
    DATA_API void set_cell_tex_coord(cell_t _id, const std::array<texcoord_t, 2>& _t);
    DATA_API void set_cell_tex_coord(cell_t _id, texcoord_t _u, texcoord_t _v);
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

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const mesh& _other) const noexcept;
    DATA_API bool operator!=(const mesh& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Add a lock on the mesh in the given vector to prevent from dumping the buffer on the disk
    /// This is needed for IBuffered interface implementation
    DATA_API void dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const override;

private:

    /// Helper function used to get the array given a point or cell attribute type
    template<class ATTR>
    array::sptr get_array();

    /// Helper function used to get the array given a point or cell attribute type
    template<class ATTR>
    array::csptr get_array() const;

    /// Helper function used to get the number of points or cells given a point or cell attribute type
    template<class ATTR>
    std::size_t num_elements() const;

    enum class point_attribute : std::uint8_t
    {
        position = 0,
        colors,
        normals,
        tex_coords,
        size
    };

    enum class cell_attribute : std::uint8_t
    {
        index = 0,
        colors,
        normals,
        tex_coords,
        size
    };

    /// Number of points defined for the mesh
    mesh::size_t m_num_points {0};

    /// Number of cells defined for the mesh
    mesh::size_t m_num_cells {0};

    /// Type of primitive
    cell_type_t m_cell_type {cell_type_t::size};

    /**
     * @brief Points arrays
     *
     * Position : 3-components 1-dimension float array, size = m_numPoints x 3.
     * Contains points : [ x1 y1 z1 x2 y2 z2 ... xn yn zn ]
     *
     * Point colors array : 3 or 4-components 1-dimension float array, size = m_numPoints.
     * Contains point colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     *
     * mesh point array : 3-components 1-dimension uint8_t array, size = m_numPoints.
     * Contains point normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     *
     * mesh texCoord array : 2-components 1-dimension float array, size = m_numPoints.
     * Contains point texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    std::array<array::sptr, static_cast<std::size_t>(point_attribute::size)> m_points;

    /**
     * @brief Cell data arrays
     *
     * Cell index array : 1-components 1-dimension uint64 array, size = m_cellsDataSize.
     * Contains cell data : cell points ids are contiguously stored regardless
     * of the cell type. Size depends of cell type. If we have only TRIANGLE type, size = m_numCells x 3.
     * This array contains point indexes (index in m_points) : [ TRIAN_ID1, TRIAN_ID2, TRIAN_ID3, ... ]
     *
     * Cell colors array : 3 or 4-components 1-dimension uint8_t array, size = m_numCells.
     * Contains cell colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     *
     * Cell normal array : 3-components 1-dimension float array, size = m_numCells.
     * Contains cell normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     *
     * Cell texCoord array : 2-components 1-dimension float array, size = m_numCells.
     * Contains cell texCoords : [ tx1 ty1 tx2 ty2 ... ]
     */
    std::array<array::sptr, static_cast<std::size_t>(cell_attribute::size)> m_cells;

    /// Stores current attributes.
    attribute m_attributes {attribute::none};
};

//------------------------------------------------------------------------------

inline mesh::attribute operator|(const mesh::attribute& _lhs, const mesh::attribute& _rhs)
{
    return static_cast<mesh::attribute>(
        static_cast<std::underlying_type<mesh::attribute>::type>(_lhs)
        | static_cast<std::underlying_type<mesh::attribute>::type>(_rhs)
    );
}

//------------------------------------------------------------------------------

inline mesh::attribute operator&(const mesh::attribute& _lhs, const mesh::attribute& _rhs)
{
    return static_cast<mesh::attribute>(
        static_cast<std::underlying_type<mesh::attribute>::type>(_lhs)
        & static_cast<std::underlying_type<mesh::attribute>::type>(_rhs)
    );
}

//------------------------------------------------------------------------------

inline mesh::attribute operator~(const mesh::attribute& _lhs)
{
    return static_cast<mesh::attribute>(~static_cast<std::underlying_type<mesh::attribute>::type>(_lhs));
}

//------------------------------------------------------------------------------

inline mesh::size_t mesh::num_points() const
{
    return m_num_points;
}

//------------------------------------------------------------------------------

inline mesh::size_t mesh::num_cells() const
{
    return m_num_cells;
}

//------------------------------------------------------------------------------

inline mesh::cell_type_t mesh::cell_type() const
{
    return m_cell_type;
}

//------------------------------------------------------------------------------

inline mesh::size_t mesh::cell_size() const
{
    static const std::array<mesh::size_t, static_cast<std::size_t>(mesh::cell_type_t::size)> s_CELL_TYPE_TO_SIZE =
    {1, 2, 3, 4, 4};
    return s_CELL_TYPE_TO_SIZE[static_cast<std::size_t>(m_cell_type)];
}

//------------------------------------------------------------------------------

inline mesh::attribute mesh::attributes() const
{
    return m_attributes;
}

//------------------------------------------------------------------------------

template<mesh::attribute A>
void mesh::clear()
{
    m_attributes = m_attributes & ~A;
    m_points[static_cast<std::size_t>(A)]->clear();
}

//------------------------------------------------------------------------------

template<mesh::attribute A>
inline bool mesh::has() const
{
    return static_cast<bool>(m_attributes & A);
}

//------------------------------------------------------------------------------

template<typename T>
inline array_iterator<T> mesh::begin()
{
    auto array = get_array<T>();
    return array_iterator<T>(static_cast<typename array_iterator<T>::pointer_t>(array->buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline array_iterator<T> mesh::end()
{
    auto itr = begin<T>();
    itr += static_cast<typename array_iterator<T>::difference_type>(num_elements<T>());
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> mesh::begin() const
{
    auto array = get_array<T>();
    return const_array_iterator<T>(static_cast<typename const_array_iterator<T>::pointer_t>(array->buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> mesh::end() const
{
    auto itr = cbegin<T>();
    itr += static_cast<typename const_array_iterator<T>::difference_type>(num_elements<T>());
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> mesh::cbegin() const
{
    auto array = get_array<T>();
    return const_array_iterator<T>(static_cast<typename const_array_iterator<T>::pointer_t>(array->buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const_array_iterator<T> mesh::cend() const
{
    auto itr = cbegin<T>();
    itr += static_cast<typename const_array_iterator<T>::difference_type>(num_elements<T>());
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
auto mesh::range()
{
    auto b = begin<T>();
    auto e = end<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename ... ATTRS>
auto mesh::zip_range()
{
    return boost::combine(range<ATTRS>() ...);
}

//------------------------------------------------------------------------------

template<typename T>
auto mesh::range() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename ... ATTRS>
auto mesh::zip_range() const
{
    return boost::combine(crange<ATTRS>() ...);
}

//------------------------------------------------------------------------------

template<typename T>
auto mesh::crange() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename ... ATTRS>
auto mesh::czip_range() const
{
    return boost::combine(crange<ATTRS>() ...);
}

//------------------------------------------------------------------------------

} // namespace sight::data
