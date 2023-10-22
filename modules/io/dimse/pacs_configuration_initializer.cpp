/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "pacs_configuration_initializer.hpp"

#include <data/composite.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/__/preferences.hpp>

#include <utility>

namespace sight::module::io::dimse
{

static const std::string s_LocalApplicationTitle("localApplicationTitle");
static const std::string s_PacsHostName("pacsHostName");
static const std::string s_PacsApplicationTitle("pacsApplicationTitle");
static const std::string s_PacsApplicationPort("pacsApplicationPort");
static const std::string s_MoveApplicationTitle("moveApplicationTitle");
static const std::string s_MoveApplicationPort("moveApplicationPort");
static const std::string s_RetrieveMethod("retrieveMethod");

//------------------------------------------------------------------------------

pacs_configuration_initializer::pacs_configuration_initializer() noexcept =
    default;

//------------------------------------------------------------------------------

pacs_configuration_initializer::~pacs_configuration_initializer() noexcept =
    default;

//------------------------------------------------------------------------------

std::string pacs_configuration_initializer::getKey(const std::string& _sub_key) const noexcept
{
    return m_preferenceKey + "." + _sub_key;
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::configuring()
{
    const config_t config_tree = this->get_config();
    const auto config          = config_tree.get_child("config.<xmlattr>");

    // Local application title.
    m_SCUAppEntityTitle = config.get<std::string>(s_LocalApplicationTitle);

    // Pacs host name.
    m_SCPHostName = config.get<std::string>(s_PacsHostName);

    // Pacs application title.
    m_SCPAppEntityTitle = config.get<std::string>(s_PacsApplicationTitle);

    // Pacs port.
    const auto pacs_application_port = config.get<std::string>(s_PacsApplicationPort);
    m_SCPPort = static_cast<decltype(m_SCPPort)>(std::stoi(pacs_application_port));

    // Retrieve Method.
    const auto retrieve_method = config.get<std::string>(s_RetrieveMethod);
    m_retrieveMethod = (retrieve_method == "MOVE")
                       ? (sight::io::dimse::data::PacsConfiguration::MOVE_RETRIEVE_METHOD)
                       : (sight::io::dimse::data::PacsConfiguration::GET_RETRIEVE_METHOD);

    // Move application title.
    m_moveAppEntityTitle = config.get<std::string>(s_MoveApplicationTitle);

    // Move application port.
    const auto move_application_port = config.get<std::string>(s_MoveApplicationPort);
    m_movePort = static_cast<decltype(m_movePort)>(std::stoi(move_application_port));

    // Preference Key.
    m_preferenceKey = config.get<std::string>("preferenceKey", m_preferenceKey);
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::starting()
{
    const auto pacs_configuration = m_config.lock();
    SIGHT_ASSERT("inout '" << s_CONFIG_INOUT << "' does not exist.", pacs_configuration);

    // Set information from xml and update PacsConfiguration.
    if(!m_preferenceKey.empty())
    {
        try
        {
            ui::preferences preferences;

            m_SCUAppEntityTitle  = preferences.get(getKey(s_LocalApplicationTitle), m_SCUAppEntityTitle);
            m_SCPHostName        = preferences.get(getKey(s_PacsHostName), m_SCPHostName);
            m_SCPAppEntityTitle  = preferences.get(getKey(s_PacsApplicationTitle), m_SCPAppEntityTitle);
            m_SCPPort            = preferences.get(getKey(s_PacsApplicationPort), m_SCPPort);
            m_moveAppEntityTitle = preferences.get(getKey(s_MoveApplicationTitle), m_moveAppEntityTitle);
            m_movePort           = preferences.get(getKey(s_MoveApplicationPort), m_movePort);
            m_retrieveMethod     = static_cast<sight::io::dimse::data::PacsConfiguration::RETRIEVE_METHOD>(
                preferences.get(getKey(s_RetrieveMethod), static_cast<int>(m_retrieveMethod))
            );
        }
        catch(const ui::preferences_disabled& /*e*/)
        {
            // Nothing to do..
        }
    }

    pacs_configuration->setLocalApplicationTitle(m_SCUAppEntityTitle);
    pacs_configuration->setPacsHostName(m_SCPHostName);
    pacs_configuration->setPacsApplicationTitle(m_SCPAppEntityTitle);
    pacs_configuration->setPacsApplicationPort(m_SCPPort);
    pacs_configuration->setMoveApplicationTitle(m_moveAppEntityTitle);
    pacs_configuration->setMoveApplicationPort(m_movePort);
    pacs_configuration->setRetrieveMethod(m_retrieveMethod);
}

//------------------------------------------------------------------------------

service::connections_t pacs_configuration_initializer::auto_connections() const
{
    service::connections_t connections;

    connections.push(
        s_CONFIG_INOUT,
        sight::io::dimse::data::PacsConfiguration::MODIFIED_SIG,
        service::slots::UPDATE
    );

    return connections;
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::updating()
{
    const auto pacs_configuration = m_config.lock();
    SIGHT_ASSERT("inout '" << s_CONFIG_INOUT << "' does not exist.", pacs_configuration);

    // Check if the user has changed the Pacs configuration and update the local var
    if(pacs_configuration->getLocalApplicationTitle() != m_SCUAppEntityTitle
       || pacs_configuration->getPacsHostName() != m_SCPHostName
       || pacs_configuration->getPacsApplicationTitle() != m_SCPAppEntityTitle
       || pacs_configuration->getPacsApplicationPort() != m_SCPPort
       || pacs_configuration->getMoveApplicationTitle() != m_moveAppEntityTitle
       || pacs_configuration->getMoveApplicationPort() != m_movePort
       || pacs_configuration->getRetrieveMethod() != m_retrieveMethod)
    {
        m_SCUAppEntityTitle  = pacs_configuration->getLocalApplicationTitle();
        m_SCPHostName        = pacs_configuration->getPacsHostName();
        m_SCPAppEntityTitle  = pacs_configuration->getPacsApplicationTitle();
        m_SCPPort            = pacs_configuration->getPacsApplicationPort();
        m_moveAppEntityTitle = pacs_configuration->getMoveApplicationTitle();
        m_movePort           = pacs_configuration->getMoveApplicationPort();
        m_retrieveMethod     = pacs_configuration->getRetrieveMethod();
    }

    // If a preference key is set, save the local var to the preferences
    if(!m_preferenceKey.empty())
    {
        try
        {
            ui::preferences preferences;

            preferences.put(getKey(s_LocalApplicationTitle), m_SCUAppEntityTitle);
            preferences.put(getKey(s_PacsHostName), m_SCPHostName);
            preferences.put(getKey(s_PacsApplicationTitle), m_SCPAppEntityTitle);
            preferences.put(getKey(s_PacsApplicationPort), m_SCPPort);
            preferences.put(getKey(s_MoveApplicationTitle), m_moveAppEntityTitle);
            preferences.put(getKey(s_MoveApplicationPort), m_movePort);
            preferences.put(getKey(s_RetrieveMethod), static_cast<int>(m_retrieveMethod));
        }
        catch(const ui::preferences_disabled& /*e*/)
        {
            // Nothing to do..
        }
    }
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::stopping()
{
    this->updating();
}

} // namespace sight::module::io::dimse
