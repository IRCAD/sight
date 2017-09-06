/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/manager/SwapperSrv.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/Composite.hpp>

#include <boost/foreach.hpp>

namespace ctrlSelection
{

namespace manager
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObjects";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObjects";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SwapperSrv, ::fwData::Composite );

//-----------------------------------------------------------------------------

SwapperSrv::SwapperSrv() noexcept : m_dummyStopMode(false)
{
    newSlot(s_ADD_OBJECTS_SLOT, &SwapperSrv::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &SwapperSrv::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SwapperSrv::removeObjects, this);
}

//-----------------------------------------------------------------------------

SwapperSrv::~SwapperSrv() noexcept
{
}

//-----------------------------------------------------------------------------

void SwapperSrv::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void SwapperSrv::updating()
{
    stopping();
    starting();
}

//-----------------------------------------------------------------------------

void SwapperSrv::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SwapperSrv::stopping()
{
    SLM_TRACE_FUNC();

    while( !m_objectConnections.empty())
    {
        this->removeConnections(m_objectConnections.begin()->first);
    }
    SLM_ASSERT("Connections must be empty", m_objectConnections.empty());

    while( !m_proxyCtns.empty())
    {
        this->disconnectProxies(m_proxyCtns.begin()->first);
    }
    SLM_ASSERT("Proxy connections must be empty", m_proxyCtns.empty());


    for(SubServicesMapType::value_type elt :  m_objectsSubServices)
    {
        SubServicesVecType subServices = elt.second;
        BOOST_REVERSE_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on "<< elt.first <<" expired !", subSrv->getService() );

            if( subSrv->m_hasAutoConnection )
            {
                subSrv->m_connections.disconnect();
            }
            subSrv->getService()->stop().wait();
            ::fwServices::OSR::unregisterService(subSrv->getService());
            subSrv->m_service.reset();
        }
    }
    m_objectsSubServices.clear();
}

//-----------------------------------------------------------------------------

void SwapperSrv::configuring()
{
    SLM_TRACE_FUNC();
    std::vector < ConfigurationType > vectMode = m_configuration->find("mode");
    if(!vectMode.empty())
    {
        ConfigurationType modeConfiguration = vectMode.at(0);
        SLM_ASSERT("Missing attribute type", modeConfiguration->hasAttribute("type"));
        std::string mode = modeConfiguration->getAttributeValue("type");
        SLM_ASSERT("Wrong type mode", (mode == "dummy" ) || (mode == "stop" ) || mode=="startAndUpdate");
        m_dummyStopMode = (mode == "dummy" );
        m_mode          = mode;
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SwapperSrv::starting()
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin(); iter != m_managerConfiguration->end(); ++iter)
    {
        if ((*iter)->getName() == "object")
        {
            const std::string objectId = (*iter)->getAttributeValue("id");

            if (composite->find(objectId) != composite->end())
            {
                this->addObject(objectId, (*composite)[objectId]);
            }
            else if (m_dummyStopMode)
            {
                // Initialize on dummy objects
                this->initOnDummyObject( objectId );
            }
        }
    }
}

//------------------------------------------------------------------------------

void SwapperSrv::addObjects(::fwData::Composite::ContainerType objects)
{
    for( ::fwData::Composite::ValueType addedObjectId :  objects)
    {
        if(m_objectsSubServices.find(addedObjectId.first) != m_objectsSubServices.end())
        {
            // Services are on dummyObject
            this->changeObject(addedObjectId.first, addedObjectId.second);
        }
        else
        {
            this->addObject(addedObjectId.first, addedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::sptr SwapperSrv::add( ::fwData::Object::sptr obj, ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(),
                _elt->getName() == "service" );
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") );
    SLM_ASSERT("Attribute \"impl\" is missing", _elt->hasAttribute("impl") );

    ::fwServices::IService::sptr service;

    std::string serviceType        = _elt->getExistingAttributeValue("type");
    std::string implementationType = _elt->getExistingAttributeValue("impl");

    // Add service with possible id
    if( _elt->hasAttribute("uid")  )
    {
        service = ::fwServices::add( obj, serviceType, implementationType, _elt->getExistingAttributeValue("uid") );
    }
    else
    {
        service = ::fwServices::add( obj, serviceType, implementationType );
    }

    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg = _elt;
    if( _elt->hasAttribute("config"))
    {
        cfg = ::fwRuntime::ConfigurationElement::constCast(
            ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
                _elt->getExistingAttributeValue("config"), implementationType ) );
    }

    // Set configuration
    service->setConfiguration( cfg );

    // Configure
    service->configure();

    // Return
    return service;
}

//-----------------------------------------------------------------------------

void SwapperSrv::addObject( const std::string& objectId, ::fwData::Object::sptr object )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf       = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType = conf->getAttributeValue("type");

        OSLM_ASSERT("ObjectType "<<objectType<<" does not match ObjectType in Composite "<<object->getClassname(),
                    objectType == object->getClassname());
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > confVec = conf->find("service");
        for( ConfigurationType cfg :  confVec )
        {
            ::fwServices::IService::sptr srv = this->add( object, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);

            SPTR(SubService) subSrv = SPTR(SubService)( new SubService());
            subSrv->m_config        = cfg;
            subSrv->m_service       = srv;

            // Standard communication management
            if ( cfg->getAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
                subSrv->m_connections.connect( object, srv, srv->getObjSrvConnections() );
            }

            std::string workerKey = cfg->getAttributeValue("worker");

            if (!workerKey.empty())
            {
                ::fwServices::registry::ActiveWorkers::sptr activeWorkers =
                    ::fwServices::registry::ActiveWorkers::getDefault();
                ::fwThread::Worker::sptr worker;
                worker = activeWorkers->getWorker(workerKey);
                if (!worker)
                {
                    worker = ::fwThread::Worker::New();
                    activeWorkers->addWorker(workerKey, worker);
                }
                srv->setWorker(worker);
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
            if (m_mode =="startAndUpdate")
            {
                subSrv->getService()->update();
            }
        }
        m_objectsSubServices[objectId] = subVecSrv;


        this->manageConnections(objectId, object, conf);
        this->manageProxies(objectId, object, conf);
    }
    else
    {
        OSLM_DEBUG("Object "<<objectId<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SwapperSrv::changeObjects(::fwData::Composite::ContainerType newObjects,
                               ::fwData::Composite::ContainerType oldObjects)
{
    for( ::fwData::Composite::ValueType swappedObjectId : newObjects)
    {
        this->changeObject(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::changeObject(const std::string& objectId, ::fwData::Object::sptr object)
{
    std::vector< ConfigurationType > confVec = m_managerConfiguration->find("object", "id", objectId);
    for( ConfigurationType cfg :  confVec )
    {
        this->removeConnections(objectId);
        this->disconnectProxies(objectId);

        SubServicesVecType subServices = m_objectsSubServices[objectId];
        for( SPTR(SubService) subSrv :  subServices )
        {
            OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

            OSLM_TRACE("Swapping subService " << subSrv->getService()->getID() << " on "<< objectId );
            if(subSrv->getService()->getObject() != object)
            {
                subSrv->getService()->swap(object);
                subSrv->m_dummy.reset();

                if (subSrv->m_hasAutoConnection)
                {
                    subSrv->m_connections.disconnect();
                    subSrv->m_connections.connect( object, subSrv->getService(),
                                                   subSrv->getService()->getObjSrvConnections() );
                }
            }
            else
            {
                OSLM_WARN( subSrv->getService()->getID()
                           << "'s object already is '"
                           << subSrv->getService()->getObject()->getID()
                           << "', no need to swap");
            }
        }

        this->manageConnections(objectId, object, cfg);
        this->manageProxies(objectId, object, cfg);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeObjects( ::fwData::Composite::ContainerType objects )
{
    for( ::fwData::Composite::ValueType swappedObjectId : objects)
    {
        this->removeObject(swappedObjectId.first);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeObject( const std::string& objectId )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf       = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType = conf->getAttributeValue("type");

        this->removeConnections(objectId);
        this->disconnectProxies(objectId);

        SubServicesVecType subServices = m_objectsSubServices[objectId];
        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(objectType);
        for( SPTR(SubService) subSrv :  subServices )
        {
            OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

            if (subSrv->m_hasAutoConnection)
            {
                subSrv->m_connections.disconnect();
            }
            if(m_dummyStopMode)
            {
                subSrv->getService()->swap(dummyObj);
                subSrv->m_dummy = dummyObj;
            }
            else
            {
                subSrv->getService()->stop();
                ::fwServices::OSR::unregisterService(subSrv->getService());
                subSrv->m_service.reset();
            }
        }
        if(!m_dummyStopMode)
        {
            m_objectsSubServices.erase(objectId);
        }
        else
        {
            this->manageConnections(objectId, dummyObj, conf);
            this->manageProxies(objectId, dummyObj, conf);
        }
    }
    else
    {
        OSLM_DEBUG("Object "<<objectId<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SwapperSrv::initOnDummyObject( std::string objectId )
{
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    OSLM_ASSERT(objectId << " not found in composite.", composite->find(objectId) == composite->end());

    ConfigurationType conf       = m_managerConfiguration->find("object", "id", objectId).at(0);
    const std::string objectType = conf->getAttributeValue("type");
    SLM_ASSERT( "'type' required attribute missing or empty", !objectType.empty() );

    // Any subServices have been registered with object.
    if ( m_objectsSubServices.count(objectId) == 0 )
    {
        OSLM_TRACE ( "'"<< objectId << "' nonexistent'");

        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(objectType);
        SubServicesVecType subVecSrv;
        std::vector < ConfigurationType > confVec = conf->find("service");
        for( ConfigurationType cfg :  confVec )
        {
            ::fwServices::IService::sptr srv = this->add( dummyObj, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);

            SPTR(SubService) subSrv = SPTR(SubService)( new SubService());
            subSrv->m_config        = cfg;
            subSrv->m_service       = srv;
            subSrv->m_dummy         = dummyObj;

            if ( cfg->getAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SwapperSrv::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
