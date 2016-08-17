/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
