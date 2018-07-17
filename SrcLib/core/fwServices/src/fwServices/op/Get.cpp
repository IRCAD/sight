/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/op/Get.hpp"

#include "fwServices/IService.hpp"

#include <vector>

namespace fwServices
{

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( ::fwData::Object::sptr obj, std::string serviceType )
{
    FW_DEPRECATED_MSG("'fwServices::get(object, srvType)' is deprecated.", "20.0");

    ::fwServices::IService::sptr service;
    auto services = ::fwServices::OSR::getServices( obj, serviceType );
    OSLM_ASSERT("Service "<<serviceType<<" is not unique, registered "<<services.size()<<" times",
                services.size() == 1);
    return *services.begin();
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( std::string uid )
{
    OSLM_ASSERT("service does not exist with uid "<<uid, ::fwTools::fwID::exist(uid ));
    return ::fwServices::IService::dynamicCast( ::fwTools::fwID::getObject( uid ) );
}

//------------------------------------------------------------------------------

}
