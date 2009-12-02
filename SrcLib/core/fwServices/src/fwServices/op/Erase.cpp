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
#include "fwServices/op/Erase.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

void erase( ::fwTools::Object::sptr obj , std::string serviceId , unsigned int _index )
{
	::fwServices::IService::sptr service = ::fwServices::get( obj , serviceId , _index ) ;
	OSR::unregisterService( service ) ;
}
void erase( ::fwServices::IService::sptr service )
{
	OSR::unregisterService( service ) ;
}
void eraseServices( ::fwTools::Object::sptr obj , std::string serviceId )
{
	std::vector< ::fwServices::IService::sptr > services = ::fwServices::getServices( obj , serviceId ) ;
	for( std::vector< ::fwServices::IService::sptr >::iterator iter = services.begin() ; iter != services.end() ; ++iter )
	{
		OSLM_INFO("eraseServices  : serviceId" << *iter );
		OSR::unregisterService( *iter ) ;
	}
}

void clear( ::fwTools::Object::sptr obj )
{
	OSR::unregisterServices( obj ) ;
}


}

