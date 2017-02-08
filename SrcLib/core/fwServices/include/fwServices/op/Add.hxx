/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_OP_ADD_HXX__
#define __FWSERVICES_OP_ADD_HXX__

#include "fwServices/IService.hpp"
#include "fwServices/registry/ObjectService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) add( ::fwData::Object::csptr obj, std::string _implementationId, std::string _id)
{
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname();
    // TODO: Remove this ConstCast ?
    auto const notConstObj = ::fwData::Object::constCast(obj);
    ::fwServices::IService::sptr service = ::fwServices::add( notConstObj, serviceType, _implementationId, _id );
    OSLM_ASSERT("Failed to add "<<_implementationId, service );
    SPTR(SERVICE) castedService = std::dynamic_pointer_cast< SERVICE >( service );
    SLM_ASSERT("DynamicCast failed", castedService );
    return castedService;
}

//------------------------------------------------------------------------------

}

#endif /* __FWSERVICES_OP_ADD_HXX__ */
