/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwCore/base.hpp>
#include <fwData/Object.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include "fwMemory/Initializer.hpp"
#include "fwMemory/IMemoryService.hpp"
#include "fwMemory/IDumpService.hpp"

namespace fwMemory
{

//------------------------------------------------------------------------------

void Initializer::init( ::fwTools::Object::wptr _obj)
{
    OSLM_TRACE("fwMemory::Initializer install and start MEMORY service for " + _obj.lock()->className() ) ;
    if( ::fwServices::registry::ServiceFactory::getDefault()->support(_obj.lock()->getClassname(), "::fwMemory::IMemoryService") )
    {
        // To avoid invoking the IMemoryService::start() several times
        if( ! ::fwServices::OSR::has(_obj.lock(), "::fwMemory::IMemoryService") )
        {
            std::string defaultImpl = ::fwServices::registry::ServiceFactory::getDefault()->getDefaultImplementationIdFromObjectAndType(_obj.lock()->getClassname(), "::fwMemory::IMemoryService");
            ::fwServices::IService::sptr srv = ::fwServices::add( _obj.lock(), "::fwMemory::IMemoryService", defaultImpl) ;
            srv->start();
            m_container[_obj.lock()->getOSRKey()].push_back(srv);
        }
    }

    OSLM_TRACE("fwMemory::Initializer install and start DUMP service if possible for " + _obj.lock()->className() ) ;
    if( ::fwServices::registry::ServiceFactory::getDefault()->support(_obj.lock()->getClassname(), "::fwMemory::IDumpService") )
    {
        OSLM_TRACE("Support DUMP service for " + _obj.lock()->className() ) ;
        // To avoid invoking the IDumpService::start() several times
        if( ! ::fwServices::OSR::has(_obj.lock(), "::fwMemory::IDumpService") )
        {
            OSLM_TRACE("Start DUMP service for " + _obj.lock()->className() ) ;
            std::string defaultImpl = ::fwServices::registry::ServiceFactory::getDefault()->getDefaultImplementationIdFromObjectAndType(_obj.lock()->getClassname(), "::fwMemory::IDumpService");
            ::fwServices::IService::sptr srv = ::fwServices::add( _obj.lock(), "::fwMemory::IDumpService", defaultImpl) ;
            srv->start();
            m_container[_obj.lock()->getOSRKey()].push_back(srv);
        }
    }
}

//------------------------------------------------------------------------------

void Initializer::uninit( ::fwCore::LogicStamp::csptr key )
{
    SContainer services = m_container[key];
    BOOST_FOREACH(::fwServices::IService::wptr srv, services)
    {
        srv.lock()->stop();
        ::fwServices::OSR::unregisterService(srv.lock());
    }
    m_container.erase(key);
    OSLM_ASSERT( "Expired key "<<key->getLogicStamp()<<" found in OSR\n"
            <<::fwServices::OSR::getRegistryInformation(), ! ::fwServices::OSR::hasKey(key));
}

//------------------------------------------------------------------------------

} //end namespace
