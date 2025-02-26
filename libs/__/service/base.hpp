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

#pragma once

#include <sight/service/config.hpp>

#include "service/factory/new.hpp"

#include <core/com/has_signals.hpp>
#include <core/com/has_slots.hpp>
#include <core/com/slot.hpp>
#include <core/object.hpp>

#include <data/map.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/ptr.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::service
{

class base;
class manager;

namespace detail
{

class service;

}

using config_t = boost::property_tree::ptree;

struct slots
{
    static inline const core::com::slots::key_t START    = "start";
    static inline const core::com::slots::key_t STOP     = "stop";
    static inline const core::com::slots::key_t UPDATE   = "update";
    static inline const core::com::slots::key_t SWAP_KEY = "swap_key";
};

struct signals
{
    using started_t = core::com::signal<void (wptr<base>)>;
    using stopped_t = core::com::signal<void (wptr<base>)>;
    using updated_t = core::com::signal<void (wptr<base>)>;
    using swapped_t = core::com::signal<void (wptr<base>)>;

    static inline const core::com::signals::key_t STARTED = "started";
    static inline const core::com::signals::key_t UPDATED = "updated";
    static inline const core::com::signals::key_t SWAPPED = "swapped";
    static inline const core::com::signals::key_t STOPPED = "stopped";
};

/// Helper to define the connections between a service and its data.
struct SIGHT_SERVICE_CLASS_API connections_t
{
    using key_connection_pair_t = std::pair<core::com::signals::key_t, core::com::slots::key_t>;
    using key_connections_t     = std::vector<key_connection_pair_t>;
    using key_connections_map_t = std::map<std::string_view, key_connections_t>;

    connections_t() = default;
    SIGHT_SERVICE_API connections_t(
        std::initializer_list<std::tuple<const std::string_view, core::com::signals::key_t,
                                         core::com::slots::key_t> > _init
    );

    SIGHT_SERVICE_API void push(
        std::string_view _key,
        const core::com::signals::key_t& _sig,
        const core::com::slots::key_t& _slot
    );
    [[nodiscard]] SIGHT_SERVICE_API bool contains(std::string_view _key) const;
    [[nodiscard]] SIGHT_SERVICE_API key_connections_map_t::const_iterator find(std::string_view _key) const;
    [[nodiscard]] SIGHT_SERVICE_API key_connections_map_t::const_iterator end() const;
    [[nodiscard]] SIGHT_SERVICE_API bool empty() const;
    [[nodiscard]] SIGHT_SERVICE_API std::size_t size() const;
    SIGHT_SERVICE_API connections_t operator+(const connections_t& _other) const;

    private:

        key_connections_map_t m_key_connections_map;
};

/**
 * @brief   Base class for all services.
 *
 * This class defines the API to use and declare services.
 * The service state aims to impose the execution order (i.e. configure(), start(), update() or, stop()).
 *
 * @section Signals Signals
 * - \b started() : Emitted when the service has started.
 * - \b updated() : Emitted when the service has updated.
 * - \b stopped() : Emitted when the service has stopped.
 * - \b swapped() : Emitted when the service has swapped.
 *
 * @section Slots Slots
 * - \b start() : Start the service.
 * - \b update() : Update the service.
 * - \b stop() : Stop the service.
 * - \b swap_key(const key_t&, data::object::sptr) : Swap the object at the given key with the object in parameter.
 */
class SIGHT_SERVICE_CLASS_API base : public core::object,
                                     public core::com::has_slots,
                                     public core::com::has_signals,
                                     public data::has_data
{
public:

    SIGHT_DECLARE_CLASS(base, core::object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    using config_t = service::config_t;
    using key_t    = std::string;

    using connections_t = service::connections_t;
    using slots         = service::slots;
    using signals       = service::signals;

    /**
     * @name Definition of service status
     */

    //@{

    /// Defines all possible global status for a service, including transitions
    enum class global_status
    {
        started,
        /**< state after start */
        starting,
        /**< state during start */
        swapping,
        /**< state during swap */
        stopped,
        /**< state after stop */
        stopping /**< state during stop */
    };

    /// Defines all possible status for an update process
    enum class updating_status
    {
        updating,
        /**< state during update */
        notupdating /**< state during when the service is not updating */
    };

    /// Defines all possible status for a configuration process
    enum class configuration_status
    {
        configuring,
        /**< state during configuration */
        configured,
        /**< state after configuration */
        unconfigured /**< state when the service is not configured */
    };

    //@}

    /**
     * @name Worker API
     */

    //@{
    using shared_future_t = std::shared_future<void>;

    /// Sets a worker to all service slots
    SIGHT_SERVICE_API void set_worker(SPTR(core::thread::worker) _worker);
    SIGHT_SERVICE_API SPTR(core::thread::worker) worker() const;
    //@}

    /**
     * @name All concerning configuration
     */

    //@{

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] _ptree property tree
     * @post m_configurationState == UNCONFIGURED
     */
    SIGHT_SERVICE_API void set_config(const config_t& _ptree);

    /// Return the service configuration
    SIGHT_SERVICE_API const base::config_t& get_config() const;

    //@}

    /**
     * @name State management API
     */

    //@{

    /**
     * @brief Set configuration and then invoke configuring() if m_globalState == STOPPED
     * @post m_configurationState == CONFIGURED
     * @param[in] _ptree property tree
     */
    SIGHT_SERVICE_API void configure(const config_t& _ptree);

    /**
     * @brief Invoke configuring() if m_globalState == STOPPED. Does nothing otherwise.
     * @pre m_configurationState == UNCONFIGURED
     * @post m_configurationState == CONFIGURED
     */
    SIGHT_SERVICE_API void configure();

    /**
     * @brief Invoke starting() if m_globalState == STOPPED. Does nothing otherwise.
     * @post m_globalState == STARTED
     */
    SIGHT_SERVICE_API shared_future_t start();

    /**
     * @brief Invoke stopping() if m_globalState == STARTED. Does nothing otherwise. Stops all observations.
     *
     * @post m_globalState == STOPPED
     *
     */
    SIGHT_SERVICE_API shared_future_t stop();

    /**
     * @brief Invoke updating() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_globalState == STARTED
     */
    SIGHT_SERVICE_API shared_future_t update();

    /**
     * @brief Associate the service to another object
     * @param[in] _key key of the object
     * @param[in] _obj change object at given key to _obj
     * @pre m_globalState == STARTED
     */
    SIGHT_SERVICE_API shared_future_t swap_key(std::string_view _key, data::object::sptr _obj);
    //@}

    /**
     * @name All concerning status access
     */

    //@{

    /// Return the global process status
    SIGHT_SERVICE_API global_status status() const noexcept;

    /// Return the configuration process status
    SIGHT_SERVICE_API configuration_status config_status() const noexcept;

    /// Test if the service is started or not
    SIGHT_SERVICE_API bool started() const noexcept;

    /// Test if the service is stopped or not
    SIGHT_SERVICE_API bool stopped() const noexcept;

    /// Return the update process status
    SIGHT_SERVICE_API updating_status updating_status() const noexcept;

    //@}

    /// Returns a boolean to indicate if the service is autoconnected
    SIGHT_SERVICE_API bool is_auto_connected() const;

    /**
     * @name Misc
     */

    //@{

    /**
     * @brief Streaming a service
     * @see base::operator<<(std::ostream & _ostream, base& _service)
     * @note Invoke base::info( std::ostream )
     */
    SIGHT_SERVICE_API friend std::ostream& operator<<(std::ostream& _sstream, base& _service);
    //@}

protected:

    SIGHT_SERVICE_API base();
    SIGHT_SERVICE_API ~base() override;

    /**
     * @name Interface to override
     */

    //@{

    /// Implementation of the service initialization. For example : installs a button in a frame and show the frame.
    SIGHT_SERVICE_API virtual void starting() = 0;

    /// Implementation of the service deinitialization. Always invoked before destroying a service.
    SIGHT_SERVICE_API virtual void stopping() = 0;

    /// Configures the service before starting. Apply the configuration to service.
    SIGHT_SERVICE_API virtual void configuring();

    /// Configures the service before starting. Apply the configuration to service.
    SIGHT_SERVICE_API virtual void configuring(const config_t&);

    /// Performs some processing.
    SIGHT_SERVICE_API virtual void updating() = 0;

    /**
     * @brief Swap the service from an associated object to another object.
     * The key in parameter indicates allows to retrieve the new data with input(), inout() or output().
     * If you need the old object, you need to keep a shared pointer on it inside your service implementation.
     *
     * @see swap_key()
     * @todo This method must be pure virtual
     * @todo This method must have in parameter the new object or the old ?
     */
    virtual void swapping(std::string_view /*unused*/);

    /**
     * @brief Returns proposals to connect service slots to associated objects signals,
     * this method is used for obj/srv auto connection
     */
    SIGHT_SERVICE_API virtual connections_t auto_connections() const;

    /**
     * @brief Called when a property is modified, only if no auto connection is provided
     */
    SIGHT_SERVICE_API virtual void on_property_set(std::string_view);

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, base& _service)
     * to avoid declaration of << by all services.
     */
    SIGHT_SERVICE_API virtual void info(std::ostream& _sstream);
    //@}

private:

    /// Notify about a newly deferred object
    SIGHT_SERVICE_API void notify_register_out(data::object::sptr, const std::string&) override;

    /// Notify about a destroyed deferred object
    SIGHT_SERVICE_API void notify_unregister_out(data::object::sptr, const std::string&) override;

    friend class manager;
    friend class detail::service;
    std::unique_ptr<detail::service> m_pimpl;

    // Object storing properties when passed as a map instead of individual objects
    sight::data::ptr<sight::data::map, sight::data::access::inout> m_properties_map {this, "from"};
};

//------------------------------------------------------------------------------

inline void base::configuring()
{
}

//------------------------------------------------------------------------------

inline void base::configuring(const config_t&)
{
}

//------------------------------------------------------------------------------

inline void base::swapping(std::string_view /*unused*/)
{
}

} // namespace sight::service
