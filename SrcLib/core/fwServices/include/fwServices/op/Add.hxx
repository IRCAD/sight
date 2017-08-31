/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_OP_ADD_HXX__
#define __FWSERVICES_OP_ADD_HXX__

#include "fwServices/IService.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) add( ::fwData::Object::csptr _obj, const std::string& _implType, const std::string& _id)
{
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname();
    // TODO: Remove this ConstCast ?
    auto const notConstObj = ::fwData::Object::constCast(_obj);
    ::fwServices::IService::sptr service = ::fwServices::add( notConstObj, serviceType, _implType, _id );
    SLM_ASSERT("Failed to add " + _implType, service );
    SPTR(SERVICE) castedService = std::dynamic_pointer_cast< SERVICE >( service );
    SLM_ASSERT("DynamicCast failed", castedService );
    return castedService;
}

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) add(const std::string& _implType, const std::string& _id)
{
    ::fwServices::IService::sptr genericSrv = ::fwServices::add( _implType, _id );
    auto srv = std::dynamic_pointer_cast< SERVICE >(genericSrv);
    FW_RAISE_IF("Failed to cast service from factory type '" + _implType + "' into '" +
                ::fwCore::TypeDemangler<SERVICE>().getFullClassname() + "'", !srv );

    return srv;
}

//------------------------------------------------------------------------------

}

#endif /* __FWSERVICES_OP_ADD_HXX__ */
