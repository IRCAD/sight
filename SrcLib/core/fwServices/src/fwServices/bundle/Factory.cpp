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


namespace fwServices
{

namespace bundle
{

bool support( ::fwTools::Object::sptr obj , std::string serviceId ) throw()
{
        // Looking for services provided by components (n implementations)
        std::vector< std::string > extensionIds = ::fwServices::bundle::getValidExtensionIdsForObjectAndService( obj , serviceId ) ;
        return !extensionIds.empty() ;
}

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId )
{
        OSLM_TRACE( "Add New Service : " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );

        // Looking for services provided by components (n implementations)
        std::vector< std::string > extensionIds = ::fwServices::bundle::getValidExtensionIdsForObjectAndService( obj , serviceId ) ;
    OSLM_ASSERT( "ImplementionId not found: " << _implementationId  , std::find(extensionIds.begin() , extensionIds.end() , _implementationId ) != extensionIds.end() );
        if( std::find(extensionIds.begin() , extensionIds.end() , _implementationId ) == extensionIds.end() )
        {
                OSLM_ERROR( "FAILED to add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
                return ::fwServices::IService::sptr() ;
        }

        // serviceId should be an extension to ''service''
        ::boost::shared_ptr< ::fwRuntime::Extension > ext = ::fwRuntime::findExtension(serviceId) ;
        assert( ext );
        assert( ext->getPoint() == ::fwCore::getClassname< ::fwServices::IService >());
        ext->getBundle()->start(); // start dll providing class instances

        ::fwServices::IService::sptr implementation = fwTools::ClassFactoryRegistry::create< fwServices::IService , std::string >( _implementationId ) ;
        // If null : Try to start the associated bundle dynamic library so that registration operates
        if( !implementation )
        {
                ::boost::shared_ptr< ::fwRuntime::Extension > extImpl = fwServices::bundle::getRuntimeExtension( _implementationId , serviceId ) ;
                assert( extImpl ) ;
                OSLM_INFO( "Starting bundle " << extImpl->getBundle()->getIdentifier() << " to retrieve implementation service " << _implementationId ) ;
                extImpl->getBundle()->start();
                implementation = fwTools::ClassFactoryRegistry::create< fwServices::IService , std::string >( _implementationId ) ;
        }
        assert( implementation ) ;
        OSLM_DEBUG( "Add implementation " << _implementationId << " (service type = " << serviceId << " )" << " to objet type " << obj->className() );
        ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , implementation );
        return implementation ;
}

}

}
