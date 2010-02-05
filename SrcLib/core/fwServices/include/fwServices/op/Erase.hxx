/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ERASE_HXX_
#define _FWSERVICES_ERASE_HXX_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

template<class SERVICE>
void erase( ::fwTools::Object::sptr obj , unsigned int _index )
{
        std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
        ::fwServices::erase( obj , serviceId , _index ) ;
}

template<class SERVICE>
void eraseServices( ::fwTools::Object::sptr obj )
{
        std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
        ::fwServices::eraseServices( obj , serviceId ) ;
}


}

#endif /*_FWSERVICES_ERASE_HXX_*/
