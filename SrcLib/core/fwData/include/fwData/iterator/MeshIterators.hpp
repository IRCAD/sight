/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwData/config.hpp"

#include <fwMemory/BufferObject.hpp>

#include <iterator>

namespace fwData
{
class Mesh;

namespace iterator
{

struct Point {
    typedef float element_type;
    typedef std::array<element_type, 3> value_type;
    //------------------------------------------------------------------------------

    Point& operator=(const value_type& val)
    {
        x = val[0];
        y = val[1];
        z = val[2];
        return *this;
    }

    element_type x;
    element_type y;
    element_type z;
    static constexpr size_t elementSize{3};
};

struct Normal {
    typedef float element_type;
    typedef std::array<element_type, 3> value_type;
    //------------------------------------------------------------------------------

    Normal& operator=(const value_type& val)
    {
        nx = val[0];
        ny = val[1];
        nz = val[2];
        return *this;
    }

    element_type nx;
    element_type ny;
    element_type nz;
    static constexpr size_t elementSize{3};
};

struct RGBA {
    typedef std::uint8_t element_type;
    typedef std::array<element_type, 4> value_type;
    //------------------------------------------------------------------------------

    RGBA& operator=(const value_type& val)
    {
        r = val[0];
        g = val[1];
        b = val[2];
        a = val[3];
        return *this;
    }

    element_type r;
    element_type g;
    element_type b;
    element_type a;
    static constexpr size_t elementSize{4};
};

struct TexCoords {
    typedef float element_type;
    typedef std::array<element_type, 2> value_type;
    //------------------------------------------------------------------------------

    TexCoords& operator=(const value_type& val)
    {
        u = val[0];
        v = val[1];
        return *this;
    }

    element_type u;
    element_type v;
    static constexpr size_t elementSize{2};
};

/**
 * @brief Base class for mesh's point iterators
 *
 * Iterate through the point arrays and check if the index is not out of the bounds
 */
template<bool isConst>
class FWDATA_CLASS_API PointIteratorBase
{
public:

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef typename std::conditional<isConst, const char, char>::type value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    typedef typename std::conditional<isConst, const Point, Point>::type point_value_type;
    typedef point_value_type& point_reference;

    typedef typename std::conditional<isConst, const RGBA, RGBA>::type color_value_type;
    typedef color_value_type& color_reference;

    typedef typename std::conditional<isConst, const Normal, Normal>::type normal_value_type;
    typedef normal_value_type& normal_reference;

    typedef typename std::conditional<isConst, const TexCoords, TexCoords>::type tex_value_type;
    typedef tex_value_type& tex_reference;

    struct PointInfo {
        point_value_type* point{nullptr};
        normal_value_type* normal{nullptr};
        color_value_type* color{nullptr};
        tex_value_type* tex{nullptr};
    };

    /// Constructor
    FWDATA_API PointIteratorBase();
    /// Destructor
    FWDATA_API virtual ~PointIteratorBase();
    /// Copy constructor
    FWDATA_API PointIteratorBase(const PointIteratorBase& other) = default;

    FWDATA_API PointIteratorBase& operator=(const PointIteratorBase& other) = default;

    /// Comparison operators
    FWDATA_API bool operator==(const PointIteratorBase& other) const;
    FWDATA_API bool operator!=(const PointIteratorBase& other) const;

    /// Increment/Decrement operators
    FWDATA_API PointIteratorBase& operator++();
    FWDATA_API PointIteratorBase operator++(int);
    FWDATA_API PointIteratorBase operator+(difference_type index);
    FWDATA_API PointIteratorBase& operator+=(difference_type index);
    FWDATA_API PointIteratorBase& operator--();
    FWDATA_API PointIteratorBase operator--(int);
    FWDATA_API PointIteratorBase operator-(difference_type index);
    FWDATA_API PointIteratorBase& operator-=(difference_type index);

    FWDATA_API difference_type operator+(const PointIteratorBase& other) const;
    FWDATA_API difference_type operator-(const PointIteratorBase& other) const;
    FWDATA_API PointInfo operator*();
    FWDATA_API PointInfo* operator->();

protected:

    friend class ::fwData::Mesh;
    friend class PointIterator;
    friend class ConstPointIterator;

    PointInfo m_pointInfo;
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;
    difference_type m_idx{0};
    difference_type m_numberOfElements{0};
};

/**
 * @brief Iterator on Mesh points, point colors and/or point normals
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        iter->point().x = val1;
        iter->point().y = val2;
        iter->point().z = val2;
    }
   @endcode
 */
class FWDATA_CLASS_API PointIterator : public PointIteratorBase<false>
{
public:

    /// Constructor
    FWDATA_API PointIterator(Mesh* mesh);
    FWDATA_API PointIterator(const PointIterator& other) = default;
    FWDATA_API PointIterator(const PointIteratorBase& other);
    FWDATA_API virtual ~PointIterator() override;

    FWDATA_API PointIterator& operator=(const PointIterator& other) = default;
    FWDATA_API PointIterator& operator=(const PointIteratorBase& other);

    friend class ConstPointIterator;
};

/**
 * @brief Const iterator on Mesh points, point colors and/or point normals
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        iter->point().x = val1;
        iter->point().y = val2;
        iter->point().z = val2;
    }
   @endcode
 */
class FWDATA_CLASS_API ConstPointIterator : public PointIteratorBase<true>
{
public:
    /// Constructor
    FWDATA_API ConstPointIterator(const Mesh* mesh);
    FWDATA_API ConstPointIterator(const ConstPointIterator& other) = default;
    FWDATA_API ConstPointIterator(const PointIteratorBase& other);
    FWDATA_API virtual ~ConstPointIterator() override;

    FWDATA_API ConstPointIterator& operator=(const ConstPointIterator& other) = default;
    FWDATA_API ConstPointIterator& operator=(const PointIteratorBase& other);
};

/**
 * @brief Base class for mesh's cells iterators
 *
 * Iterate through the point arrays and check if the index is not out of the bounds
 */
template<bool isConst>
class FWDATA_CLASS_API CellIteratorBase
{
public:

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef typename std::conditional<isConst, const char, char>::type value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    /// @}
    typedef typename std::conditional<isConst, const std::uint64_t, std::uint64_t>::type cell_data_value_type;
    typedef typename std::conditional<isConst, const std::uint8_t, std::uint8_t>::type cell_type_value_type;
    typedef typename std::conditional<isConst, const std::uint64_t, std::uint64_t>::type cell_offset_value_type;

    typedef typename std::conditional<isConst, const RGBA, RGBA>::type color_value_type;
    typedef color_value_type& color_reference;

    typedef typename std::conditional<isConst, const Normal, Normal>::type normal_value_type;
    typedef normal_value_type& normal_reference;

    typedef typename std::conditional<isConst, const TexCoords, TexCoords>::type tex_value_type;
    typedef tex_value_type& tex_reference;

    struct CellInfo {
        cell_data_value_type* pointIdx{nullptr};
        cell_type_value_type* type{nullptr};
        cell_offset_value_type* offset{nullptr};
        normal_value_type* normal{nullptr};
        color_value_type* color{nullptr};
        tex_value_type* tex{nullptr};
        size_t nbPoints{0};
    };

    /// Constructor
    FWDATA_API CellIteratorBase();
    /// Destructor
    FWDATA_API virtual ~CellIteratorBase();
    /// Copy constructor
    FWDATA_API CellIteratorBase(const CellIteratorBase& other)            = default;
    FWDATA_API CellIteratorBase& operator=(const CellIteratorBase& other) = default;

    /// Comparison operators
    FWDATA_API bool operator==(const CellIteratorBase& other) const;
    FWDATA_API bool operator!=(const CellIteratorBase& other) const;

    /// Increment/Decrement operators
    FWDATA_API CellIteratorBase& operator++();
    FWDATA_API CellIteratorBase operator++(int);
    FWDATA_API CellIteratorBase operator+(difference_type index);
    FWDATA_API CellIteratorBase& operator+=(difference_type index);
    FWDATA_API CellIteratorBase& operator--();
    FWDATA_API CellIteratorBase operator--(int);
    FWDATA_API CellIteratorBase operator-(difference_type index);
    FWDATA_API CellIteratorBase& operator-=(difference_type index);

    FWDATA_API difference_type operator+(const CellIteratorBase& other) const;
    FWDATA_API difference_type operator-(const CellIteratorBase& other) const;

    FWDATA_API CellInfo operator*();
    FWDATA_API CellInfo* operator->();

    FWDATA_API cell_data_value_type& operator[](size_t index);
    FWDATA_API size_t nbPoints() const;

protected:

    friend class ::fwData::Mesh;
    friend class CellIterator;
    friend class ConstCellIterator;

    CellInfo m_cellInfo;
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;
    difference_type m_idx{0};
    difference_type m_numberOfElements{0};
    std::uint64_t m_cellDataSize{0};
    difference_type m_currentOffset{0};
};

/**
 * @brief Iterator on Mesh cells, cell colors and/or cell normals
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        const size_t nbPoints = iter.nbPoints();
        for (int i = 0; i< nbPoints ; ++i)
        {
            const auto pointIdx = iter.pointIdx(i);
            //...
        }
    }
   @endcode
 */
class FWDATA_CLASS_API CellIterator : public CellIteratorBase<false>
{
public:

    friend class ConstCellIterator;

    /// Constructor
    FWDATA_API CellIterator(::fwData::Mesh* mesh);
    FWDATA_API CellIterator(const CellIterator& other) = default;
    FWDATA_API virtual ~CellIterator() override;

    FWDATA_API CellIterator& operator=(const CellIterator& other) = default;
};

/**
 * @brief Iterator on Mesh cells, cell colors and/or cell normals
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        const size_t nbPoints = iter.nbPoints();
        for (int i = 0; i< nbPoints ; ++i)
        {
            const auto pointIdx = iter.pointIdx(i);
            //...
        }
    }
   @endcode
 */
class FWDATA_CLASS_API ConstCellIterator : public CellIteratorBase<true>
{
public:

    /// Constructor
    FWDATA_API ConstCellIterator(const ::fwData::Mesh* mesh);
    FWDATA_API ConstCellIterator(const ConstCellIterator& other) = default;
    FWDATA_API ConstCellIterator(const CellIterator& other);
    FWDATA_API virtual ~ConstCellIterator() override;

    FWDATA_API ConstCellIterator& operator=(const ConstCellIterator& other) = default;
};

} // namespace iterator
} // namespace fwData

#include "fwData/iterator/MeshIterators.hxx"
