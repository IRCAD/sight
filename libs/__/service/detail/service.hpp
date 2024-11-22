/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <sight/service/config.hpp>

#include "service/base.hpp"
#include "service/detail/service_connection.hpp"

#include <core/com/helper/sig_slot_connection.hpp>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::service::detail
{

/**
 * @brief   Private implementation of the public base interface, which is the base class for all services.
 *
 * This hides the concrete implementation of the state management and connections.
 */
class service final
{
public:

    using config_t        = sight::service::config_t;
    using packaged_task_t = std::packaged_task<void ()>;

    service(sight::service::base& _service);
    ~service();

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] _config property tree
     * @post m_configurationState == UNCONFIGURED
     */
    void set_config(const config_t& _config);

    /// Returns the service configuration
    const sight::service::config_t& get_config() const;

    // Configures the service
    void configure();

    // Slot: starts the service
    base::shared_future_t start(bool _async);

    // Slot: stops the service
    base::shared_future_t stop(bool _async);

    // Slot: swaps an object
    base::shared_future_t swap_key(std::string_view _key, data::object::sptr _obj, bool _async);

    // Slot: updates the service
    base::shared_future_t update(bool _async);

    /// Connects the service with its data
    void auto_connect();

    /// Disconnects the service from its data
    void auto_disconnect();

    /// Returns a boolean to indicate if the service is autoconnected
    bool is_auto_connected() const;

    /// Returns a boolean to indicate if the object at the given key is optional
    bool is_key_optional(const std::string& _key) const;

    /// Connections with data and other services, connected at start, and disconnected at stop
    service_connection m_connections;

    /// Configuration of the service
    config_t m_configuration;

    /// Associated worker
    SPTR(core::thread::worker) m_worker;

    /**
     * @brief Defines the current global status of the service.
     */
    base::global_status m_global_state {base::global_status::stopped};

    /**
     * @brief Defines if the service is updating.
     */
    enum base::updating_status m_updating_state
    {
        base::updating_status::notupdating
    };

    /**
     * @brief Defines if the service is configured or not.
     */
    base::configuration_status m_configuration_state {base::configuration_status::unconfigured};

    /// Auto connections between this service and its objects
    core::com::helper::sig_slot_connection m_auto_connections;

    /// Reference to the service
    sight::service::base& m_service;

    /// List of created properties during configuring in order to keep them alive
    using properties_t = std::vector<sight::data::object::sptr>;
    properties_t m_created_properties;

    /// Tells if the service is auto-connected or not. Could be reevaluated but normally safe to store.
    bool m_auto_connected {false};
};

} // namespace sight::service::detail
