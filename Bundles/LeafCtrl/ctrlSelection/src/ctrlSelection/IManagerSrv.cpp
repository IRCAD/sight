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
    for(ConfigurationType connectCfg :  config->find("connect"))
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
    for(ConfigurationType proxyCfg :  config->find("proxy"))
    {
        ::fwServices::helper::Config::createProxy(objectId, proxyCfg, m_proxyCtns, object);
    }
}

//-----------------------------------------------------------------------------

void IManagerSrv::disconnectProxies(const std::string &objectId)
{
    ::fwServices::helper::Config::disconnectProxies(objectId, m_proxyCtns);
}

//-----------------------------------------------------------------------------

}
