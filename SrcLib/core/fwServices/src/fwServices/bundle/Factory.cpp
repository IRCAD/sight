/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"

#include "fwServices/registry/ServiceFactory.hpp"


namespace fwServices
{

namespace bundle
{

bool support( ::fwTools::Object::sptr obj , std::string serviceId ) throw()
{
    // Looking for services provided by components (n implementations)
    //std::vector< std::string > extensionIds = ::fwServices::bundle::getValidExtensionIdsForObjectAndService( obj , serviceId ) ;
    std::vector< std::string > extensionIds = ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType( obj->getClassname(), serviceId );
    return !extensionIds.empty() ;
}

//::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId )
//{
//    OSLM_TRACE( "Add New Service : " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
//
//    // Looking for services provided by components (n implementations)
//    //std::vector< std::string > extensionIds = ::fwServices::bundle::getValidExtensionIdsForObjectAndService( obj , serviceId ) ;
//    std::vector< std::string > extensionIds = ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromTypeAndObject( serviceId, obj->getClassname() );
//    OSLM_ASSERT( "ImplementionId not found: " << _implementationId << " for the object type " << obj->className() , std::find(extensionIds.begin() , extensionIds.end() , _implementationId ) != extensionIds.end() );
//    if( std::find(extensionIds.begin() , extensionIds.end() , _implementationId ) == extensionIds.end() )
//    {
//        OSLM_ERROR( "FAILED to add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
//        return ::fwServices::IService::sptr() ;
//    }
//
//    // serviceId should be an extension to ''service''
//    ::boost::shared_ptr< ::fwRuntime::Extension > ext = ::fwRuntime::findExtension(serviceId) ;
//    OSLM_ASSERT("Extension for "<<serviceId<<" not found", ext );
//    OSLM_ASSERT("Wrong Classname for "<<ext->getPoint() << " ("<<::fwCore::getClassname< ::fwServices::IService >() <<" required)",
//            ext->getPoint() == ::fwCore::getClassname< ::fwServices::IService >())
//    ext->getBundle()->start(); // start dll providing class instances
//
//    ::fwServices::IService::sptr implementation = ::fwTools::ClassFactoryRegistry::create< ::fwServices::IService , std::string >( _implementationId ) ;
//    // If null : Try to start the associated bundle dynamic library so that registration operates
//    if( !implementation )
//    {
//        ::boost::shared_ptr< ::fwRuntime::Extension > extImpl = ::fwServices::bundle::getRuntimeExtension( _implementationId , serviceId ) ;
//        OSLM_ASSERT("Extension for implementation "<< _implementationId << " and ServiceId " << serviceId << " not found", extImpl );
//        OSLM_INFO( "Starting bundle " << extImpl->getBundle()->getIdentifier() << " to retrieve implementation service " << _implementationId ) ;
//        extImpl->getBundle()->start();
//        implementation = fwTools::ClassFactoryRegistry::create< ::fwServices::IService , std::string >( _implementationId ) ;
//    }
//    OSLM_ASSERT("Implementation "<< _implementationId << " not found for ServiceId " << serviceId, implementation );
//    OSLM_DEBUG( "Add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
//    ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , implementation );
//    return implementation ;
//}

}

}
