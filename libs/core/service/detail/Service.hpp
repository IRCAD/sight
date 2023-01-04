/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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
#include "service/detail/ServiceConnection.hpp"
#include "service/IService.hpp"

#include <core/com/helper/SigSlotConnection.hpp>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::service::detail
{

/**
 * @brief   Private implementation of the public IService interface, which is the base class for all services.
 *
 * This hides the concrete implementation of the state management and connections.
 */
class Service final
{
public:

    using config_t         = IService::config_t;
    using PackagedTaskType = std::packaged_task<void ()>;

    Service(sight::service::IService& service);

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] config property tree
     * @post m_configurationState == UNCONFIGURED
     */
    void setConfiguration(const config_t& config);

    /// Returns the service configuration
    const IService::config_t& getConfiguration() const;

    // Configures the service
    void configure();

    // Slot: starts the service
    IService::SharedFutureType start(bool _async);

    // Slot: stops the service
    IService::SharedFutureType stop(bool _async);

    // Slot: swaps an object
    IService::SharedFutureType swapKey(std::string_view _key, data::Object::sptr _obj, bool _async);

    // Slot: updates the service
    IService::SharedFutureType update(bool _async);

    /// Connects the service with its data
    void autoConnect();

    /// Disconnects the service from its data
    void autoDisconnect();

    static core::com::Connection connectRegisterOut(const core::com::SlotBase::sptr& _slot);
    static core::com::Connection connectUnregisterOut(const core::com::SlotBase::sptr& _slot);

    static void notifyRegisterOut(data::Object::sptr, const std::string&);
    static void notifyUnregisterOut(data::Object::sptr, const std::string&);

    /// Returns the information about the required key
    std::pair<bool, bool> getKeyProps(const std::string& _key) const;

    /// Connections with data and other services, connected at start, and disconnected at stop
    ServiceConnection m_connections;

    /// Configuration of the service
    config_t m_configuration;

    /// Associated worker
    SPTR(core::thread::Worker) m_worker;

    /**
     * @brief Defines the current global status of the service.
     */
    IService::GlobalStatus m_globalState {IService::STOPPED};

    /**
     * @brief Defines if the service is updating.
     */
    IService::UpdatingStatus m_updatingState {IService::NOTUPDATING};

    /**
     * @brief Defines if the service is configured or not.
     */
    IService::ConfigurationStatus m_configurationState {IService::UNCONFIGURED};

    /// Auto connections between this service and its objects
    core::com::helper::SigSlotConnection m_autoConnections;

    /// Reference to the service
    sight::service::IService& m_service;
};

} // namespace sight::service::detail
