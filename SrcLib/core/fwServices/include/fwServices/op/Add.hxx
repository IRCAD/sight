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

#pragma once

#include "fwServices/IService.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------
#ifndef REMOVE_DEPRECATED
template<class SERVICE>
SPTR(SERVICE) add( ::fwData::Object::csptr _obj, const std::string& _implType, const std::string& _id)
{
    FW_DEPRECATED_MSG("'fwServices::add(object, srvImpl[,uid])' is deprecated.", "20.0");

    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname();
    // TODO: Remove this ConstCast ?
    auto const notConstObj = ::fwData::Object::constCast(_obj);
    ::fwServices::IService::sptr service = ::fwServices::add( notConstObj, serviceType, _implType, _id );
    SLM_ASSERT("Failed to add " + _implType, service );
    SPTR(SERVICE) castedService = std::dynamic_pointer_cast< SERVICE >( service );
    SLM_ASSERT("DynamicCast failed", castedService );
    return castedService;
}
#endif
//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) add(const std::string& _implType, const std::string& _id)
{
    ::fwServices::IService::sptr genericSrv = ::fwServices::add( _implType, _id );
    auto srv = std::dynamic_pointer_cast< SERVICE >(genericSrv);
    FW_RAISE_IF("Failed to cast service from factory type '" + _implType + "' into '" +
                ::fwCore::TypeDemangler<SERVICE>().getClassname() + "'", !srv );

    return srv;
}

//------------------------------------------------------------------------------

}
