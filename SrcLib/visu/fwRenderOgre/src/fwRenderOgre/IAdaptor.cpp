/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/IAdaptor.hpp"

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace fwRenderOgre
{

//------------------------------------------------------------------------------

IAdaptor::IAdaptor() throw() :
    m_layerID("")
{
    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

//------------------------------------------------------------------------------

IAdaptor::~IAdaptor() throw()
{
}

//------------------------------------------------------------------------------

void IAdaptor::info(std::ostream &_sstream )
{
    _sstream << "IAdaptor : ";
    this->SuperClass::info( _sstream );
}

//------------------------------------------------------------------------------

void IAdaptor::starting() throw(fwTools::Failed)
{
    /// Install observation
    m_connections->connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());

    SLM_ASSERT("Unable to retrieve the render service's shared pointer (lock)", m_renderService.lock());

    doStart();
}

//------------------------------------------------------------------------------

void IAdaptor::stopping() throw(fwTools::Failed)
{
    /// Stop observation
    m_connections->disconnect();
    doStop();
}
//------------------------------------------------------------------------------

void IAdaptor::swapping() throw(fwTools::Failed)
{
    m_connections->disconnect();
    m_connections->connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
    doSwap();
}

//------------------------------------------------------------------------------

void IAdaptor::updating() throw(fwTools::Failed)
{
    doUpdate();
}

//------------------------------------------------------------------------------

void IAdaptor::configuring() throw ( ::fwTools::Failed )
{
    assert(m_configuration->getName() == "config");

    m_layerID = m_configuration->getAttributeValue("renderer");

    this->doConfigure();
}

//------------------------------------------------------------------------------

void IAdaptor::setLayerID(const std::string& id)
{
    m_layerID = id;
}

//------------------------------------------------------------------------------

void IAdaptor::setRenderService( SRender::sptr service)
{
    /// Preconditions
    SLM_ASSERT("service not instanced", service);
    SLM_ASSERT("The adaptor ('"+this->getID()+"') is not stopped", this->isStopped());

    m_renderService = service;
}

//------------------------------------------------------------------------------

SRender::sptr IAdaptor::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

bool IAdaptor::isAdaptorRegistered(::fwTools::fwID::IDType _adaptorID) const
{
    for(auto subAdaptor : m_subAdaptors)
    {
        if(subAdaptor.lock()->getID() == _adaptorID)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

int IAdaptor::getStartPriority()
{
    return 0;
}

//------------------------------------------------------------------------------

::Ogre::SceneManager* IAdaptor::getSceneManager()
{
    return m_renderService.lock()->getSceneManager(m_layerID);
}

//------------------------------------------------------------------------------

void IAdaptor::registerService( ::fwRenderOgre::IAdaptor::sptr service)
{
    m_subAdaptors.push_back(service);
}

//------------------------------------------------------------------------------

void IAdaptor::unregisterServices(std::string classname)
{
    for(auto service = m_subAdaptors.begin(); service != m_subAdaptors.end(); )
    {
        ::fwServices::IService::sptr srv = service->lock();
        if(!service->expired() && (classname.empty() || ( !classname.empty() && srv->getClassname() == classname)))
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

void fwRenderOgre::IAdaptor::requestRender()
{
    this->getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

} // namespace fwRenderOgre
