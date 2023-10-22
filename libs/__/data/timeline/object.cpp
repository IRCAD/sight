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

#include "data/timeline/object.hpp"

namespace sight::data::timeline
{

//-----------------------------------------------------------------------------

object::object(core::hires_clock::type _timestamp) :
    m_timestamp(_timestamp)
{
}

//-----------------------------------------------------------------------------

object::~object()
= default;

//-----------------------------------------------------------------------------

void object::deep_copy(const object& _source)
{
    m_timestamp = _source.m_timestamp;
}

//-----------------------------------------------------------------------------

bool object::operator==(const object& _other) const noexcept
{
    return m_timestamp == _other.m_timestamp;
}

//------------------------------------------------------------------------------

bool object::operator!=(const object& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data::timeline
