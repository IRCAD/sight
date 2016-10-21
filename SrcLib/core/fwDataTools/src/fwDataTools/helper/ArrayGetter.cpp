/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/ArrayGetter.hpp"

namespace fwDataTools
{

namespace helper
{

ArrayGetter::ArrayGetter( ::fwData::Array::csptr array ) : m_array (array)
{
    SLM_ASSERT("Array ptr is null.", array);
    m_lock = array->getBufferObject()->lock();
}

//-----------------------------------------------------------------------------

ArrayGetter::~ArrayGetter()
{

}

//-----------------------------------------------------------------------------

const void* ArrayGetter::getBuffer() const
{
    return m_lock.getBuffer();
}

//------------------------------------------------------------------------------

const char* ArrayGetter::begin() const
{
    return static_cast<const char*>(this->getBuffer());
}

//------------------------------------------------------------------------------

const char* ArrayGetter::end() const
{
    return reinterpret_cast<const char*> (static_cast<const char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

const char* ArrayGetter::getBufferPtr( const ::fwData::Array::IndexType& id, size_t component, size_t sizeOfType ) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    size_t offset    = m_array->getBufferOffset(id, component, sizeOf);
    const char* item = static_cast<const char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const void* ArrayGetter::getItem(const ::fwData::Array::IndexType& id, const size_t component) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, component, sizeOf);
    return item;
}

//------------------------------------------------------------------------------

void ArrayGetter::getItem(const ::fwData::Array::IndexType& id, void* value) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, 0, sizeOf);
    char* val        = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents()*sizeOf, val);
}

//------------------------------------------------------------------------------

void ArrayGetter::getItem(const ::fwData::Array::IndexType& id, const size_t component, void* value) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, component, sizeOf);
    char* val        = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents()*sizeOf, val);
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::Lock ArrayGetter::getLock() const
{
    return m_lock;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwDataTools
