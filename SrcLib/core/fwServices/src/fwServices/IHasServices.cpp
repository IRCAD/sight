/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IHasServices.hpp"

#include "fwServices/IService.hpp"
#include <fwServices/registry/ObjectService.hpp>

namespace fwServices
{

//------------------------------------------------------------------------------

IHasServices::IHasServices() noexcept
{
}

//------------------------------------------------------------------------------

IHasServices::~IHasServices() noexcept
{
    SLM_ASSERT("Some sub-services were not unregistered, something is probably wrong. "
               "Please use unregisterService() or unregisterServices() before destroying the sub-services owner.",
               m_subServices.empty());
}

//------------------------------------------------------------------------------

::fwServices::IService::csptr IHasServices::getRegisteredService(const fwTools::fwID::IDType& _id) const
{
    ::fwServices::IService::sptr srv;
    for(const auto& wService : m_subServices)
    {
        const ::fwServices::IService::sptr& service = wService.lock();
        if(service && (service->getID() == _id))
        {
            srv = service;
            break;
        }
    }
    return srv;
}

//------------------------------------------------------------------------------

void IHasServices::unregisterService(const fwTools::fwID::IDType& _id)
{
    for(auto itSrv = m_subServices.cbegin(); itSrv != m_subServices.cend(); )
    {
        const ::fwServices::IService::sptr& service = itSrv->lock();
        if(service && (service->getID() == _id))
        {
            service->stop();
            ::fwServices::OSR::unregisterService(service);
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
    auto iter = std::find_if(m_subServices.begin(), m_subServices.end(),
                             [ = ](const ::fwServices::IService::wptr& adaptor)
        {
            return adaptor.lock() == _service;
        });

    SLM_ASSERT("service '" + _service->getID() + "' is not registered", iter != m_subServices.end());
    m_subServices.erase(iter);

    _service->stop();
    ::fwServices::OSR::unregisterService(_service);
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr IHasServices::registerService(const std::string& _implType, const std::string& _id )
{
    auto srv = ::fwServices::add(_implType, _id);
    m_subServices.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

void IHasServices::unregisterServices(const std::string& _classname)
{
    for(auto itSrv = m_subServices.begin(); itSrv != m_subServices.end(); )
    {
        const ::fwServices::IService::sptr& srv = itSrv->lock();
        if(srv && (_classname.empty() || ( !_classname.empty() && srv->getClassname() == _classname)))
        {
            srv->stop();
            ::fwServices::OSR::unregisterService(srv);
            itSrv = m_subServices.erase(itSrv);
        }
        else
        {
            itSrv++;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace fwServices
