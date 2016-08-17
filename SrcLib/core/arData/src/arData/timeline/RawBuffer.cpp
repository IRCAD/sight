/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/timeline/RawBuffer.hpp"

namespace arData
{

namespace timeline
{

//-----------------------------------------------------------------------------

RawBuffer::RawBuffer(::fwCore::HiResClock::HiResClockType timestamp,
                     BufferDataType buffer, size_t size, DeleterType d) :
    Buffer(timestamp, buffer, size, d)
{
}

//-----------------------------------------------------------------------------

RawBuffer::~RawBuffer()
{
}

//-----------------------------------------------------------------------------

void RawBuffer::setBufferValues(BufferDataType values)
{
    memcpy(m_buffer, values, m_size);
}

} // namespace timeline

} // namespace arData
