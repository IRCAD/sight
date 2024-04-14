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

#include "data/timeline/generic_object.hpp"

namespace sight::data::timeline
{

//-----------------------------------------------------------------------------

generic_object_base::generic_object_base(
    unsigned int _max_element_num,
    core::clock::type _timestamp,
    buffer_data_t _buffer,
    std::size_t _size,
    deleter_t _d
) :
    timeline::buffer(_timestamp, _buffer, _size, _d),
    m_max_element_num(_max_element_num)
{
    SIGHT_ASSERT(
        "m_maxElementNum can't be more than " << sizeof(uint64_t) * 8,
        m_max_element_num < sizeof(uint64_t) * 8
    );
}

//-----------------------------------------------------------------------------

generic_object_base::~generic_object_base()
= default;

//-----------------------------------------------------------------------------

unsigned int generic_object_base::get_present_element_num() const
{
    return m_num_present;
}

//-----------------------------------------------------------------------------

bool generic_object_base::is_present(unsigned int _index) const
{
    return (m_presence_mask & (uint64_t(1) << _index)) != 0U;
}

//-----------------------------------------------------------------------------

uint64_t generic_object_base::get_mask() const
{
    return m_presence_mask;
}

//-----------------------------------------------------------------------------

unsigned int generic_object_base::get_max_element_num() const
{
    return m_max_element_num;
}

//-----------------------------------------------------------------------------

std::size_t generic_object_base::get_element_size() const
{
    return m_size / m_max_element_num;
}

//-----------------------------------------------------------------------------

void generic_object_base::deep_copy(const data::timeline::object& _other)
{
    buffer::deep_copy(_other);

    const auto& other_object = static_cast<const generic_object_base&>(_other);
    m_num_present     = other_object.m_num_present;
    m_presence_mask   = other_object.m_presence_mask;
    m_max_element_num = other_object.m_max_element_num;
}

//-----------------------------------------------------------------------------

} // namespace sight::data::timeline
