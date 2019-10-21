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
    static const size_t elementSize{3};
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
    static const size_t elementSize{3};
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
    static const size_t elementSize{4};
};

struct TextCoords {
    typedef float element_type;
    typedef std::array<element_type, 2> value_type;
    //------------------------------------------------------------------------------

    TextCoords& operator=(const value_type& val)
    {
        u = val[0];
        v = val[1];
        return *this;
    }

    element_type u;
    element_type v;
    static const size_t elementSize{2};
};

/**
 * @brief Base class for mesh iterators
 *
 * Iterate through the arrays and check if the index is not out of the bounds
 */
class FWDATA_CLASS_API MeshIteratorBase
{
public:

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef char* pointer;
    typedef char value_type;
    typedef char& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    /// Constructor
    FWDATA_API MeshIteratorBase();
    /// Copy constructor
    FWDATA_API MeshIteratorBase(const MeshIteratorBase& other);
    /// Destructor
    FWDATA_API virtual ~MeshIteratorBase();

    FWDATA_API MeshIteratorBase& operator=(const MeshIteratorBase& other);

    /// Comparison operators
    FWDATA_API bool operator==(const MeshIteratorBase& other) const;
    FWDATA_API bool operator!=(const MeshIteratorBase& other) const;

    /// Increment/Decrement operators
    FWDATA_API MeshIteratorBase& operator++();
    FWDATA_API MeshIteratorBase operator++(int);
    FWDATA_API MeshIteratorBase& operator+(difference_type index);
    FWDATA_API MeshIteratorBase& operator+=(difference_type index);
    FWDATA_API MeshIteratorBase& operator--();
    FWDATA_API MeshIteratorBase operator--(int);
    FWDATA_API MeshIteratorBase& operator-(difference_type index);
    FWDATA_API MeshIteratorBase& operator-=(difference_type index);

    FWDATA_API difference_type operator+(const MeshIteratorBase& other) const;
    FWDATA_API difference_type operator-(const MeshIteratorBase& other) const;

protected:

    std::array< pointer, 4> m_pointers;
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;
    difference_type m_idx{0};
    difference_type m_numberOfElements{0};
    std::array< difference_type, 4> m_elementSizes;
    std::uint8_t m_nbArrays{1};

    std::uint8_t m_colorIdx{1};
    std::uint8_t m_normalIdx{2};
    std::uint8_t m_texIdx{3};
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
class FWDATA_CLASS_API PointIterator : public MeshIteratorBase
{
public:

    typedef Point point_value_type;
    typedef Point& point_reference;

    typedef RGBA color_value_type;
    typedef RGBA& color_reference;

    typedef Normal normal_value_type;
    typedef Normal& normal_reference;

    typedef TextCoords tex_value_type;
    typedef TextCoords& tex_reference;

    /// Constructor
    FWDATA_API PointIterator(Mesh* mesh);
    FWDATA_API virtual ~PointIterator() override;

    friend class ConstPointIterator;

    FWDATA_API point_reference point();
    FWDATA_API color_reference color();
    FWDATA_API normal_reference normal();
    FWDATA_API tex_reference tex();
    FWDATA_API point_reference operator*();
    FWDATA_API PointIterator* operator->();
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
class FWDATA_CLASS_API ConstPointIterator : public MeshIteratorBase
{
public:
    typedef const Point point_value_type;
    typedef const Point& point_reference;

    typedef const RGBA color_value_type;
    typedef const RGBA& color_reference;

    typedef const Normal normal_value_type;
    typedef const Normal& normal_reference;

    typedef const TextCoords tex_value_type;
    typedef const TextCoords& tex_reference;

    /// Constructor
    FWDATA_API ConstPointIterator(const Mesh* mesh);
    FWDATA_API virtual ~ConstPointIterator() override;

    FWDATA_API point_reference point();
    FWDATA_API color_reference color();
    FWDATA_API normal_reference normal();
    FWDATA_API tex_reference tex();
    FWDATA_API point_reference operator*();
    FWDATA_API ConstPointIterator* operator->();
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
        iter->point().x = val1;
        iter->point().y = val2;
        iter->point().z = val2;
    }
   @endcode
 */
class FWDATA_CLASS_API CellIterator : public MeshIteratorBase
{
public:

    typedef std::uint64_t point_idx_value_type;
    typedef std::uint64_t& point_idx_reference;

    typedef Point point_value_type;
    typedef Point& point_reference;

    typedef RGBA color_value_type;
    typedef RGBA& color_reference;

    typedef Normal normal_value_type;
    typedef Normal& normal_reference;

    typedef TextCoords tex_value_type;
    typedef TextCoords& tex_reference;

    /// Constructor
    FWDATA_API CellIterator();
    FWDATA_API CellIterator(const CellIterator& other);
    FWDATA_API virtual ~CellIterator() override;

    FWDATA_API CellIterator& operator=(const CellIterator& other);

    FWDATA_API point_idx_reference pointIdx(std::uint8_t id);
    FWDATA_API color_reference color();
    FWDATA_API normal_reference normal();
    FWDATA_API tex_reference tex();
    FWDATA_API point_reference operator*();
    FWDATA_API PointIterator* operator->();

protected:

    std::uint64_t* m_cellDataPointer;
};

} // namespace iterator
} // namespace fwData
