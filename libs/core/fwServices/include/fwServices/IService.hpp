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

#include "fwServices/config.hpp"
#include "fwServices/factory/new.hpp"
#include "fwServices/helper/ProxyConnections.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <fwData/Exception.hpp>
#include <fwData/Object.hpp>
#include <fwData/mt/locked_ptr.hpp>
#include <fwData/mt/weak_ptr.hpp>
#include <fwData/mt/shared_ptr.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

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

namespace fwServices
{
namespace registry
{
class ObjectService;
}
namespace fwThread
{
class Worker;
}

#define KEY_GROUP_NAME(key, index) (key + "#" + std::to_string(index) )

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
 * - \b infoNotified(std::string _message): Emitted when the service wants to pop an info notification.
 * - \b successNotified(std::string _message): Emitted when the service wants to pop a success notification.
 * - \b failureNotified(std::string _message): Emitted when the service wants to pop a failure notification.
 *
 * @section Slots Slots
 * - \b start() : Start the service.
 * - \b update() : Update the service.
 * - \b stop() : Stop the service.
 * - \b swap() : @deprecated Swap the current object.
 * - \b swapKey(const KeyType&, ::fwData::Object::sptr) : Swap the object at the given key with the object in parameter.
 */
class FWSERVICES_CLASS_API IService : public core::tools::Object,
                                      public core::com::HasSlots,
                                      public core::com::HasSignals
{

// to give to OSR an access on IService objects;
friend class registry::ObjectService;
friend class AppConfigManager;
friend class AppManager;

public:
    fwCoreServiceMacro(IService, core::tools::Object)
    fwCoreAllowSharedFromThis();

    typedef ::boost::property_tree::ptree ConfigType;

    typedef std::string IdType;
    typedef std::string KeyType;
    typedef std::map< KeyType, ::fwData::mt::weak_ptr< const ::fwData::Object > > InputMapType;
    typedef std::map< KeyType, ::fwData::mt::weak_ptr< ::fwData::Object > > InOutMapType;
    typedef std::map< KeyType, ::fwData::mt::shared_ptr< ::fwData::Object > > OutputMapType;

    enum class AccessType : std::uint8_t
    {
        INPUT,
        OUTPUT,
        INOUT,
    };

    /// Used to store object configuration in a service.
    struct ObjectServiceConfig
    {
        /// Object identiifer
        std::string m_uid;

        /// Object key used to by getInput()/getInOut()/...
        std::string m_key;

        /// Obejt access (INPUT, INOUT, OUTPUT)
        AccessType m_access;

        /// True if the service is autoConnected this object according to the auto-connection map
        bool m_autoConnect { false };

        /// True if the object is optional (i.e. the service can start even if the object is not present)
        bool m_optional { false };
    };

    /// Used to store a service configuration.
    struct Config
    {
        /// Service uid
        std::string m_uid;

        /// Service implementation
        std::string m_type;

        /// True if the service is autoConnected to all of its inputs/inouts according to the auto-connection map
        bool m_globalAutoConnect { false };

        /// Service worker
        std::string m_worker;

        /// list of required service's objects information (inputs, inouts and outputs)
        std::vector<ObjectServiceConfig> m_objects;

        /// Number of elements in each group
        std::map<std::string, size_t> m_groupSize;

        /// Service configuration (only used with XML config)
        CSPTR(::fwRuntime::ConfigurationElement) m_config;
    };

    /// Name of the key to identify the default Composite object that is used for services that don't work on any data.
    /// @remark For V1 compatibility purpose, this object is currently present on all services.
    FWSERVICES_API static const std::string s_DEFAULT_OBJECT;

    /**
     * @name Definition of service status
     */

    //@{

    /// Defines all possible global status for a service, including transitions
    typedef enum
    {
        STARTED,    /**< state after start */
        STARTING,   /**< state during start */
        SWAPPING,   /**< state during swap */
        STOPPED,    /**< state after stop */
        STOPPING    /**< state during stop */
    } GlobalStatus;

    /// Defines all possible status for an update process
    typedef enum
    {
        UPDATING,   /**< state during update */
        NOTUPDATING /**< state during when the service is not updating */
    } UpdatingStatus;

    /// Defines all possible status for a configuration process
    typedef enum
    {
        CONFIGURING,    /**< state during configuration */
        CONFIGURED,     /**< state after configuration */
        UNCONFIGURED    /**< state when the service is not configured */
    } ConfigurationStatus;

    //@}

    /**
     * @name Signal API
     */
    //@{
    FWSERVICES_API static const core::com::Signals::SignalKeyType s_STARTED_SIG;
    typedef core::com::Signal<void ()> StartedSignalType;

    FWSERVICES_API static const core::com::Signals::SignalKeyType s_UPDATED_SIG;
    typedef core::com::Signal<void ()> UpdatedSignalType;

    FWSERVICES_API static const core::com::Signals::SignalKeyType s_STOPPED_SIG;
    typedef core::com::Signal<void ()> StoppedSignalType;

    FWSERVICES_API static const core::com::Signals::SignalKeyType s_INFO_NOTIFIED_SIG;
    typedef core::com::Signal<void (std::string)> InfoNotifiedSignalType;

    FWSERVICES_API static const core::com::Signals::SignalKeyType s_SUCCESS_NOTIFIED_SIG;
    typedef core::com::Signal<void (std::string)> SuccessNotifiedSignalType;

    FWSERVICES_API static const core::com::Signals::SignalKeyType s_FAILURE_NOTIFIED_SIG;
    typedef core::com::Signal<void (std::string)> FailureNotifiedSignalType;

    //@}

    /**
     * @name Slot API
     */
    //@{
    typedef std::shared_future< void > SharedFutureType;
    typedef std::packaged_task< void ()> PackagedTaskType;
    typedef std::future< void > UniqueFutureType;

    FWSERVICES_API static const core::com::Slots::SlotKeyType s_START_SLOT;
    typedef core::com::Slot<SharedFutureType()> StartSlotType;

    FWSERVICES_API static const core::com::Slots::SlotKeyType s_STOP_SLOT;
    typedef core::com::Slot<SharedFutureType()> StopSlotType;

    FWSERVICES_API static const core::com::Slots::SlotKeyType s_UPDATE_SLOT;
    typedef core::com::Slot<SharedFutureType()> UpdateSlotType;

    FWSERVICES_API static const core::com::Slots::SlotKeyType s_SWAP_SLOT;
    typedef core::com::Slot<SharedFutureType(::fwData::Object::sptr)> SwapSlotType;

    FWSERVICES_API static const core::com::Slots::SlotKeyType s_SWAPKEY_SLOT;
    typedef core::com::Slot<SharedFutureType(const KeyType&, ::fwData::Object::sptr)> SwapKeySlotType;

    /// Initializes m_associatedWorker and associates this worker to all service slots
    FWSERVICES_API void setWorker( SPTR(core::thread::Worker) worker );

    /// Returns associate worker
    FWSERVICES_API SPTR(core::thread::Worker) getWorker() const;

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
    FWSERVICES_API void setConfiguration( const ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    /**
     * @brief Set the configuration.
     * @param[in] _configuration whole configuration of the service.
     * @post m_configurationState == UNCONFIGURED
     */
    FWSERVICES_API void setConfiguration( const Config& _configuration);

    /**
     * @brief Affect the configuration, using a boost property tree
     * @param[in] ptree property tree
     * @post m_configurationState == UNCONFIGURED
     */
    FWSERVICES_API void setConfiguration( const ConfigType& ptree );

    /**
     * @brief Set configuration and then invoke configuring() if m_globalState == STOPPED or reconfiguring() if
     *  m_globalState == STARTED.
     * @post m_configurationState == CONFIGURED
     * @param[in] ptree property tree
     */
    FWSERVICES_API void configure( const ConfigType& ptree );

    /**
     * @brief Invoke configuring() if m_globalState == STOPPED. Invoke reconfiguring() if m_globalState == STARTED. Does
     * nothing otherwise.
     * @pre m_configurationState == UNCONFIGURED
     * @post m_configurationState == CONFIGURED
     */
    FWSERVICES_API void configure();

    /**
     * @brief Invoke starting() if m_globalState == STOPPED. Does nothing otherwise.
     * @post m_globalState == STARTED
     */
    FWSERVICES_API SharedFutureType start();

    /**
     * @brief Invoke stopping() if m_globalState == STARTED. Does nothing otherwise. Stops all observations.
     *
     * @post m_globalState == STOPPED
     *
     */
    FWSERVICES_API SharedFutureType stop();

    /**
     * @brief Invoke updating() if m_globalState == STARTED. Does nothing otherwise.
     * @pre m_globalState == STARTED
     */
    FWSERVICES_API SharedFutureType update();

    /**
     * @brief Associate the service to another object
     * @param[in] _obj change association service from key to _obj
     * @pre m_globalState == STARTED
     * @pre old object != _obj
     *
     * This method provides to associate the service to another object without stopping
     * and deleting it. Furthermore, this method modify all observations to be aware to
     * _obj notifications.
     *
     *
     */
    FWSERVICES_API SharedFutureType swapKey( const KeyType& _key, ::fwData::Object::sptr _obj );
    //@}

    /**
     * @name All concerning status access
     */

    //@{

    /**
     * @brief Return the global process status
     * @return m_globalState
     */
    FWSERVICES_API GlobalStatus getStatus() const noexcept;

    /**
     * @brief Test if the service is started or not
     * @return true if m_globalState == STARTED
     */
    FWSERVICES_API bool isStarted() const noexcept;

    /**
     * @brief Test if the service is stopped or not
     * @return true if m_globalState == STOPPED
     */
    FWSERVICES_API bool isStopped() const noexcept;

    /**
     * @brief Return the configuration process status
     * @return m_configurationState
     */
    FWSERVICES_API ConfigurationStatus getConfigurationStatus() const noexcept;

    /**
     * @brief Return the update process status
     * @return m_updatingState
     */
    FWSERVICES_API UpdatingStatus getUpdatingStatus() const noexcept;
    //@}

    /**
     * @name All concerning configuration
     */

    //@{

    /**
     * @brief Return the configuration, in an xml format read using runtime library
     * @return m_configuration, a structure which represents the service configuration
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::sptr getConfiguration() const;

    /**
     * @brief Return the configuration, in an boost property tree
     */
    FWSERVICES_API ConfigType getConfigTree() const;
    //@}

    /**
     * @name Optimized access to associated Object & Helper
     */
    //@{

    /**
     * @brief Return the inputs map associated to service
     * @return m_inputsMap
     */
    FWSERVICES_API const InputMapType& getInputs() const;

    /**
     * @brief Return the inouts map associated to service
     * @return m_inoutsMap
     */
    FWSERVICES_API const InOutMapType& getInOuts() const;

    /**
     * @brief Return the outouts map associated to service
     * @return m_outputsMap
     */
    FWSERVICES_API const OutputMapType& getOutputs() const;

    /**
     * @brief Return the input object at the given key. Asserts if the data is not of the right type.
     * @param key name of the data to retrieve.
     * @return object cast in the right type, nullptr if not found.
     */
    template< class DATATYPE >
    [[deprecated("it will be removed in sight 21.0, use getWeakXXX() or getLockedXXX()")]]
    inline CSPTR(DATATYPE) getInput(const KeyType& key) const;

    /**
     * @brief Return the inout object at the given key. Asserts if the data is not of the right type.
     * @param key name of the data to retrieve.
     * @return object cast in the right type, nullptr if not found.
     */
    template< class DATATYPE >
    [[deprecated("it will be removed in sight 21.0, use getWeakXXX() or getLockedXXX()")]]
    inline SPTR(DATATYPE) getInOut(const KeyType& key) const;

    /**
     * @brief Return the output object at the given key. Asserts if the data is not of the right type.
     * @param key name of the data to retrieve.
     * @return object cast in the right type, nullptr if not found.
     */
    template< class DATATYPE >
    [[deprecated("it will be removed in sight 21.0, use getWeakXXX() or getLockedXXX()")]]
    inline SPTR(DATATYPE) getOutput(const KeyType& key) const;

    /**
     * @brief Return the input object at the given key. Asserts if the data is not of the right type.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return object cast in the right type, nullptr if not found.
     */
    template< class DATATYPE >
    [[deprecated("it will be removed in sight 21.0, use getWeakXXX() or getLockedXXX()")]]
    inline CSPTR(DATATYPE) getInput(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return the inout object at the given key. Asserts if the data is not of the right type.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return object cast in the right type, nullptr if not found.
     */
    template< class DATATYPE >
    [[deprecated("it will be removed in sight 21.0, use getWeakXXX() or getLockedXXX()")]]
    inline SPTR(DATATYPE) getInOut(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return the output object at the given key. Asserts if the data is not of the right type.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return object cast in the right type, nullptr if not found.
     */
    template< class DATATYPE >
    [[deprecated("it will be removed in sight 21.0, use getWeakXXX() or getLockedXXX()")]]
    inline SPTR(DATATYPE) getOutput(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return a weak data pointer of the input object at the given key.
     * @param key name of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template< class DATATYPE, typename CONST_DATATYPE = std::add_const_t< DATATYPE > >
    inline ::fwData::mt::weak_ptr< CONST_DATATYPE > getWeakInput(const KeyType& key) const;

    /**
     * @brief Return a weak data pointer of the in/out object at the given key.
     * @param key name of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::weak_ptr< DATATYPE > getWeakInOut(const KeyType& key) const;

    /**
     * @brief Return a weak data pointer of the out object at the given key.
     * @param key name of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::weak_ptr< DATATYPE > getWeakOutput(const KeyType& key) const;

    /**
     * @brief Return a weak data pointer of the input object at the given key and index.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template< class DATATYPE, typename CONST_DATATYPE = std::add_const_t< DATATYPE > >
    inline ::fwData::mt::weak_ptr< CONST_DATATYPE > getWeakInput(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return a weak data pointer of the in/out object at the given key and index.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::weak_ptr< DATATYPE > getWeakInOut(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return a weak data pointer of the out object at the given key and index.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::weak_ptr< DATATYPE > getWeakOutput(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return a locked data pointer of the input object at the given key.
     * @param key name of the data to retrieve.
     * @return locked data pointer in the right type.
     * @throw ::fwData::Exception if the data object is not found.
     */
    template< class DATATYPE, typename CONST_DATATYPE = std::add_const_t< DATATYPE > >
    inline ::fwData::mt::locked_ptr< CONST_DATATYPE > getLockedInput(const KeyType& key) const;

    /**
     * @brief Return a locked data pointer of the in/out object at the given key.
     * @param key name of the data to retrieve.
     * @return locked data pointer in the right type.
     * @throw ::fwData::Exception if the data object is not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::locked_ptr< DATATYPE > getLockedInOut(const KeyType& key) const;

    /**
     * @brief Return a locked data pointer of the out object at the given key.
     * @param key name of the data to retrieve.
     * @return locked data pointer in the right type.
     * @throw ::fwData::Exception if the data object is not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::locked_ptr< DATATYPE > getLockedOutput(const KeyType& key) const;

    /**
     * @brief Return a locked data pointer of the input object at the given key and index.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return locked data pointer in the right type.
     * @throw ::fwData::Exception if the data object is not found.
     */
    template< class DATATYPE, typename CONST_DATATYPE = std::add_const_t< DATATYPE > >
    inline ::fwData::mt::locked_ptr< CONST_DATATYPE > getLockedInput(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return a locked data pointer of the in/out object at the given key and index.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return locked data pointer in the right type.
     * @throw ::fwData::Exception if the data object is not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::locked_ptr< DATATYPE > getLockedInOut(const KeyType& keybase, size_t index) const;

    /**
     * @brief Return a locked data pointer of the out object at the given key and index.
     * @param group key of data to retrieve.
     * @param index of the data to retrieve.
     * @return locked data pointer in the right type.
     * @throw ::fwData::Exception if the data object is not found.
     */
    template< class DATATYPE >
    inline ::fwData::mt::locked_ptr< DATATYPE > geLockedOutput(const KeyType& keybase, size_t index) const;

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
    FWSERVICES_API void setOutput(const ::fwServices::IService::KeyType& key, const ::fwData::Object::sptr& object,
                                  size_t index = 0);

    /**
     * @brief Return the number of key in a group of keys.
     * @param keybase group name.
     * @return number of keys in this group.
     */
    size_t getKeyGroupSize(const KeyType& keybase) const;
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
        //------------------------------------------------------------------------------

        void push (const KeyType& key,
                   const core::com::Signals::SignalKeyType& sig,
                   const core::com::Slots::SlotKeyType& slot)
        {
            m_keyConnectionsMap[key].push_back(std::make_pair(sig, slot));
        }

        typedef std::map< KeyType, KeyConnectionsType> KeyConnectionsMapType;

        //------------------------------------------------------------------------------

        KeyConnectionsMapType::const_iterator find(const KeyType& key) const
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

        size_t size() const
        {
            return m_keyConnectionsMap.size();
        }

    private:
        std::map< KeyType, KeyConnectionsType> m_keyConnectionsMap;
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
    FWSERVICES_API friend std::ostream& operator<<(std::ostream& _sstream, IService& _service);

    /**
     * @brief Return true if the object with the given key has an identifier.
     * @param _key object key
     */
    FWSERVICES_API bool hasObjectId(const KeyType& _key) const;

    /**
     * @brief Return the id of the object, throw if it is not found
     */
    FWSERVICES_API IdType getObjectId(const KeyType& _key) const;

    /**
     * @brief Set the id of an object key
     */
    FWSERVICES_API void setObjectId(const KeyType& _key, const IdType& _id);

    /**
     * @brief Set the id of an object key from a group
     */
    FWSERVICES_API void setObjectId(const IService::KeyType& _key, const size_t index, const IService::IdType& _id);
    //@}

    /**
     * @brief Register an input object for this service
     *
     * @param[in] obj input object used by the service
     * @param[in] key key of the object
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     */
    FWSERVICES_API void registerInput(const ::fwData::Object::csptr& obj, const std::string& key,
                                      const bool autoConnect = false, const bool optional = false);

    /**
     * @brief Unregister an input object for this service
     *
     * @param[in] key key of the object
     */
    FWSERVICES_API void unregisterInput(const std::string& key);

    /**
     * @brief Register an in/out object for this service
     *
     * @param[in] obj in/out object used by the service
     * @param[in] key key of the object
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     */
    FWSERVICES_API void registerInOut(const ::fwData::Object::sptr& obj, const std::string& key,
                                      const bool autoConnect = false, const bool optional = false);

    /**
     * @brief Unregister an inout object for this service
     *
     * If the service is defined with autoStart=true, it will be automatically stopped if the removed object is not
     * optional.
     *
     * @param[in] key key of the object
     */
    FWSERVICES_API void unregisterInOut(const std::string& key);

    /**
     * @brief Register an object for this service
     *
     * @param[in] obj input object used by the service
     * @param[in] key key of the object
     * @param[in] access access to the object (in or inout)
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     */
    FWSERVICES_API void registerObject(const ::fwData::Object::sptr& obj, const std::string& key,
                                       AccessType access, const bool autoConnect = false, const bool optional = false);

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
    FWSERVICES_API void registerObject(const std::string& objId, const std::string& key,
                                       AccessType access, const bool autoConnect = false, const bool optional = false);

    /**
     * @brief Unregister an object for this service
     *
     * @param[in] key key of the object
     */
    FWSERVICES_API void unregisterObject(const std::string& key, AccessType access);

    /**
     * @brief Unregister an object for this service
     *
     * @param[in] objId identifier of the object to be used by the service.
     */
    FWSERVICES_API void unregisterObject(const std::string& objId);

    /// Return true if all the non-optional object required by the service are present
    FWSERVICES_API bool hasAllRequiredObjects() const;

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
    FWSERVICES_API IService();

    /**
     * @brief IService desctructor.
     *
     * This destructor does nothing.
     */
    FWSERVICES_API virtual ~IService() override;

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
    FWSERVICES_API virtual void starting() = 0;

    /**
     * @brief Uninitialize the service activity. The stop() method is always invoked before destroying a service.
     *
     * @see stop()
     */
    FWSERVICES_API virtual void stopping() = 0;

    /**
     * @brief Swap the service from associated object to another object
     * @see swap()
     * @todo This method must be pure virtual
     * @todo FIXME after code update for all services
     * @todo This method must have in parameter the new object or the old ?
     * @deprecated use swapping(const KeyType& key) instead
     */
    virtual void swapping()
    {
    }

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
    virtual void swapping(const KeyType&)
    {
    }

    /**
     * @brief Configure the service before starting. Apply the configuration to service.
     * @see configure()
     */
    FWSERVICES_API virtual void configuring() = 0;

    /**
     * @brief Reconfigure the service activity when is started.
     * @todo This method should be pure virtual
     * @see configure()
     */
    FWSERVICES_API virtual void reconfiguring();

    /**
     * @brief Perform some computations according to object (this service is attached to) attribute values and its
     * internal state.
     * @see update()
     */
    FWSERVICES_API virtual void updating() = 0;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, IService& _service)
     * to avoid declaration of << by all services.
     */
    FWSERVICES_API virtual void info( std::ostream& _sstream );

    /**
     * @brief Returns proposals to connect service slots to associated objects signals,
     * this method is used for obj/srv auto connection
     */
    FWSERVICES_API virtual KeyConnectionsMap getAutoConnections() const;
    //@}

    /**
     * @brief Define an object required by this service.
     *
     * This method allows to define the required objects to use the service. It can be called in the constructor of the
     * service implementation. So you can call 'hasAllRequiredObjects()' to know if the service can be started.
     *
     * @param[in] key key of the object
     * @param[in] access access to the object (in or inout)
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] optional if true, the service can be started even if the objet is not present
     */
    FWSERVICES_API void registerObject(const std::string& key, AccessType access, const bool autoConnect = false,
                                       const bool optional = false);

    /**
     * @brief Define an object group required by this service.
     *
     * This method allows to define the required objects to use the service. It can be called in the constructor of the
     * service implementation. So you can call 'hasAllRequiredObjects()' to know if the service can be started.
     *
     * @param[in] key key of the object
     * @param[in] access access to the object (in or inout)
     * @param[in] minNbObject number of object to register (it is the minimum number of objects required by the service)
     * @param[in] autoConnect if true, the service will be connected to the object's signals
     * @param[in] maxNbObject maximum number of object to register (they are defined as optional
     *
     * @note This method will register maxNbObject in the group named (<key>#0, <key>#1, ... <key>#<maxNbObject>). The
     * first Nth objects (minNbObject) are required, the other are optional.
     */
    FWSERVICES_API void registerObjectGroup(const std::string& key, AccessType access, const std::uint8_t minNbObject,
                                            const bool autoConnect = false, const std::uint8_t maxNbObject = 10);

    /**
     * @brief Configuration element used to configure service internal state using a generic XML like structure
     * TODO Make this const, we are not supposed to edit that !
     */
    ::fwRuntime::ConfigurationElement::sptr m_configuration;

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

    // Slot: start the service
    SharedFutureType startSlot();
    SharedFutureType internalStart(bool _async);

    // Slot: stop the service
    SharedFutureType stopSlot();
    SharedFutureType internalStop(bool _async);

    // Slot: swap an object
    SharedFutureType swapKeySlot(const KeyType& _key, ::fwData::Object::sptr _obj);
    SharedFutureType internalSwapKey(const KeyType& _key, ::fwData::Object::sptr _obj, bool _async);

    // Slot: update the service
    SharedFutureType updateSlot();
    SharedFutureType internalUpdate(bool _async);

    /// Connect the service with configuration services and objects
    void connectToConfig();

    /// Disconnect the service from configuration services and objects
    void disconnectFromConfig();

    /// Connect the service with its data
    void autoConnect();

    /// Disconnect the service from its data
    void autoDisconnect();

    /// Add a known connection from the appConfig
    void addProxyConnection(const helper::ProxyConnections& info);

    /// Return true if the service contains this object into its requirement
    bool hasObjInfoFromId(const std::string& objId) const;

    /// Return the information about the required object
    const ::fwServices::IService::ObjectServiceConfig& getObjInfoFromId(const std::string& objId) const;

    /// Return the information about the required object
    const ::fwServices::IService::ObjectServiceConfig& getObjInfoFromKey(const std::string& key) const;

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
     * @brief size of key groups if they exist
     */
    std::map<std::string, size_t> m_keyGroupSize;

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
    ::fwServices::IService::Config m_serviceConfig;

    /// Regular connections between this service and other services, or this service and objects
    core::com::helper::SigSlotConnection m_srvConnections;

    /// Auto connections between this service and its objects
    core::com::helper::SigSlotConnection m_autoConnections;

    /// Proxies configurations, connected at start, and disconnected at stop
    std::map<std::string, helper::ProxyConnections> m_proxies;
};

} // namespace fwServices

#include "fwServices/IService.hxx"
