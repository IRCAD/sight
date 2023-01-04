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

#pragma once

#include "service/config.hpp"
#include "service/factory/new.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/Slot.hpp>
#include <core/tools/Object.hpp>

#include <data/mt/locked_ptr.hpp>
#include <data/ptr.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::service
{

namespace detail
{

class AppConfigManager;
class Service;

}

namespace helper
{

class Config;

}

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
 * - \b notified(NotificationType _type, std::string _message): Emitted when the service wants to pop a notification.
 *
 * @section Slots Slots
 * - \b start() : Start the service.
 * - \b update() : Update the service.
 * - \b stop() : Stop the service.
 * - \b swapKey(const KeyType&, data::Object::sptr) : Swap the object at the given key with the object in parameter.
 */
class SERVICE_CLASS_API IService : public core::tools::Object,
                                   public core::com::HasSlots,
                                   public core::com::HasSignals,
                                   public data::IHasData
{
public:

    SIGHT_DECLARE_SERVICE(IService, core::tools::Object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    using config_t   = boost::property_tree::ptree;
    using ConfigType = config_t; // For backwards compatibility

    using IdType  = std::string;
    using KeyType = std::string;

    friend class detail::AppConfigManager;
    friend class helper::Config;

    /**
     * @name Definition of service status
     */

    //@{

    /// Defines all possible global status for a service, including transitions
    typedef enum
    {
        STARTED,
        /**< state after start */
        STARTING,
        /**< state during start */
        SWAPPING,
        /**< state during swap */
        STOPPED,
        /**< state after stop */
        STOPPING /**< state during stop */
    } GlobalStatus;

    /// Defines all possible status for an update process
    typedef enum
    {
        UPDATING,
        /**< state during update */
        NOTUPDATING /**< state during when the service is not updating */
    } UpdatingStatus;

    /// Defines all possible status for a configuration process
    typedef enum
    {
        CONFIGURING,
        /**< state during configuration */
        CONFIGURED,
        /**< state after configuration */
        UNCONFIGURED /**< state when the service is not configured */
    } ConfigurationStatus;

    //@}

    /// Defines Notification type, default is INFO.
    enum class NotificationType
    {
        SUCCESS,
        INFO,
        FAILURE,
        DEFAULT = INFO
    };

    struct signals
    {
        using started_t      = core::com::Signal<void ()>;
        using stopped_t      = core::com::Signal<void ()>;
        using updated_t      = core::com::Signal<void ()>;
        using swapped_t      = core::com::Signal<void ()>;
        using notification_t = core::com::Signal<void (NotificationType, std::string)>;

        static inline const core::com::Signals::key_t s_STARTED  = "started";
        static inline const core::com::Signals::key_t s_UPDATED  = "updated";
        static inline const core::com::Signals::key_t s_SWAPPED  = "swapped";
        static inline const core::com::Signals::key_t s_STOPPED  = "stopped";
        static inline const core::com::Signals::key_t s_NOTIFIED = "notified";
    };

    struct slots
    {
        static inline const core::com::Slots::key_t s_START   = "start";
        static inline const core::com::Slots::key_t s_STOP    = "stop";
        static inline const core::com::Slots::key_t s_UPDATE  = "update";
        static inline const core::com::Slots::key_t s_SWAPKEY = "swapKey";
    };

    /**
     * @name Worker API
     */

    //@{
    using SharedFutureType = std::shared_future<void>;

    /// Sets a worker to all service slots
    SERVICE_API void setWorker(SPTR(core::thread::Worker) worker);
    SERVICE_API SPTR(core::thread::Worker) worker() const;
    //@}

    /**
     * @name All concerning configuration
     */

    //@{

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] ptree property tree
     * @post m_configurationState == UNCONFIGURED
     */
    SERVICE_API void setConfiguration(const config_t& ptree);

    /// Return the service configuration
    SERVICE_API const IService::ConfigType& getConfiguration() const;
    [[deprecated("Removed in sight 23.0.")]] SERVICE_API const IService::ConfigType& getConfigTree() const;

    //@}

    /**
     * @name State management API
     */

    //@{

    /**
     * @brief Set configuration and then invoke configuring() if m_globalState == STOPPED
     * @post m_configurationState == CONFIGURED
     * @param[in] ptree property tree
     */
    SERVICE_API void configure(const config_t& ptree);

    /**
     * @brief Invoke configuring() if m_globalState == STOPPED. Does nothing otherwise.
     * @pre m_configurationState == UNCONFIGURED
     * @post m_configurationState == CONFIGURED
     */
    SERVICE_API void configure();

    /**
     * @brief Invoke starting() if m_globalState == STOPPED. Does nothing otherwise.
     * @post m_globalState == STARTED
     */
    SERVICE_API SharedFutureType start();

    /**
     * @brief Invoke stopping() if m_globalState == STARTED. Does nothing otherwise. Stops all observations.
     *
     * @post m_globalState == STOPPED
     *
     */
    SERVICE_API SharedFutureType stop();

    /**
     * @brief Invoke updating() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_globalState == STARTED
     */
    SERVICE_API SharedFutureType update();

    /**
     * @brief Associate the service to another object
     * @param[in] _key key of the object
     * @param[in] _obj change object at given key to _obj
     * @pre m_globalState == STARTED
     */
    SERVICE_API SharedFutureType swapKey(std::string_view _key, data::Object::sptr _obj);
    //@}

    /**
     * @name All concerning status access
     */

    //@{

    /// Return the global process status
    SERVICE_API GlobalStatus getStatus() const noexcept;

    /// Return the configuration process status
    SERVICE_API ConfigurationStatus getConfigurationStatus() const noexcept;

    /// Test if the service is started or not
    SERVICE_API bool isStarted() const noexcept;

    /// Test if the service is stopped or not
    SERVICE_API bool isStopped() const noexcept;

    /// Return the update process status
    SERVICE_API UpdatingStatus getUpdatingStatus() const noexcept;

    //@}

    /**
     * @name Misc
     */

    //@{

    /**
     * @brief Streaming a service
     * @see IService::operator<<(std::ostream & _ostream, IService& _service)
     * @note Invoke IService::info( std::ostream )
     */
    SERVICE_API friend std::ostream& operator<<(std::ostream& _sstream, IService& _service);

    /**
     * @brief Emits notification signal with 'message' and Notification 'type' provided
     *
     * @param[in] type type of the notification to emit @see Notification enum class.
     * @param[in] message message as std::string.
     */
    SERVICE_API void notify(NotificationType type, const std::string& message) const;
    //@}

protected:

    SERVICE_API IService();
    SERVICE_API ~IService() override;

    /**
     * @name Interface to override
     */

    //@{

    /// Implementation of the service initialization. For example : installs a button in a frame and show the frame.
    SERVICE_API virtual void starting() = 0;

    /// Implementation of the service deinitialization. Always invoked before destroying a service.
    SERVICE_API virtual void stopping() = 0;

    /// Configures the service before starting. Apply the configuration to service.
    SERVICE_API virtual void configuring();

    /// Configures the service before starting. Apply the configuration to service.
    SERVICE_API virtual void configuring(const config_t&);

    /// Performs some processing.
    SERVICE_API virtual void updating() = 0;

    /**
     * @brief Swap the service from an associated object to another object.
     * The key in parameter indicates allows to retrieve the new data with getInput(), getInOut() or getOutput().
     * If you need the old object, you need to keep a shared pointer on it inside your service implementation.
     *
     * @param key of the object
     * @see swapKey()
     * @todo This method must be pure virtual
     * @todo This method must have in parameter the new object or the old ?
     */
    virtual void swapping(std::string_view /*unused*/);

    /// Helper to define the connections between a service and its data.
    class SERVICE_CLASS_API KeyConnectionsMap
    {
    public:

        using KeyConnectionPairType = std::pair<core::com::Signals::SignalKeyType, core::com::Slots::SlotKeyType>;
        using KeyConnectionsType    = std::vector<KeyConnectionPairType>;
        using KeyConnectionsMapType = std::map<std::string_view, KeyConnectionsType>;

        KeyConnectionsMap() = default;
        SERVICE_API KeyConnectionsMap(
            std::initializer_list<std::tuple<const std::string_view, core::com::Signals::key_t,
                                             core::com::Slots::key_t> > init
        );

        SERVICE_API void push(
            std::string_view key,
            const core::com::Signals::key_t& sig,
            const core::com::Slots::key_t& slot
        );
        [[nodiscard]] SERVICE_API KeyConnectionsMapType::const_iterator find(std::string_view key) const;
        [[nodiscard]] SERVICE_API KeyConnectionsMapType::const_iterator end() const;
        [[nodiscard]] SERVICE_API bool empty() const;
        [[nodiscard]] SERVICE_API std::size_t size() const;

    private:

        KeyConnectionsMapType m_keyConnectionsMap;
    };

    /**
     * @brief Returns proposals to connect service slots to associated objects signals,
     * this method is used for obj/srv auto connection
     */
    SERVICE_API virtual KeyConnectionsMap getAutoConnections() const;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, IService& _service)
     * to avoid declaration of << by all services.
     */
    SERVICE_API virtual void info(std::ostream& _sstream);
    //@}

private:

    SERVICE_API void notifyRegisterOut(data::Object::sptr, const std::string&) override;
    SERVICE_API void notifyUnregisterOut(data::Object::sptr, const std::string&) override;

    friend class detail::Service;
    std::unique_ptr<detail::Service> m_pimpl;
};

//------------------------------------------------------------------------------

inline void IService::configuring()
{
}

//------------------------------------------------------------------------------

inline void IService::configuring(const config_t&)
{
}

//------------------------------------------------------------------------------

inline void IService::swapping(std::string_view /*unused*/)
{
}

} // namespace sight::service
