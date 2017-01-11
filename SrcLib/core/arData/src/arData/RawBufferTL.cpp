/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/RawBufferTL.hpp"
#include "arData/timeline/RawBuffer.hpp"

#include <fwCore/base.hpp>

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

#include <boost/pool/pool.hpp>
#include <functional>

fwDataRegisterMacro( ::arData::RawBufferTL );

namespace arData
{

//------------------------------------------------------------------------------

RawBufferTL::RawBufferTL ( ::fwData::Object::Key key ) :
    BufferTL(key)
{
}

//------------------------------------------------------------------------------

RawBufferTL::~RawBufferTL ()
{
}

//------------------------------------------------------------------------------

void RawBufferTL::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    RawBufferTL::csptr other = RawBufferTL::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );

    this->clearTimeline();
    this->allocPoolSize(other->m_pool->get_requested_size());
    ::fwCore::mt::WriteLock writeLock(m_tlMutex);
    ::fwCore::mt::WriteLock readLock(other->m_tlMutex);

    for(TimelineType::value_type elt :  other->m_timeline)
    {
        SPTR(::arData::timeline::RawBuffer) tlObj = this->createBuffer(elt.first);
        tlObj->deepCopy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }
}

//------------------------------------------------------------------------------

CSPTR(::arData::timeline::RawBuffer) RawBufferTL::getClosestBuffer( ::fwCore::HiResClock::HiResClockType timestamp,
                                                                    DirectionType direction) const
{
    CSPTR(::arData::timeline::Object) buffer = this->getClosestObject(timestamp, direction);
    return std::dynamic_pointer_cast< const ::arData::timeline::RawBuffer >(buffer);
}

//------------------------------------------------------------------------------

CSPTR(::arData::timeline::RawBuffer) RawBufferTL::getBuffer(::fwCore::HiResClock::HiResClockType timestamp) const
{
    CSPTR(::arData::timeline::Object) buffer = this->getObject(timestamp);
    return std::dynamic_pointer_cast< const ::arData::timeline::RawBuffer >(buffer);
}

//------------------------------------------------------------------------------

void RawBufferTL::initPoolSize(std::size_t size)
{
    this->allocPoolSize(size);
}

//------------------------------------------------------------------------------

SPTR(::arData::timeline::Object) RawBufferTL::createObject(::fwCore::HiResClock::HiResClockType timestamp)
{
    return this->createBuffer(timestamp);
}

//------------------------------------------------------------------------------

SPTR(::arData::timeline::RawBuffer) RawBufferTL::createBuffer(::fwCore::HiResClock::HiResClockType timestamp)
{
    return std::make_shared< ::arData::timeline::RawBuffer >(
        timestamp,
        (::arData::timeline::Buffer::BufferDataType) m_pool->malloc(),
        m_pool->get_requested_size(),
        ::boost::bind( &::boost::pool<>::free, m_pool, _1)
        );
}

//------------------------------------------------------------------------------

bool RawBufferTL::isObjectValid(const CSPTR(::arData::timeline::Object)& obj) const
{
    CSPTR(::arData::timeline::RawBuffer) srcObj =
        std::dynamic_pointer_cast< const ::arData::timeline::RawBuffer >(obj);
    return srcObj != NULL;
}

//------------------------------------------------------------------------------

} // namespace arData


