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

#include "data/timeline/raw_buffer.hpp"

namespace sight::data::timeline
{

//-----------------------------------------------------------------------------

raw_buffer::raw_buffer(
    core::clock::type _timestamp,
    buffer_data_t _buffer,
    std::size_t _size,
    deleter_t _d
) :
    timeline::buffer(_timestamp, _buffer, _size, _d)
{
}

//-----------------------------------------------------------------------------

raw_buffer::~raw_buffer()
= default;

//-----------------------------------------------------------------------------

void raw_buffer::set_buffer_values(buffer_data_t _values)
{
    memcpy(m_buffer, _values, m_size);
}

} // namespace sight::data::timeline
