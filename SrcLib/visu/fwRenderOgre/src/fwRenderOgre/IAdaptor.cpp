/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/IAdaptor.hpp"

#include <fwRenderOgre/registry/Adaptor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Slots.hxx>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace fwRenderOgre
{

//------------------------------------------------------------------------------

IAdaptor::IAdaptor() noexcept
{
}

//------------------------------------------------------------------------------

IAdaptor::~IAdaptor() noexcept
{
}

//------------------------------------------------------------------------------

void IAdaptor::info(std::ostream& _sstream )
{
    _sstream << "IAdaptor : ";
    this->fwServices::IService::info( _sstream );
}

//------------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");
    m_layerID = config.get<std::string>("layer");
}

//------------------------------------------------------------------------------

void IAdaptor::initialize()
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

const std::string& IAdaptor::getLayerID() const
{
    return m_layerID;
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

Layer::sptr IAdaptor::getLayer() const
{
    return this->getRenderService()->getLayer(m_layerID);
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
