/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <sight/app/config.hpp>

#include "app/config_manager.hpp"

#include "service/base.hpp"
#include "service/extension/config.hpp"

#include <core/tools/failed.hpp>

namespace sight::app::helper
{

/**
 * @brief This class provides few methods to manage config (parsing, starting, stopping...).
 */
class SIGHT_APP_CLASS_API config_launcher final
{
public:

    using uptr = std::unique_ptr<config_launcher>;
    static constexpr std::string_view DATA_GROUP = "data";

    /**
     * @brief Parses a configuration.
     * @param _config The config to parse.
     * @param _service Related service.
     */
    SIGHT_APP_API void parse_config(
        const service::config_t& _config,
        const service::base::sptr& _service
    );

    /**
     * @brief Launches Appconfig.
     * @param _srv  service to connect with config root object.
     * @param _opt_replace_map optional replace map used to replace patterns (concatenated with parsed parameter).
     */
    SIGHT_APP_API void start_config(
        service::base::sptr _srv,
        const field_adaptor_t& _opt_replace_map = field_adaptor_t()
    );

    /// Stops/destroys config and disconnect connection with config root object.
    SIGHT_APP_API void stop_config();

    /**
     * @brief Gets the running status of the configuration.
     * @return True if the configuration is running.
     */
    [[nodiscard]] bool config_is_running() const
    {
        return m_config_is_running;
    }

    SIGHT_APP_API void set_config(const std::string& _key);
    const std::string& config() const;

private:

    /**
     * @brief initialise a config provided in param
     *
     * @param _old_config   the current service config
     * @param _service      Service to connect with config root object.
     * @return the initialised config
     */
    service::config_t init_config(
        const service::config_t& _old_config,
        service::base::sptr _service
    );

    // map of supported configs. Each map is associated to a key, (attribute name i the xml)
    // a default config is stored.
    std::vector<std::pair<std::string, std::string> > m_parameters;
    std::string m_config_key = {};

    /// Sets the configuration running state.
    bool m_config_is_running {false};

    /// Stores the config manager.
    app::config_manager::sptr m_config_manager;

    /// Defines a special key to defines the associated object him self.
    static const std::string SELF_KEY;

    /// Stores key and uid of optional inputs.
    std::map<std::string, std::pair<std::string, size_t> > m_optional_inputs;
};

//------------------------------------------------------------------------------

inline const std::string& config_launcher::config() const
{
    return m_config_key;
}

} // namespace sight::app::helper
