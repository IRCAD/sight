/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_TIMELINE_GENERICOBJECT_HXX__
#define __ARDATA_TIMELINE_GENERICOBJECT_HXX__

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
    memcpy(dstElement, &element, getElementSize());

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
    m_object(&object)
    ,
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

#endif /* __ARDATA_TIMELINE_GENERICOBJECT_HXX__ */
