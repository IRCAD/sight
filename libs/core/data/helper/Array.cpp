/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/helper/Array.hpp"

namespace sight::data
{

namespace helper
{

Array::Array(const data::Array::sptr& array) :
    m_array(array)
{
    FW_DEPRECATED_MSG(
        "::data::helper::Array is no longer supported, the methods have been moved to "
        "::sight::data::Array",
        "22.0"
    );
    SIGHT_ASSERT("Array ptr is null.", array);
    m_lock = array->getBufferObject()->lock();
}

//-----------------------------------------------------------------------------

Array::~Array()
{
}

//-----------------------------------------------------------------------------

void* Array::getBuffer()
{
    return m_lock.getBuffer();
}

//-----------------------------------------------------------------------------

const void* Array::getBuffer() const
{
    return m_lock.getBuffer();
}

//------------------------------------------------------------------------------

void Array::setBuffer(void* buf, bool takeOwnership, core::memory::BufferAllocationPolicy::sptr policy)
{
    if(m_array->getIsBufferOwner())
    {
        if(!m_array->getBufferObject()->isEmpty())
        {
            m_array->getBufferObject()->destroy();
        }
    }
    else
    {
        core::memory::BufferObject::sptr newBufferObject = core::memory::BufferObject::New();
        core::memory::BufferObject::sptr oldBufferObject = m_array->getBufferObject();
        oldBufferObject->swap(newBufferObject);
    }

    m_array->getBufferObject()->setBuffer(buf, (buf == NULL) ? 0 : m_array->getSizeInBytes(), policy);
    m_array->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

void Array::setBuffer(
    void* buf,
    bool takeOwnership,
    const core::tools::Type& type,
    const data::Array::SizeType& size,
    size_t nbOfComponents,
    core::memory::BufferAllocationPolicy::sptr policy
)
{
    m_array->resize(type, size, nbOfComponents, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//-----------------------------------------------------------------------------

char* Array::begin()
{
    void* v = this->getBuffer();
    return static_cast<char*>(v);
}

//------------------------------------------------------------------------------

char* Array::end()
{
    return reinterpret_cast<char*>(static_cast<char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//------------------------------------------------------------------------------

const char* Array::begin() const
{
    return static_cast<const char*>(this->getBuffer());
}

//------------------------------------------------------------------------------

const char* Array::end() const
{
    return reinterpret_cast<const char*>(static_cast<const char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

char* Array::getBufferPtr(const data::Array::IndexType& id, size_t component, size_t)
{
    size_t sizeOf = m_array->getType().sizeOf();
    size_t offset = m_array->getBufferOffset(id, component, sizeOf);
    char* item    = static_cast<char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const char* Array::getBufferPtr(const data::Array::IndexType& id, size_t component, size_t) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    size_t offset    = m_array->getBufferOffset(id, component, sizeOf);
    const char* item = static_cast<const char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

void Array::setItem(const data::Array::IndexType& id, const void* value)
{
    size_t sizeOf   = m_array->getType().sizeOf();
    const char* val = static_cast<const char*>(value);
    char* item      = this->getBufferPtr(id, 0, sizeOf);
    std::copy(val, val + m_array->getNumberOfComponents() * sizeOf, item);
}

//------------------------------------------------------------------------------

void Array::setItem(const data::Array::IndexType& id, const size_t component, const void* value)
{
    size_t sizeOf   = m_array->getType().sizeOf();
    const char* val = static_cast<const char*>(value);
    char* item      = this->getBufferPtr(id, component, sizeOf);
    std::copy(val, val + sizeOf, item);
}

//------------------------------------------------------------------------------

void* Array::getItem(const data::Array::IndexType& id, const size_t component)
{
    size_t sizeOf = m_array->getType().sizeOf();
    char* item    = this->getBufferPtr(id, component, sizeOf);
    return item;
}

//------------------------------------------------------------------------------

void Array::getItem(const data::Array::IndexType& id, void* value) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, 0, sizeOf);
    char* val        = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents() * sizeOf, val);
}

//------------------------------------------------------------------------------

void Array::getItem(const data::Array::IndexType& id, const size_t component, void* value) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, component, sizeOf);
    char* val        = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents() * sizeOf, val);
}

//------------------------------------------------------------------------------

core::memory::BufferObject::Lock Array::getLock() const
{
    return m_lock;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace sight::data
