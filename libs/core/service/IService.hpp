/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "service/helper/ProxyConnections.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <data/Exception.hpp>
#include <data/Object.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/mt/weak_ptr.hpp>
#include <data/mt/shared_ptr.hpp>
#include <data/ptr.hpp>

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/helper.hpp>

#include <core/tools/Failed.hpp>
#include <core/tools/Object.hpp>

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4702) // unreachable code
#endif
#include <boost/property_tree/ptree.hpp>
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <cstdint>
#include <optional>

namespace sight::service
{

namespace registry
{

class ObjectService;

}
namespace thread
{

class Worker;

}
namespace helper
{

class Config;

}

#define KEY_GROUP_NAME(key, \
                       index) (std::string(key) + "#" + std::to_string(index))

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
 * - \b notified(NotificationType _type, std::string _message): Emitted when the service wants to pop a notification.
 *
 * @section Slots Slots
 * - \b start() : Start the service.
 * - \b update() : Update the service.
 * - \b stop() : Stop the service.
 * - \b swap() : @deprecated Swap the current object.
 * - \b swapKey(const KeyType&, data::Object::sptr) : Swap the object at the given key with the object in parameter.
 */
class SERVICE_CLASS_API IService : public core::tools::Object,
                                   public core::com::HasSlots,
                                   public core::com::HasSignals,
                                   public data::IHasData
{
// to give to OSR an access on IService objects;
friend class registry::ObjectService;
friend class AppConfigManager;
friend class AppManager;
friend class helper::Config;

public:

    SIGHT_DECLARE_SERVICE(IService, core::tools::Object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    using ConfigType = boost::property_tree::ptree;

    using IdType       = std::string;
    using KeyType      = std::string;
    using InputMapType = std::map<std::pair<std::string, std::optional<std::size_t> >,
                                  data::mt::weak_ptr<const data::Object> >;
    using InOutMapType = std::map<std::pair<std::string, std::optional<std::size_t> >,
                                  data::mt::weak_ptr<data::Object> >;
    using OutputMapType = std::map<std::pair<std::string, std::optional<std::size_t> >,
                                   data::mt::shared_ptr<data::Object> >;

    /// Used to store object configuration in a service.
    struct ObjectServiceConfig
    {
        /// Object key name, may contains a suffix #N with the number of the key if it is part of a group
        std::string m_key;

        /// Object identifier
        std::string m_uid;

        /// Object access (INPUT, INOUT, OUTPUT)
        data::Access m_access;

        /// True if the service is autoConnected this object according to the auto-connection map
        bool m_autoConnect {false};

        /// True if the object is optional (i.e. the service can start even if the object is not present)
        bool m_optional {false};
    };

    /// Used to store a service configuration.
    struct Config
    {
        /// Service uid
        std::string m_uid;

        /// Service implementation
        std::string m_type;

        /// True if the service is autoConnected to all of its inputs/inouts according to the auto-connection map
        bool m_globalAutoConnect {false};

        /// Service worker
        std::string m_worker;

        /// list of required objects information (inputs, inouts and outputs), indexed by key name and index
        std::map<std::pair<std::string, std::optional<std::size_t> >, ObjectServiceConfig> m_objects;

        /// list of required object groups information (inputs, inouts and outputs), indexed by key name
        std::map<std::string, ObjectServiceConfig> m_groups;

        /// Service configuration (only used with XML config)
        CSPTR(core::runtime::ConfigurationElement) m_config;
    };

    /// Name of the key to identify the default Composite object that is used for services that don't work on any data.
    /// @remark For V1 compatibility purpose, this object is currently present on all services.
    SERVICE_API static const std::string s_DEFAULT_OBJECT;

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

    /**
     * @name Signal API
     */

    //@{
    SERVICE_API static const core::com::Signals::SignalKeyType s_STARTED_SIG;
    typedef core::com::Signal<void ()> StartedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_UPDATED_SIG;
    typedef core::com::Signal<void ()> UpdatedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_SWAPPED_SIG;
    typedef core::com::Signal<void ()> SwappedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_STOPPED_SIG;
    typedef core::com::Signal<void ()> StoppedSignalType;

    /// Single signal for notifications.
    using notification_signal_type = core::com::Signal<void (NotificationType, std::string)>;
    SERVICE_API static const core::com::Signals::SignalKeyType s_NOTIFIED_SIG;

    //@}

    /**
     * @name Slot API
     */

    //@{
    typedef std::shared_future<void> SharedFutureType;
    typedef std::packaged_task<void ()> PackagedTaskType;
    typedef std::future<void> UniqueFutureType;

    SERVICE_API static const core::com::Slots::SlotKeyType s_START_SLOT;
    typedef core::com::Slot<SharedFutureType()> StartSlotType;

    SERVICE_API static const core::com::Slots::SlotKeyType s_STOP_SLOT;
    typedef core::com::Slot<SharedFutureType()> StopSlotType;

    SERVICE_API static const core::com::Slots::SlotKeyType s_UPDATE_SLOT;
    typedef core::com::Slot<SharedFutureType()> UpdateSlotType;

    SERVICE_API static const core::com::Slots::SlotKeyType s_SWAP_SLOT;
    typedef core::com::Slot<SharedFutureType(data::Object::sptr)> SwapSlotType;

    SERVICE_API static const core::com::Slots::SlotKeyType s_SWAPKEY_SLOT;
    typedef core::com::Slot<SharedFutureType(std::string_view, data::Object::sptr)> SwapKeySlotType;

    /// Initializes m_associatedWorker and associates this worker to all service slots
    SERVICE_API void setWorker(SPTR(core::thread::Worker) worker);

    /// Returns associate worker
    SERVICE_API SPTR(core::thread::Worker) getWorker() const;

    //@}

    /**
     * @name Key service API
     */

    //@{

    /**
     * @brief Affect the configuration, using a generic XML like structure.
     * @param[in] _cfgElement a structure which represents the xml configuration
     * @post m_configurationState == UNCONFIGURED
     */
    SERVICE_API void setConfiguration(const core::runtime::ConfigurationElement::sptr _cfgElement);

    /**
     * @brief Set the configuration.
     * @param[in] _configuration whole configuration of the service.
     * @post m_configurationState == UNCONFIGURED
     */
    SERVICE_API void setConfiguration(const Config& _configuration);

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] ptree property tree
     * @post m_configurationState == UNCONFIGURED
     */
    SERVICE_API void setConfiguration(const ConfigType& ptree);

    /**
     * @brief Set configuration and then invoke configuring() if m_globalState == STOPPED or reconfiguring() if
     *  m_globalState == STARTED.
     * @post m_configurationState == CONFIGURED
     * @param[in] ptree property tree
     */
    SERVICE_API void configure(const ConfigType& ptree);

    /**
     * @brief Invoke configuring() if m_globalState == STOPPED. Invoke reconfiguring() if m_globalState == STARTED. Does
     * nothing otherwise.
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
     * @param[in] _index of the data to retrieve
     * @param[in] _obj change object at given key to _obj
     * @pre m_globalState == STARTED
     */
    SERVICE_API SharedFutureType swapKey(std::string_view _key, data::Object::sptr _obj);
    //@}

    /**
     * @name All concerning status access
     */

    //@{

    /**
     * @brief Return the global process status
     * @return m_globalState
     */
    SERVICE_API GlobalStatus getStatus() const noexcept;

    /**
     * @brief Test if the service is started or not
     * @return true if m_globalState == STARTED
     */
    SERVICE_API bool isStarted() const noexcept;

    /**
     * @brief Test if the service is stopped or not
     * @return true if m_globalState == STOPPED
     */
    SERVICE_API bool isStopped() const noexcept;

    /**
     * @brief Return the configuration process status
     * @return m_configurationState
     */
    SERVICE_API ConfigurationStatus getConfigurationStatus() const noexcept;

    /**
     * @brief Return the update process status
     * @return m_updatingState
     */
    SERVICE_API UpdatingStatus getUpdatingStatus() const noexcept;

    //@}

    /**
     * @name All concerning configuration
     */

    //@{

    /**
     * @brief Return the configuration, in an xml format read using runtime library
     * @return m_configuration, a structure which represents the service configuration
     */
    SERVICE_API core::runtime::ConfigurationElement::sptr getConfiguration() const;

    /**
     * @brief Return the configuration, in an boost property tree
     */
    SERVICE_API ConfigType getConfigTree() const;

    //@}

    /**
     * @name Optimized access to associated Object & Helper
     */

    //@{

    /**
     * @brief Return the inputs map associated to service
     * @return m_inputsMap
     */
    SERVICE_API const InputMapType& getInputs() const;

    /**
     * @brief Return the inouts map associated to service
     * @return m_inoutsMap
     */
    SERVICE_API const InOutMapType& getInOuts() const;

    /**
     * @brief Return the outouts map associated to service
     * @return m_outputsMap
     */
    SERVICE_API const OutputMapType& getOutputs() const;

    /**
     * @brief Return a weak data pointer of the input object at the given key and index.
     * @param key key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template<class DATATYPE, typename CONST_DATATYPE = std::add_const_t<DATATYPE> >
    inline data::mt::weak_ptr<CONST_DATATYPE> getInput(
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    ) const;

    /**
     * @brief Return a weak data pointer of the in/out object at the given key and index.
     * @param key key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template<class DATATYPE>
    inline data::mt::weak_ptr<DATATYPE> getInOut(
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    ) const;

    /**
     * @brief Return a weak data pointer of the out object at the given key and index.
     * @param key key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template<class DATATYPE>
    inline data::mt::weak_ptr<DATATYPE> getOutput(
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    ) const;
    //@}

    /**
     * @name Communication connection between object::signals and service::slots
     */

    //@{

    typedef core::com::helper::SigSlotConnection::KeyConnectionsType KeyConnectionsType;

    /**
     * @brief This class is a helper to define the connections of a service and its data.
     */
    class KeyConnectionsMap
    {
    public:

        using KeyConnectionsMapType = std::map<std::string_view, KeyConnectionsType>;

        KeyConnectionsMap() = default;
        KeyConnectionsMap(
            std::initializer_list<std::tuple<const std::string_view, core::com::Signals::SignalKeyType,
                                             core::com::Slots::SlotKeyType> > init
        )
        {
            for(const auto& [key, sig, slot] : init)
            {
                m_keyConnectionsMap[key].push_back(std::make_pair(sig, slot));
            }
        }

        //------------------------------------------------------------------------------

        void push(
            std::string_view key,
            const core::com::Signals::SignalKeyType& sig,
            const core::com::Slots::SlotKeyType& slot
)
        {
            m_keyConnectionsMap[key].push_back(std::make_pair(sig, slot));
        }

        //------------------------------------------------------------------------------

        KeyConnectionsMapType::const_iterator find(std::string_view key) const
        {
            return m_keyConnectionsMap.find(key);
        }

        //------------------------------------------------------------------------------

        KeyConnectionsMapType::const_iterator end() const
        {
            return m_keyConnectionsMap.cend();
        }

        //------------------------------------------------------------------------------

        bool empty() const
        {
            return m_keyConnectionsMap.empty();
        }

        //------------------------------------------------------------------------------

        std::size_t size() const
        {
            return m_keyConnectionsMap.size();
        }

    private:

        KeyConnectionsMapType m_keyConnectionsMap;
    };

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
     * @brief Return true if the object with the given key has an identifier.
     * @param _key object key
     * @param[in] _index index of the data in the group
     */
    SERVICE_API bool hasObjectId(std::string_view _key, std::optional<std::size_t> index = std::nullopt) const;

    /**
     * @brief Return the id of the object, throw if it is not found
     */
    SERVICE_API IdType getObjectId(std::string_view _key, std::optional<std::size_t> index = std::nullopt) const;

    /**
     * @brief Set the id of an object key from a group
     */
    SERVICE_API void setObjectId(
        std::string_view _key,
        const IService::IdType& _id,
        std::optional<std::size_t> index = std::nullopt
    );

    //@}

    /**
     * @brief Set an object of a group of inputs
     *
     * @param[in] obj input object used by the service
     * @param[in] key key of the object
     * @param[in] index index of the data in the group
     */
    SERVICE_API void setInput(
        data::Object::csptr obj,
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Set an object of a group of inputs, and overrides the default autoConnect and optional settings.
     *
     * @param[in] obj input object used by the service
     * @param[in] key key of the object
     * @param[in] index index of the data in the group
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     * @param[in] index index of the data in the group
     */
    SERVICE_API void setInput(
        data::Object::csptr obj,
        std::string_view key,
        const bool autoConnect,
        const bool optional              = false,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Set an object of a group of in/outs
     *
     * @param[in] obj in/out object used by the service
     * @param[in] key key of the object
     * @param[in] index index of the data in the group
     */
    SERVICE_API void setInOut(
        data::Object::sptr obj,
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Set an object of a group of in/outs, and overrides the default autoConnect and optional settings.
     *
     * @param[in] obj in/out object used by the service
     * @param[in] key key of the object
     * @param[in] index index of the data in the group
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     * @param[in] index index of the data in the group
     */
    SERVICE_API void setInOut(
        data::Object::sptr obj,
        std::string_view key,
        const bool autoConnect,
        const bool optional              = false,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Register an output object at a given key in the OSR, replacing it if it already exists.
     * @param key name of the data or the group to register.
     * @param object pointer to the object to register.
     * @param index optional index of the key in the case of a member of a group of keys.
     * @warning The service manages the output object lifetime: if it creates a new object, it will be the only
     * maintainer of this object, when calling setOutput, it allows to share the object with other services. But these
     * services will not maintain a reference to this object (only weak_ptr). When the service stops, it should remove
     * its outputs by calling setOutput(key, nullptr). Otherwise, a service may work on an expired object.
     */
    SERVICE_API void setOutput(
        std::string_view key,
        data::Object::sptr object,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Set a registered object for this service
     *
     * @param[in] obj object used by the service
     * @param[in] key key of the object
     * @param[in] index index of the data in the group
     * @param[in] access access to the object (in/inout/out)
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     */
    SERVICE_API void setObject(
        data::Object::sptr obj,
        std::string_view key,
        std::optional<std::size_t> index,
        data::Access access,
        const bool autoConnect,
        const bool optional
    );

    /**
     * @brief Unset a registered object for this service
     *
     * @param[in] key key of the object
     * @param[in] access access to the object (in/inout/out)
     */
    SERVICE_API void resetObject(std::string_view key, std::optional<std::size_t> index, data::Access access);

    /**
     * @brief Return the input, inout or output object at the given key.
     * @param _key key of data to retrieve.
     * @param[in] _access access to the object (in/inout/out)
     * @param _index optional index of the data to retrieve.
     * @return data object, nullptr if not found.
     */
    SERVICE_API data::Object::csptr getObject(
        std::string_view _key,
        data::Access _access,
        std::optional<std::size_t> index = std::nullopt
    ) const;

    /**
     * @brief Define an object required by this service.
     *
     * This method is useful when the object does not exist yet and will be created later and managed by an AppManager
     * or an AppConfigManager. The object identifier is used to retrieve the associated object and/or notify its
     * creation when it is an output.
     *
     * @param[in] objId identifier of the object to be used by the service.
     * @param[in] key key of the object
     * @param[in] access access to the object (in or inout)
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     */
    SERVICE_API void registerObject(
        const std::string& objId,
        std::string_view key,
        data::Access access,
        const bool autoConnect           = false,
        const bool optional              = false,
        std::optional<std::size_t> index = std::nullopt
    );

    /// Return true if all the non-optional object required by the service are present
    SERVICE_API bool hasAllRequiredObjects() const;

    /**
     * @brief Emits notification signal with 'message' and Notification 'type' provided
     *
     * @param[in] type type of the notification to emit @see Notification enum class.
     * @param[in] message message as std::string.
     */
    SERVICE_API void notify(NotificationType type, const std::string& message) const;

protected:

    /**
     * @name Constructor and Destructor
     */

    //@{

    /**
     * @brief IService constructor.
     *
     * This constructor does nothing. By default, the service is considered as STOPPED, NOTUPDATING and UNCONFIGURED.
     */
    SERVICE_API IService();

    /**
     * @brief IService desctructor.
     *
     * This destructor does nothing.
     */
    SERVICE_API ~IService() override;

    //@}

    /**
     * @name Interface to override (implementation contract)
     */

    //@{

    /**
     * @brief Initialize the service activity.
     *
     * Use configuration to start his job. For example : the start method
     * installs a button in a frame and show the frame.
     * @see start()
     */
    SERVICE_API virtual void starting() = 0;

    /**
     * @brief Uninitialize the service activity. The stop() method is always invoked before destroying a service.
     *
     * @see stop()
     */
    SERVICE_API virtual void stopping() = 0;

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
    virtual void swapping(std::string_view)
    {
    }

    /**
     * @brief Configure the service before starting. Apply the configuration to service.
     * @see configure()
     */
    SERVICE_API virtual void configuring() = 0;

    /**
     * @brief Reconfigure the service activity when is started.
     * @todo This method should be pure virtual
     * @see configure()
     */
    SERVICE_API virtual void reconfiguring();

    /**
     * @brief Perform some computations according to object (this service is attached to) attribute values and its
     * internal state.
     * @see update()
     */
    SERVICE_API virtual void updating() = 0;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, IService& _service)
     * to avoid declaration of << by all services.
     */
    SERVICE_API virtual void info(std::ostream& _sstream);

    /**
     * @brief Returns proposals to connect service slots to associated objects signals,
     * this method is used for obj/srv auto connection
     */
    SERVICE_API virtual KeyConnectionsMap getAutoConnections() const;

    //@}

    /**
     * @brief Configuration element used to configure service internal state using a generic XML like structure
     * TODO Make this const, we are not supposed to edit that !
     */
    core::runtime::ConfigurationElement::sptr m_configuration;

    /**
     * @name Slot API
     */

    //@{

    /// Slot to call start method
    StartSlotType::sptr m_slotStart;

    /// Slot to call stop method
    StopSlotType::sptr m_slotStop;

    /// Slot to call update method
    UpdateSlotType::sptr m_slotUpdate;

    /// Slot to call swap method
    SwapSlotType::sptr m_slotSwap;

    /// Slot to call swap method
    SwapKeySlotType::sptr m_slotSwapKey;

    /// Associated worker
    SPTR(core::thread::Worker) m_associatedWorker;

    //@}

private:

    /**
     * @brief Internal method that sets an input of the service and registers it into the OSR.
     * @param[in] obj data object
     * @param[in] key key of the object
     */
    void _setInput(
        data::Object::csptr obj,
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Internal method that sets an inout of the service and registers it into the OSR.
     * @param[in] obj data object
     * @param[in] key key of the object
     */
    void _setInOut(
        data::Object::sptr obj,
        std::string_view key,
        std::optional<std::size_t> index = std::nullopt
    );

    /**
     * @brief Register an output object at a given key in the OSR, replacing it if it already exists.
     * @param key name of the data or the group to register.
     * @param object pointer to the object to register.
     * @param index optional index of the key in the case of a member of a group of keys.
     */
    SERVICE_API void _setOutput(
        std::string_view key,
        data::Object::sptr object,
        std::optional<std::size_t> index = std::nullopt
    ) override;

    /// @copydoc sight::data::IHasData::_registerObject
    SERVICE_API void _registerObject(
        std::string_view key,
        const data::Access access,
        std::optional<std::size_t> index,
        const bool autoConnect,
        const bool optional = false
    ) override;

    /// @copydoc sight::data::IHasData::_registerObjectGroup
    SERVICE_API void _registerObjectGroup(
        std::string_view key,
        const data::Access access,
        const bool autoConnect,
        const bool optional
    ) override;

    // Slot: start the service
    SharedFutureType _startSlot();
    SharedFutureType _start(bool _async);

    // Slot: stop the service
    SharedFutureType _stopSlot();
    SharedFutureType _stop(bool _async);

    // Slot: swap an object
    SharedFutureType _swapKeySlot(std::string_view _key, data::Object::sptr _obj);
    SharedFutureType _swapKey(std::string_view _key, data::Object::sptr _obj, bool _async);

    // Slot: update the service
    SharedFutureType _updateSlot();
    SharedFutureType _update(bool _async);

    /// Connect the service with configuration services and objects
    void _connectToConfig();

    /// Disconnect the service from configuration services and objects
    void _disconnectFromConfig();

    /// Connect the service with its data
    void _autoConnect();

    /// Disconnect the service from its data
    void _autoDisconnect();

    /// Add a known connection from the appConfig
    void _addProxyConnection(const helper::ProxyConnections& info);

    /// Return the information about the required object
    std::optional<std::tuple<const std::string&, std::optional<std::size_t>,
                             const service::IService::ObjectServiceConfig&> > _getObjInfoFromId(
        const std::string& objId
    ) const;
    /// Return the information about the required key
    const service::IService::ObjectServiceConfig* _getObjInfoFromKey(
        const std::string& objKey
    ) const;

    /**
     * @brief associated inputs of the service ordered by key
     */
    InputMapType m_inputsMap;

    /**
     * @brief associated input/outputs of the service ordered by key
     */
    InOutMapType m_inOutsMap;

    /**
     * @brief associated outputs of the service ordered by key
     */
    OutputMapType m_outputsMap;

    /**
     * @brief Defines the current global status of the service.
     */
    GlobalStatus m_globalState;

    /**
     * @brief Defines if the service is updating.
     */
    UpdatingStatus m_updatingState;

    /**
     * @brief Defines if the service is configured or not.
     */
    ConfigurationStatus m_configurationState;

    /**
     * @brief Defines the configuration of the objects. Used for autoConnect.
     */
    service::IService::Config m_serviceConfig;

    /// Regular connections between this service and other services, or this service and objects
    core::com::helper::SigSlotConnection m_srvConnections;

    /// Auto connections between this service and its objects
    core::com::helper::SigSlotConnection m_autoConnections;

    /// Proxies configurations, connected at start, and disconnected at stop
    std::map<std::string, helper::ProxyConnections> m_proxies;
};

} // namespace sight::service

#include "service/IService.hxx"
