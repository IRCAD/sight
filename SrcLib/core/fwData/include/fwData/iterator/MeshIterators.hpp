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
 * @brief Iterator on Mesh points, point colors or point normals
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * @tparam FORMAT format used to iterate though the buffer, should be one of the format PointIteration (for points or
 * normals), PointRBGIteration, PointRBGAIteration.
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        iter->x = val1;
        iter->y = val2;
        iter->z = val2;
    }
   @endcode
 */
class FWDATA_CLASS_API PointIteratorBase
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
    FWDATA_API PointIteratorBase();
    /// Copy constructor
    FWDATA_API PointIteratorBase(const PointIteratorBase& other);
    /// Destructor
    FWDATA_API virtual ~PointIteratorBase();

    FWDATA_API PointIteratorBase& operator=(const PointIteratorBase& other);

    /// Comparison operators
    FWDATA_API bool operator==(const PointIteratorBase& other) const;
    FWDATA_API bool operator!=(const PointIteratorBase& other) const;

    /// Increment/Decrement operators
    FWDATA_API PointIteratorBase& operator++();
    FWDATA_API PointIteratorBase operator++(int);
    FWDATA_API PointIteratorBase& operator+(difference_type index);
    FWDATA_API PointIteratorBase& operator+=(difference_type index);
    FWDATA_API PointIteratorBase& operator--();
    FWDATA_API PointIteratorBase operator--(int);
    FWDATA_API PointIteratorBase& operator-(difference_type index);
    FWDATA_API PointIteratorBase& operator-=(difference_type index);

    FWDATA_API difference_type operator+(const PointIteratorBase& other) const;
    FWDATA_API difference_type operator-(const PointIteratorBase& other) const;

    /// Value access operators
    FWDATA_API reference operator*();

    /// Value access operators
    FWDATA_API value_type* operator->();

protected:

    pointer m_pointer{nullptr};
    ::fwMemory::BufferObject::Lock m_lock;
    difference_type m_idx{0};
    difference_type m_elementSize{3};
    difference_type m_numberOfElements{0};
};

class FWDATA_CLASS_API PointIterator : public PointIteratorBase
{
public:
    typedef Point value_type;
    typedef Point& reference;
    /// Constructor
    FWDATA_API PointIterator(Mesh* mesh);
    FWDATA_API virtual ~PointIterator()
    {
    }

    /// Value access operators
    FWDATA_API reference operator*();

    /// Value access operators
    FWDATA_API value_type* operator->();

    friend class ConstPointIterator;
};

class FWDATA_CLASS_API ConstPointIterator : public PointIteratorBase
{
public:

    typedef const Point value_type;
    typedef const Point& reference;

    /// Constructor
    FWDATA_API ConstPointIterator(const Mesh* mesh);
    FWDATA_API ConstPointIterator(const PointIterator* mesh);
    FWDATA_API virtual ~ConstPointIterator()
    {
    }

    /// Value access operators
    FWDATA_API reference operator*();

    /// Value access operators
    FWDATA_API value_type* operator->();
};
} // namespace iterator
} // namespace fwData
