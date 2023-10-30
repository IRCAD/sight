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

// cspell:ignore NOLINT

#pragma once

#include "service/config.hpp"

#include <core/mt/types.hpp>
#include <core/runtime/module.hpp>
#include <core/tools/object.hpp>

#include <boost/unordered_map.hpp>

#include <unordered_map>

namespace sight::service
{

class base;

namespace extension
{

/**
 * The factory is a registry of all the services. It allows to retrieve the informations necessary to
 * instantiate the services.
 */
class SERVICE_CLASS_API factory : public core::base_object // NOLINT(bugprone-forward-declaration-namespace)
{
public:

    using key_t         = std::string;
    using key_vector_t  = std::vector<key_t>;
    using string_pair_t = std::pair<std::string, std::string>;

    /// We keep boost here because std implementation does not support a pair of std::string as key
    using support_map_t = boost::unordered_map<string_pair_t, bool>;
    using factoryType   = std::function<std::shared_ptr<service::base>()>;

    SIGHT_DECLARE_CLASS(factory, core::base_object);

    /// Return the unique Instance, create it if required at first access
    SERVICE_API static factory::sptr get();

    /// Parse module information to retrieve service declaration
    SERVICE_API void parse_plugin_infos();

    SERVICE_API void add_service_factory(
        factoryType _factory,
        const std::string& _simpl,
        const std::string& _stype
    );

    SERVICE_API void add_object_factory(
        const std::string& _simpl,
        const std::string& _oimpl
    );

    /// Create a service from a factory type
    SERVICE_API SPTR(base) create(const std::string& _srv_impl) const;

    SERVICE_API void clear_factory();

    /// return a vector of service implementation
    SERVICE_API std::vector<std::string> get_implementation_id_from_object_and_type(
        const std::string& _object,
        const std::string& _type
    ) const;

    /// return the default service implementation for an object
    SERVICE_API std::string get_default_implementation_id_from_object_and_type(
        const std::string& _object,
        const std::string& _type
    ) const;

    /// return the objects registered for a given service.
    SERVICE_API const std::vector<std::string>& get_service_objects(const std::string& _srv_impl) const;

    /// return the service description.
    SERVICE_API std::string get_service_description(const std::string& _srv_impl) const;

    /// return the service capabilities.
    SERVICE_API std::string get_service_tags(const std::string& _srv_impl) const;

    /// Check if the service with given object and implementation is valid
    SERVICE_API bool check_service_validity(const std::string& _object, const std::string& _srv_impl) const;

    /**
     * @brief Check whether an object (object) supports service of type srvType
     * @return true if service type supported
     */
    SERVICE_API bool support(const std::string& _object, const std::string& _srv_type);

    /**
     * @brief Check whether an object (object) supports service of type srvType and implementation srvImpl
     * @return true if service type supported
     */
    SERVICE_API bool support(
        const std::string& _object,
        const std::string& _srv_type,
        const std::string& _srv_impl
    ) const;

    /**
     * @brief returns the registered factory keys.
     */
    SERVICE_API virtual key_vector_t get_factory_keys() const;

private:

    struct service_info
    {
        std::string service_type;
        std::vector<std::string> object_impl;
        std::string desc;
        std::string tags;
        std::shared_ptr<core::runtime::module> module;
        factoryType factory;
        bool objects_set_from_module {false}; // True if the objects implementation are set from the module information
    };
    using srv_reg_container_t = std::unordered_map<key_t, service_info>;

    /**
     * @brief print services informations
     * @warning not thread-safe
     */
    static void print_info_map(const srv_reg_container_t& _src);

    /**
     * @brief Trace services not declared in plugin.xml
     * @warning not thread-safe
     */
    void check_services_not_declared_in_plugin_xml() const;

    /// Container of service information
    srv_reg_container_t m_srv_impl_to_srv_info;
    support_map_t m_support_map;

    mutable core::mt::read_write_mutex m_srv_impl_to_srv_info_mutex;
    mutable core::mt::read_write_mutex m_support_map_mutex;
};

} // namespace extension

} // namespace sight::service
