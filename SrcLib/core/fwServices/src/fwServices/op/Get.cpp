/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include "fwServices/IService.hpp"
#include "fwServices/op/Get.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceType ) throw(fwTools::Failed )
{
    ::fwServices::IService::sptr service;
    std::vector< ::fwServices::IService::sptr >  services = ::fwServices::OSR::getServices( obj , serviceType );
    OSLM_ASSERT("Service "<<serviceType<<" not unique, registered "<<services.size()<<" time", services.size() == 1);
    return services.at(0) ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( std::string uid ) throw(fwTools::Failed )
{
    OSLM_ASSERT("service not exist with uid "<<uid, ::fwTools::fwID::exist(uid )) ;
    return ::fwServices::IService::dynamicCast( ::fwTools::fwID::getObject( uid ) );
}

//------------------------------------------------------------------------------

}

