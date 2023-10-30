/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "service/has_services.hpp"

#include "service/base.hpp"

#include <core/runtime/runtime.hpp>

#include <service/registry.hpp>

namespace sight::service
{

//------------------------------------------------------------------------------

has_services::has_services() noexcept =
    default;

//------------------------------------------------------------------------------

has_services::~has_services() noexcept
{
    SIGHT_ASSERT(
        "Some sub-services were not unregistered, something is probably wrong. "
        "Please use unregisterService() or unregisterServices() before destroying the sub-services owner.",
        m_sub_services.empty()
    );
}

//------------------------------------------------------------------------------

service::base::csptr has_services::get_registered_service(const core::tools::id::type& _id) const
{
    service::base::sptr srv;
    for(const auto& w_service : m_sub_services)
    {
        const service::base::sptr& service = w_service.lock();
        if(service && (service->get_id() == _id))
        {
            srv = service;
            break;
        }
    }

    return srv;
}

//------------------------------------------------------------------------------

void has_services::unregister_service(const core::tools::id::type& _id)
{
    for(auto it_srv = m_sub_services.begin() ; it_srv != m_sub_services.end() ; )
    {
        const service::base::sptr& service = it_srv->lock();
        if(service && (service->get_id() == _id))
        {
            service->stop().wait();
            service::unregister_service(service);
            it_srv = m_sub_services.erase(it_srv);
        }
        else
        {
            it_srv++;
        }
    }
}

//------------------------------------------------------------------------------

void has_services::unregister_service(const base::sptr& _service)
{
    auto iter = std::find_if(
        m_sub_services.begin(),
        m_sub_services.end(),
        [ = ](const service::base::wptr& _adaptor)
        {
            return _adaptor.lock() == _service;
        });

    SIGHT_ASSERT("service '" + _service->get_id() + "' is not registered", iter != m_sub_services.end());
    m_sub_services.erase(iter);

    _service->stop().wait();
    service::unregister_service(_service);
}

//------------------------------------------------------------------------------

service::base::sptr has_services::register_service(const std::string& _impl_type, const std::string& _id)
{
    auto srv = service::add(_impl_type, _id);
    m_sub_services.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

void has_services::unregister_services(const std::string& _classname)
{
    const std::string classname = core::runtime::filter_id(_classname);
    for(auto it_srv = m_sub_services.begin() ; it_srv != m_sub_services.end() ; )
    {
        const service::base::sptr& srv = it_srv->lock();
        if(srv && (classname.empty() || srv->get_classname() == classname))
        {
            srv->stop().wait();
            service::unregister_service(srv);
            it_srv = m_sub_services.erase(it_srv);
        }
        else
        {
            it_srv++;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service
