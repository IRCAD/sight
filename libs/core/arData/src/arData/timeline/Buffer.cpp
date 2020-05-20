/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <fwMemory/exception/Memory.hpp>

#include "arData/timeline/Buffer.hpp"

namespace arData
{

namespace timeline
{

//-----------------------------------------------------------------------------

Buffer::Buffer( ::fwCore::HiResClock::HiResClockType timestamp,
                BufferDataType buffer, size_t size, DeleterType d) :
    Object(timestamp),
    m_size(size),
    m_buffer(buffer),
    m_deleter(d)
{
}

//-----------------------------------------------------------------------------

Buffer::~Buffer()
{
    if(m_deleter)
    {
        m_deleter(m_buffer);
    }
}

//-----------------------------------------------------------------------------

void Buffer::deepCopy(const ::arData::timeline::Object& other)
{
    Object::deepCopy(other);

    const Buffer& otherObject = static_cast<const Buffer&>(other);
    memcpy(m_buffer, otherObject.m_buffer, m_size);
}

} // namespace timeline

} // namespace arData
