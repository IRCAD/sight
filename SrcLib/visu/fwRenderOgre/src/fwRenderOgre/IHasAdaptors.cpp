/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/IHasAdaptors.hpp"
#include "fwRenderOgre/IAdaptor.hpp"

#include <fwServices/registry/ObjectService.hpp>

namespace fwRenderOgre
{

//------------------------------------------------------------------------------

IHasAdaptors::IHasAdaptors() throw()
{
}

//------------------------------------------------------------------------------

IHasAdaptors::~IHasAdaptors() throw()
{
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr IHasAdaptors::getRegisteredService(fwTools::fwID::IDType id)
{
    ::fwServices::IService::sptr srv;
    for(auto service : m_subAdaptors)
    {
        ::fwServices::IService::sptr currentSrv = service.lock();
        if(currentSrv && (currentSrv->getID() == id))
        {
            srv = currentSrv;
            break;
        }
    }
    return srv;
}

//------------------------------------------------------------------------------

void IHasAdaptors::unregisterService(fwTools::fwID::IDType id)
{
    for(auto service = m_subAdaptors.begin(); service != m_subAdaptors.end(); )
    {
        ::fwServices::IService::sptr srv = service->lock();
        if(srv && (srv->getID() == id))
        {
            srv->stop();
            ::fwServices::OSR::unregisterService(srv);
            service = m_subAdaptors.erase(service);
        }
        else
        {
            service++;
        }
    }
}

//------------------------------------------------------------------------------

void IHasAdaptors::registerService( ::fwRenderOgre::IAdaptor::sptr service)
{
    m_subAdaptors.push_back(service);
}

//------------------------------------------------------------------------------

void IHasAdaptors::unregisterServices(std::string classname)
{
    for(auto service = m_subAdaptors.begin(); service != m_subAdaptors.end(); )
    {
        ::fwServices::IService::sptr srv = service->lock();
        if(srv && (classname.empty() || ( !classname.empty() && srv->getClassname() == classname)))
        {
            srv->stop();
            ::fwServices::OSR::unregisterService(srv);
            service = m_subAdaptors.erase(service);
        }
        else
        {
            service++;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace fwRenderOgre
