/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>

#include "fwMemory/IMemoryService.hpp"
#include "fwMemory/MemoryMonitor.hpp"

namespace fwMemory
{

::boost::uint64_t IMemoryService::localTimer = 0;

//------------------------------------------------------------------------------

IMemoryService::IMemoryService()
{}

//------------------------------------------------------------------------------

IMemoryService::~IMemoryService()
{}

//------------------------------------------------------------------------------

void IMemoryService::starting() throw( ::fwTools::Failed )
{
    /// Install observation
    ::fwData::Object::sptr obj = this->getObject< ::fwData::Object >();
    m_relatedObject = obj;

    OSLM_INFO("IMemoryService::start for " << obj->className() << " " << obj );

    m_comChannel = ::fwServices::registerCommunicationChannel(obj ,this->getSptr() );
    m_comChannel.lock()->start();

    //register object
    MemoryMonitor::getDefault()->registerObject( obj );
    // then update its memInformation
    this->updating();
}

//------------------------------------------------------------------------------

void IMemoryService::stopping() throw( ::fwTools::Failed )
{
    m_comChannel.lock()->stop();
    ::fwServices::OSR::unregisterService(m_comChannel.lock());

    // unregister in MemoryMonitor registry;
    if( !m_relatedObject.expired() )
    {
        MemoryMonitor::getDefault()->unregisterObject( m_relatedObject );
    }
    else
    {
        MemoryMonitor::getDefault()->clean();
    }
}

//------------------------------------------------------------------------------

void IMemoryService::updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->update();
}

//------------------------------------------------------------------------------

void IMemoryService::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    assert ( !m_relatedObject.expired() );
    assert ( m_relatedObject.lock() == this->getObject( )) ;

    // update its meminfo value value
    ::fwMemory::MemoryMonitor::MemoryInfo memInfo = ::fwMemory::MemoryMonitor::getDefault()->getStatus(m_relatedObject);
    memInfo.virtualSizeInBytes = this->getVirtualSize();
    memInfo.lastAccessTime = localTimer++;
    ::fwMemory::MemoryMonitor::getDefault()->statusChanged( m_relatedObject, memInfo );

    // for debug purpose
    OSLM_DEBUG( "IMemoryService::update : " << MemoryMonitor::getDefault()->log() );
}

//------------------------------------------------------------------------------

void IMemoryService::info(std::ostream &_sstream )
{
    _sstream << "Memory Service (info)";
}

//------------------------------------------------------------------------------

std::string IMemoryService::getPersistanceId()
{
    return "memory";
}

} // namespace fwMemory
