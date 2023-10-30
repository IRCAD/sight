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

#include "app/config.hpp"
#include "app/extension/config.hpp"

#include <core/tools/object.hpp>

#include <data/composite.hpp>

#include <service/manager.hpp>

namespace sight::app
{

/**
 * @brief This class provides an interface to manage configurations template.
 * @deprecated This class is no longer supported, use app::config_manager.
 */
class APP_CLASS_API config_manager : public core::tools::object,
                                     public service::manager
{
public:

    using config_t = boost::property_tree::ptree;

    SIGHT_DECLARE_CLASS(config_manager, core::tools::object);

    /// Destructor. Do nothing.
    APP_API ~config_manager() override = default;

    /// Return a new config_manager implementation. Should be used for all the config_manager of the
    /// application,
    /// except the first one which must be explicitly called.
    APP_API static SPTR(config_manager) make();

    /// Return state
    bool is_created() const;
    bool started() const;
    bool stopped() const;
    bool is_destroyed() const;

    /// Set configuration
    void set_config(const config_t& _cfg);

    /**
     * @brief Set configuration
     * @param configId the identifier of the requested config.
     * @param replaceFields the associations between the value and the pattern to replace in the config.
     * @param autoPrefixId prefix every id with the name of the configuration .
     */
    APP_API virtual void set_config(
        const std::string& _config_id,
        const field_adaptor_t& _replace_fields = field_adaptor_t(),
        bool _auto_prefix_id                   = true
    )                                          = 0;

    /// Get config root
    APP_API virtual data::object::sptr get_config_root() const = 0;

    /// Calls methods : create, start then update.
    APP_API virtual void launch() = 0;

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    APP_API virtual void stop_and_destroy() = 0;

    /// Creates objects and services from config
    APP_API virtual void create() = 0;

    /// Starts services specified in config
    APP_API virtual void start() = 0;

    /// Updates services specified in config
    APP_API virtual void update() = 0;

    /// Stops services specified in config
    APP_API virtual void stop() = 0;

    /// Destroys services specified in config
    APP_API virtual void destroy() = 0;

protected:

    /// Constructor. Does nothing.
    APP_API config_manager() = default;

    enum config_state
    {
        state_created,
        state_started,
        state_stopped,
        state_destroyed
    };

    /// XML Configuration tree
    core::runtime::config_t m_cfg_elem;

    /// Running state of the app config manager
    config_state m_state {state_destroyed};
};

//------------------------------------------------------------------------------

inline bool config_manager::is_created() const
{
    return m_state == state_created;
}

//------------------------------------------------------------------------------

inline bool config_manager::started() const
{
    return m_state == state_started;
}

//------------------------------------------------------------------------------

inline bool config_manager::stopped() const
{
    return m_state == state_stopped;
}

//------------------------------------------------------------------------------

inline bool config_manager::is_destroyed() const
{
    return m_state == state_destroyed;
}

//------------------------------------------------------------------------------

inline void config_manager::set_config(const config_t& _cfg)
{
    m_cfg_elem = _cfg;
}

//------------------------------------------------------------------------------

} // namespace sight::app
