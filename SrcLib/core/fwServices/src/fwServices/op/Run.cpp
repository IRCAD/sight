/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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
#include "fwServices/op/Run.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{
void start( ::fwRuntime::ConfigurationElement::sptr _elt)
{
	for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = _elt->begin() ; iter != _elt->end() ; ++iter )
	{
		if( (*iter)->getName() == "start" )
		{
			if( (*iter)->hasAttribute("type") )
			{
				std::string serviceTypeToStart = (*iter)->getExistingAttributeValue("type") ;
				std::vector< ::fwServices::IService::sptr > servicesToStart = getServices( serviceTypeToStart ) ;
				std::vector< ::fwServices::IService::sptr >::iterator iter = servicesToStart.begin() ;
				for( ; iter != servicesToStart.end() ; ++iter )
				{
					(*iter)->start();
				}
			}
			if( (*iter)->hasAttribute("uid") )
			{
				std::string uid = (*iter)->getExistingAttributeValue("uid") ;
				if( ::fwServices::has(uid) )
				{
					::fwServices::get(uid)->start() ;
				}
				else
				{
					OSLM_WARN("Configuration : element " << uid << " not found");
				}
			}
		}
	}
}

void update(::fwRuntime::ConfigurationElement::sptr _elt)
{
	for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = _elt->begin() ; iter != _elt->end() ; ++iter )
	{
		if( (*iter)->getName() == "update" )
		{
			if( (*iter)->hasAttribute("type") )
			{
				std::string serviceTypeToUpdate = (*iter)->getExistingAttributeValue("type") ;
				std::vector< ::fwServices::IService::sptr > servicesToUpdate = getServices( serviceTypeToUpdate ) ;
				std::vector< ::fwServices::IService::sptr >::iterator iter = servicesToUpdate.begin() ;
				for( ; iter != servicesToUpdate.end() ; ++iter )
				{
					(*iter)->update();
				}
			}
			if( (*iter)->hasAttribute("uid") )
			{
				std::string uid = (*iter)->getExistingAttributeValue("uid") ;
		    	OSLM_INFO("Updating service with UUID " << uid ) ;
				if( ::fwServices::has(uid) )
				{
					::fwServices::get(uid)->update() ;
				}
				else
				{
					OSLM_WARN("Configuration : element " << uid << " not found");
				}
			}
		}
	}
}

void stop( ::fwRuntime::ConfigurationElement::sptr _elt)
{
	for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = _elt->begin() ; iter != _elt->end() ; ++iter )
	{
		if( (*iter)->getName() == "stop" )
		{
			if( (*iter)->hasAttribute("type") )
			{
				std::string serviceTypeToStop = (*iter)->getExistingAttributeValue("type") ;
				std::vector< ::fwServices::IService::sptr > servicesToStop = getServices( serviceTypeToStop ) ;
				std::vector< ::fwServices::IService::sptr >::iterator iter = servicesToStop.begin() ;
				for( ; iter != servicesToStop.end() ; ++iter )
				{
					(*iter)->stop();
				}
			}
			if( (*iter)->hasAttribute("uid") )
			{
				std::string uid = (*iter)->getExistingAttributeValue("uid") ;
				OSLM_ASSERT("Configuration : element " << uid << " not found", ::fwServices::has(uid));
		    	OSLM_INFO("Stopping service with UUID " << uid ) ;
				::fwServices::get(uid)->stop() ;
			}
		}
	}
}

void stopAndUnregister( ::fwRuntime::ConfigurationElement::sptr _elt)
{
	for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = _elt->begin() ; iter != _elt->end() ; ++iter )
	{
		if( (*iter)->getName() == "stop" )
		{
			if( (*iter)->hasAttribute("type") )
			{
				std::string serviceTypeToStop = (*iter)->getExistingAttributeValue("type") ;
				std::vector< ::fwServices::IService::sptr > servicesToStop = getServices( serviceTypeToStop ) ;
				std::vector< ::fwServices::IService::sptr >::iterator iter = servicesToStop.begin() ;
				for( ; iter != servicesToStop.end() ; ++iter )
				{
					erase(*iter);
				}
			}
			if( (*iter)->hasAttribute("uid") )
			{
				std::string uid = (*iter)->getExistingAttributeValue("uid") ;
				OSLM_ASSERT("Configuration : element " << uid << " not found", ::fwServices::has(uid));
		    	OSLM_INFO("Stopping service with UUID " << uid ) ;
		    	erase(::fwServices::get(uid)) ;
			}
		}
	}
}

void startServices( ::fwTools::Object::sptr obj )
{
	std::vector< ::fwServices::IService::sptr > vfwServices = OSR::getServices<IService>( obj ) ;
	for( std::vector< ::fwServices::IService::sptr >::iterator iter = vfwServices.begin() ; iter != vfwServices.end() ; ++iter )
	{
		(*iter)->start();
	}
}

}

