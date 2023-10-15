/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/timeline/buffer.hpp"

#include <core/memory/exception/memory.hpp>

namespace sight::data::timeline
{

//-----------------------------------------------------------------------------

buffer::buffer(
    core::hires_clock::type timestamp,
    BufferDataType buffer,
    std::size_t size,
    DeleterType d
) :
    object(timestamp),
    m_size(size),
    m_buffer(buffer),
    m_deleter(std::move(d))
{
}

//-----------------------------------------------------------------------------

buffer::~buffer()
{
    if(m_deleter)
    {
        m_deleter(m_buffer);
    }
}

//-----------------------------------------------------------------------------

void buffer::deep_copy(const data::timeline::object& other)
{
    object::deep_copy(other);

    const auto& otherObject = static_cast<const buffer&>(other);
    memcpy(m_buffer, otherObject.m_buffer, m_size);
}

//------------------------------------------------------------------------------

bool buffer::operator==(const buffer& other) const noexcept
{
    if(m_size != other.m_size)
    {
        return false;
    }

    if(m_buffer != other.m_buffer && std::memcmp(m_buffer, other.m_buffer, m_size) != 0)
    {
        return false;
    }

    // Super class last
    return object::operator==(other);
}

//------------------------------------------------------------------------------

bool buffer::operator!=(const buffer& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data::timeline
