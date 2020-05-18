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

namespace arData
{

namespace timeline
{
//-----------------------------------------------------------------------------

template< typename TYPE >
GenericObject<TYPE>::GenericObject( unsigned int m_maxElementNum, ::fwCore::HiResClock::HiResClockType timestamp,
                                    BufferDataType buffer, size_t size, DeleterType d) :
    GenericObjectBase(m_maxElementNum, timestamp, buffer, size, d)
{
    // init all floating point values to 0
    memset(m_buffer, 0, m_maxElementNum * getElementSize());
}

//-----------------------------------------------------------------------------

template< typename TYPE >
GenericObject<TYPE>::~GenericObject()
{
}

//-----------------------------------------------------------------------------

template< typename TYPE >
const TYPE& GenericObject<TYPE>::getElement(unsigned int index) const
{
    SLM_ASSERT("Index out of bounds", index < m_maxElementNum);

    return *(reinterpret_cast<const ElementType*>(m_buffer + index * getElementSize()));
}

//-----------------------------------------------------------------------------

template< typename TYPE >
void GenericObject<TYPE>::setElement(const ElementType& element, unsigned int index)
{
    SLM_ASSERT("Index out of bounds", index < m_maxElementNum);

    // store floating point values
    ElementType* dstElement = reinterpret_cast<TYPE*>(m_buffer + index * getElementSize());
    memcpy(static_cast<void*>(dstElement), &element, getElementSize());

    // update presence mask
    uint64_t oldMask = m_presenceMask;
    m_presenceMask |= (uint64_t(1) << index);

    if( oldMask != m_presenceMask)
    {
        ++m_numPresent;
    }
}

//-----------------------------------------------------------------------------

template< typename TYPE >
TYPE* GenericObject<TYPE>::addElement(unsigned int index)
{
    SLM_ASSERT("Index out of bounds", index < m_maxElementNum);

    // update presence mask
    uint64_t oldMask = m_presenceMask;
    m_presenceMask |= (uint64_t(1) << index);

    if( oldMask != m_presenceMask)
    {
        ++m_numPresent;
    }

    // return pointer on the new element
    ElementType* dstElement = reinterpret_cast<TYPE*>(m_buffer + index * getElementSize());

    return dstElement;
}

//-----------------------------------------------------------------------------

template< typename TYPE >
typename GenericObject<TYPE>::iterator GenericObject<TYPE>::getPresenceIterator() const
{
    return iterator(*this);
}

//-----------------------------------------------------------------------------

template< typename TYPE >
GenericObject<TYPE>::iterator::iterator(const GenericObjectBase& object) :
    m_object(&object),
    m_currentIndex(0)
{
    m_maxElement = m_object->getMaxElementNum();

    while(m_currentIndex < m_maxElement && !m_object->isPresent(m_currentIndex))
    {
        m_currentIndex++;
    }
}

//-----------------------------------------------------------------------------

template< typename TYPE >
void GenericObject<TYPE>::iterator::operator++()
{
    SLM_ASSERT("Iterator out of bounds", m_currentIndex < m_maxElement);
    while(++m_currentIndex < m_maxElement && !m_object->isPresent(m_currentIndex))
    {
    }
}

//-----------------------------------------------------------------------------

template< typename TYPE >
const TYPE& GenericObject<TYPE>::iterator::operator*() const
{
    const GenericObject<ElementType>* object = static_cast< const GenericObject<ElementType>* >(m_object);
    return object->getElement(m_currentIndex);
}

//-----------------------------------------------------------------------------

} // namespace timeline

} // namespace arData
