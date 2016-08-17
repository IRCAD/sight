/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
