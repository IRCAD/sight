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
