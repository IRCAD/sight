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

#include <fwData/Mesh.hpp>

#include <iterator>

namespace fwData
{

namespace iterator
{

PointIteratorBase::PointIteratorBase()
{

}

PointIteratorBase::PointIteratorBase(const PointIteratorBase& other) :
    m_pointer(other.m_pointer),
    m_lock(other.m_lock),
    m_idx(other.m_idx),
    m_elementSize(other.m_elementSize),
    m_numberOfElements(other.m_numberOfElements)
{

}

PointIteratorBase::~PointIteratorBase()
{
    m_lock.reset();
}

//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator=(const PointIteratorBase& other)
{
    if (this != &other)
    {
        m_pointer          = other.m_pointer;
        m_lock             = other.m_lock;
        m_idx              = other.m_idx;
        m_elementSize      = other.m_elementSize;
        m_numberOfElements = other.m_numberOfElements;
    }
    return *this;
}

//------------------------------------------------------------------------------

bool PointIteratorBase::operator==(const PointIteratorBase& other) const
{
    return m_idx == other.m_idx;
}
//------------------------------------------------------------------------------

bool PointIteratorBase::operator!=(const PointIteratorBase& other) const
{
    return m_idx != other.m_idx;
}

//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator++()
{
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += m_elementSize;
    return *this;
}
//------------------------------------------------------------------------------

PointIteratorBase PointIteratorBase::operator++(int)
{
    PointIteratorBase tmp(*this);
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += m_elementSize;
    return tmp;
}
//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator+(difference_type index)
{
    m_idx += index;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += index*m_elementSize;
    return *this;
}
//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator+=(difference_type index)
{
    m_idx += index;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += index*m_elementSize;
    return *this;
}
//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    m_pointer -= m_elementSize;
    return *this;
}
//------------------------------------------------------------------------------

PointIteratorBase PointIteratorBase::operator--(int)
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    PointIteratorBase tmp(*this);
    m_pointer -= m_elementSize;
    return tmp;
}

//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator-(difference_type index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx     = m_idx - index;
    m_pointer = m_pointer - index*m_elementSize;
    return *this;
}
//------------------------------------------------------------------------------

PointIteratorBase& PointIteratorBase::operator-=(difference_type index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx     -= index;
    m_pointer -= index*m_elementSize;
    return *this;
}

//------------------------------------------------------------------------------

PointIteratorBase::difference_type PointIteratorBase::operator+(const PointIteratorBase& other) const
{
    return m_idx + other.m_idx;
}
//------------------------------------------------------------------------------

PointIteratorBase::difference_type PointIteratorBase::operator-(const PointIteratorBase& other) const
{
    return m_idx - other.m_idx;

}

//------------------------------------------------------------------------------

PointIteratorBase::reference PointIteratorBase::operator*()
{
    return *m_pointer;
}

//------------------------------------------------------------------------------

PointIteratorBase::value_type* PointIteratorBase::operator->()
{
    return m_pointer;
}

PointIterator::PointIterator(::fwData::Mesh* mesh)
{
    m_lock             = mesh->m_points->lock();
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointer          = static_cast<char*>(mesh->m_points->getBuffer());
    m_elementSize      = sizeof (value_type);
}

//------------------------------------------------------------------------------

PointIterator::reference PointIterator::operator*()
{
    return *(reinterpret_cast<Point*>(m_pointer));
}

//------------------------------------------------------------------------------

PointIterator::value_type* PointIterator::operator->()
{
    return reinterpret_cast<Point*>(m_pointer);
}

ConstPointIterator::ConstPointIterator(const ::fwData::Mesh* mesh)
{
    m_lock             = mesh->m_points->lock();
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointer          = static_cast<char*>(mesh->m_points->getBuffer());
    m_elementSize      = sizeof (value_type);
}

//------------------------------------------------------------------------------

ConstPointIterator::reference ConstPointIterator::operator*()
{
    return *(reinterpret_cast<Point*>(m_pointer));
}

//------------------------------------------------------------------------------

ConstPointIterator::value_type* ConstPointIterator::operator->()
{
    return reinterpret_cast<Point*>(m_pointer);
}

} // namespace iterator
} // namespace fwData
