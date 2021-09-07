/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/iterator/ImageIterator.hpp"

#include <core/memory/BufferObject.hpp>

#include <iterator>

namespace sight::data
{

class Mesh;

namespace iterator
{

typedef std::uint32_t CellId;
typedef std::uint32_t PointId;
typedef std::uint32_t Size;

/// Stores a representation of a point.
struct Point
{
    float x;
    float y;
    float z;
};

/// Stores a representation of a normal.
struct Normal
{
    float nx;
    float ny;
    float nz;
};

/// Stores a representation of a texture coordinate.
struct TexCoords
{
    float u;
    float v;
};

/// Stores a cell type.
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

/// Stores point iterator information.
template<bool isConst>
struct PointInfoBase
{
    typedef typename std::conditional<isConst, const Point, Point>::type point_value_type;
    typedef point_value_type& point_reference;

    typedef typename std::conditional<isConst, const RGBA, RGBA>::type rgba_value_type;
    typedef rgba_value_type& rgba_reference;

    // TMP: to support old API using RGB colors
    typedef typename std::conditional<isConst, const RGB, RGB>::type rgb_value_type;
    typedef rgb_value_type& rgb_reference;

    typedef typename std::conditional<isConst, const Normal, Normal>::type normal_value_type;
    typedef normal_value_type& normal_reference;

    typedef typename std::conditional<isConst, const TexCoords, TexCoords>::type tex_value_type;
    typedef tex_value_type& tex_reference;

    PointInfoBase& operator=(const PointInfoBase<false>& other);

    PointInfoBase& operator=(const PointInfoBase<true>& other);

    bool operator==(const PointInfoBase& other);

    /// Contains coordinates of the point.
    point_value_type* point {nullptr};

    /// Contains normals of the point.
    normal_value_type* normal {nullptr};

    /// Contains the rgba color of the point.
    rgba_value_type* rgba {nullptr};

    /// Contains the rgb color of the point.
    rgb_value_type* rgb {nullptr};

    /// Contains texture coordinates of the point.
    tex_value_type* tex {nullptr};
};

/// Stores cell iterator information.
template<bool isConst>
struct CellInfoBase
{
    typedef typename std::conditional<isConst, const CellId, CellId>::type cell_data_value_type;
    typedef typename std::conditional<isConst, const CellType, CellType>::type cell_type_value_type;
    typedef typename std::conditional<isConst, const CellId, CellId>::type cell_offset_value_type;

    typedef typename std::conditional<isConst, const RGBA, RGBA>::type rgba_value_type;
    typedef rgba_value_type& rgba_reference;

    // TMP: to support old API using RGB colors
    typedef typename std::conditional<isConst, const RGB, RGB>::type rgb_value_type;
    typedef rgb_value_type& rgb_reference;

    typedef typename std::conditional<isConst, const Normal, Normal>::type normal_value_type;
    typedef normal_value_type& normal_reference;

    typedef typename std::conditional<isConst, const TexCoords, TexCoords>::type tex_value_type;
    typedef tex_value_type& tex_reference;

    CellInfoBase& operator=(const CellInfoBase<false>& other);

    CellInfoBase& operator=(const CellInfoBase<true>& other);

    bool operator==(const CellInfoBase& other);

    /// Contains the current cell data.
    cell_data_value_type* pointIdx {nullptr};

    /// Contains the current cell type.
    cell_type_value_type* type {nullptr};

    /// Contains the current cell offset.
    cell_offset_value_type* offset {nullptr};

    /// Contains the current cell normal.
    normal_value_type* normal {nullptr};

    /// Contains the current cell rgba color.
    rgba_value_type* rgba {nullptr};

    /// Contains the current cell rgb color.
    rgb_value_type* rgb {nullptr};

    /// Contains the current cell texture coordinates.
    tex_value_type* tex {nullptr};

    /// Contains the number of points in the cell.
    Size nbPoints {0};

    /// Stores current idx (from CellIterator).
    std::ptrdiff_t idx {0};

    /// Stores number of elements (from CellIterator).
    std::ptrdiff_t numberOfElements {0};
};

/**
 * @brief Base class for mesh's point iterators
 *
 * Iterate through the point arrays and check if the index is not out of the bounds
 */
template<bool isConst>
class PointIteratorBase
{
public:

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef PointInfoBase<isConst> PointInfo;
    typedef PointInfo value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    /// Constructor
    PointIteratorBase();
    /// Destructor
    virtual ~PointIteratorBase();
    /// Copy constructor
    PointIteratorBase(const PointIteratorBase& other);

    virtual PointIteratorBase& operator=(const PointIteratorBase& other);

    /// Comparison operators
    bool operator==(const PointIteratorBase& other) const;
    bool operator!=(const PointIteratorBase& other) const;
    bool operator<(const PointIteratorBase& other) const;
    bool operator<=(const PointIteratorBase& other) const;
    bool operator>(const PointIteratorBase& other) const;
    bool operator>=(const PointIteratorBase& other) const;

    /// Increment/Decrement operators
    PointIteratorBase& operator++();
    PointIteratorBase operator++(int);
    PointIteratorBase operator+(difference_type index) const;
    PointIteratorBase& operator+=(difference_type index);
    PointIteratorBase& operator--();
    PointIteratorBase operator--(int);
    PointIteratorBase operator-(difference_type index) const;
    PointIteratorBase& operator-=(difference_type index);

    difference_type operator+(const PointIteratorBase& other) const;
    difference_type operator-(const PointIteratorBase& other) const;
    reference operator*() const;
    pointer operator->() const;

protected:

    friend class data::Mesh;
    friend class PointIterator;
    friend class ConstPointIterator;

    /// Contains iterator informations.
    std::unique_ptr<PointInfo> m_pointInfo;

    /// Defines the current index of the iterator.
    difference_type m_idx {0};

    /// Defines the number of elements of the iterator.
    difference_type m_numberOfElements {0};
};

/**
 * @brief Iterator on Mesh points, point colors, point normals and/or point texture coordinates
 *
 * Iterate through the point buffers and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::TRIANGLE);
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
 */
class DATA_CLASS_API PointIterator : public PointIteratorBase<false>
{
public:

    /// Constructor
    DATA_API PointIterator();
    DATA_API PointIterator(const Mesh* const mesh);
    DATA_API PointIterator(const PointIterator& other) = default;
    DATA_API PointIterator(const PointIteratorBase& other);
    DATA_API ~PointIterator() override;

    DATA_API PointIterator& operator=(const PointIterator& other) = default;
    DATA_API PointIterator& operator=(const PointIteratorBase& other) override;

    friend class ConstPointIterator;
};

/**
 * @brief Const iterator on Mesh points, point colors, point normals and/or point texture coordinates
 *
 * Iterate through the point buffers and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, data::Mesh::TRIANGLE);
    auto iter    = mesh->begin< data::iterator::PointIterator >();
    const auto iterEnd = mesh->end< data::iterator::PointIterator >();
    float p[3];

   for (; iter != iterEnd; ++iter)
   {
       p[0] = iter->point->x;
       p[1] = iter->point->y;
       p[2] = iter->point->z;
   }
   @endcode
 */
class DATA_CLASS_API ConstPointIterator : public PointIteratorBase<true>
{
public:

    /// Constructor
    DATA_API ConstPointIterator();
    DATA_API ConstPointIterator(const Mesh* mesh);
    DATA_API ConstPointIterator(const ConstPointIterator& other) = default;
    DATA_API ConstPointIterator(const PointIteratorBase& other);
    DATA_API ~ConstPointIterator() override;

    DATA_API ConstPointIterator& operator=(const ConstPointIterator& other) = default;
    DATA_API ConstPointIterator& operator=(const PointIteratorBase& other) override;
};

/**
 * @brief Base class for mesh's cells iterators
 *
 * Iterate through the cell arrays and check if the index is not out of the bounds
 */
template<bool isConst>
class CellIteratorBase
{
public:

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef CellInfoBase<isConst> CellInfo;
    typedef CellInfo value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    /// @}

    /// Constructor
    CellIteratorBase();
    /// Destructor
    virtual ~CellIteratorBase();
    /// Copy constructor
    CellIteratorBase(const CellIteratorBase& other);
    virtual CellIteratorBase& operator=(const CellIteratorBase& other);

    /// Comparison operators
    bool operator==(const CellIteratorBase& other) const;
    bool operator!=(const CellIteratorBase& other) const;
    bool operator<(const CellIteratorBase& other) const;
    bool operator<=(const CellIteratorBase& other) const;
    bool operator>(const CellIteratorBase& other) const;
    bool operator>=(const CellIteratorBase& other) const;

    /// Increment/Decrement operators
    CellIteratorBase& operator++();
    CellIteratorBase operator++(int);
    CellIteratorBase operator+(difference_type index) const;
    CellIteratorBase& operator+=(difference_type index);
    CellIteratorBase& operator--();
    CellIteratorBase operator--(int);
    CellIteratorBase operator-(difference_type index) const;
    CellIteratorBase& operator-=(difference_type index);

    difference_type operator+(const CellIteratorBase& other) const;
    difference_type operator-(const CellIteratorBase& other) const;

    reference operator*() const;
    pointer operator->() const;

    typename CellInfo::cell_data_value_type& operator[](size_t index);
    Size nbPoints() const;

protected:

    friend class data::Mesh;
    friend class CellIterator;
    friend class ConstCellIterator;

    /// Contains iterator information.
    std::unique_ptr<CellInfo> m_cellInfo;

    /// Defines the current index of the iterator.
    difference_type m_idx {0};

    /// Defines the number of elements of the iterator.
    difference_type m_numberOfElements {0};

    /// Defines the cell data size of the mesh.
    Size m_cellDataSize {0};

    /// Defines the current offset of the cell.
    difference_type m_currentOffset {0};
};

/**
 * @brief Iterator on Mesh cells, cell colors, cell normals and/or cell texture coordinates
 *
 * Iterate through the buffers and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        const size_t nbPoints = iter.nbPoints();
        for (int i = 0; i< nbPoints ; ++i)
        {
            const auto pointIdx = iter->pointIdx[i];
            //...
        }
    }
   @endcode
 */
class DATA_CLASS_API CellIterator : public CellIteratorBase<false>
{
public:

    friend class ConstCellIterator;

    /// Constructor
    DATA_API CellIterator();
    DATA_API CellIterator(data::Mesh* mesh);
    DATA_API CellIterator(const CellIterator& other) = default;
    DATA_API ~CellIterator() override;

    DATA_API CellIterator& operator=(const CellIterator& other) = default;
    DATA_API CellIterator& operator=(const CellIteratorBase& other) override;
};

/**
 * @brief Iterator on Mesh cells, cell colors, cell normals and/or cell texture coordinates
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        const size_t nbPoints = iter.nbPoints();
        for (int i = 0; i< nbPoints ; ++i)
        {
            const auto pointIdx = iter->pointIdx[i];
            //...
        }
    }
   @endcode
 */
class DATA_CLASS_API ConstCellIterator : public CellIteratorBase<true>
{
public:

    /// Constructor
    DATA_API ConstCellIterator();
    DATA_API ConstCellIterator(const data::Mesh* mesh);
    DATA_API ConstCellIterator(const ConstCellIterator& other) = default;
    DATA_API ConstCellIterator(const CellIterator& other);
    DATA_API ~ConstCellIterator() override;

    DATA_API ConstCellIterator& operator=(const ConstCellIterator& other) = default;
    DATA_API ConstCellIterator& operator=(const CellIteratorBase& other) override;
};

} // namespace iterator.

} // namespace sight::data.

#include "data/iterator/MeshIterators.hxx"
