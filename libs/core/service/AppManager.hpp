/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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
#include "service/IService.hpp"

#include <core/com/SlotBase.hpp>

#include <data/Object.hpp>

#include <mutex>

namespace sight::service
{

/**
 * @brief Base class for application using qml
 *
 * This class allows to manage the service used by an application.
 * We can easily manage its inputs/inouts and the connections. It connects/disconnects the signals and slots when the
 * service is started/stopped and when an object is added/removed.
 *
 * @code{.cpp}
    m_appMgr = std::make_unique< service::AppManager >();

    // Initialize the manager
    m_appMgr->create();
    // Create and register a service
    // - readerService will be automatically started and updated
    // - mesherService will be automatically started but not updated
    auto readerService = m_appMgr->addService("::modules::ui::base::editor::SIOSelector", true, true);
    auto mesherService = m_appMgr->addService("::modules::filter::mesh::SVTKMesher", true, false);

    // configure the services ...

    // Register the objects to associate with the services:
    // - readerService will generate an output image, it is registered as "loadedImage" in the application
    // - mesherService require an input image registered as "loadedImage" in the application
    // - mesherService will generate an output model series, it is registered as "generatedModel" in the application
    readerService->setObjectId("imageSeries", "loadedImage");
    mesherService->setObjectId("imageSeries", "loadedImage");
    mesherService->setObjectId("modelSeries", "generatedModel");

    // Start the reader service:
    // - readerService will be started because it does not require input or inout. It will also be updated.
    // - mesherService will not be started because it requires an input image.
    m_appMgr->startServices();

    // When readerService will be updated, it will generate the image required by the mesher service. As the image is
    // registered with the same identifier in the application, the mesherService will be automatically started.
   @endcode
 *
 * You can access the objects managed by the configuration using addObject(), getObject() and removeObject().
 *
 * If you want to dynamically launch an AppManager, you should inherit from this class. You can define the inputs you
 * required by string that will be replaced when the manager is launched. You will need to call the method
 * getInputID("...") to retrieve the string that are replaced.
 *
 * The method "checkInputs" checks if all the required inputs are present and add the object in the manager.
 *
 * You can find an example in ExActivitiesQml sample.
 *
 * @code{.cpp}
    static const std::string s_IMAGE_ID = "image";
    static const std::string s_MODEL_ID = "model";
    static const std::string s_VALIDATION_CHANNEL = "validationChannel";

    MyManager::MyManager() noexcept
    {
        this->requireInput(s_IMAGE_ID, InputType::OBJECT);
        this->requireInput(s_MODEL_ID, InputType::OBJECT);
        this->requireInput(s_VALIDATION_CHANNEL, InputType::CHANNEL);
    }

    MyManager::~MyManager() noexcept
    {
        this->destroy();
    }

    void MyManager::initialize()
    {
        this->create();

        if (this->checkInputs())
        {
            auto mesher = this->addService("::modules::filter::mesh::SVTKMesher", true, true);
            mesher->setObjectId("imageSeries", this->getInputID(s_IMAGE_ID));
            mesher->setObjectId("modelSeries", this->getInputID(s_MODEL_ID));

            service::IService::ConfigType mesherConfig;
            mesherConfig.put("config.percentReduction", reduction);
            mesher->configure(mesherConfig);

            service::helper::ProxyConnections connection(this->getInputID(s_VALIDATION_CHANNEL));
            connection.addSignalConnection(mesher->getID(), service::IService::s_UPDATED_SIG);
            this->addProxyConnection(connection);

            this->startServices();
        }
        else
        {
            const std::string msg = "All the required inputs are not present, '" + this->getID() +
                                    "' activity will not be launched";
            ui::base::dialog::MessageDialog::show("Manager Initialization",
                                                              msg,
                                                              ui::base::dialog::IMessageDialog::CRITICAL);
        }
    }
   @endcode
 */
class SERVICE_CLASS_API AppManager : public core::com::HasSlots
{
public:

    SERVICE_API AppManager();
    SERVICE_API ~AppManager();

    typedef service::IService::AccessType AccessType;

    /// Initialize the Manager
    SERVICE_API virtual void create();

    /// Uninitialize the Manager
    SERVICE_API virtual void destroy();

    /**
     * @brief Create and register the service in the OSR
     *
     * This method does not start the service even if autoStart=true, you must call startService(srv) or startServices()
     * to start the service when all its required inputs are present.
     * To define the required inputs, use IService::registerObject().
     *
     * @param type service classname
     * @param uid service uid. If it is empty, it will be generated
     * @param autoStart if true, the service will be started when all its required inputs are present.
     * @param autoUpdate if true, the service will be updated after starting.
     * @return Return the created service
     */
    SERVICE_API service::IService::sptr addService(const std::string& type, const std::string& uid,
                                                   bool autoStart, bool autoUpdate);

    /**
     * @brief Create and register the service in the OSR. Its uid will be generated.
     *
     * This method does not start the service even if autoStart=true, you must call startService(srv) or startServices()
     * to start the service when all its required inputs are present.
     * To define the required inputs, use IService::registerObject().
     *
     * @param type service classname
     * @param autoStart if true, the service will be started when all its required inputs are present.
     * @param autoUpdate if true, the service will be updated after starting.
     * @return Return the created service
     */
    SERVICE_API service::IService::sptr addService(const std::string& type, bool autoStart = false,
                                                   bool autoUpdate = false);

    /**
     * @brief Create and register the service in the OSR
     *
     * This method does not start the service even if autoStart=true, you must call startService(srv) or startServices()
     * to start the service when all its required inputs are present.
     * To define the required inputs, use IService::registerObject() or IService::registerObject().
     *
     * @param type service classname
     * @param uid service uid. If it is empty, it will be generated
     * @param autoStart if true, the service will be started when all its required inputs are present.
     * @return Return the created service
     */
    template<class SERVICE>
    SPTR(SERVICE) addService( const std::string& type, const std::string& uid, bool autoStart, bool autoUpdate);

    /**
     * @brief Create and register the service in the OSR. Its uid will be generated.
     *
     * This method does not start the service even if autoStart=true, you must call startService(srv) or startServices()
     * to start the service when all its required inputs are present.
     * To define the required inputs, use IService::registerObject().
     *
     * @param type service classname
     * @param autoStart if true, the service will be started when all its required inputs are present.
     * @return Return the created service
     */
    template<class SERVICE>
    SPTR(SERVICE) addService( const std::string& type, bool autoStart = false, bool autoUpdate = false);

    /**
     * @brief Register the service in the OSR.
     *
     * This method does not start the service even if autoStart=true, you must call startService(srv) or startServices()
     * to start the service when all its required inputs are present.
     * To define the required inputs, use IService::registerObject().
     *
     * @param srv service to register
     * @param autoStart if true, the service will be started when all its required inputs are present.
     */
    SERVICE_API void addService(const service::IService::sptr& srv, bool autoStart = false,
                                bool autoUpdate = false);

    /**
     * @brief Start the service and register it in the started service container
     *
     * It also connect the service's signals and slots defined by connectSignal() and connectSlot().
     * If service is register with autoUpdate=true, it will be updated.
     *
     * @throw raise a core::Exception if all the required objects are not present
     */
    SERVICE_API void startService(const service::IService::sptr& srv);

    /**
     * @brief Stop the service.
     *
     * It also disconnect the service's signals and slots defined by connectSignal() and connectSlot().
     */
    SERVICE_API void stopService(const service::IService::sptr& srv);

    /**
     * @brief Start all the services with autoStart=true and with all the required objects.
     *
     * It also define a flag that allow to start a service when adding an object.
     */
    SERVICE_API void startServices();

    /// Stop all the started service and unregister all the registered service
    SERVICE_API void stopAndUnregisterServices();

    /**
     * @brief Slot: register the added object into all the services that require it
     *
     * This slot is connected to the OSR to listen the created object, it could also be called by the AppManager
     * inherited classes to register an object.
     *
     * If startServices() has been called previouly, the services that require this object and have all their required
     * objects are started.
     *
     * @param obj the new object to register
     * @param id the identifier of the object, this identifier is only used to retrieve the object inside this
     *        AppManager, it is different from the uid.
     */
    SERVICE_API void addObject(data::Object::sptr obj, const std::string& id);

    /**
     * @brief Slot: unregister the object from all the services that require it
     *
     * This slot is connected to the OSR to listen the removed object, it could also be called by the AppManager
     * inherited classes to unregister an object.
     *
     * @param obj the object to unregister
     * @param id the identifier of the object, this identifier is only used to retrieve the object inside this
     *        AppManager, it is different from the uid.
     */
    SERVICE_API void removeObject(data::Object::sptr obj, const std::string& id);

    /**
     * @brief Return the object registered with the given identifier.
     * @param id object identifier
     * @return Return the object registered with the given identifier or nullptr if it is not present
     */
    SERVICE_API data::Object::sptr getObject(const std::string& id) const;

    /// Add a proxy connection
    SERVICE_API void addProxyConnection(const helper::ProxyConnections& proxy);

    /**
     * @brief Define the value of a required input
     *
     * For OBJECT, the value must be the fwID of an existing object.
     */
    SERVICE_API void replaceInput(const std::string& key, const std::string& value);

    /// Check if all the required inputs are present and add the object in the manager
    SERVICE_API bool checkInputs();

    /// Return the AppManager unique identifier
    SERVICE_API const std::string& getID() const;

protected:

    enum class InputType
    {
        OBJECT,
        CHANNEL,
        OTHER
    };

    struct Input
    {
        InputType type;
        std::string key;
        std::string value;
        std::string defaultValue;
        bool isOptional;
    };

    typedef std::map< std::string, Input > InputContainer;

    /**
     * @brief Return the input identifier or create a unique human readable identifier by concatenating the given id
     * with the AppManaget uid(ie. AppManager_1_<id>)
     */
    SERVICE_API std::string getInputID(const std::string& id) const;

    /**
     * @brief Define the object, channels or other parameters that are required to launch the manager
     * @param key name of the input
     * @param type type of the input OBJECT (for a data::Object), CHANNEL (for communication) or OTHER (replaced by
     * a String)
     * @param defaultValue default value of the input, if it is empty, the input is not present, it will be replaces by
     * this value (for OBJECT, the default value must be the type of the object to create (ex. data::Image)
     */
    SERVICE_API void requireInput(const std::string& key, const InputType& type,
                                  const std::string& defaultValue = "");

private:

    /// Information about connection <channel, sig/slot name>
    typedef std::unordered_map< std::string, std::string > ConnectionInfo;

    /// Register the service information
    struct ServiceInfo
    {
        /// constructor
        SERVICE_API ServiceInfo(const service::IService::sptr& srv, const bool autoStart,
                                const bool autoUpdate);

        /// service
        service::IService::wptr m_service;

        /// True if the service will be automatically started when all its required objects are present
        bool m_autoStart;

        /// True if the service will be automatically updated after starting
        bool m_autoUpdate;
    };

    /// Add the service and register the existing object
    void internalAddService(const service::IService::sptr& srv, const bool autoStart, const bool autoUpdate);

    /// Return the service information
    ServiceInfo& getServiceInfo(const service::IService::sptr& srv);

    /// Return the service information
    const ServiceInfo& getServiceInfo(const service::IService::sptr& srv) const;

    /// Start the service and connect its slots and signals
    service::IService::SharedFutureType start(const ServiceInfo& info);

    /// Stop the service and connect its slots and signals
    service::IService::SharedFutureType stop(const ServiceInfo& info);

    bool m_isStarted {false};

    InputContainer m_inputs;

    /// Store the information of the services (objects, autoStart, autoUpdate)
    std::vector< ServiceInfo > m_services;

    /// Store started services
    std::vector< service::IService::sptr > m_startedService;

    /// Store registered objects <id, obj>
    std::unordered_map< std::string, data::Object::sptr > m_registeredObject;

    /// Store the object connections <objId, connection >
    std::unordered_map< std::string, ConnectionInfo > m_objectConnection;

    struct ServiceProxyType
    {
        std::unordered_map< std::string, helper::ProxyConnections > m_proxyCnt;
    };
    std::unordered_map< std::string, ServiceProxyType > m_proxies;

    /// Connections to the OSR
    core::com::Connection m_addObjectConnection;
    core::com::Connection m_removeObjectConnection;

    mutable std::recursive_mutex m_objectMutex;
    std::mutex m_serviceMutex;

    /// Unique human readable identifier (ex: AppManager-<count>)
    mutable std::string m_uid;

    /// Number of instantiated AppManager, it allows to create unique identifier for object and channels
    static size_t s_counter;
};

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) AppManager::addService( const std::string& type, const std::string& uid, bool autoStart,
                                      bool autoUpdate)
{
    auto srv = this->addService(type, uid, autoStart, autoUpdate);

    auto castedSrv = std::dynamic_pointer_cast< SERVICE >(srv);
    FW_RAISE_IF("Failed to cast service from factory type '" + type + "' into '" +
                core::TypeDemangler<SERVICE>().getClassname() + "'", !srv );

    return castedSrv;
}

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) AppManager::addService( const std::string& type, bool autoStart, bool autoUpdate)
{
    return this->addService<SERVICE>(type, "", autoStart, autoUpdate);
}

} // fwServices
