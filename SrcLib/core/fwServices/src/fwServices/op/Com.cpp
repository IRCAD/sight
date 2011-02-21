/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

::fwServices::ComChannelService::sptr getCommunicationChannel( ::fwTools::Object::sptr _src, ::fwServices::IService::sptr _dest)
{
    ::fwServices::ComChannelService::sptr comChannel;
    std::vector< ::fwServices::IEditionService::sptr > editors = ::fwServices::OSR::getServices< ::fwServices::IEditionService >( _src ) ;
    if(!editors.empty())
    {
        SLM_ASSERT("IEditionService is not unique on Object "<<_src->getID(), editors.size() == 1);
        ::fwServices::IEditionService::sptr  srcEditor = editors.at(0);
        std::vector< ::fwServices::ICommunication::sptr > comChannels = ::fwServices::OSR::getServices< ::fwServices::ICommunication >( _src ) ;
        BOOST_FOREACH(::fwServices::ICommunication::sptr comChan, comChannels)
        {
            // Check whether _service is the subject (IEditionService) or the destination service
            if( comChan->isValid() && comChan->getDest() == _dest  )
            {
                OSLM_ASSERT("ComChannel has not a correct  IEditionService [src= " << _src->getID() << " dest= "<< _dest->getID()<<"]",
                        srcEditor == comChan->getSrc());
                comChannel = ::fwServices::ComChannelService::dynamicCast(comChan);
                break;
            }
        }
    }
    return comChannel;
}

//------------------------------------------------------------------------------

::fwServices::ComChannelService::sptr registerCommunicationChannel( ::fwTools::Object::sptr _src , ::fwServices::IService::sptr _dest)
{
    ::fwServices::ComChannelService::sptr comChannel = ::fwServices::getCommunicationChannel(_src, _dest);
    OSLM_FATAL_IF("ComChannel already exist [src= " << _src->getID() << " dest= "<< _dest->getID()<<"]", comChannel);

    ::fwServices::IService::sptr srv = ::fwServices::add(_src, "::fwServices::ICommunication", "::fwServices::ComChannelService");
    comChannel = ::fwServices::ComChannelService::dynamicCast(srv);
    // Configuring communication channel
    comChannel->setDest( _dest ) ;

    return comChannel;
}

//------------------------------------------------------------------------------

void unregisterCommunicationChannel( ::fwTools::Object::sptr _src , ::fwServices::IService::sptr _dest)
{
    ::fwServices::IEditionService::sptr srcEditor = ::fwServices::get< ::fwServices::IEditionService >( _src ) ;
    std::vector< ::fwServices::ICommunication::sptr > comChannels = ::fwServices::OSR::getServices< ::fwServices::ICommunication >( _src ) ;
    bool isFound = false;
    BOOST_FOREACH(::fwServices::ICommunication::sptr comChan, comChannels)
    {
        if( comChan->isValid() )
        {
            // Check whether _service is the subject (IEditionService) or the destination service
            if( comChan->getDest() == _dest && comChan->getSrc() == srcEditor  )
            {
                comChan->stop();
                ::fwServices::OSR::unregisterService( comChan ) ;
                isFound = true;
                break;
            }
        }
    }
    OSLM_ASSERT("ComChannel [src="<< _src->getID() << " dest="<< _dest->getID()<<"] not found", isFound);
}

//------------------------------------------------------------------------------

}

