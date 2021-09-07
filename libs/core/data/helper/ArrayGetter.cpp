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

#include "data/helper/ArrayGetter.hpp"

namespace sight::data
{

namespace helper
{

ArrayGetter::ArrayGetter(const data::Array::csptr& array) :
    m_array(array)
{
    FW_DEPRECATED_MSG(
        "::data::helper::ArrayGetter is no longer supported, the methods have been moved to "
        "::sight::data::Array",
        "22.0"
    )
    SIGHT_ASSERT("Array ptr is null.", array);
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
    return reinterpret_cast<const char*>(static_cast<const char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

const char* ArrayGetter::getBufferPtr(const data::Array::IndexType& id, size_t component, size_t) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    size_t offset    = m_array->getBufferOffset(id, component, sizeOf);
    const char* item = static_cast<const char*>(this->getBuffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const void* ArrayGetter::getItem(const data::Array::IndexType& id, const size_t component) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, component, sizeOf);
    return item;
}

//------------------------------------------------------------------------------

void ArrayGetter::getItem(const data::Array::IndexType& id, void* value) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, 0, sizeOf);
    char* val        = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents() * sizeOf, val);
}

//------------------------------------------------------------------------------

void ArrayGetter::getItem(const data::Array::IndexType& id, const size_t component, void* value) const
{
    size_t sizeOf    = m_array->getType().sizeOf();
    const char* item = this->getBufferPtr(id, component, sizeOf);
    char* val        = static_cast<char*>(value);
    std::copy(item, item + m_array->getNumberOfComponents() * sizeOf, val);
}

//------------------------------------------------------------------------------

core::memory::BufferObject::Lock ArrayGetter::getLock() const
{
    return m_lock;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace sight::data
