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
#include "service/helper/Config.hpp"
#include "service/helper/ProxyConnections.hpp"
#include "service/IAppConfigManager.hpp"
#include "service/IService.hpp"
#include "service/IXMLParser.hpp"

#include <core/com/HasSlots.hpp>
#include <core/com/helper/SigSlotConnection.hpp>

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sight::data
{

class Composite;

} // namespace sight::data

namespace sight::service::detail
{

/**
 * @brief This class provides an API to manage config template.
 *
 * @section Slots Slots
 * - \b addObject(data::Object::sptr, const std::string&): adds objects to the configuration.
 * - \b removeObject(data::Object::sptr, const std::string&): removes objects from the configuration.
 */
class AppConfigManager : public service::IAppConfigManager,
                         public core::com::HasSlots
{
public:

    SIGHT_DECLARE_CLASS(AppConfigManager, service::IAppConfigManager, std::make_shared<AppConfigManager>);

    SIGHT_ALLOW_SHARED_FROM_THIS()
    /// Creates slots.
    AppConfigManager();

    /// Does nothing.
    ~AppConfigManager() override;

    /**
     * @brief Sets configuration.
     * @param _configId The identifier of the requested config.
     * @param _replaceFields The associations between the value and the pattern to replace in the config.
     */
    void setConfig(
        const std::string& _configId,
        const FieldAdaptorType& _replaceFields = FieldAdaptorType(),
        bool autoPrefixId                      = true
    ) override;

    /// Get the configuration root.
    data::Object::sptr getConfigRoot() const override;

    /// Calls methods : create, start then update.
    void launch() override;

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    void stopAndDestroy() override;

    /// Creates objects and services from config.
    void create() override;

    /// Starts services specified in config.
    void start() override;

    /// Updates services specified in config.
    void update() override;

    /// Stops services specified in config.
    void stop() override;

    /// Destroys services specified in config.
    void destroy() override;

    /**
     * @brief Adds an existing deferred object to the deferred objects map.
     *
     * @pre The manager mustn't be started.
     *
     * When a configuration is launched, deferred objects may already exist.
     * This loop allow to notify the app config manager that this data exist and can be used by services.
     * Whitout that, the data is considered as null.
     *
     * @param _obj The object to add.
     * @param _uid The uid of this object.
     */
    void addExistingDeferredObject(const data::Object::sptr& _obj, const std::string& _uid);

private:

    typedef std::pair<std::string, bool> ConfigAttribute;
    typedef service::helper::ProxyConnections ProxyConnections;

    /**
     * @brief Starts the module associated to the config
     * @note  Does nothing if the module is already started or if the config id is not specified (ie. if config is set
     *        with setConfig(const core::runtime::config_t& cfgElem) ).
     */
    virtual void startModule();

    data::Object::sptr findObject(const std::string& uid, std::string_view errMsgTail) const;

    data::Object::sptr getNewObject(ConfigAttribute type, const std::string& uid) const;

    static data::Object::sptr getNewObject(
        ConfigAttribute type,
        ConfigAttribute uid = ConfigAttribute("", false)
    );

    data::Object::sptr getObject(ConfigAttribute type, const std::string& uid) const;

    service::IService::sptr getNewService(const std::string& uid, const std::string& implType) const;

    /// Destroyes all created services
    void destroyCreatedServices();

    void processStartItems();

    void processUpdateItems();

    /// Parses objects section and create objects.
    void createObjects(const core::runtime::config_t&);

    /// Parses services and create all the services that can be instantiated.
    void createServices(const core::runtime::config_t&);

    /// Creates a single service from its configuration.
    service::IService::sptr createService(const detail::ServiceConfig& srvConfig);

    /// Parses connection sections and creates them.
    void createConnections();

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    std::string msgHead() const;

    /**
     * @brief Adds objects to the configuration.
     * @param _obj The object to add.
     * @param _id The id of the object.
     */
    void addObjects(data::Object::sptr _obj, const std::string& _id);

    /**
     * @brief Removes objects from the configuration.
     * @param _obj The object to remove.
     * @param _id The id of the remove.
     */
    void removeObjects(data::Object::sptr _obj, const std::string& _id);

    void connectProxy(const std::string& _channel, const ProxyConnections& _connectCfg);

    void destroyProxy(
        const std::string& _channel,
        const ProxyConnections& _proxyCfg,
        const std::string& _key      = "",
        data::Object::csptr _hintObj = nullptr
    );

    void destroyProxies();

    /// Gets a list of UIDs or WIDs, get a friendly printable message.
    static std::string getUIDListAsString(const std::vector<std::string>& uidList);

    typedef std::pair<data::Object::sptr, service::IXMLParser::sptr> CreatedObjectType;

    /// Map containing the object and its XML parser.
    std::unordered_map<std::string, CreatedObjectType> m_createdObjects;

    struct DeferredObjectType
    {
        std::vector<detail::ServiceConfig> m_servicesCfg;
        std::unordered_map<std::string, ProxyConnections> m_proxyCnt;

        /// Copy of the object pointer necessary to access signals/slots when destroying proxy.
        data::Object::sptr m_object;
    };

    /// Map indexed by the object uid, containing all the service configurations that depend on this object.
    std::unordered_map<std::string, DeferredObjectType> m_deferredObjects;

    /// All the identifiers of the deferred services.
    std::unordered_set<std::string> m_deferredServices;

    /// All proxies of created objects, ordered by channel name.
    std::unordered_map<std::string, ProxyConnections> m_createdObjectsProxies;

    struct ServiceProxyType
    {
        std::unordered_map<std::string, ProxyConnections> m_proxyCnt;
    };
    std::unordered_map<std::string, ServiceProxyType> m_servicesProxies;

    /// Identifier of this configuration.
    std::string m_configId;

    typedef std::vector<service::IService::wptr> ServiceContainer;

    /// List of services created in this configuration.
    ServiceContainer m_createdSrv;

    /// List of services started in this configuration.
    ServiceContainer m_startedSrv;

    /// Start ordered list of deferred services.
    std::vector<std::string> m_deferredStartSrv;

    /// Update ordered list of deferred services.
    std::vector<std::string> m_deferredUpdateSrv;

    /// List of created workers
    std::vector<core::thread::Worker::sptr> m_createdWorkers;

    /// Counter used to generate a unique proxy name.
    unsigned int m_proxyID {0};

    /// Keep the connection between the OSR and `addObjects`.
    core::com::Connection m_addObjectConnection;

    /// Keep the connection between the OSR and `removeObjects`.
    core::com::Connection m_removeObjectConnection;

    /// Synchronize start/stop sequences with add/remove objects slots;
    core::mt::Mutex m_mutex;
};

// ------------------------------------------------------------------------

inline std::string AppConfigManager::msgHead() const
{
    return "[" + m_configId + "] ";
}

} // namespace sight::service::detail
