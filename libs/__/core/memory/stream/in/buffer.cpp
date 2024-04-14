/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/memory/stream/in/buffer.hpp"

#include <core/macros.hpp>
#include <core/spy_log.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <utility>

namespace sight::core::memory::stream::in
{

struct hold_counter_stream : boost::iostreams::stream<boost::iostreams::array_source>
{
    hold_counter_stream(char* _buf, std::size_t _size, buffer::lock_type _lock) :
        boost::iostreams::stream<boost::iostreams::array_source>(_buf, _size),
        m_counter(std::move(_lock))
    {
    }

    buffer::lock_type m_counter;
};

//------------------------------------------------------------------------------

buffer::lock_type no_factory()
{
    return {};
}

buffer::buffer(void* _buf, std::size_t _size) :
    m_buf(_buf),
    m_size(_size),
    m_counter_factory(&no_factory)
{
    SIGHT_ASSERT("Buffer is null.", m_buf || _size == 0);
}

buffer::buffer(void* _buf, std::size_t _size, counter_factory_type _counter_factory) :
    m_buf(_buf),
    m_size(_size),
    m_counter_factory(std::move(_counter_factory))
{
    SIGHT_ASSERT("Buffer is null.", m_buf || _size == 0);
}

SPTR(std::istream) buffer::get()
{
    using array_stream_type                 = hold_counter_stream;
    SPTR(array_stream_type) array_in_stream =
        std::make_shared<array_stream_type>(static_cast<char*>(m_buf), m_size, m_counter_factory());

    return array_in_stream;
}

} // namespace sight::core::memory::stream::in
