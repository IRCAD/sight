/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
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

::fwServices::IService::csptr IHasAdaptors::getRegisteredService(fwTools::fwID::IDType _id) const
{
    ::fwServices::IService::sptr srv;
    for(auto service : m_subAdaptors)
    {
        ::fwServices::IService::sptr currentSrv = service.lock();
        if(currentSrv && (currentSrv->getID() == _id))
        {
            srv = currentSrv;
            break;
        }
    }
    return srv;
}

//------------------------------------------------------------------------------

void IHasAdaptors::unregisterService(fwTools::fwID::IDType _id)
{
    for(auto service = m_subAdaptors.begin(); service != m_subAdaptors.end(); )
    {
        ::fwServices::IService::sptr srv = service->lock();
        if(srv && (srv->getID() == _id))
        {
            ::fwServices::IService::sptr srv       = service->lock();
            ::fwRenderOgre::IAdaptor::sptr adaptor = ::fwRenderOgre::IAdaptor::dynamicCast(srv);
            adaptor->disconnect();
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

void IHasAdaptors::registerService( ::fwRenderOgre::IAdaptor::sptr _service)
{
    m_subAdaptors.push_back(_service);
}

//------------------------------------------------------------------------------

void IHasAdaptors::unregisterServices(std::string _classname)
{
    for(auto service = m_subAdaptors.begin(); service != m_subAdaptors.end(); )
    {
        ::fwServices::IService::sptr srv = service->lock();
        if(srv && (_classname.empty() || ( !_classname.empty() && srv->getClassname() == _classname)))
        {
            ::fwRenderOgre::IAdaptor::sptr adaptor = ::fwRenderOgre::IAdaptor::dynamicCast(srv);
            adaptor->disconnect();
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
