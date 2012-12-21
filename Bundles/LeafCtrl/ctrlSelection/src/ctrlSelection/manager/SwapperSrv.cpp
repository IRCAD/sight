/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Acquisition.hpp>

#include "ctrlSelection/manager/SwapperSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SwapperSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SwapperSrv::SwapperSrv() throw() : m_dummyStopMode(false)
{
    //handlingEventOff ::fwComEd::CompositeMsg::ADDED_KEYS );
    //handlingEventOff ::fwComEd::CompositeMsg::REMOVED_KEYS );
    //handlingEventOff ::fwComEd::CompositeMsg::CHANGED_KEYS );
}

//-----------------------------------------------------------------------------

SwapperSrv::~SwapperSrv() throw()
{}

//-----------------------------------------------------------------------------

void SwapperSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    if (compositeMsg)
    {
        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getAddedKeys();
            this->addObjects( fields );
        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getRemovedKeys();
            this->removeObjects( fields );
        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::CHANGED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getNewChangedKeys();
            this->swapObjects( fields );
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SwapperSrv::updating() throw ( ::fwTools::Failed )
{
    stopping();
    starting();
}

//-----------------------------------------------------------------------------

void SwapperSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

void SwapperSrv::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    BOOST_FOREACH(SubServicesMapType::value_type elt, m_objectsSubServices)
    {
        SubServicesVecType subServices = elt.second;
        BOOST_REVERSE_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on "<< elt.first <<" expired !", subSrv->getService() );

            if( subSrv->m_hasAutoConnection )
            {
                subSrv->m_connections->disconnect();
            }
            subSrv->getService()->stop();
            ::fwServices::OSR::unregisterService(subSrv->getService());
            subSrv->m_service.reset();
        }
    }
    m_objectsSubServices.clear();

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
}

//-----------------------------------------------------------------------------

void SwapperSrv::configuring()  throw ( ::fwTools::Failed )
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
        m_mode = mode;
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SwapperSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin() ; iter != m_managerConfiguration->end() ; ++iter)
    {
        if ((*iter)->getName() == "object")
        {
            const std::string objectId      = (*iter)->getAttributeValue("id");

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

//-----------------------------------------------------------------------------

void SwapperSrv::addObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::ValueType addedObjectId, _composite->getContainer())
    {
        if(m_objectsSubServices.find(addedObjectId.first) != m_objectsSubServices.end())
        {
            // Services are on dummyObject
            this->swapObject(addedObjectId.first, addedObjectId.second);
        }
        else
        {
            this->addObject(addedObjectId.first, addedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::sptr SwapperSrv::add( ::fwData::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(), _elt->getName() == "service" ) ;
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") ) ;
    SLM_ASSERT("Attribute \"impl\" is missing", _elt->hasAttribute("impl") ) ;

    ::fwServices::IService::sptr service ;

    std::string serviceType = _elt->getExistingAttributeValue("type") ;
    std::string implementationType = _elt->getExistingAttributeValue("impl");

    // Add service with possible id
    if( _elt->hasAttribute("uid")  )
    {
        service = ::fwServices::add( obj , serviceType , implementationType , _elt->getExistingAttributeValue("uid") );
    }
    else
    {
        service =  ::fwServices::add( obj , serviceType , implementationType )  ;
    }

    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg = _elt;
    if( _elt->hasAttribute("config"))
    {
        cfg = ::fwRuntime::ConfigurationElement::constCast(
                ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
                        _elt->getExistingAttributeValue("config") , implementationType ) );
    }

    // Set configuration
    service->setConfiguration( cfg ) ;

    // Configure
    service->configure();

    // Return
    return service ;
}

//-----------------------------------------------------------------------------

void SwapperSrv::addObject( const std::string objectId, ::fwData::Object::sptr object )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType   = conf->getAttributeValue("type");

        OSLM_ASSERT("ObjectType "<<objectType<<" does not match ObjectType in Composite "<<object->getClassname(),
                objectType == object->getClassname());
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > confVec = conf->find("service");
        BOOST_FOREACH( ConfigurationType cfg, confVec )
        {
            ::fwServices::IService::sptr srv = this->add( object, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);

            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;

            // Standard communication management
            SLM_ASSERT("autoConnect attribute missing in service "<< srv->getClassname(),
                       cfg->hasAttribute("autoConnect"));

            if ( cfg->getExistingAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
                if (!subSrv->m_connections)
                {
                    subSrv->m_connections = ::fwServices::helper::SigSlotConnection::New();
                }
                subSrv->m_connections->connect( object, srv, srv->getObjSrvConnections() );
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

void SwapperSrv::swapObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::ValueType swappedObjectId, _composite->getContainer())
    {
        this->swapObject(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::swapObject(const std::string objectId, ::fwData::Object::sptr object)
{
    std::vector< ConfigurationType > confVec = m_managerConfiguration->find("object", "id", objectId);
    BOOST_FOREACH( ConfigurationType cfg, confVec )
    {
        this->removeConnections(objectId);
        this->disconnectProxies(objectId);

        SubServicesVecType subServices = m_objectsSubServices[objectId];
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
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
                    subSrv->m_connections->disconnect();
                    subSrv->m_connections->connect( object, subSrv->getService(),
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

void SwapperSrv::removeObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::ValueType swappedObjectId, _composite->getContainer())
    {
        this->removeObject(swappedObjectId.first);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeObject( const std::string objectId )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType   = conf->getAttributeValue("type");

        this->removeConnections(objectId);
        this->disconnectProxies(objectId);

        SubServicesVecType subServices = m_objectsSubServices[objectId];
        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(objectType);
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

            if (subSrv->m_hasAutoConnection)
            {
                subSrv->m_connections->disconnect();
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

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;

    OSLM_ASSERT(objectId << " not found in composite.", composite->find(objectId) == composite->end());

    ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
    const std::string objectType    = conf->getAttributeValue("type");
    SLM_ASSERT( "'type' required attribute missing or empty", !objectType.empty() );

    // Any subServices have been registered with object.
    if ( m_objectsSubServices.count(objectId) == 0 )
    {
        OSLM_TRACE ( "'"<< objectId << "' nonexistent'");

        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(objectType);
        SubServicesVecType subVecSrv;
        std::vector < ConfigurationType > confVec = conf->find("service");
        BOOST_FOREACH( ConfigurationType cfg, confVec )
        {
            ::fwServices::IService::sptr srv = this->add( dummyObj, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);

            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;
            subSrv->m_dummy = dummyObj;

            if ( cfg->getExistingAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
                subSrv->m_connections = ::fwServices::helper::SigSlotConnection::New();
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::manageConnections(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    BOOST_FOREACH(ConfigurationType connectCfg, config->find("connect"))
    {
        this->manageConnection(objectId, object, connectCfg);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::manageConnection(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    typedef std::pair< std::string, ::fwCom::Signals::SignalKeyType > SignalInfoType;
    typedef std::pair< std::string, ::fwCom::Slots::SlotKeyType > SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    SignalInfoType signalInfo;
    SlotInfoContainerType slotInfos;

    ::boost::regex re("(.*)/(.*)");
    ::boost::smatch match;
    std::string src, uid, key;

    BOOST_FOREACH(::fwRuntime::ConfigurationElement::csptr elem,  config->getElements())
    {
        SLM_ASSERT("Bad tag '" <<elem->getName() << "', only <signal> or <slot> are allowed.",
                   elem->getName() == "signal" || elem->getName() == "slot");

        src = elem->getValue();
        if( ::boost::regex_match(src, match, re) )
        {
            OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            OSLM_ASSERT(src << " configuration is not correct for "<< elem->getName() ,
                        !uid.empty() && !key.empty());

            if (elem->getName() == "signal")
            {
                SLM_ASSERT("There must be only one signal by connection",
                           signalInfo.first.empty() && signalInfo.second.empty());
                signalInfo = std::make_pair(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                slotInfos.push_back( std::make_pair(uid, key) );
            }
        }
        else
        {
            uid = object->getID();
            key = src;
            SLM_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            SLM_ASSERT("There must be only one signal by connection",
                       signalInfo.first.empty() && signalInfo.second.empty());
            signalInfo = std::make_pair(uid, key);
        }
    }

    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(signalInfo.first);
    ::fwCom::HasSignals::sptr hasSignals = ::boost::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);

    ::fwServices::helper::SigSlotConnection::sptr connection;
    ObjectConnectionsMapType::iterator iter = m_objectConnections.find(objectId);
    if (iter != m_objectConnections.end())
    {
        connection = iter->second;
    }
    else
    {
        connection = ::fwServices::helper::SigSlotConnection::New();
        m_objectConnections[objectId] = connection;
    }

    BOOST_FOREACH(SlotInfoType slotInfo,  slotInfos)
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(slotInfo.first);
        ::fwCom::HasSlots::sptr hasSlots = ::boost::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);

        connection->connect(hasSignals, signalInfo.second, hasSlots, slotInfo.second);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeConnections(const std::string &objectId)
{
    ObjectConnectionsMapType::iterator iter = m_objectConnections.find(objectId);
    if (iter != m_objectConnections.end())
    {
        ::fwServices::helper::SigSlotConnection::sptr connection = iter->second;
        connection->disconnect();
    }
    m_objectConnections.erase(objectId);
}

//-----------------------------------------------------------------------------

void SwapperSrv::manageProxies(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    BOOST_FOREACH(ConfigurationType proxyCfg, config->find("proxy"))
    {
        this->manageProxy(objectId, object, proxyCfg);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::manageProxy(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    SLM_ASSERT("Missing 'channel' attribute", config->hasAttribute("channel"));
    const std::string channel = config->getExistingAttributeValue("channel");
    ProxyConnections proxyCnt(channel);

    ::boost::regex re("(.*)/(.*)");
    ::boost::smatch match;
    std::string src, uid, key;
    BOOST_FOREACH(::fwRuntime::ConfigurationElement::csptr elem,  config->getElements())
    {
        src = elem->getValue();
        if( ::boost::regex_match(src, match, re) )
        {
            OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            OSLM_ASSERT(src << " configuration is not correct for "<< elem->getName() ,
                        !uid.empty() && !key.empty());

            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(uid);

            if (elem->getName() == "signal")
            {
                ::fwCom::HasSignals::sptr hasSignals = ::boost::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);
                ::fwCom::SignalBase::sptr sig = hasSignals->signal(key);
                proxy->connect(channel, sig);
                proxyCnt.addSignalConnection(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                ::fwCom::HasSlots::sptr hasSlots = ::boost::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                ::fwCom::SlotBase::sptr slot = hasSlots->slot(key);
                proxy->connect(channel, slot);
                proxyCnt.addSlotConnection(uid, key);
            }
        }
        else
        {
            uid = object->getID();
            key = src;
            SLM_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            ::fwCom::SignalBase::sptr sig = object->signal(key);
            proxy->connect(channel, sig);
            proxyCnt.addSignalConnection(uid, key);
        }
    }
    m_proxyCtns[objectId].push_back(proxyCnt);
}

//-----------------------------------------------------------------------------

void SwapperSrv::disconnectProxies(const std::string &objectId)
{
    ProxyConnectionsMapType::iterator iter = m_proxyCtns.find(objectId);
    if (iter != m_proxyCtns.end())
    {
        ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

        ProxyConnectionsVectType vectProxyConnections = iter->second;

        BOOST_FOREACH(ProxyConnectionsVectType::value_type proxyConnections,  vectProxyConnections)
        {
            BOOST_FOREACH(ProxyConnections::ProxyEltType signalElt, proxyConnections.m_signals)
            {
                ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(signalElt.first);
                ::fwCom::HasSignals::sptr hasSignals = ::boost::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);
                ::fwCom::SignalBase::sptr sig = hasSignals->signal(signalElt.second);
                proxy->disconnect(proxyConnections.m_channel, sig);
            }
            BOOST_FOREACH(ProxyConnections::ProxyEltType slotElt, proxyConnections.m_slots)
            {
                ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(slotElt.first);
                ::fwCom::HasSlots::sptr hasSlots = ::boost::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                ::fwCom::SlotBase::sptr slot = hasSlots->slot(slotElt.second);
                proxy->disconnect(proxyConnections.m_channel, slot);
            }
        }
        vectProxyConnections.clear();
    }
    m_proxyCtns.erase(objectId);
}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
