/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_GET_HXX_
#define _FWSERVICES_GET_HXX_

#include <fwTools/Object.hpp>

#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Add.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) get( ::fwTools::Object::sptr obj ) throw(fwTools::Failed )
{
    std::vector< typename SPTR(SERVICE) > services = ::fwServices::OSR::getServices< SERVICE >( obj );
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
    OSLM_ASSERT("Service "<<serviceType<<" not unique, registered "<<services.size()<<" time", services.size() == 1);
    return services.at(0) ;
}

//------------------------------------------------------------------------------

}

#endif /*HELPER_HPP_*/
