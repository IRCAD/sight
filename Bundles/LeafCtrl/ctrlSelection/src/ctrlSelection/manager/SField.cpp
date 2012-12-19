/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>
#include <fwServices/registry/Proxy.hpp>

#include "ctrlSelection/manager/SField.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SField, ::fwData::Object ) ;

//-----------------------------------------------------------------------------

SField::SField() throw() : m_dummyStopMode(false)
{
    //this->addNewHandledEvent( ::fwServices::ObjectMsg::ADDED_FIELDS );
    //this->addNewHandledEvent( ::fwServices::ObjectMsg::REMOVED_FIELDS );
    //this->addNewHandledEvent( ::fwServices::ObjectMsg::CHANGED_FIELDS );
}

//-----------------------------------------------------------------------------

SField::~SField() throw()
{}

//-----------------------------------------------------------------------------

void SField::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwServices::ObjectMsg::csptr fieldMsg = ::fwServices::ObjectMsg::dynamicConstCast(message);
    SLM_FATAL_IF("Received message must be fieldMsg", fieldMsg == 0 );

    if ( fieldMsg->hasEvent( ::fwServices::ObjectMsg::ADDED_FIELDS ) )
    {
        this->addFields( fieldMsg->getAddedFields() );
    }

    if ( fieldMsg->hasEvent( ::fwServices::ObjectMsg::REMOVED_FIELDS ) )
    {
        this->removeFields( fieldMsg->getRemovedFields() );
    }

    if ( fieldMsg->hasEvent( ::fwServices::ObjectMsg::CHANGED_FIELDS ) )
    {
        this->swapFields( fieldMsg->getNewChangedFields() );
    }
}

//-----------------------------------------------------------------------------

void SField::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SField::updating() throw ( ::fwTools::Failed )
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void SField::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

void SField::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    BOOST_FOREACH(SubServicesMapType::value_type elt, m_fieldsSubServices)
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
    m_fieldsSubServices.clear();

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

void SField::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    std::vector < ConfigurationType > vectMode = m_configuration->find("mode");
    if(!vectMode.empty())
    {
        ConfigurationType modeConfiguration = vectMode.at(0);
        SLM_ASSERT("Missing 'type' attribute", modeConfiguration->hasAttribute("type"));
        std::string mode = modeConfiguration->getAttributeValue("type");
        SLM_ASSERT("Wrong mode type", (mode == "dummy" ) || (mode == "stop" ) || mode=="startAndUpdate");
        m_dummyStopMode = (mode == "dummy" );
        m_mode = mode;
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SField::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Object::sptr object = this->getObject() ;
    const ::fwData::Object::FieldMapType& fieldsMap = object->getFields();
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin() ; iter != m_managerConfiguration->end() ; ++iter)
    {
        if ((*iter)->getName() == "field")
        {
            const std::string fieldName = (*iter)->getAttributeValue("id");

            if (fieldsMap.find(fieldName) != fieldsMap.end())
            {
                this->addField(fieldName, fieldsMap.at(fieldName));
            }
            else if (m_dummyStopMode)
            {
                // Initialize on dummy fields
                this->initOnDummyObject( fieldName );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SField::addFields( const ModifiedFieldsContainerType& fields )
{
    BOOST_FOREACH( ModifiedFieldsContainerType::value_type addedObjectId, fields)
    {
        if(m_fieldsSubServices.find(addedObjectId.first) != m_fieldsSubServices.end())
        {
            // Services are on dummyObject
            this->swapField(addedObjectId.first, addedObjectId.second);
        }
        else
        {
            this->addField(addedObjectId.first, addedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::sptr SField::add( ::fwData::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
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
        cfg = ::fwRuntime::ConfigurationElement::constCast( ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( _elt->getExistingAttributeValue("config") , implementationType ) );
    }

    // Set configuration
    service->setConfiguration( cfg ) ;

    // Configure
    service->configure();

    // Return
    return service ;
}

//-----------------------------------------------------------------------------

void SField::addField( const FieldNameType& fieldName, ::fwData::Object::sptr field )
{
    if(!m_managerConfiguration->find("field", "id", fieldName).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("field", "id", fieldName).at(0);
        const std::string fieldType   = conf->getAttributeValue("type");

        OSLM_ASSERT("FieldType "<<fieldType<<" does not match ObjectType in Object "<<field->getClassname(),
                fieldType == field->getClassname());
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > services = conf->find("service");
        BOOST_FOREACH( ConfigurationType cfg, services)
        {
            ::fwServices::IService::sptr srv = this->add( field, cfg );
            OSLM_ASSERT("Instantiation Service failed on field "<<fieldName, srv);
            srv->configure();
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
                subSrv->m_connections->connect( field, srv, srv->getObjSrvConnections() );
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
            if (m_mode =="startAndUpdate")
            {
                subSrv->getService()->update();
            }
        }
        m_fieldsSubServices[fieldName] = subVecSrv;

        this->manageConnections(fieldName, field, conf);
        this->manageProxies(fieldName, field, conf);
    }
    else
    {
        OSLM_DEBUG("Field "<<fieldName<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SField::swapFields( const ModifiedFieldsContainerType& fields )
{
    BOOST_FOREACH( ModifiedFieldsContainerType::value_type swappedObjectId, fields)
    {
        this->swapField(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SField::swapField(const FieldNameType& fieldName, ::fwData::Object::sptr field)
{
    std::vector< ConfigurationType > fields = m_managerConfiguration->find("field", "id", fieldName);
    BOOST_FOREACH( ConfigurationType cfg, fields)
    {
        SubServicesVecType subServices = m_fieldsSubServices[fieldName];
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on " << fieldName <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

            OSLM_TRACE("Swapping subService " << subSrv->getService()->getID() << " on "<< fieldName );
            if(subSrv->getService()->getObject() != field)
            {
                subSrv->getService()->swap(field);
                subSrv->m_dummy.reset();

                if (subSrv->m_hasAutoConnection)
                {
                    subSrv->m_connections->disconnect();
                    subSrv->m_connections->connect( field, subSrv->getService(),
                                                    subSrv->getService()->getObjSrvConnections() );
                }
            }
            else
            {
                OSLM_WARN( subSrv->getService()->getID()
                           << "'s field already is '"
                           << subSrv->getService()->getObject()->getID()
                           << "', no need to swap");
            }
        }
        this->manageConnections(fieldName, field, cfg);
        this->manageProxies(fieldName, field, cfg);
    }
}

//-----------------------------------------------------------------------------

void SField::removeFields( const ModifiedFieldsContainerType& fields )
{
    BOOST_FOREACH( ModifiedFieldsContainerType::value_type swappedObjectId, fields)
    {
        this->removeField(swappedObjectId.first);
    }
}

//-----------------------------------------------------------------------------

void SField::removeField( const FieldNameType& fieldName )
{
    if(!m_managerConfiguration->find("field", "id", fieldName).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("field", "id", fieldName).at(0);
        const std::string fieldType   = conf->getAttributeValue("type");

        this->removeConnections(fieldName);
        this->disconnectProxies(fieldName);

        SubServicesVecType subServices = m_fieldsSubServices[fieldName];
        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(fieldType);
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on " << fieldName <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());
            if(m_dummyStopMode)
            {
                subSrv->getService()->swap(dummyObj);
                subSrv->m_dummy = dummyObj;
            }
            else
            {
                if( subSrv->m_hasAutoConnection )
                {
                    subSrv->m_connections->disconnect();
                }

                subSrv->getService()->stop();
                ::fwServices::OSR::unregisterService(subSrv->getService());
                subSrv->m_service.reset();
            }
        }
        if(!m_dummyStopMode)
        {
            m_fieldsSubServices.erase(fieldName);
        }
        else
        {
            this->manageConnections(fieldName, dummyObj, conf);
            this->manageProxies(fieldName, dummyObj, conf);
        }
    }
    else
    {
        OSLM_DEBUG("Object "<<fieldName<<" not managed.");
    }
}

//-----------------------------------------------------------------------------

void SField::initOnDummyObject( const FieldNameType& fieldName )
{
    SLM_ASSERT( "'fieldName' required attribute missing or empty", !fieldName.empty() );

    ::fwData::Object::sptr object = this->getObject() ;

    OSLM_ASSERT(fieldName << " not found in object.",
            object->getFields().find(fieldName) == object->getFields().end());

    ConfigurationType conf = m_managerConfiguration->find("field", "id", fieldName).at(0);
    const std::string fieldType    = conf->getAttributeValue("type");
    SLM_ASSERT( "'type' required attribute missing or empty", !fieldType.empty() );

    // Any subServices have been registered with field.
    if ( m_fieldsSubServices.count(fieldName) == 0 )
    {
        OSLM_TRACE ( "'"<< fieldName << "' nonexistent'");

        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(fieldType);
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > services = conf->find("service");
        BOOST_FOREACH( ConfigurationType cfg, services)
        {
            ::fwServices::IService::sptr srv = this->add( dummyObj, cfg );
            OSLM_ASSERT("Instantiation Service failed ofieldct "<<fieldName, srv);
            srv->configure();
            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;
            subSrv->m_dummy = dummyObj;
            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();

            if ( cfg->getExistingAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
                subSrv->m_connections = ::fwServices::helper::SigSlotConnection::New();
            }
        }
        m_fieldsSubServices[fieldName] = subVecSrv;
    }
}

//-----------------------------------------------------------------------------

void SField::manageConnections(const std::string &fieldName, ::fwData::Object::sptr object, ConfigurationType config)
{
    BOOST_FOREACH(ConfigurationType connectCfg, config->find("connect"))
    {
        this->manageConnection(fieldName, object, connectCfg);
    }
}

//-----------------------------------------------------------------------------

void SField::manageConnection(const std::string &fieldName, ::fwData::Object::sptr object, ConfigurationType config)
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
    ObjectConnectionsMapType::iterator iter = m_objectConnections.find(fieldName);
    if (iter != m_objectConnections.end())
    {
        connection = iter->second;
    }
    else
    {
        connection = ::fwServices::helper::SigSlotConnection::New();
        m_objectConnections[fieldName] = connection;
    }

    BOOST_FOREACH(SlotInfoType slotInfo,  slotInfos)
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(slotInfo.first);
        ::fwCom::HasSlots::sptr hasSlots = ::boost::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);

        connection->connect(hasSignals, signalInfo.second, hasSlots, slotInfo.second);
    }
}

//-----------------------------------------------------------------------------

void SField::removeConnections(const std::string &fieldName)
{
    ObjectConnectionsMapType::iterator iter = m_objectConnections.find(fieldName);
    if (iter != m_objectConnections.end())
    {
        ::fwServices::helper::SigSlotConnection::sptr connection = iter->second;
        connection->disconnect();
    }
    m_objectConnections.erase(fieldName);
}

//-----------------------------------------------------------------------------

void SField::manageProxies(const std::string &fieldName, ::fwData::Object::sptr object, ConfigurationType config)
{
    BOOST_FOREACH(ConfigurationType proxyCfg, config->find("proxy"))
    {
        this->manageProxy(fieldName, object, proxyCfg);
    }
}

//-----------------------------------------------------------------------------

void SField::manageProxy(const std::string &fieldName, ::fwData::Object::sptr object, ConfigurationType config)
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
    m_proxyCtns[fieldName].push_back(proxyCnt);
}

//-----------------------------------------------------------------------------

void SField::disconnectProxies(const std::string &fieldName)
{
    ProxyConnectionsMapType::iterator iter = m_proxyCtns.find(fieldName);
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
    m_proxyCtns.erase(fieldName);
}

//-----------------------------------------------------------------------------


} // manager
} // ctrlSelection
