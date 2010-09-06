/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/UUID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

::fwServices::ComChannelService::sptr getCommunicationChannel( ::fwTools::Object::sptr _src, ::fwServices::IService::sptr _dest)
{
    typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
    OContainerType obs = ::fwServices::OSR::getServices< ::fwServices::ComChannelService >(_src) ;
    for( OContainerType::iterator iter = obs.begin() ; iter != obs.end() ; ++iter )
    {
        if( (*iter)->isValid() && (*iter)->getDest() == _dest )
        {
            assert( ::fwServices::get< ::fwServices::IEditionService >(_src) == (*iter)->getSrc());
            return *iter;
        }
    }
    return ::fwServices::ComChannelService::sptr();
}

//------------------------------------------------------------------------------

::fwServices::ComChannelService::sptr registerCommunicationChannel( ::fwTools::Object::sptr _src , ::fwServices::IService::sptr _dest)
{
    ::fwServices::ComChannelService::sptr comChannel = getCommunicationChannel(_src, _dest);
    if(comChannel)
    {
        OSLM_FATAL("comChannel already exist src= " << _src->getUUID() << " dest= "<< _dest->getUUID());
        return comChannel;
    }

    comChannel = ::fwServices::ComChannelService::sptr ( new ::fwServices::ComChannelService );
    ::fwServices::IEditionService::sptr srcEditor = ::fwServices::get< ::fwServices::IEditionService >( _src ) ;
    assert( srcEditor ) ;
    // Configuring communication channel
    comChannel->setSrc( srcEditor ) ;
    comChannel->setDest( _dest ) ;
    // Registering it as a _src service
    ::fwServices::OSR::registerService( _src , comChannel );
    return comChannel;
}

//------------------------------------------------------------------------------

void unregisterCommunicationChannel( ::fwTools::Object::sptr _src , ::fwServices::IService::sptr _dest)
{
    ::fwServices::IEditionService::sptr srcEditor = ::fwServices::get< ::fwServices::IEditionService >( _src ) ;
    typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
    OContainerType obs = OSR::getServices<fwServices::ComChannelService>() ;
    for( OContainerType::iterator iter = obs.begin() ; iter != obs.end() ; ++iter )
    {
        if( (*iter)->isValid() )
        {
            // Check whether _service is the subject (IEditionService) or the destination service
            if( (*iter)->getDest() == _dest && (*iter)->getSrc() == srcEditor  )
            {
                (*iter)->stop();
                ::fwServices::OSR::removeFromContainer( *iter ) ;
            }
        }
    }
}

//------------------------------------------------------------------------------

void stopComChannels( ::fwServices::IService::sptr _service)
{
    typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
    OContainerType obs = ::fwServices::OSR::getServices< ::fwServices::ComChannelService >() ;
    for( OContainerType::iterator iter = obs.begin() ; iter != obs.end() ; ++iter )
    {
        if(  (*iter)->isStarted() )
        {
            // Check whether _service is the subject (IEditionService) or the destination service
            if( (*iter)->getDest() == _service || (*iter)->getSrc() == _service )
            {
                (*iter)->stop() ;
            }
        }
    }
}

//------------------------------------------------------------------------------

void startComChannels( ::fwServices::IService::sptr _service)
{
    typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
    OContainerType obs = ::fwServices::OSR::getServices< ::fwServices::ComChannelService >() ;
    for( OContainerType::iterator iter = obs.begin() ; iter != obs.end() ; ++iter )
    {
        if(  (*iter)->isStopped() )
        {
            // Check whether _service is the subject (IEditionService) or the destination service
            if( (*iter)->getDest() == _service || (*iter)->getSrc() == _service )
            {
                (*iter)->start() ;
            }
        }
    }
}

//------------------------------------------------------------------------------

void unregisterComChannels( ::fwServices::IService::sptr _service)
{
    typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
    OContainerType obs = ::fwServices::OSR::getServices< ::fwServices::ComChannelService >() ;
    for( OContainerType::iterator iter = obs.begin() ; iter != obs.end() ; ++iter )
    {
        if( (*iter)->isValid() )
        {
            // Check whether _service is the subject (IEditionService) or the destination service
            if( (*iter)->getDest() == _service || (*iter)->getSrc() == _service )
            {
                (*iter)->stop();
                ::fwServices::OSR::removeFromContainer( *iter ) ;
            }
        }
    }
}


}

