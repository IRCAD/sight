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

#include "arData/timeline/Object.hpp"

namespace arData
{

namespace timeline
{

//-----------------------------------------------------------------------------

Object::Object(::fwCore::HiResClock::HiResClockType timestamp) : m_timestamp(timestamp)
{
}

//-----------------------------------------------------------------------------

Object::~Object()
{
}

//-----------------------------------------------------------------------------

void Object::deepCopy( const Object& _source )
{
    m_timestamp = _source.m_timestamp;
}

//-----------------------------------------------------------------------------

} // namespace timeline

} // namespace arData
