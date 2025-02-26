/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <data/map.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/__/preferences.hpp>

#include <utility>

namespace sight::module::io::dimse
{

static const std::string LOCAL_APPLICATION_TITLE("localApplicationTitle");
static const std::string PACS_HOST_NAME("pacsHostName");
static const std::string PACS_APPLICATION_TITLE("pacsApplicationTitle");
static const std::string PACS_APPLICATION_PORT("pacsApplicationPort");
static const std::string MOVE_APPLICATION_TITLE("moveApplicationTitle");
static const std::string MOVE_APPLICATION_PORT("moveApplicationPort");
static const std::string RETRIEVE_METHOD("retrieveMethod");

//------------------------------------------------------------------------------

pacs_configuration_initializer::pacs_configuration_initializer() noexcept =
    default;

//------------------------------------------------------------------------------

pacs_configuration_initializer::~pacs_configuration_initializer() noexcept =
    default;

//------------------------------------------------------------------------------

std::string pacs_configuration_initializer::get_key(const std::string& _sub_key) const noexcept
{
    return m_preference_key + "." + _sub_key;
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::configuring()
{
    const config_t config_tree = this->get_config();
    const auto config          = config_tree.get_child("config.<xmlattr>");

    // Local application title.
    m_scu_app_entity_title = config.get<std::string>(LOCAL_APPLICATION_TITLE);

    // Pacs host name.
    m_scp_host_name = config.get<std::string>(PACS_HOST_NAME);

    // Pacs application title.
    m_scp_app_entity_title = config.get<std::string>(PACS_APPLICATION_TITLE);

    // Pacs port.
    const auto pacs_application_port = config.get<std::string>(PACS_APPLICATION_PORT);
    m_scp_port = static_cast<decltype(m_scp_port)>(std::stoi(pacs_application_port));

    // Retrieve Method.
    const auto retrieve_method = config.get<std::string>(RETRIEVE_METHOD);
    m_retrieve_method = (retrieve_method == "MOVE")
                        ? (sight::io::dimse::data::pacs_configuration::retrieve_method::move)
                        : (sight::io::dimse::data::pacs_configuration::retrieve_method::get);

    // Move application title.
    m_move_app_entity_title = config.get<std::string>(MOVE_APPLICATION_TITLE);

    // Move application port.
    const auto move_application_port = config.get<std::string>(MOVE_APPLICATION_PORT);
    m_move_port = static_cast<decltype(m_move_port)>(std::stoi(move_application_port));

    // Preference Key.
    m_preference_key = config.get<std::string>("preferenceKey", m_preference_key);
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::starting()
{
    const auto pacs_configuration = m_config.lock();
    SIGHT_ASSERT("inout '" << CONFIG_INOUT << "' does not exist.", pacs_configuration);

    // Set information from xml and update PacsConfiguration.
    if(!m_preference_key.empty())
    {
        try
        {
            ui::preferences preferences;

            m_scu_app_entity_title  = preferences.get(get_key(LOCAL_APPLICATION_TITLE), m_scu_app_entity_title);
            m_scp_host_name         = preferences.get(get_key(PACS_HOST_NAME), m_scp_host_name);
            m_scp_app_entity_title  = preferences.get(get_key(PACS_APPLICATION_TITLE), m_scp_app_entity_title);
            m_scp_port              = preferences.get(get_key(PACS_APPLICATION_PORT), m_scp_port);
            m_move_app_entity_title = preferences.get(get_key(MOVE_APPLICATION_TITLE), m_move_app_entity_title);
            m_move_port             = preferences.get(get_key(MOVE_APPLICATION_PORT), m_move_port);
            m_retrieve_method       = static_cast<sight::io::dimse::data::pacs_configuration::retrieve_method>(
                preferences.get(get_key(RETRIEVE_METHOD), static_cast<int>(m_retrieve_method))
            );
        }
        catch(const ui::preferences_disabled& /*e*/)
        {
            // Nothing to do..
        }
    }

    pacs_configuration->set_local_application_title(m_scu_app_entity_title);
    pacs_configuration->set_pacs_host_name(m_scp_host_name);
    pacs_configuration->set_pacs_application_title(m_scp_app_entity_title);
    pacs_configuration->set_pacs_application_port(m_scp_port);
    pacs_configuration->set_move_application_title(m_move_app_entity_title);
    pacs_configuration->set_move_application_port(m_move_port);
    pacs_configuration->set_retrieve_method(m_retrieve_method);
}

//------------------------------------------------------------------------------

service::connections_t pacs_configuration_initializer::auto_connections() const
{
    service::connections_t connections;

    connections.push(
        CONFIG_INOUT,
        sight::io::dimse::data::pacs_configuration::MODIFIED_SIG,
        service::slots::UPDATE
    );

    return connections;
}

//------------------------------------------------------------------------------

void pacs_configuration_initializer::updating()
{
    const auto pacs_configuration = m_config.lock();
    SIGHT_ASSERT("inout '" << CONFIG_INOUT << "' does not exist.", pacs_configuration);

    // Check if the user has changed the Pacs configuration and update the local var
    if(pacs_configuration->get_local_application_title() != m_scu_app_entity_title
       || pacs_configuration->get_pacs_host_name() != m_scp_host_name
       || pacs_configuration->get_pacs_application_title() != m_scp_app_entity_title
       || pacs_configuration->get_pacs_application_port() != m_scp_port
       || pacs_configuration->get_move_application_title() != m_move_app_entity_title
       || pacs_configuration->get_move_application_port() != m_move_port
       || pacs_configuration->get_retrieve_method() != m_retrieve_method)
    {
        m_scu_app_entity_title  = pacs_configuration->get_local_application_title();
        m_scp_host_name         = pacs_configuration->get_pacs_host_name();
        m_scp_app_entity_title  = pacs_configuration->get_pacs_application_title();
        m_scp_port              = pacs_configuration->get_pacs_application_port();
        m_move_app_entity_title = pacs_configuration->get_move_application_title();
        m_move_port             = pacs_configuration->get_move_application_port();
        m_retrieve_method       = pacs_configuration->get_retrieve_method();
    }

    // If a preference key is set, save the local var to the preferences
    if(!m_preference_key.empty())
    {
        try
        {
            ui::preferences preferences;

            preferences.put(get_key(LOCAL_APPLICATION_TITLE), m_scu_app_entity_title);
            preferences.put(get_key(PACS_HOST_NAME), m_scp_host_name);
            preferences.put(get_key(PACS_APPLICATION_TITLE), m_scp_app_entity_title);
            preferences.put(get_key(PACS_APPLICATION_PORT), m_scp_port);
            preferences.put(get_key(MOVE_APPLICATION_TITLE), m_move_app_entity_title);
            preferences.put(get_key(MOVE_APPLICATION_PORT), m_move_port);
            preferences.put(get_key(RETRIEVE_METHOD), static_cast<int>(m_retrieve_method));
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
