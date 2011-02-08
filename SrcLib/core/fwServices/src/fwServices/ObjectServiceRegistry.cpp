/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <sstream>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/fwID.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include "fwServices/Deleter.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/validation/Validator.hpp"
#include "fwServices/op/New.hpp"
#include "fwServices/op/Run.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/GlobalEventManager.hpp"

#include "fwServices/ServiceFactoryRegistry.hpp"


namespace fwServices
{

//------------------------------------------------------------------------------

ObjectServiceRegistry::sptr ObjectServiceRegistry::m_instance;

//------------------------------------------------------------------------------

const std::string ObjectServiceRegistry::CONFIG_EXTENSION_POINT = "::fwServices::ServiceObjectConfig";

//------------------------------------------------------------------------------

ObjectServiceRegistry::ObjectServiceRegistry()
{
    m_rootObjectConfigurationName.first = false ;
    m_isRootInitialized = false ;
}

//------------------------------------------------------------------------------

ObjectServiceRegistry::~ObjectServiceRegistry()
{
    m_instance.reset() ;
}

//------------------------------------------------------------------------------

//void ObjectServiceRegistry::setRootObject(::fwTools::Object::sptr obj)
//{
//    getDefault()->m_rootObject = obj;
//}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::setRootObjectConfigurationName(std::string name)
{
    SLM_ASSERT("Sorry, configuration name is an empty string.", ! name.empty() );
    getDefault()->m_rootObjectConfigurationName = std::pair< bool , std::string >( true , name ) ;
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::setRootObjectConfigurationFile(std::string _rootObjectConfigurationFile)
{
    SLM_ASSERT("Sorry, configuration file is an empty string.", ! _rootObjectConfigurationFile.empty() );
    getDefault()->m_rootObjectConfigurationFile = std::pair< bool , std::string >( true , _rootObjectConfigurationFile ) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr ObjectServiceRegistry::getRootObject()
{
    //return getDefault()->m_rootObject ;
    return getDefault()->m_ctm->getConfigRoot();
}

////------------------------------------------------------------------------------
//
//bool ObjectServiceRegistry::isRootObjectConfigurationValid()
//{
//    return ::fwServices::validation::checkObject( getDefault()->m_rootObjectConfiguration ) ;
//}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::initializeRootObject()
{

    SLM_ASSERT("Sorry, the OSR is already initialized.", ! getDefault()->m_isRootInitialized );
    SLM_ASSERT("Sorry, configuration name parameter is not initialized.", getDefault()->m_rootObjectConfigurationName.first );
    SLM_ASSERT("Sorry, configuration file parameter is not initialized.", getDefault()->m_rootObjectConfigurationFile.first );

    ::fwServices::ServiceFactoryRegistry::getDefault()->parseBundleInformation();


    // ToDo Correct this hack
    // Load another "pseudo" bundle
    ::boost::filesystem::path filePath ( getDefault()->m_rootObjectConfigurationFile.second );
    SPTR(::fwRuntime::Bundle) configBundle = ::fwRuntime::io::BundleDescriptorReader::createBundleFromXmlPlugin( filePath );
    ::fwRuntime::Runtime::getDefault()->addBundle( configBundle );
    configBundle->setEnable( true );


//    // Research the config extension
//    bool extensionIsFound = false;
//    std::vector< SPTR(::fwRuntime::Extension) > extensions = ::fwServices::bundle::findExtensionsForPoint( CONFIG_EXTENSION_POINT ) ;
//    for(    std::vector< SPTR(::fwRuntime::Extension) >::iterator iter = extensions.begin() ;
//            iter != extensions.end() ;
//            ++iter )
//    {
//        if( (*iter)->getIdentifier() == getDefault()->m_rootObjectConfigurationName.second )
//        {
//            getDefault()->m_rootObjectConfiguration = *((*iter)->begin()) ;
//            extensionIsFound = true;
//        }
//    }



    //OSLM_ASSERT("Sorry, extension "<< getDefault()->m_rootObjectConfigurationName.second <<" (that contains the configuration of the App) is not found. This extension must exist and must implements extension-point " << CONFIG_EXTENSION_POINT, extensionIsFound );

    //SLM_ASSERT("Sorry, the xml that describes extension "<< getDefault()->m_rootObjectConfigurationName.second <<" is not valid.", getDefault()->isRootObjectConfigurationValid() );

    getDefault()->m_ctm = ConfigTemplateManager::New();
    getDefault()->m_ctm->setConfig( getDefault()->m_rootObjectConfigurationName.second, CONFIG_EXTENSION_POINT );
    getDefault()->m_ctm->launch();
    getDefault()->m_isRootInitialized = true;



//    getDefault()->m_rootObject = ::fwServices::New(getDefault()->m_rootObjectConfiguration);
//    ::fwServices::start(getDefault()->m_rootObjectConfiguration) ;
//    ::fwServices::update(getDefault()->m_rootObjectConfiguration) ;
//    getDefault()->m_isRootInitialized = true ;
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::uninitializeRootObject()
{
    SLM_TRACE_FUNC();
    SLM_WARN_IF("(Hack) Sorry, the OSR must be initialized to uninitialize it. ToDo => transform in assert", ! getDefault()->m_isRootInitialized );
    if ( getDefault()->m_isRootInitialized )
    {
        // Setting initialization to false
        getDefault()->m_isRootInitialized = false ;

        assert( getDefault()->m_rootObjectConfigurationName.first ) ;
        // Stop services reported in m_rootObjectConfiguration before stopping everything

        //::fwServices::stopAndUnregister(getDefault()->m_rootObjectConfiguration) ;
        getDefault()->m_ctm->stopAndDestroy();

        OSLM_WARN_IF("Sorry, few services still exist before erasing root object ( cf debug following message )" << std::endl << ::fwServices::OSR::getRegistryInformation(),
                getDefault()->m_container.size() != 1 || getDefault()->m_container.begin()->second.size() != 0 );

        // Unregister root object services
        //::fwServices::OSR::unregisterServices( getDefault()->m_ctm->getConfigRoot() );


        ::fwServices::GlobalEventManager::getDefault()->clearMessages();

        SLM_TRACE("uninitializeRootObject : Reset the last shared_ptr on root object.");

        // Reset the root object: involve complete m_container clearing
        //getDefault()->m_rootObject.reset();
        //assert( getDefault()->m_rootObject.use_count() == 0 );

        ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
        SLM_TRACE("Stopping Profile");
        profile->stop();
        SLM_TRACE("Profile Stopped");

        // Clear all service factories
        ServiceFactoryRegistry::getDefault()->clearFactory();

        // Clear all factories before stop application.
        ::fwTools::ClassFactoryRegistry::getFactories().clear();
    }
}

//------------------------------------------------------------------------------

bool ObjectServiceRegistry::isRootObjectInitialized()
{
    return getDefault()->m_isRootInitialized ;
}

//------------------------------------------------------------------------------

void  ObjectServiceRegistry::updateObjectDeleter( ::fwTools::Object::sptr object)
{
    if ( ::fwServices::DefaultObjectDeleter::dynamicCast(object->getDeleter() ) == 0 )
    {
        object->setDeleter( ::fwServices::DefaultObjectDeleter::New() ) ;
    }
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::registerObject( ::fwTools::Object::sptr obj )
{
    bool isAlreadyRegistered = false ;
    for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
    {
        if( pos->first.lock() ==  obj )
        {
            isAlreadyRegistered = true ;
        }
    }
    if( !isAlreadyRegistered )
    {
         ::fwTools::Object::wptr newRef(obj) ;
         getDefault()->m_container.insert( std::make_pair( newRef , SContainer() ) );
         getDefault()-> updateObjectDeleter(obj);
    }
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::registerService(  fwTools::Object * obj, ::fwServices::IService::sptr service)
{
    OSR::registerService( OSR::shared_from( obj ) , service ) ;
}

//------------------------------------------------------------------------------

ObjectServiceRegistry::sptr ObjectServiceRegistry::getDefault()
{
    if ( m_instance==0 )
    {
        m_instance = ObjectServiceRegistry::sptr(new ObjectServiceRegistry() );
    }
    return m_instance;
}

//------------------------------------------------------------------------------

void  ObjectServiceRegistry::registerService( ::fwTools::Object::sptr object , ::fwServices::IService::sptr service)
{
    ::fwTools::Object::wptr refKey ;
    for ( OSContainer::iterator iter = getDefault()->m_container.begin(); iter!= getDefault()->m_container.end() ; ++iter )
    {
        ::fwTools::Object::wptr wo = iter->first ;
        assert( !wo.expired() ) ;
        if( (iter->first).lock() == object )
        {
            refKey = iter->first ;
            for ( SContainer::iterator lIter = getDefault()->m_container[refKey].begin() ; lIter!= getDefault()->m_container[refKey].end() ; ++lIter )
            {
                if( (*lIter) == service )
                {
                    SLM_WARN( "Registering the same service instance to the same object" ) ;
                    return ;
                }
            }
            getDefault()->m_container[refKey].push_back(service) ;
            service->m_associatedObject = object;
            getDefault()->updateObjectDeleter(object);
            OSLM_INFO( "Registering service " << *service ) ;
            return ;
        }
    }

    // If first registration: object is registered as a new object (new reference key) as well as the service
    refKey = object ;
    getDefault()->m_container[refKey].push_back(service) ;

    assert( service->m_associatedObject.use_count()==0 );
    service->m_associatedObject = object;
    getDefault()->updateObjectDeleter(object);
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::swapService(  ::fwTools::Object::sptr  _objSrc, ::fwTools::Object::sptr  _objDst, ::fwServices::IService::sptr _service )
{
    std::vector< ::fwTools::Object::wptr >   lobjects;
    for ( OSContainer::iterator iter = getDefault()->m_container.begin(); iter != getDefault()->m_container.end() ; ++iter )
    {
        // Usually, unregisterServices is invoked at obj destruction (from its destructor) : its weak_ptr has therefore expired
        // For this reason, for that method only, one unregister all fwServices because expired weak_ptr can (but not sure) correspond to the considered obj
        if( iter->first.expired() )
        {
            SLM_WARN( "Expired object discovered : swap on expired object !" ) ;
        }
        // Normal case : obj is not expired
        else if( iter->first.lock() == _objSrc )
        {
            SContainer::iterator lIter = iter->second.begin() ;
            bool bFind = false;
            while(  !bFind  && lIter != iter->second.end() )
            {
                if ( (*lIter) == _service )
                {
                    iter->second.erase(lIter);
                    bFind = true;
                }
                else
                {
                    ++lIter;
                }
            }
        }
    }
    getDefault()->registerService(_objDst, _service);
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::unregisterServices(  ::fwTools::Object::sptr  obj )
{
    SLM_ASSERT("Object not found in OSContainer",
            getDefault()->m_container.find(obj) != getDefault()->m_container.end());

    // Clean service vector clearing
    SContainer services = getDefault()->m_container[obj];
    BOOST_FOREACH(::fwServices::IService::sptr srv, services)
    {
        if (srv != NULL)
        {
            srv->stop();
            ::fwServices::unregisterComChannels( srv ) ;
        }
    }
    services.clear() ;

    getDefault()->m_container.erase( obj ) ;
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::cleanExpiredObject()
{
    static std::set < ::fwTools::Object::wptr > lobjects;
    static int depth = 0;

    depth++;
    BOOST_FOREACH( OSContainer::value_type objSrvMap, getDefault()->m_container)
    {
        ::fwTools::Object::wptr lobject = objSrvMap.first;

        // TODO Normally always true (shared_ptr definition)
        SLM_ASSERT("ACH : shared_ptr def ?", ! lobject.expired() || lobject.use_count() == 0  );
        SLM_ASSERT("ACH : wptr in the map not initialized ?", lobject.expired() || lobject.use_count() != 0 );

        // Usually, unregisterServices is invoked at obj destruction (from its destructor) : its weak_ptr has therefore expired
        // For this reason, for that method only, one unregister all fwServices because expired weak_ptr can (but not sure) correspond to the considered obj
        if( lobject.expired() )
        {
            SLM_WARN( "Expired object discovered : stopping and unregistering all associated fwServices" ) ;
            lobjects.insert( lobject ) ;
            // Clean service vector clearing
            SContainer services = objSrvMap.second;
            BOOST_FOREACH(::fwServices::IService::sptr srv, services)
            {
                assert( srv.use_count() != 0 );
                if (srv != NULL )
                {
                    srv->stop();
                }
            }
            services.clear() ;
        }
    }

    if(depth == 1)
    {
        // Erase all associated object with the refKey
        BOOST_FOREACH(::fwTools::Object::wptr lobject, lobjects)
        {
            if(getDefault()->m_container.find(lobject) != getDefault()->m_container.end())
            {
                getDefault()->m_container.erase( lobject ) ;
            }
        }
        lobjects.clear();
    }
    depth--;
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::unregisterService( ::fwServices::IService::sptr _service )
{
    SLM_TRACE_FUNC();

    OSLM_ASSERT( "Sorry, the service ( "<< _service->getID() <<" ) must be stop before unregister it.", _service->isStopped() );

    // TODO verify that there are no com channel on this service.

    removeFromContainer( _service );
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::removeFromContainer( ::fwServices::IService::sptr _service )
{
    SLM_TRACE_FUNC();
    for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
    {
        SContainer::iterator positionToDelete = pos->second.end() ;
        for( SContainer::iterator lIter = pos->second.begin() ; lIter != pos->second.end() ; ++lIter )
        {
            OSLM_TRACE( "OBJ=" << ( ( ! pos->first.expired() ) ? _service->getObject()->getID() : "expiredIdObject" ) <<
                        " SContainer::iterator lIter" <<  ( *lIter) ->getID()
                      );
            if( ( *lIter) == _service )
            {
                SLM_ASSERT( "service registered twice for the same object :"
                            << " OBJ=" << _service->getObject()->getID()
                            << " SRV=" <<  ( *lIter)->getID() ,
                            positionToDelete == pos->second.end()
                          );
                positionToDelete = lIter ;
            }
        }
        if ( positionToDelete != pos->second.end() )
        {
            /// Logger Information
            std::stringstream msg;
            msg << "remove from container service  : " << (*_service) << " (" << _service << ") " ;
            if( !pos->first.expired() ) // If service being unregistered is associated with an object having expired
            {
                msg << "Initially attached to object " << pos->first.lock()->className() << " (" << pos->first.lock() << ")";
            }
            SLM_INFO( msg.str() );
            pos->second.erase( positionToDelete ) ;
        }
    }
}

//------------------------------------------------------------------------------

bool ObjectServiceRegistry::hasObject(::fwServices::IService * _service)
{
    return _service->m_associatedObject.use_count();
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr ObjectServiceRegistry::shared_from( ::fwTools::Object *obj )
{
    BOOST_FOREACH( OSContainer::value_type objSrvMap, getDefault()->m_container)
    {
        ::fwTools::Object::wptr lobject = objSrvMap.first;
        assert( !lobject.expired() ) ;
        if ( (lobject.lock()).get() == obj )
        {
            return  lobject.lock() ;
        }
    }

    OSLM_WARN( "Requested ObjectServiceRegistry::shared_from( fwTools::Object *obj [("
            << obj << "]) for object not registered in  ObjectServiceRegistry" ); ;

    // avoid compilation warning
    return ::fwTools::Object::sptr();
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr ObjectServiceRegistry::shared_from( fwServices::IService *_service )
{
    BOOST_FOREACH( OSContainer::value_type objSrvMap, getDefault()->m_container)
    {
        SLM_ASSERT("Object has expired", !objSrvMap.first.expired() ) ;
        SContainer services = objSrvMap.second;
        BOOST_FOREACH(::fwServices::IService::sptr srv, services)
        {
            if (srv.get() == _service )
            {
                return srv ;
            }
        }
    }
    OSLM_WARN( "Requested ObjectServiceRegistry::shared_from( fwServices::IService *_service [("
            << _service << "]) for object not registered in  ObjectServiceRegistry" ); ;

    // avoid compilation warning
    return ::fwServices::IService::sptr();
}

//------------------------------------------------------------------------------

std::vector< ::fwTools::Object::sptr > ObjectServiceRegistry::getObjects()
{
    std::vector< ::fwTools::Object::sptr > allObjects ;
    BOOST_FOREACH( OSContainer::value_type objSrvMap, getDefault()->m_container)
    {
        ::fwTools::Object::wptr obj = objSrvMap.first;
        SLM_ASSERT("Object has expired", !obj.expired() ) ;
        if( std::find( allObjects.begin() , allObjects.end() , obj.lock() ) == allObjects.end() )
        {
            allObjects.push_back( obj.lock( ) ) ;
        }
    }
    return allObjects ;
}

//------------------------------------------------------------------------------

const ObjectServiceRegistry::OSContainer  & ObjectServiceRegistry::getOSContainer()
{
    return getDefault()->m_container;
}

//------------------------------------------------------------------------------

std::string ObjectServiceRegistry::getRegistryInformation()
{
    std::stringstream info;
    ObjectServiceRegistry::OSContainer registry = getDefault()->m_container;
    BOOST_FOREACH( OSContainer::value_type objSrvMap, getDefault()->m_container)
    {
        SLM_ASSERT("Object has expired", !objSrvMap.first.expired() ) ;

        ::fwTools::Object::sptr obj = objSrvMap.first.lock();
        SContainer services = objSrvMap.second;

        info << "New object found in OSR" << std::endl;
        info << "Object ( uid = "<< obj->getID() <<" , classname = "<< obj->getClassname() <<" ) has "<< services.size() <<" services." << std::endl;

        BOOST_FOREACH( IService::sptr service, services )
        {
            info << "    srv : uid = "<< service->getID() <<" , classname = "<< service->getClassname() <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
        }
    }

    return info.str();

}

//------------------------------------------------------------------------------

}
