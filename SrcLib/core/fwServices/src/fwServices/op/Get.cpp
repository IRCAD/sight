/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwServices/op/Get.hpp"

#include "fwServices/IService.hpp"

#include <vector>

namespace fwServices
{

//------------------------------------------------------------------------------
#ifndef REMOVE_DEPRECATED
::fwServices::IService::sptr get( ::fwData::Object::sptr obj, std::string serviceType )
{
    FW_DEPRECATED_MSG("'fwServices::get(object, srvType)' is deprecated.", "20.0");

    ::fwServices::IService::sptr service;
    auto services = ::fwServices::OSR::getServices( obj, serviceType );
    OSLM_ASSERT("Service "<<serviceType<<" is not unique, registered "<<services.size()<<" times",
                services.size() == 1);
    return *services.begin();
}
#endif
//------------------------------------------------------------------------------

::fwServices::IService::sptr get( std::string uid )
{
    OSLM_ASSERT("service does not exist with uid "<<uid, ::fwTools::fwID::exist(uid ));
    return ::fwServices::IService::dynamicCast( ::fwTools::fwID::getObject( uid ) );
}

//------------------------------------------------------------------------------

}
