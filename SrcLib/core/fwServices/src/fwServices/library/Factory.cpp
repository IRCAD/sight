/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/helper.hpp"
#include <fwTools/TypeInfo.hpp>
#include <vector>

#include <fwCore/base.hpp>


namespace fwServices
{

namespace library
{

bool support( ::boost::shared_ptr< fwTools::Object > obj , std::string serviceId ) throw()
{
	/// Direct implementation
	::fwServices::ObjectServiceKeyType myKey(serviceId,obj->getClassname());
	OSLM_DEBUG("**support**" <<serviceId<<"**"<< obj->getClassname() << "**");
	std::list< ::boost::shared_ptr< fwServices::IService > > serviceList = ::fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >(myKey) ;
	bool supportSpecificObject = !serviceList.empty() ;
	/// ::fwTools::Object implementation
	::fwServices::ObjectServiceKeyType objectKey(serviceId, ::fwCore::getClassname< ::fwTools::Object >() );
	std::list< ::boost::shared_ptr< fwServices::IService > > serviceObjectList = ::fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >(objectKey) ;
	bool supportGenericObject = !serviceObjectList.empty() ;

	//return !serviceList.empty() ;
	return supportGenericObject || supportSpecificObject ;
}

::boost::shared_ptr< fwServices::IService > add( ::boost::shared_ptr< fwTools::Object > obj , std::string serviceId , std::string _implementationId )
{
	/// Return all compliant services as generic object
	::fwServices::ObjectServiceKeyType myRootKey(serviceId ,  ::fwCore::getClassname< ::fwTools::Object >() ) ;
	std::list< ::boost::shared_ptr< fwServices::IService > > genericImplementations = fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >( myRootKey ) ;
	for( std::list< ::boost::shared_ptr< fwServices::IService > >::iterator iter = genericImplementations.begin() ; iter != genericImplementations.end() ; ++iter )
	{
		if( (*iter)->getClassname() == _implementationId )
		{
			OSLM_INFO( "Add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
			::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , (*iter) );
			return (*iter) ;
		}
	}

	/// Return all compliant services for specific obj
	::fwServices::ObjectServiceKeyType myKey(serviceId , obj->getClassname() ) ;
	std::list< ::boost::shared_ptr< fwServices::IService > > implementations = fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >( myKey ) ;
	for( std::list< ::boost::shared_ptr< fwServices::IService > >::iterator iter = implementations.begin() ; iter != implementations.end() ; ++iter )
	{
		if( (*iter)->getClassname() == _implementationId )
		{
			OSLM_INFO( "Add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
			::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , (*iter) );
			return (*iter) ;
		}
	}

	OSLM_ERROR( "FAILED to add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
	return ::boost::shared_ptr< fwServices::IService >() ;
}

}

}
