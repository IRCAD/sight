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
    unsigned int maxElementNum,
    core::hires_clock::type timestamp,
    BufferDataType buffer,
    std::size_t size,
    DeleterType d
) :
    timeline::buffer(timestamp, buffer, size, d),
    m_maxElementNum(maxElementNum)
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

bool generic_object_base::isPresent(unsigned int index) const
{
    return (m_presenceMask & (uint64_t(1) << index)) != 0U;
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

void generic_object_base::deep_copy(const data::timeline::object& other)
{
    buffer::deep_copy(other);

    const auto& otherObject = static_cast<const generic_object_base&>(other);
    m_numPresent    = otherObject.m_numPresent;
    m_presenceMask  = otherObject.m_presenceMask;
    m_maxElementNum = otherObject.m_maxElementNum;
}

//-----------------------------------------------------------------------------

} // namespace sight::data::timeline
