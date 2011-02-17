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

#include "fwServices/Deleter.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/GlobalEventManager.hpp"

#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/ServiceFactory.hpp"
#include "fwServices/registry/AppConfig.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

ObjectServiceRegistry::sptr ObjectServiceRegistry::m_instance;

//------------------------------------------------------------------------------

const std::string ObjectServiceRegistry::CONFIG_EXTENSION_POINT = "::fwServices::registry::AppConfig";

//------------------------------------------------------------------------------

ObjectServiceRegistry::ObjectServiceRegistry()
{}

//------------------------------------------------------------------------------

ObjectServiceRegistry::~ObjectServiceRegistry()
{
    m_instance.reset() ;
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
    OSLM_ASSERT("Sorry, this service "<< service->getClassname() << " is not valid for object " << object->getClassname(),
                ::fwServices::registry::ServiceFactory::getDefault()->checkServiceValidity(object->getClassname(), service->getClassname()));

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
                if (srv != NULL && srv->isStarted() )
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
    ::fwTools::Object::sptr obj = _service->getObject();
    OSLM_ASSERT("Unknown Object "<<obj->getID()<<" in OSR", getDefault()->m_container.find(obj) != getDefault()->m_container.end());
    SContainer& services =  getDefault()->m_container[obj];
    SContainer::iterator iter = std::find( services.begin() , services.end() , _service );
    OSLM_ASSERT("Unknown Service "<<_service->getID()<<" in OSR", iter != services.end());
    services.erase(iter);
}

//------------------------------------------------------------------------------

std::vector< ::fwTools::Object::sptr > ObjectServiceRegistry::getObjects()
{
    std::vector< ::fwTools::Object::sptr > allObjects ;
    BOOST_FOREACH( OSContainer::value_type objSrvMap, getDefault()->m_container)
    {
        ::fwTools::Object::wptr obj = objSrvMap.first;
        SLM_ASSERT("Object has expired", !obj.expired() ) ;
        allObjects.push_back( obj.lock( ) ) ;
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

bool ObjectServiceRegistry::has( ::fwTools::Object::sptr obj , const std::string & srvType)
{
    bool hasServices = false;
    if( ObjectServiceRegistry::getDefault()->m_container.find(obj) != ObjectServiceRegistry::getDefault()->m_container.end())
    {
        SContainer services = ObjectServiceRegistry::getDefault()->m_container[obj];
        BOOST_FOREACH( ::fwServices::IService::sptr srv, services)
        {
            if(srv->isA(srvType))
            {
                hasServices = true;
                break;
            }
        }
    }
    return hasServices ;
}

//------------------------------------------------------------------------------

}
