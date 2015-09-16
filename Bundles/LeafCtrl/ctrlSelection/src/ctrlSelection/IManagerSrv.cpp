/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/IManagerSrv.hpp"

#include <fwServices/helper/Config.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <boost/regex.hpp>

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

IManagerSrv::IManagerSrv()
{
}

//-----------------------------------------------------------------------------

IManagerSrv::~IManagerSrv()
{
}

//-----------------------------------------------------------------------------

void IManagerSrv::swapping() throw ( ::fwTools::Failed )
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void IManagerSrv::manageConnections(const std::string &objectId, ::fwData::Object::sptr object,
                                    ConfigurationType config)
{
    BOOST_FOREACH(ConfigurationType connectCfg, config->find("connect"))
    {
        this->manageConnection(objectId, object, connectCfg);
    }
}

//-----------------------------------------------------------------------------

void IManagerSrv::manageConnection(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    ::fwServices::helper::SigSlotConnection::sptr connection;
    ObjectConnectionsMapType::iterator iter = m_objectConnections.find(objectId);
    if (iter != m_objectConnections.end())
    {
        connection = iter->second;
    }
    else
    {
        connection                    = ::fwServices::helper::SigSlotConnection::New();
        m_objectConnections[objectId] = connection;
    }

    ::fwServices::helper::Config::createConnections(config, connection, object);
}

//-----------------------------------------------------------------------------

void IManagerSrv::removeConnections(const std::string &objectId)
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

void IManagerSrv::manageProxies(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    BOOST_FOREACH(ConfigurationType proxyCfg, config->find("proxy"))
    {
        this->manageProxy(objectId, object, proxyCfg);
    }
}

//-----------------------------------------------------------------------------

void IManagerSrv::manageProxy(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config)
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

            OSLM_ASSERT(src << " configuration is not correct for "<< elem->getName(),
                        !uid.empty() && !key.empty());

            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(uid);

            if (elem->getName() == "signal")
            {
                ::fwCom::HasSignals::sptr hasSignals = ::boost::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);
                ::fwCom::SignalBase::sptr sig        = hasSignals->signal(key);
                proxy->connect(channel, sig);
                proxyCnt.addSignalConnection(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                ::fwCom::HasSlots::sptr hasSlots = ::boost::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                ::fwCom::SlotBase::sptr slot     = hasSlots->slot(key);
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

void IManagerSrv::disconnectProxies(const std::string &objectId)
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
                ::fwTools::Object::sptr obj          = ::fwTools::fwID::getObject(signalElt.first);
                ::fwCom::HasSignals::sptr hasSignals = ::boost::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);
                ::fwCom::SignalBase::sptr sig        = hasSignals->signal(signalElt.second);
                proxy->disconnect(proxyConnections.m_channel, sig);
            }
            BOOST_FOREACH(ProxyConnections::ProxyEltType slotElt, proxyConnections.m_slots)
            {
                ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(slotElt.first);
                ::fwCom::HasSlots::sptr hasSlots = ::boost::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                ::fwCom::SlotBase::sptr slot     = hasSlots->slot(slotElt.second);
                proxy->disconnect(proxyConnections.m_channel, slot);
            }
        }
        vectProxyConnections.clear();
    }
    m_proxyCtns.erase(objectId);
}

//-----------------------------------------------------------------------------

}
