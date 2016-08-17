/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
