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

#include <fwCore/LogicStamp.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/fwID.hpp>

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

    OSLM_ASSERT("Service "<< service->getID()<<" already registered",
            getDefault()->m_container.right.find(service) == getDefault()->m_container.right.end());

    OSLM_ASSERT("Service "<< service->getID()<<" has already a valid associated object" ,
            service->m_associatedObject.expired() || object != service->m_associatedObject.lock());

    service->m_associatedObject = object;
    getDefault()->m_container.insert( KSContainer::value_type( object->getOSRKey()->getLogicStamp(), service ) );
}

//------------------------------------------------------------------------------

void ObjectServiceRegistry::swapService(  ::fwTools::Object::sptr  _objSrc, ::fwTools::Object::sptr  _objDst, ::fwServices::IService::sptr _service )
{
    OSLM_ASSERT("Object "<< _objSrc->getID()<<" is not registered in OSR",
                getDefault()->m_container.left.find(_objSrc->getOSRKey()->getLogicStamp()) != getDefault()->m_container.left.end());

    OSLM_ASSERT("Service "<< _service->getID()<<" is not registered in OSR",
                    getDefault()->m_container.right.find(_service) != getDefault()->m_container.right.end());

    getDefault()->m_container.right.erase(_service);
    getDefault()->registerService(_objDst, _service);
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
    OSLM_ASSERT("Unknown service "<<_service->getID()<<" in OSR",
            getDefault()->m_container.right.find(_service) != getDefault()->m_container.right.end());
    getDefault()->m_container.right.erase(_service);
}

//------------------------------------------------------------------------------

std::vector< ::fwTools::Object::sptr > ObjectServiceRegistry::getObjects()
{
    std::vector< ::fwTools::Object::sptr >   lobjects;
    ObjectServiceRegistry::KSContainer::right_map & right = getDefault()->m_container.right;
    BOOST_FOREACH( ObjectServiceRegistry::KSContainer::right_map::value_type elt, right)
    {
        if ( std::find(lobjects.begin(), lobjects.end(), elt.first->getObject()) != lobjects.end() )
        {
            lobjects.push_back( elt.first->getObject() ) ;
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", lobjects.empty() );
    return lobjects;
}

//------------------------------------------------------------------------------

const ObjectServiceRegistry::KSContainer  & ObjectServiceRegistry::getKSContainer()
{
    return getDefault()->m_container;
}

//------------------------------------------------------------------------------

std::string ObjectServiceRegistry::getRegistryInformation()
{
    std::stringstream info;
    BOOST_FOREACH( KSContainer::left_map::value_type objSrvMap, getDefault()->m_container.left)
    {
        // TODO FIXME getObject() failed if there are expired object in OSR
        //::fwTools::Object::sptr obj = objSrvMap.second->getObject();
        ::fwCore::LogicStamp::LogicStampType key = objSrvMap.first;
        info << "New object found in OSR" << std::endl;
        //info << "Object ( uid = "<< obj->getID() <<" , classname = "<< obj->getClassname() <<" ) has "
        info << "Object ( key = "<<key<<" ) has "
                << getDefault()->m_container.left.count(key) <<" services." << std::endl;

        ::fwServices::IService::sptr service = objSrvMap.second;
        info << "    srv : uid = "<< service->getID() <<" , classname = "<< service->getClassname()
                         <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
    }
    return info.str();
}

//------------------------------------------------------------------------------

bool ObjectServiceRegistry::has( ::fwTools::Object::sptr obj , const std::string & srvType)
{
    bool hasServices = false;
    if( ObjectServiceRegistry::getDefault()->m_container.left.find(obj->getOSRKey()->getLogicStamp()) != ObjectServiceRegistry::getDefault()->m_container.left.end())
    {
        ObjectServiceRegistry::KSContainer::left_map::iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ObjectServiceRegistry::KSContainer::left_map::iterator firstElement = getDefault()->m_container.left.find(key);
        ObjectServiceRegistry::KSContainer::left_map::iterator lastElement = getDefault()->m_container.left.upper_bound(key);
        for (iter = firstElement ; iter != lastElement ; ++iter)
        {
            if( iter->second->isA(srvType))
            {
                hasServices = true;
                break;
            }
        }
    }
    return hasServices ;
}

//------------------------------------------------------------------------------

bool ObjectServiceRegistry::hasKey( ::fwCore::LogicStamp::csptr key )
{
    return (ObjectServiceRegistry::getDefault()->m_container.left.find(key->getLogicStamp()) != ObjectServiceRegistry::getDefault()->m_container.left.end());
}

//------------------------------------------------------------------------------

}
