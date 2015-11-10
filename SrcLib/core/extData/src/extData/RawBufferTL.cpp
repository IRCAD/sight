/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "extData/RawBufferTL.hpp"
#include "extData/timeline/RawBuffer.hpp"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/pool/pool.hpp>

#include <fwCore/base.hpp>

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::extData::RawBufferTL );

namespace extData
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

void RawBufferTL::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache)
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

    BOOST_FOREACH(TimelineType::value_type elt, other->m_timeline)
    {
        SPTR(::extData::timeline::RawBuffer) tlObj = this->createBuffer(elt.first);
        tlObj->deepCopy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }
}

//------------------------------------------------------------------------------

CSPTR(::extData::timeline::RawBuffer) RawBufferTL::getClosestBuffer( ::fwCore::HiResClock::HiResClockType timestamp,
                                                                     DirectionType direction) const
{
    CSPTR(::extData::timeline::Object) buffer = this->getClosestObject(timestamp, direction);
    return ::boost::dynamic_pointer_cast< const ::extData::timeline::RawBuffer >(buffer);
}

//------------------------------------------------------------------------------

CSPTR(::extData::timeline::RawBuffer) RawBufferTL::getBuffer(::fwCore::HiResClock::HiResClockType timestamp) const
{
    CSPTR(::extData::timeline::Object) buffer = this->getObject(timestamp);
    return ::boost::dynamic_pointer_cast< const ::extData::timeline::RawBuffer >(buffer);
}

//------------------------------------------------------------------------------

void RawBufferTL::initPoolSize(std::size_t size)
{
    this->allocPoolSize(size);
}

//------------------------------------------------------------------------------

SPTR(::extData::timeline::Object) RawBufferTL::createObject(::fwCore::HiResClock::HiResClockType timestamp)
{
    return this->createBuffer(timestamp);
}

//------------------------------------------------------------------------------

SPTR(::extData::timeline::RawBuffer) RawBufferTL::createBuffer(::fwCore::HiResClock::HiResClockType timestamp)
{
    return ::boost::make_shared< ::extData::timeline::RawBuffer >(
        timestamp,
        (::extData::timeline::Buffer::BufferDataType) m_pool->malloc(),
        m_pool->get_requested_size(),
        ::boost::bind( &::boost::pool<>::free, m_pool, _1)
        );
}

//------------------------------------------------------------------------------

bool RawBufferTL::isObjectValid(const CSPTR(::extData::timeline::Object) &obj) const
{
    CSPTR(::extData::timeline::RawBuffer) srcObj =
        ::boost::dynamic_pointer_cast< const ::extData::timeline::RawBuffer >(obj);
    return srcObj != NULL;
}

//------------------------------------------------------------------------------

} // namespace extData


