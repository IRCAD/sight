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
        m_subServices.empty()
    );
}

//------------------------------------------------------------------------------

service::base::csptr has_services::getRegisteredService(const core::tools::id::type& _id) const
{
    service::base::sptr srv;
    for(const auto& wService : m_subServices)
    {
        const service::base::sptr& service = wService.lock();
        if(service && (service->get_id() == _id))
        {
            srv = service;
            break;
        }
    }

    return srv;
}

//------------------------------------------------------------------------------

void has_services::unregisterService(const core::tools::id::type& _id)
{
    for(auto itSrv = m_subServices.begin() ; itSrv != m_subServices.end() ; )
    {
        const service::base::sptr& service = itSrv->lock();
        if(service && (service->get_id() == _id))
        {
            service->stop().wait();
            service::unregisterService(service);
            itSrv = m_subServices.erase(itSrv);
        }
        else
        {
            itSrv++;
        }
    }
}

//------------------------------------------------------------------------------

void has_services::unregisterService(const base::sptr& _service)
{
    auto iter = std::find_if(
        m_subServices.begin(),
        m_subServices.end(),
        [ = ](const service::base::wptr& adaptor)
        {
            return adaptor.lock() == _service;
        });

    SIGHT_ASSERT("service '" + _service->get_id() + "' is not registered", iter != m_subServices.end());
    m_subServices.erase(iter);

    _service->stop().wait();
    service::unregisterService(_service);
}

//------------------------------------------------------------------------------

service::base::sptr has_services::registerService(const std::string& _implType, const std::string& _id)
{
    auto srv = service::add(_implType, _id);
    m_subServices.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

void has_services::unregisterServices(const std::string& _classname)
{
    const std::string classname = core::runtime::filter_id(_classname);
    for(auto itSrv = m_subServices.begin() ; itSrv != m_subServices.end() ; )
    {
        const service::base::sptr& srv = itSrv->lock();
        if(srv && (classname.empty() || srv->get_classname() == classname))
        {
            srv->stop().wait();
            service::unregisterService(srv);
            itSrv = m_subServices.erase(itSrv);
        }
        else
        {
            itSrv++;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service
