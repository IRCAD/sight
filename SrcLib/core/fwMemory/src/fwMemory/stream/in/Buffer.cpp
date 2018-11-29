/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <fwCore/macros.hpp>
#include <fwCore/spyLog.hpp>

#include "fwMemory/stream/in/Buffer.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

struct HoldCounterStream : ::boost::iostreams::stream< ::boost::iostreams::array_source >
{
    HoldCounterStream(char *buf, size_t size, const Buffer::LockType &lock) :
        ::boost::iostreams::stream< ::boost::iostreams::array_source >(buf, size),
        m_counter(lock)
    {
    }

    Buffer::LockType m_counter;
};

Buffer::LockType noFactory()
{
    return Buffer::LockType();
}

Buffer::Buffer(void* buf, size_t size) :
    m_buf(buf), m_size(size), m_counterFactory(&noFactory)
{
    OSLM_ASSERT("Buffer is null.", m_buf || size == 0);
}

Buffer::Buffer(void* buf, size_t size, CounterFactoryType counterFactory) :
    m_buf(buf), m_size(size), m_counterFactory(counterFactory)
{
    OSLM_ASSERT("Buffer is null.", m_buf || size == 0);
}

SPTR(std::istream) Buffer::get()
{
    namespace io = boost::iostreams;

    typedef HoldCounterStream ArrayStreamType;
    SPTR( ArrayStreamType ) arrayInStream
        = std::make_shared< ArrayStreamType > ( static_cast<char *>(m_buf), m_size, m_counterFactory() );

    return arrayInStream;
}

} // namespace in
} // namespace stream
} // namespace fwMemory

