/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwServices/AppConfigManager.hpp"
#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwTools/Failed.hpp>

namespace fwServices
{

namespace helper
{

/**
 * @brief This class provides few methods to manage AppConfig (parsing, starting, stopping...).
 *
 * @code{.xml}
    <service type="::fwServices::SConfigController" >
        <appConfig id="IdOfConfig" />
        <inout key="object" uid="..." />
        <parameter replace="channel" by="changeValueChannel"  />
        <parameter replace="service" by="serviceUid" />
    </service>
   @endcode
 */
class FWSERVICES_CLASS_API ConfigLauncher
{

public:

    typedef std::unique_ptr<ConfigLauncher> uptr;
    typedef ::fwServices::registry::FieldAdaptorType FieldAdaptorType;

    /// Initializes member.
    FWSERVICES_API ConfigLauncher();

    /// Does nothing.
    FWSERVICES_API virtual ~ConfigLauncher();

    /**
     * @brief Parses a configuration.
     * @param _config The config to parse.
     * @param _service Related service.
     */
    FWSERVICES_API virtual void parseConfig(const ::fwServices::IService::ConfigType& _config,
                                            const ::fwServices::IService::sptr& _service);

    /**
     * @brief Launches Appconfig.
     * @param _srv  service to connect with config root object.
     * @param _optReplaceMap optional replace map used to replace patterns (concatenated with parsed parameter).
     */
    FWSERVICES_API virtual void startConfig( ::fwServices::IService::sptr _srv,
                                             const FieldAdaptorType& _optReplaceMap = FieldAdaptorType() );

    /// Stops/destroys AppConfig and disconnect connection with config root object.
    FWSERVICES_API virtual void stopConfig();

    /**
     * @brief Gets the running status of the configuration.
     * @return True if the configuration is running.
     */
    virtual bool configIsRunning() const
    {
        return m_configIsRunning;
    }

private:

    /// App config.
    ::fwActivities::registry::ActivityAppConfig m_appConfig;

    /// To know if configuration is running.
    bool m_configIsRunning;

    /// Config manager.
    ::fwServices::AppConfigManager::sptr m_appConfigManager;

    /// Defines a special key to defines the associated object him self.
    static const std::string s_SELF_KEY;

    /// Defines a special key to defines the generated uid.
    static const std::string s_GENERIC_UID_KEY;

    /// Stores key and uid of optional inputs.
    std::map<std::string, std::string> m_optionalInputs;

};

} // helper
} // fwServices
