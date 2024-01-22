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

#include "service/registry.hpp"

#include "service/base.hpp"
#include "service/extension/factory.hpp"

#include <core/lazy_instantiator.hpp>
#include <core/logic_stamp.hpp>
#include <core/mt/types.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/failed.hpp>
#include <core/tools/id.hpp>
#include <core/tools/object.hpp>

#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>

namespace sight::service
{

/**
 * @brief maintain the relation between objects and services
 *
 * The API of ObjectService should not be directly invoked,
 * service/op methods (i.e. add, get,...) should be used instead
 */
class SERVICE_CLASS_API object_service : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(object_service, core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    using service_vector_t = std::set<SPTR(service::base)>;

    /// Return some informations contain in the registry
    SERVICE_API std::string get_registry_information() const;

    /**
     * @brief Register the service alone
     *
     * @param _service Service to add to the OSR
     */
    SERVICE_API void register_service(service::base::sptr _service);

    /**
     * @brief Remove the service (service) from the m_container
     *
     * @param _service Service whose key should be removed
     */
    SERVICE_API void unregister_service(service::base::sptr _service);

    /**
     * @name Some useful getters
     */

    //@{

    /// @brief Returns all services
    const service_vector_t& get_services() const;
    /**
     * @brief Return a container with all services of type SERVICE registered in m_container
     * @note Services may be associated to different object
     */
    template<class SERVICE>
    std::set<SPTR(SERVICE)> get_services() const;

    /**
     * @brief Return registered services matching serviceType
     * @note Invoke get_services( data::object::sptr , const std::string & ) for each registered object
     */
    SERVICE_API service_vector_t get_services(const std::string& _service_type) const;

    //@}

private:

    /// Registered services
    service_vector_t m_services;

    mutable core::mt::read_write_mutex m_container_mutex;
};

//------------------------------------------------------------------------------

std::string object_service::get_registry_information() const
{
    std::stringstream info;
    data::object::csptr previous_obj;
    core::mt::read_lock lock(m_container_mutex);

    for(const auto& service : m_services)
    {
        info << "Service : uid = " << service->get_id() << " , classname = " << service->get_classname()
        << " , service is stopped = " << (service->stopped() ? "true" : "false") << std::endl;
    }

    return info.str();
}

//------------------------------------------------------------------------------

void object_service::register_service(service::base::sptr _service)
{
    core::mt::write_lock write_lock(m_container_mutex);
    m_services.insert(_service);
}

//------------------------------------------------------------------------------

void object_service::unregister_service(service::base::sptr _service)
{
    core::mt::write_lock write_lock(m_container_mutex);

    SIGHT_ASSERT(
        "The service ( " + _service->get_id() + " ) must be stopped before being unregistered.",
        _service->stopped()
    );

    auto it = m_services.find(_service);
    SIGHT_THROW_IF("service '" + _service->get_id() + "' is not found in the OSR", it == m_services.end());
    m_services.erase(it);
}

//------------------------------------------------------------------------------

object_service::service_vector_t object_service::get_services(const std::string& _service_type) const
{
    const std::string service_type = core::runtime::filter_id(_service_type);
    service_vector_t services;
    core::mt::read_lock lock(m_container_mutex);

    for(const auto& srv : m_services)
    {
        if(srv->is_a(service_type))
        {
            services.insert(srv);
        }
    }

    return services;
}

//------------------------------------------------------------------------------

const service::service_vector_t& object_service::get_services() const
{
    return m_services;
}

//------------------------------------------------------------------------------

service::object_service::sptr get()
{
    return core::lazy_instantiator<service::object_service>::get_instance();
}

//------------------------------------------------------------------------------

std::string get_registry_information()
{
    return service::get()->get_registry_information();
}

//------------------------------------------------------------------------------

const service::service_vector_t& get_services()
{
    return service::get()->get_services();
}

//------------------------------------------------------------------------------

service::object_service::service_vector_t get_services(const std::string& _service_type)
{
    return service::get()->get_services(_service_type);
}

//------------------------------------------------------------------------------

void register_service(service::base::sptr _service)
{
    service::get()->register_service(_service);
}

//------------------------------------------------------------------------------

void unregister_service(service::base::sptr _service)
{
    service::get()->unregister_service(_service);
}

//------------------------------------------------------------------------------

} // namespace sight::service
