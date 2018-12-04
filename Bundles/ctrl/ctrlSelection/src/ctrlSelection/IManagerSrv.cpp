/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ctrlSelection/IManagerSrv.hpp"

#include <fwServices/helper/Config.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <boost/regex.hpp>

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

IManagerSrv::IManagerSrv()
{
    FW_DEPRECATED_MSG("The IManagerSrv type of service is deprecated.", "18.0.");
}

//-----------------------------------------------------------------------------

IManagerSrv::~IManagerSrv()
{
}

//-----------------------------------------------------------------------------

void IManagerSrv::swapping()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void IManagerSrv::manageConnections(const std::string& objectId, ::fwData::Object::sptr object,
                                    ConfigurationType config)
{
    for(ConfigurationType connectCfg :  config->find("connect"))
    {
        this->manageConnection(objectId, object, connectCfg);
    }
}

//-----------------------------------------------------------------------------

void IManagerSrv::manageConnection(const std::string& objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    ::fwServices::helper::Config::createConnections(config, m_objectConnections[objectId], object);
}

//-----------------------------------------------------------------------------

void IManagerSrv::removeConnections(const std::string& objectId)
{
    ObjectConnectionsMapType::iterator iter = m_objectConnections.find(objectId);
    if (iter != m_objectConnections.end())
    {
        ::fwCom::helper::SigSlotConnection& connection = iter->second;
        connection.disconnect();
    }
    m_objectConnections.erase(objectId);
}

//-----------------------------------------------------------------------------

void IManagerSrv::manageProxies(const std::string& objectId, ::fwData::Object::sptr object, ConfigurationType config)
{
    for(ConfigurationType proxyCfg :  config->find("proxy"))
    {
        ::fwServices::helper::Config::createProxy(objectId, proxyCfg, m_proxyCtns, object);
    }
}

//-----------------------------------------------------------------------------

void IManagerSrv::disconnectProxies(const std::string& objectId)
{
    ::fwServices::helper::Config::disconnectProxies(objectId, m_proxyCtns);
}

//-----------------------------------------------------------------------------

}
