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

#include "arData/timeline/GenericObject.hpp"

namespace arData
{

namespace timeline
{

//-----------------------------------------------------------------------------

GenericObjectBase::GenericObjectBase(unsigned int maxElementNum, ::fwCore::HiResClock::HiResClockType timestamp,
                                     BufferDataType buffer, size_t size, DeleterType d) :
    Buffer(timestamp, buffer, size, d),
    m_numPresent(0),
    m_presenceMask(0),
    m_maxElementNum(maxElementNum)
{
    OSLM_ASSERT("m_maxElementNum can't be more than " << sizeof(uint64_t) * 8,
                m_maxElementNum < sizeof(uint64_t) * 8);
}

//-----------------------------------------------------------------------------

GenericObjectBase::~GenericObjectBase()
{
}

//-----------------------------------------------------------------------------

unsigned int GenericObjectBase::getPresentElementNum() const
{
    return m_numPresent;
}

//-----------------------------------------------------------------------------

bool GenericObjectBase::isPresent(unsigned int index) const
{
    return m_presenceMask & (uint64_t(1) << index);
}

//-----------------------------------------------------------------------------

uint64_t GenericObjectBase::getMask() const
{
    return m_presenceMask;
}

//-----------------------------------------------------------------------------

unsigned int GenericObjectBase::getMaxElementNum() const
{
    return m_maxElementNum;
}

//-----------------------------------------------------------------------------

size_t GenericObjectBase::getElementSize() const
{
    return m_size / m_maxElementNum;
}

//-----------------------------------------------------------------------------

void GenericObjectBase::deepCopy(const ::arData::timeline::Object& other)
{
    Buffer::deepCopy(other);

    const GenericObjectBase& otherObject = static_cast<const GenericObjectBase&>(other);
    m_numPresent    = otherObject.m_numPresent;
    m_presenceMask  = otherObject.m_presenceMask;
    m_maxElementNum = otherObject.m_maxElementNum;
}

//-----------------------------------------------------------------------------

} // namespace timeline

} // namespace arData
