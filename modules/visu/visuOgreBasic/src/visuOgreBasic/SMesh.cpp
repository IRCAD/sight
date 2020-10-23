/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "visuOgreBasic/SMesh.hpp"

#include <fwGui/GuiRegistry.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/op/Add.hpp>

#include <fwCom/Slots.hxx>

namespace visuOgreBasic
{

const ::fwCom::Slots::SlotKeyType SMesh::s_UPDATE_CAM_POSITION_SLOT  = "updateCamPosition";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_TRANSFORM_SLOT = "updateCamTransform";
const ::fwCom::Signals::SignalKeyType SMesh::s_CAM_UPDATED_SIG       = "camUpdated";

static const std::string s_MESH_INPUT = "mesh";

//------------------------------------------------------------------------------

SMesh::SMesh() noexcept
{
    newSlot(s_UPDATE_CAM_POSITION_SLOT, &SMesh::updateCamPosition, this);
    newSlot(s_UPDATE_CAM_TRANSFORM_SLOT, &SMesh::updateCamTransform, this);

    m_sigCamUpdated = newSignal<CamUpdatedSignalType>(s_CAM_UPDATED_SIG);
}

//------------------------------------------------------------------------------

SMesh::~SMesh() noexcept
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMesh::getAutoConnections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    KeyConnectionsMap connections;
    connections.push(s_MESH_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SMesh::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();

    const auto config = this->getConfigTree();

    auto inoutsCfg = config.equal_range("in");
    for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
    {
        if (itCfg->second.get<std::string>("<xmlattr>.key") == s_MESH_INPUT)
        {
            m_meshAutoConnect =
                itCfg->second.get_optional<std::string>("<xmlattr>.autoConnect").get_value_or("no") == "yes";
        }
    }
}

//------------------------------------------------------------------------------

void SMesh::starting()
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    const auto genericSceneId = this->getID() + "-genericScene";
    ::fwGui::GuiRegistry::registerSIDContainer(genericSceneId, qtContainer);

    auto mesh = this->getLockedInput< ::fwData::Object>(s_MESH_INPUT);

    // create and register the render service
    // create the frame configuration
    ::fwServices::IService::ConfigType renderConfig;
    renderConfig.put("scene.background.<xmlattr>.color", "#36393E");
    renderConfig.put("scene.layer.<xmlattr>.id", "default");
    renderConfig.put("scene.layer.<xmlattr>.order", "1");

    ::fwServices::IService::ConfigType interactorCfg;
    interactorCfg.put("<xmlattr>.uid", this->getID() + "interactorAdaptor");
    ::fwServices::IService::ConfigType negatoCfg;
    negatoCfg.put("<xmlattr>.uid", this->getID() + "meshAdaptor");
    ::fwServices::IService::ConfigType cameraCfg;
    cameraCfg.put("<xmlattr>.uid", this->getID() + "cameraAdaptor");

    renderConfig.add_child("scene.adaptor", interactorCfg);
    renderConfig.add_child("scene.adaptor", negatoCfg);
    renderConfig.add_child("scene.adaptor", cameraCfg);

    m_renderSrv = ::fwServices::add("::fwRenderOgre::SRender");
    m_renderSrv->setConfiguration(renderConfig);
    m_renderSrv->setID(genericSceneId);
    m_renderSrv->configure();

    ::fwServices::IService::ConfigType interactorConfig;
    interactorConfig.put("config.<xmlattr>.layer", "default");
    m_interactorSrv = ::fwServices::add("::visuOgreAdaptor::STrackballCamera");
    m_interactorSrv->setConfiguration(interactorConfig);
    m_interactorSrv->setID(this->getID() + "interactorAdaptor");
    m_interactorSrv->configure();

    ::fwServices::IService::ConfigType meshConfig;
    meshConfig.put("config.<xmlattr>.layer", "default");
    m_meshSrv = ::fwServices::add("::visuOgreAdaptor::SMesh");
    m_meshSrv->setConfiguration(meshConfig);
    m_meshSrv->registerInOut(std::const_pointer_cast< ::fwData::Object>(mesh->getConstSptr()), "mesh", true);
    m_meshSrv->setID(this->getID() + "meshAdaptor");
    m_meshSrv->configure();

    m_cameraTransform = ::fwData::TransformationMatrix3D::New();
    m_connections.connect(m_cameraTransform, ::fwData::Object::s_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_CAM_TRANSFORM_SLOT);

    ::fwServices::IService::ConfigType cameraConfig;
    cameraConfig.put("config.<xmlattr>.layer", "default");
    m_cameraSrv = ::fwServices::add("::visuOgreAdaptor::SCamera");
    m_cameraSrv->setConfiguration(cameraConfig);
    m_cameraSrv->registerInOut(m_cameraTransform->getSptr(), "transform", true);
    m_cameraSrv->setID(this->getID() + "cameraAdaptor");
    m_cameraSrv->configure();

    m_renderSrv->slot("start")->asyncRun();
    m_interactorSrv->slot("start")->asyncRun();
    m_meshSrv->slot("start")->asyncRun();
    m_cameraSrv->slot("start")->asyncRun();
}

//------------------------------------------------------------------------------

void SMesh::updating()
{
}

//------------------------------------------------------------------------------

void SMesh::stopping()
{
    m_cameraSrv->stop().wait();
    m_meshSrv->stop().wait();
    m_interactorSrv->stop().wait();
    m_renderSrv->stop().wait();

    ::fwGui::GuiRegistry::unregisterSIDContainer(this->getID() + "-genericScene");

    ::fwServices::OSR::unregisterService(m_cameraSrv);
    ::fwServices::OSR::unregisterService(m_meshSrv);
    ::fwServices::OSR::unregisterService(m_interactorSrv);
    ::fwServices::OSR::unregisterService(m_renderSrv);

    m_cameraSrv.reset();
    m_meshSrv.reset();
    m_interactorSrv.reset();
    m_renderSrv.reset();

    m_connections.disconnect();
    m_cameraTransform.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void SMesh::updateCamPosition(::fwData::TransformationMatrix3D::sptr _transform)
{
    m_cameraTransform->shallowCopy(_transform);
    m_cameraSrv->update().wait();
}

//------------------------------------------------------------------------------

void SMesh::updateCamTransform()
{
    {
        ::fwCom::Connection::Blocker block(m_sigCamUpdated->getConnection(this->slot(s_UPDATE_CAM_TRANSFORM_SLOT)));
        m_sigCamUpdated->asyncEmit(m_cameraTransform);
    }
}

//------------------------------------------------------------------------------

} // namespace visuOgreBasic
