/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "service/IHasServices.hpp"

#include "service/IService.hpp"

#include <core/runtime/Runtime.hpp>

#include <service/registry/ObjectService.hpp>

namespace sight::service
{

//------------------------------------------------------------------------------

IHasServices::IHasServices() noexcept =
    default;

//------------------------------------------------------------------------------

IHasServices::~IHasServices() noexcept
{
    SIGHT_ASSERT(
        "Some sub-services were not unregistered, something is probably wrong. "
        "Please use unregisterService() or unregisterServices() before destroying the sub-services owner.",
        m_subServices.empty()
    );
}

//------------------------------------------------------------------------------

service::IService::csptr IHasServices::getRegisteredService(const core::tools::fwID::IDType& _id) const
{
    service::IService::sptr srv;
    for(const auto& wService : m_subServices)
    {
        const service::IService::sptr& service = wService.lock();
        if(service && (service->getID() == _id))
        {
            srv = service;
            break;
        }
    }

    return srv;
}

//------------------------------------------------------------------------------

void IHasServices::unregisterService(const core::tools::fwID::IDType& _id)
{
    for(auto itSrv = m_subServices.begin() ; itSrv != m_subServices.end() ; )
    {
        const service::IService::sptr& service = itSrv->lock();
        if(service && (service->getID() == _id))
        {
            service->stop().wait();
            service::OSR::unregisterService(service);
            itSrv = m_subServices.erase(itSrv);
        }
        else
        {
            itSrv++;
        }
    }
}

//------------------------------------------------------------------------------

void IHasServices::unregisterService(const IService::sptr& _service)
{
    auto iter = std::find_if(
        m_subServices.begin(),
        m_subServices.end(),
        [ = ](const service::IService::wptr& adaptor)
        {
            return adaptor.lock() == _service;
        });

    SIGHT_ASSERT("service '" + _service->getID() + "' is not registered", iter != m_subServices.end());
    m_subServices.erase(iter);

    _service->stop().wait();
    service::OSR::unregisterService(_service);
}

//------------------------------------------------------------------------------

service::IService::sptr IHasServices::registerService(const std::string& _implType, const std::string& _id)
{
    auto srv = service::add(_implType, _id);
    m_subServices.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

void IHasServices::unregisterServices(const std::string& _classname)
{
    const std::string classname = core::runtime::filterID(_classname);
    for(auto itSrv = m_subServices.begin() ; itSrv != m_subServices.end() ; )
    {
        const service::IService::sptr& srv = itSrv->lock();
        if(srv && (classname.empty() || (!classname.empty() && srv->getClassname() == classname)))
        {
            srv->stop().wait();
            service::OSR::unregisterService(srv);
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
