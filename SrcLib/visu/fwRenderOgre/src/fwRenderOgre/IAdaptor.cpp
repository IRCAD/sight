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
}

//------------------------------------------------------------------------------

IAdaptor::~IAdaptor() throw()
{
}

//------------------------------------------------------------------------------

void IAdaptor::info(std::ostream& _sstream )
{
    _sstream << "IAdaptor : ";
    this->fwServices::IService::info( _sstream );
}

//------------------------------------------------------------------------------

void IAdaptor::starting() throw(fwTools::Failed)
{
    /// Install observation
    m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());

    SLM_ASSERT("Unable to retrieve the render service's shared pointer (lock)", m_renderService.lock());

    doStart();
}

//------------------------------------------------------------------------------

void IAdaptor::stopping() throw(fwTools::Failed)
{
    /// Stop observation
    m_connections.disconnect();
    doStop();
}
//------------------------------------------------------------------------------

void IAdaptor::swapping() throw(fwTools::Failed)
{
    m_connections.disconnect();
    m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
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
    SLM_ASSERT("Can't find 'config' tag.", m_configuration->getName() == "config");

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

void fwRenderOgre::IAdaptor::requestRender()
{
    this->getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

} // namespace fwRenderOgre
