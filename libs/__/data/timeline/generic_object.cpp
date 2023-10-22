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
    core::hires_clock::type _timestamp,
    buffer_data_t _buffer,
    std::size_t _size,
    deleter_t _d
) :
    timeline::buffer(_timestamp, _buffer, _size, _d),
    m_maxElementNum(_max_element_num)
{
    SIGHT_ASSERT(
        "m_maxElementNum can't be more than " << sizeof(uint64_t) * 8,
        m_maxElementNum < sizeof(uint64_t) * 8
    );
}

//-----------------------------------------------------------------------------

generic_object_base::~generic_object_base()
= default;

//-----------------------------------------------------------------------------

unsigned int generic_object_base::getPresentElementNum() const
{
    return m_numPresent;
}

//-----------------------------------------------------------------------------

bool generic_object_base::isPresent(unsigned int _index) const
{
    return (m_presenceMask & (uint64_t(1) << _index)) != 0U;
}

//-----------------------------------------------------------------------------

uint64_t generic_object_base::getMask() const
{
    return m_presenceMask;
}

//-----------------------------------------------------------------------------

unsigned int generic_object_base::getMaxElementNum() const
{
    return m_maxElementNum;
}

//-----------------------------------------------------------------------------

std::size_t generic_object_base::getElementSize() const
{
    return m_size / m_maxElementNum;
}

//-----------------------------------------------------------------------------

void generic_object_base::deep_copy(const data::timeline::object& _other)
{
    buffer::deep_copy(_other);

    const auto& other_object = static_cast<const generic_object_base&>(_other);
    m_numPresent    = other_object.m_numPresent;
    m_presenceMask  = other_object.m_presenceMask;
    m_maxElementNum = other_object.m_maxElementNum;
}

//-----------------------------------------------------------------------------

} // namespace sight::data::timeline
