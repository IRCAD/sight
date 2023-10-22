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
#include "app/config_manager.hpp"
#include "app/helper/config.hpp"

#include "core/com/helper/proxy_connections.hpp"

#include "service/base.hpp"
#include "service/extension/config.hpp"
#include "service/object_parser.hpp"

#include <core/com/has_slots.hpp>
#include <core/com/helper/sig_slot_connection.hpp>

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sight::data
{

class composite;

} // namespace sight::data

namespace sight::app::detail
{

/**
 * @brief This class provides an API to manage config template.
 *
 * @section Slots Slots
 * - \b addObject(data::object::sptr, const std::string&): adds objects to the configuration.
 * - \b removeObject(data::object::sptr, const std::string&): removes objects from the configuration.
 */
class config_manager : public app::config_manager,
                       public core::com::has_slots
{
public:

    SIGHT_DECLARE_CLASS(config_manager, app::config_manager);

    SIGHT_ALLOW_SHARED_FROM_THIS()
    /// Creates slots.
    config_manager();

    /// Does nothing.
    ~config_manager() override;

    /**
     * @brief Sets configuration.
     * @param _configId The identifier of the requested config.
     * @param _replaceFields The associations between the value and the pattern to replace in the config.
     */
    void setConfig(
        const std::string& _config_id,
        const field_adaptor_t& _replace_fields = field_adaptor_t(),
        bool _auto_prefix_id                   = true
    ) override;

    /// Get the configuration root.
    data::object::sptr getConfigRoot() const override;

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
    void addExistingDeferredObject(const data::object::sptr& _obj, const std::string& _uid);

private:

    typedef std::pair<std::string, bool> ConfigAttribute;
    typedef core::com::helper::proxy_connections proxy_connections;

    /**
     * @brief Starts the module associated to the config
     * @note  Does nothing if the module is already started or if the config id is not specified (ie. if config is set
     *        with setConfig(const core::runtime::config_t& cfgElem) ).
     */
    virtual void start_module();

    data::object::sptr findObject(const std::string& _uid, std::string_view _err_msg_tail) const;

    data::object::sptr getNewObject(ConfigAttribute _type, const std::string& _uid) const;

    static data::object::sptr getNewObject(
        ConfigAttribute _type,
        ConfigAttribute _uid = ConfigAttribute("", false)
    );

    data::object::sptr getObject(ConfigAttribute _type, const std::string& _uid) const;

    service::base::sptr getNewService(const std::string& _uid, const std::string& _impl_type) const;

    /// Destroyes all created services
    void destroyCreatedServices();

    void processStartItems();

    void processUpdateItems();

    /// Parses objects section and create objects.
    void createObjects(const core::runtime::config_t&);

    /// Parses services and create all the services that can be instantiated.
    void createServices(const core::runtime::config_t&);

    /// Creates a single service from its configuration.
    service::base::sptr createService(const detail::service_config& _srv_config);

    /// Parses connection sections and creates them.
    void createConnections();

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    std::string msgHead() const;

    /**
     * @brief Adds objects to the configuration.
     * @param _obj The object to add.
     * @param _id The id of the object.
     */
    void addObjects(data::object::sptr _obj, const std::string& _id);

    /**
     * @brief Removes objects from the configuration.
     * @param _obj The object to remove.
     * @param _id The id of the remove.
     */
    void removeObjects(data::object::sptr _obj, const std::string& _id);

    void connectProxy(const std::string& _channel, const proxy_connections& _connect_cfg);

    void destroyProxy(
        const std::string& _channel,
        const proxy_connections& _proxy_cfg,
        const std::string& _key       = "",
        data::object::csptr _hint_obj = nullptr
    );

    void destroyProxies();

    /// Gets a list of UIDs or WIDs, get a friendly printable message.
    static std::string getUIDListAsString(const std::vector<std::string>& _uid_list);

    typedef std::pair<data::object::sptr, service::object_parser::sptr> created_object_t;

    /// Map containing the object and its XML parser.
    std::unordered_map<std::string, created_object_t> m_createdObjects;

    struct deferred_object_t
    {
        std::vector<detail::service_config> m_servicesCfg;
        std::unordered_map<std::string, proxy_connections> m_proxyCnt;

        /// Copy of the object pointer necessary to access signals/slots when destroying proxy.
        data::object::sptr m_object;
    };

    /// Map indexed by the object uid, containing all the service configurations that depend on this object.
    std::unordered_map<std::string, deferred_object_t> m_deferredObjects;

    /// All the identifiers of the deferred services.
    std::unordered_set<std::string> m_deferredServices;

    /// All proxies of created objects, ordered by channel name.
    std::unordered_map<std::string, proxy_connections> m_createdObjectsProxies;

    struct service_proxy_t
    {
        std::unordered_map<std::string, proxy_connections> m_proxyCnt;
    };
    std::unordered_map<std::string, service_proxy_t> m_servicesProxies;

    /// Identifier of this configuration.
    std::string m_configId;

    typedef std::vector<service::base::wptr> ServiceContainer;

    /// List of services created in this configuration.
    ServiceContainer m_createdSrv;

    /// List of services started in this configuration.
    ServiceContainer m_startedSrv;

    /// Start ordered list of deferred services.
    std::vector<std::string> m_deferredStartSrv;

    /// Update ordered list of deferred services.
    std::vector<std::string> m_deferredUpdateSrv;

    /// List of created workers
    std::vector<core::thread::worker::sptr> m_createdWorkers;

    /// Counter used to generate a unique proxy name.
    unsigned int m_proxyID {0};

    /// Keep the connection between the OSR and `addObjects`.
    core::com::connection m_addObjectConnection;

    /// Keep the connection between the OSR and `removeObjects`.
    core::com::connection m_removeObjectConnection;

    /// Synchronize start/stop sequences with add/remove objects slots;
    core::mt::mutex m_mutex;
};

// ------------------------------------------------------------------------

inline std::string config_manager::msgHead() const
{
    return "[" + m_configId + "] ";
}

} // namespace sight::app::detail
