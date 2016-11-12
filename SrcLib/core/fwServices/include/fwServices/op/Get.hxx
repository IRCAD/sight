/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_OP_GET_HXX__
#define __FWSERVICES_OP_GET_HXX__

#include "fwServices/IService.hpp"
#include "fwServices/op/Add.hpp"
#include "fwServices/registry/ObjectService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) get( ::fwData::Object::sptr obj ) throw(fwTools::Failed )
{
    auto services           = ::fwServices::OSR::getServices< SERVICE >( obj );
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname();
    OSLM_ASSERT("Service "<<serviceType<<" not unique, registered "<<services.size()<<" time", services.size() == 1);
    return *services.begin();
}

//------------------------------------------------------------------------------

}

#endif /*__FWSERVICES_OP_GET_HXX__*/
