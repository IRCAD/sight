/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "modules/viz/sample/SMesh.hpp"

#include <core/com/Slots.hxx>

#include <service/op/Add.hpp>

#include <ui/base/GuiRegistry.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace sight::module::viz::sample
{

const core::com::Slots::SlotKeyType SMesh::s_UPDATE_CAM_POSITION_SLOT  = "updateCamPosition";
static const core::com::Slots::SlotKeyType s_UPDATE_CAM_TRANSFORM_SLOT = "updateCamTransform";

const core::com::Signals::SignalKeyType SMesh::s_CAM_UPDATED_SIG = "camUpdated";

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

void SMesh::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

void SMesh::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );
    const auto genericSceneId = this->getID() + "-genericScene";
    sight::ui::base::GuiRegistry::registerSIDContainer(genericSceneId, qtContainer);

    auto mesh = m_mesh.lock();

    // create and register the render service
    // create the frame configuration
    service::IService::ConfigType renderConfig;
    renderConfig.put("scene.background.<xmlattr>.color", "#36393E");
    renderConfig.put("scene.layer.<xmlattr>.id", "default");
    renderConfig.put("scene.layer.<xmlattr>.order", "1");

    service::IService::ConfigType interactorCfg;
    interactorCfg.put("<xmlattr>.uid", this->getID() + "interactorAdaptor");
    service::IService::ConfigType negatoCfg;
    negatoCfg.put("<xmlattr>.uid", this->getID() + "meshAdaptor");
    service::IService::ConfigType cameraCfg;
    cameraCfg.put("<xmlattr>.uid", this->getID() + "cameraAdaptor");

    renderConfig.add_child("scene.adaptor", interactorCfg);
    renderConfig.add_child("scene.adaptor", negatoCfg);
    renderConfig.add_child("scene.adaptor", cameraCfg);

    m_renderSrv = service::add("::sight::viz::scene3d::SRender");
    m_renderSrv->setConfiguration(renderConfig);
    m_renderSrv->setID(genericSceneId);
    m_renderSrv->configure();

    service::IService::ConfigType interactorConfig;
    interactorConfig.put("config.<xmlattr>.layer", "default");
    m_interactorSrv = service::add("::sight::module::viz::scene3d::adaptor::STrackballCamera");
    m_interactorSrv->setConfiguration(interactorConfig);
    m_interactorSrv->setID(this->getID() + "interactorAdaptor");
    m_interactorSrv->configure();

    service::IService::ConfigType meshConfig;
    meshConfig.put("config.<xmlattr>.layer", "default");
    m_meshSrv = service::add("::sight::module::viz::scene3d::adaptor::SMesh");
    m_meshSrv->setConfiguration(meshConfig);
    m_meshSrv->registerInOut(std::const_pointer_cast<data::Object>(mesh->getConstSptr()), "mesh", true);
    m_meshSrv->setID(this->getID() + "meshAdaptor");
    m_meshSrv->configure();

    m_cameraTransform = data::Matrix4::New();
    m_connections.connect(
        m_cameraTransform,
        data::Object::s_MODIFIED_SIG,
        this->getSptr(),
        s_UPDATE_CAM_TRANSFORM_SLOT
    );

    service::IService::ConfigType cameraConfig;
    cameraConfig.put("config.<xmlattr>.layer", "default");
    m_cameraSrv = service::add("::sight::module::viz::scene3d::adaptor::SCamera");
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

service::IService::KeyConnectionsMap SMesh::getAutoConnections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    KeyConnectionsMap connections;
    connections.push(s_MESH_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
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

    sight::ui::base::GuiRegistry::unregisterSIDContainer(this->getID() + "-genericScene");

    service::OSR::unregisterService(m_cameraSrv);
    service::OSR::unregisterService(m_meshSrv);
    service::OSR::unregisterService(m_interactorSrv);
    service::OSR::unregisterService(m_renderSrv);

    m_cameraSrv.reset();
    m_meshSrv.reset();
    m_interactorSrv.reset();
    m_renderSrv.reset();

    m_connections.disconnect();
    m_cameraTransform.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void SMesh::updateCamPosition(data::Matrix4::sptr _transform)
{
    m_cameraTransform->shallowCopy(_transform);
    m_cameraSrv->update().wait();
}

//------------------------------------------------------------------------------

void SMesh::updateCamTransform()
{
    {
        core::com::Connection::Blocker block(m_sigCamUpdated->getConnection(this->slot(s_UPDATE_CAM_TRANSFORM_SLOT)));
        m_sigCamUpdated->asyncEmit(m_cameraTransform);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::sample.
