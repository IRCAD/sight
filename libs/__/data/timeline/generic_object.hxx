/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

namespace sight::data::timeline
{

//-----------------------------------------------------------------------------

template<typename TYPE>
generic_object<TYPE>::generic_object(
    unsigned int m_maxElementNum,
    core::hires_clock::type timestamp,
    BufferDataType buffer,
    std::size_t size,
    DeleterType d
) :
    generic_object_base(m_maxElementNum, timestamp, buffer, size, d)
{
    // init all floating point values to 0
    memset(m_buffer, 0, m_maxElementNum * getElementSize());
}

//-----------------------------------------------------------------------------

template<typename TYPE> generic_object<TYPE>::~generic_object()
= default;

//-----------------------------------------------------------------------------

template<typename TYPE>
const TYPE& generic_object<TYPE>::getElement(unsigned int index) const
{
    SIGHT_ASSERT("Index out of bounds", index < m_maxElementNum);

    return *(reinterpret_cast<const ElementType*>(m_buffer + index * getElementSize()));
}

//-----------------------------------------------------------------------------

template<typename TYPE>
void generic_object<TYPE>::setElement(const ElementType& element, unsigned int index)
{
    static_assert(
        std::is_trivially_copyable_v<TYPE>,
        "TYPE must be TriviallyCopyable for generic_object<TYPE>::setElement to be used."
    );
    SIGHT_ASSERT("Index out of bounds", index < m_maxElementNum);

    // store floating point values
    auto* dstElement = reinterpret_cast<TYPE*>(m_buffer + index * getElementSize());
    // undefined behavior if TYPE isn't TriviallyCopyable:
    memcpy(static_cast<void*>(dstElement), &element, getElementSize());

    // update presence mask
    uint64_t oldMask = m_presenceMask;
    m_presenceMask |= (uint64_t(1) << index);

    if(oldMask != m_presenceMask)
    {
        ++m_numPresent;
    }
}

//-----------------------------------------------------------------------------

template<typename TYPE>
TYPE* generic_object<TYPE>::addElement(unsigned int index)
{
    SIGHT_ASSERT("Index out of bounds", index < m_maxElementNum);

    // update presence mask
    uint64_t oldMask = m_presenceMask;
    m_presenceMask |= (uint64_t(1) << index);

    if(oldMask != m_presenceMask)
    {
        ++m_numPresent;
    }

    // return pointer on the new element
    auto* dstElement = reinterpret_cast<TYPE*>(m_buffer + index * getElementSize());

    return dstElement;
}

//-----------------------------------------------------------------------------

template<typename TYPE>
typename generic_object<TYPE>::iterator generic_object<TYPE>::getPresenceIterator() const
{
    return iterator(*this);
}

//-----------------------------------------------------------------------------

template<typename TYPE>
generic_object<TYPE>::iterator::iterator(const generic_object_base& object) :
    m_object(&object),
    m_maxElement(m_object->getMaxElementNum())
{
    while(m_currentIndex < m_maxElement && !m_object->isPresent(m_currentIndex))
    {
        m_currentIndex++;
    }
}

//-----------------------------------------------------------------------------

template<typename TYPE>
void generic_object<TYPE>::iterator::operator++()
{
    SIGHT_ASSERT("Iterator out of bounds", m_currentIndex < m_maxElement);
    while(++m_currentIndex < m_maxElement && !m_object->isPresent(m_currentIndex))
    {
    }
}

//-----------------------------------------------------------------------------

template<typename TYPE>
const TYPE& generic_object<TYPE>::iterator::operator*() const
{
    const auto* object = static_cast<const generic_object<ElementType>*>(m_object);
    return object->getElement(m_currentIndex);
}

//-----------------------------------------------------------------------------

} // namespace sight::data::timeline
