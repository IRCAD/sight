/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/tr1/unordered_map.hpp>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/helper.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Info.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/registry/ServiceFactory.hpp"


namespace fwServices
{

bool has( ::fwTools::Object::sptr obj , std::string serviceId) throw()
{
    //std::vector< std::string > availableImplementations = ::fwServices::getImplementationIds( obj , serviceId ) ;
    std::vector< std::string > availableImplementations =
            ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType( obj->getClassname(), serviceId );
    std::vector< ::fwServices::IService::sptr >  allServices = ::fwServices::getRegisteredServices(obj);
    for(std::vector< ::fwServices::IService::sptr >::iterator iter = allServices.begin() ; iter != allServices.end() ; ++iter )
    {
        std::string className = (*iter)->getClassname() ;
        if( std::find( availableImplementations.begin() , availableImplementations.end() , className ) != availableImplementations.end()  )
        {
            return true ;
        }
    }
    return false ;
}

bool has( ::std::string uuid ) throw()
{
    return ::fwTools::fwID::exist(uuid ) ;
}

}

