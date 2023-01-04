/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "service/config.hpp"
#include "service/extension/AppConfig.hpp"

#include <core/tools/Object.hpp>

#include <data/Composite.hpp>

namespace sight::service
{

/**
 * @brief This class provides an interface to manage configurations template.
 * @deprecated This class is no longer supported, use service::AppConfigManager.
 */
class SERVICE_CLASS_API IAppConfigManager : public core::tools::Object
{
public:

    using config_t = boost::property_tree::ptree;

    SIGHT_DECLARE_CLASS(IAppConfigManager, core::tools::Object);

    /// Destructor. Do nothing.
    SERVICE_API ~IAppConfigManager() override = default;

    /// Return a new AppConfigManager implementation. Should be used for all the IAppConfigManager of the application,
    /// except the first one which must be explicitly called.
    SERVICE_API static SPTR(IAppConfigManager) New();

    /// Return state
    bool isCreated() const;
    bool isStarted() const;
    bool isStopped() const;
    bool isDestroyed() const;

    /// Set configuration
    void setConfig(const config_t& cfg);

    /**
     * @brief Set configuration
     * @param configId the identifier of the requested config.
     * @param replaceFields the associations between the value and the pattern to replace in the config.
     * @param autoPrefixId prefix every id with the name of the configuration .
     */
    SERVICE_API virtual void setConfig(
        const std::string& configId,
        const FieldAdaptorType& replaceFields = FieldAdaptorType(),
        bool autoPrefixId                     = true
    )                                         = 0;

    /// Get config root
    SERVICE_API virtual data::Object::sptr getConfigRoot() const = 0;

    /// Calls methods : create, start then update.
    SERVICE_API virtual void launch() = 0;

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    SERVICE_API virtual void stopAndDestroy() = 0;

    /// Creates objects and services from config
    SERVICE_API virtual void create() = 0;

    /// Starts services specified in config
    SERVICE_API virtual void start() = 0;

    /// Updates services specified in config
    SERVICE_API virtual void update() = 0;

    /// Stops services specified in config
    SERVICE_API virtual void stop() = 0;

    /// Destroys services specified in config
    SERVICE_API virtual void destroy() = 0;

protected:

    /// Constructor. Does nothing.
    SERVICE_API IAppConfigManager() = default;

    enum ConfigState
    {
        STATE_CREATED,
        STATE_STARTED,
        STATE_STOPPED,
        STATE_DESTROYED
    };

    /// XML Configuration tree
    core::runtime::config_t m_cfgElem;

    /// Running state of the app config manager
    ConfigState m_state {STATE_DESTROYED};
};

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isCreated() const
{
    return m_state == STATE_CREATED;
}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isStarted() const
{
    return m_state == STATE_STARTED;
}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isStopped() const
{
    return m_state == STATE_STOPPED;
}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isDestroyed() const
{
    return m_state == STATE_DESTROYED;
}

//------------------------------------------------------------------------------

inline void IAppConfigManager::setConfig(const config_t& cfg)
{
    m_cfgElem = cfg;
}

//------------------------------------------------------------------------------

} // namespace sight::service
