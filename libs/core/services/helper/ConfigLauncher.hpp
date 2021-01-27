/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "services/AppConfigManager.hpp"
#include "services/config.hpp"
#include "services/IService.hpp"
#include "services/registry/AppConfig.hpp"

#include <activities/registry/Activities.hpp>

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/tools/Failed.hpp>

namespace sight::services
{

namespace helper
{

/**
 * @brief This class provides few methods to manage AppConfig (parsing, starting, stopping...).
 *
 * @code{.xml}
    <service type="::sight::services::SConfigController" >
        <appConfig id="IdOfConfig" />
        <inout key="object" uid="..." />
        <parameter replace="channel" by="changeValueChannel"  />
        <parameter replace="service" by="serviceUid" />
    </service>
   @endcode
 */
class SERVICES_CLASS_API ConfigLauncher
{

public:

    typedef std::unique_ptr<ConfigLauncher> uptr;
    typedef services::registry::FieldAdaptorType FieldAdaptorType;

    /// Initializes member.
    SERVICES_API ConfigLauncher();

    /// Does nothing.
    SERVICES_API virtual ~ConfigLauncher();

    /**
     * @brief Parses a configuration.
     * @param _config The config to parse.
     * @param _service Related service.
     */
    SERVICES_API virtual void parseConfig(const services::IService::ConfigType& _config,
                                          const services::IService::sptr& _service);

    /**
     * @brief Launches Appconfig.
     * @param _srv  service to connect with config root object.
     * @param _optReplaceMap optional replace map used to replace patterns (concatenated with parsed parameter).
     */
    SERVICES_API virtual void startConfig( services::IService::sptr _srv,
                                           const FieldAdaptorType& _optReplaceMap = FieldAdaptorType() );

    /// Stops/destroys AppConfig and disconnect connection with config root object.
    SERVICES_API virtual void stopConfig();

    /**
     * @brief Gets the running status of the configuration.
     * @return True if the configuration is running.
     */
    virtual bool configIsRunning() const
    {
        return m_configIsRunning;
    }

private:

    /// Stores the app config.
    activities::registry::ActivityAppConfig m_appConfig;

    /// Sets the configuration running state.
    bool m_configIsRunning;

    /// Stores the config manager.
    services::AppConfigManager::sptr m_appConfigManager;

    /// Defines a special key to defines the associated object him self.
    static const std::string s_SELF_KEY;

    /// Defines a special key to defines the generated uid.
    static const std::string s_GENERIC_UID_KEY;

    /// Stores key and uid of optional inputs.
    std::map<std::string, std::string> m_optionalInputs;

};

} // helper
} // fwServices
