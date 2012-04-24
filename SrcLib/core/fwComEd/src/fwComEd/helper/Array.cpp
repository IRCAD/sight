/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/helper/Array.hpp"

namespace fwComEd
{

namespace helper
{

Array::Array( ::fwData::Array::sptr array ) : m_array (array)
{
    SLM_ASSERT("Array ptr is null.", array);
    m_lock = array->getBufferObject()->lock();
}

//-----------------------------------------------------------------------------

Array::~Array()
{

}

//-----------------------------------------------------------------------------

void *Array::getBuffer()
{
    return m_lock.getBuffer();
}

//-----------------------------------------------------------------------------

const void *Array::getBuffer() const
{
    return m_lock.getBuffer();
}

//------------------------------------------------------------------------------

void Array::setBuffer(void *buf, bool takeOwnership)
{
    ::fwTools::BufferObject::sptr attrBufferObject = m_array->getBufferObject();
    if(!attrBufferObject->isEmpty() && m_array->getIsBufferOwner())
    {
        attrBufferObject->destroy();
    }
    attrBufferObject->setBuffer(buf, (buf == NULL) ? 0 : m_array->getSizeInBytes());
    m_array->setIsBufferOwner(!attrBufferObject->isEmpty() && takeOwnership);
}

//------------------------------------------------------------------------------

void Array::setBuffer(
        void *buf,
        bool takeOwnership,
        const ::fwTools::Type &type,
        const  ::fwData::Array::SizeType &size,
        size_t nbOfComponents )
{
    m_array->resize( type, size, nbOfComponents, false);
    this->setBuffer(buf, takeOwnership);
}

//-----------------------------------------------------------------------------

char *Array::begin()
{
    void *v = this->getBuffer();
    return static_cast<char*>(v);
}

//------------------------------------------------------------------------------

char *Array::end()
{
    return reinterpret_cast<char*> (static_cast<char*>(this->getBuffer()) + m_array->getSizeInBytes());
}
//------------------------------------------------------------------------------

const char *Array::begin() const
{
    return static_cast<const char*>(this->getBuffer());
}

//------------------------------------------------------------------------------

const char *Array::end() const
{
    return reinterpret_cast<const char*> (static_cast<const char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

char *Array::getBufferPtr( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType )
{
    size_t sizeOf = m_array->getType().sizeOf();
    size_t offset = m_array->getBufferOffset(id, component, sizeOf);
    char *item = static_cast<char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const char *Array::getBufferPtr( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const
{
    size_t sizeOf = m_array->getType().sizeOf();
    size_t offset = m_array->getBufferOffset(id, component, sizeOf);
    const char *item = static_cast<const char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

void Array::setItem(const ::fwData::Array::IndexType &id, const void *value)
{
    size_t sizeOf = m_array->getType().sizeOf();
    const char *val  = static_cast<const char*>(value);
    char *item = this->getBufferPtr(id, 0, sizeOf);
    std::copy(val, val + m_array->getNumberOfComponents()*sizeOf, item);
}
//------------------------------------------------------------------------------


void Array::setItem(const ::fwData::Array::IndexType &id, const size_t component, const void *value)
{
    size_t sizeOf = m_array->getType().sizeOf();
    const char *val  = static_cast<const char*>(value);
    char *item = this->getBufferPtr(id, component, sizeOf);
    std::copy(val, val + sizeOf, item);
}


//------------------------------------------------------------------------------

void *Array::getItem(const ::fwData::Array::IndexType &id, const size_t component)
{
    size_t sizeOf = m_array->getType().sizeOf();
    char *item = this->getBufferPtr(id, component, sizeOf);
    return item;
}

//------------------------------------------------------------------------------

void Array::getItem(const ::fwData::Array::IndexType &id, void *value) const
{
    size_t sizeOf = m_array->getType().sizeOf();
    const char *item = this->getBufferPtr(id, 0, sizeOf);
    char *val  = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents()*sizeOf, val);
}

//------------------------------------------------------------------------------

void Array::getItem(const ::fwData::Array::IndexType &id, const size_t component, void *value) const
{
    size_t sizeOf = m_array->getType().sizeOf();
    const char *item = this->getBufferPtr(id, component, sizeOf);
    char *val  = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents()*sizeOf, val);
}

//------------------------------------------------------------------------------

::fwTools::BufferObject::Lock Array::getLock() const
{
    return m_lock;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwComEd
