/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ADD_HXX_
#define _FWSERVICES_ADD_HXX_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj )
{
	std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
	::fwServices::IService::sptr service = ::fwServices::add( obj , serviceId ) ;
	assert( service );
	SPTR(SERVICE) castedService = boost::dynamic_pointer_cast< SERVICE >( service ) ;
	assert( castedService );
	return castedService ;
}

template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj  , std::string _implementationId )
{
	std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
	::fwServices::IService::sptr service = ::fwServices::add( obj , serviceId , _implementationId ) ;
	assert( service );
	SPTR(SERVICE) castedService = boost::dynamic_pointer_cast< SERVICE >( service ) ;
	assert( castedService );
	return castedService ;
}
template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj  , std::string _implementationId , std::string _id)
{
	std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
	::fwServices::IService::sptr service = ::fwServices::add( obj , serviceId , _implementationId , _id ) ;
	assert( service );
	SPTR(SERVICE) castedService = boost::dynamic_pointer_cast< SERVICE >( service ) ;
	assert( castedService );
	return castedService ;
}


}

#endif /*HELPER_HPP_*/
