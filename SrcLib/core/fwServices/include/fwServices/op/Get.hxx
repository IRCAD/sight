/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_GET_HXX_
#define _FWSERVICES_GET_HXX_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

template<class SERVICE>
::boost::shared_ptr< SERVICE > get( fwTools::Object * obj, unsigned int _index) throw(fwTools::Failed )
{
	assert(  OSR::shared_from( obj ).get() ); // check if object registred first !!!
	return::fwServices::get< SERVICE > ( OSR::shared_from( obj ) , _index ) ;
}

template<class SERVICE>
::boost::shared_ptr< SERVICE > get( ::fwTools::Object::sptr obj, unsigned int _index ) throw(fwTools::Failed )
{
	std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
	::boost::shared_ptr< fwServices::IService > service = ::fwServices::get( obj , serviceId , _index ) ;
	assert( service );
	::boost::shared_ptr< SERVICE > castedService = boost::dynamic_pointer_cast< SERVICE >( service ) ;
	assert( castedService );
	return castedService ;
}


template<class SERVICE>
::boost::shared_ptr< SERVICE > get( ::fwTools::Object::sptr obj, std::string uid ) throw(fwTools::Failed )
{
	std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
	::boost::shared_ptr< fwServices::IService > service = ::fwServices::get( obj , serviceId , uid ) ;
	assert( service );
	::boost::shared_ptr< SERVICE > castedService = boost::dynamic_pointer_cast< SERVICE >( service ) ;
	assert( castedService );
	return castedService ;
}




template<class SERVICE>
std::vector< ::boost::shared_ptr< SERVICE > > getServices( ::fwTools::Object::sptr obj )
{
	std::string serviceId = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
	std::vector< ::boost::shared_ptr< fwServices::IService > > services = ::fwServices::getServices( obj , serviceId ) ;

	std::vector< ::boost::shared_ptr< SERVICE > > castedServices ;

	std::vector< ::boost::shared_ptr< fwServices::IService > >::iterator iter ;
	for( iter = services.begin() ; iter != services.end() ; ++iter )
	{
		::boost::shared_ptr< SERVICE > castedService = boost::dynamic_pointer_cast< SERVICE >( *iter ) ;
		assert( castedService );
		castedServices.push_back( castedService ) ;
	}

	return castedServices ;
}


}

#endif /*HELPER_HPP_*/
