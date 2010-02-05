/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <fwCore/base.hpp>
#include <fwTools/TypeInfo.hpp>

#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/helper.hpp"



namespace fwServices
{

namespace library
{


bool support( ::fwTools::Object::sptr obj , std::string serviceId ) throw()
{
        /// Direct implementation
        ::fwServices::ObjectServiceKeyType myKey(serviceId,obj->getClassname());
        OSLM_DEBUG("**support**" <<serviceId<<"**"<< obj->getClassname() << "**");
        std::list< ::fwServices::IService::sptr > serviceList = ::fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >(myKey) ;
        bool supportSpecificObject = !serviceList.empty() ;
        /// ::fwTools::Object implementation
        ::fwServices::ObjectServiceKeyType objectKey(serviceId, ::fwCore::getClassname< ::fwTools::Object >() );
        std::list< ::fwServices::IService::sptr > serviceObjectList = ::fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >(objectKey) ;
        bool supportGenericObject = !serviceObjectList.empty() ;

        //return !serviceList.empty() ;
        return supportGenericObject || supportSpecificObject ;
}


::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId )
{
        /// Return all compliant services as generic object
        ::fwServices::ObjectServiceKeyType myRootKey(serviceId ,  ::fwCore::getClassname< ::fwTools::Object >() ) ;
        std::list< ::fwServices::IService::sptr > genericImplementations = fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >( myRootKey ) ;
        for( std::list< ::fwServices::IService::sptr >::iterator iter = genericImplementations.begin() ; iter != genericImplementations.end() ; ++iter )
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
        std::list< ::fwServices::IService::sptr > implementations = fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >( myKey ) ;
        for( std::list< ::fwServices::IService::sptr >::iterator iter = implementations.begin() ; iter != implementations.end() ; ++iter )
        {
                if( (*iter)->getClassname() == _implementationId )
                {
                        OSLM_INFO( "Add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
                        ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , (*iter) );
                        return (*iter) ;
                }
        }

        OSLM_ERROR( "FAILED to add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
        return ::fwServices::IService::sptr() ;
}

}

}
