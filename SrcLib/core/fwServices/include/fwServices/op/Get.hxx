/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwServices/IService.hpp"
#include "fwServices/op/Add.hpp"
#include "fwServices/registry/ObjectService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------
#ifndef REMOVE_DEPRECATED
template<class SERVICE>
SPTR(SERVICE) get( ::fwData::Object::sptr obj )
{
    auto services           = ::fwServices::OSR::getServices< SERVICE >( obj );
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname();
    OSLM_ASSERT("Service "<<serviceType<<" not unique, registered "<<services.size()<<" time", services.size() == 1);
    return *services.begin();
}

//------------------------------------------------------------------------------
#endif
}
