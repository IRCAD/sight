/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_INFO_HXX_
#define _FWSERVICES_INFO_HXX_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

template<class SERVICE>
bool has( ::fwTools::Object::sptr obj) throw()
{
    std::vector< typename SPTR(SERVICE) > services = ::fwServices::OSR::getServices< SERVICE >( obj );
    return services.size() > 0;
}

template<class SERVICE>
bool support( ::fwTools::Object::sptr object) throw()
{
    std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
    return ::fwServices::support( object , serviceId ) ;
}

template<class SERVICE>
std::vector< std::string > getImplementationIds( ::fwTools::Object::sptr obj )
{
    std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
    return ::fwServices::getImplementationIds( obj , serviceId ) ;
}


}

#endif /*_FWSERVICES_INFO_HXX_*/
