/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/IAdaptor.hpp"

#include <fwCom/Slots.hxx>

#include <fwRenderOgre/registry/Adaptor.hpp>
#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/helper/Config.hpp>

namespace fwRenderOgre
{

const ::fwCom::Slots::SlotKeyType s_START_OBJECT_SLOT = "startObject";

//------------------------------------------------------------------------------

IAdaptor::IAdaptor() throw()
{
    newSlot(s_START_OBJECT_SLOT, &IAdaptor::doStart, this);
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

void IAdaptor::configuring() throw ( ::fwTools::Failed )
{
    auto config = m_configuration->findConfigurationElement("config");

    SLM_ASSERT("Can't find 'config' tag.", config);

    m_configuration = config;

    m_layerID = m_configuration->getAttributeValue("layer");

    this->doConfigure();
}

//------------------------------------------------------------------------------

void IAdaptor::starting() throw ( ::fwTools::Failed )
{
    if(m_renderService.expired())
    {
        auto servicesVector = ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

        auto& registry       = ::fwRenderOgre::registry::getAdaptorRegistry();
        auto renderServiceId = registry[this->getID()];

        auto result =
            std::find_if(servicesVector.begin(), servicesVector.end(),
                         [renderServiceId](const ::fwServices::IService::sptr& srv)
            {
                return srv->getID() == renderServiceId;
            });
        SLM_ASSERT("Can't find '" + renderServiceId + "' SRender service.", result != servicesVector.end());

        m_renderService = ::fwRenderOgre::SRender::dynamicCast(*result);
    }
    doStart();
}

//------------------------------------------------------------------------------

void IAdaptor::stopping() throw(fwTools::Failed)
{
    doStop();
}
//------------------------------------------------------------------------------

void IAdaptor::swapping() throw(fwTools::Failed)
{
    doSwap();
}

//------------------------------------------------------------------------------

void IAdaptor::updating() throw(fwTools::Failed)
{
    doUpdate();
}

//------------------------------------------------------------------------------

void IAdaptor::connect()
{
    ::fwServices::IService::KeyConnectionsType connections = this->getObjSrvConnections();
    m_objConnection.connect( this->getObject(), this->getSptr(), connections );
}

//------------------------------------------------------------------------------

void IAdaptor::disconnect()
{
    m_objConnection.disconnect();
}

//------------------------------------------------------------------------------

void IAdaptor::setLayerID(const std::string& id)
{
    m_layerID = id;
}

//------------------------------------------------------------------------------

void IAdaptor::setRenderService( SRender::sptr service)
{
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
