/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/viz/sample/mesh.hpp"

#include <core/com/slots.hxx>

#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

namespace sight::module::viz::sample
{

const core::com::slots::key_t mesh::UPDATE_CAM_POSITION_SLOT   = "updateCamPosition";
static const core::com::slots::key_t UPDATE_CAM_TRANSFORM_SLOT = "updateCamTransform";

const core::com::signals::key_t mesh::CAM_UPDATED_SIG = "camUpdated";

static const std::string s_MESH_INPUT = "mesh";

//------------------------------------------------------------------------------

mesh::mesh() noexcept
{
    new_slot(UPDATE_CAM_POSITION_SLOT, &mesh::updateCamPosition, this);
    new_slot(UPDATE_CAM_TRANSFORM_SLOT, &mesh::updateCamTransform, this);

    m_sigCamUpdated = new_signal<CamUpdatedSignalType>(CAM_UPDATED_SIG);
}

//------------------------------------------------------------------------------

mesh::~mesh() noexcept =
    default;

//------------------------------------------------------------------------------

void mesh::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void mesh::starting()
{
    this->sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    const auto genericSceneId = this->get_id() + "-genericScene";
    sight::ui::registry::registerSIDContainer(genericSceneId, qtContainer);

    auto mesh = m_mesh.lock();

    // create and register the render service
    // create the frame configuration
    service::config_t renderConfig;
    renderConfig.put("scene.background.<xmlattr>.color", "#36393E");
    renderConfig.put("scene.layer.<xmlattr>.id", "default");
    renderConfig.put("scene.layer.<xmlattr>.order", "1");

    service::config_t interactorCfg;
    interactorCfg.put("<xmlattr>.uid", this->get_id() + "interactorAdaptor");
    service::config_t negatoCfg;
    negatoCfg.put("<xmlattr>.uid", this->get_id() + "meshAdaptor");
    service::config_t cameraCfg;
    cameraCfg.put("<xmlattr>.uid", this->get_id() + "cameraAdaptor");

    renderConfig.add_child("scene.layer.adaptor", interactorCfg);
    renderConfig.add_child("scene.layer.adaptor", negatoCfg);
    renderConfig.add_child("scene.layer.adaptor", cameraCfg);

    m_renderSrv = sight::service::add("sight::viz::scene3d::render");
    m_renderSrv->set_config(renderConfig);
    m_renderSrv->set_id(genericSceneId);
    m_renderSrv->configure();

    service::config_t interactorConfig;
    m_interactorSrv = sight::service::add("sight::module::viz::scene3d::adaptor::trackball_camera");
    m_interactorSrv->set_id(this->get_id() + "interactorAdaptor");
    m_interactorSrv->configure();

    m_meshSrv = sight::service::add("sight::module::viz::scene3d::adaptor::mesh");
    m_meshSrv->set_input(std::const_pointer_cast<data::object>(mesh->get_const_sptr()), "mesh", true);
    m_meshSrv->set_id(this->get_id() + "meshAdaptor");
    m_meshSrv->configure();

    m_cameraTransform = std::make_shared<data::matrix4>();
    m_connections.connect(
        m_cameraTransform,
        data::object::MODIFIED_SIG,
        this->get_sptr(),
        UPDATE_CAM_TRANSFORM_SLOT
    );

    m_cameraSrv = sight::service::add("sight::module::viz::scene3d::adaptor::camera");
    m_cameraSrv->set_inout(m_cameraTransform->get_sptr(), "transform", true);
    m_cameraSrv->set_id(this->get_id() + "cameraAdaptor");
    m_cameraSrv->configure();

    m_renderSrv->start().wait();
    m_interactorSrv->start().wait();
    m_meshSrv->start().wait();
    m_cameraSrv->start().wait();
}

//------------------------------------------------------------------------------

service::connections_t mesh::auto_connections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    connections_t connections;
    connections.push(s_MESH_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void mesh::updating()
{
}

//------------------------------------------------------------------------------

void mesh::stopping()
{
    m_cameraSrv->stop().wait();
    m_meshSrv->stop().wait();
    m_interactorSrv->stop().wait();
    m_renderSrv->stop().wait();

    sight::ui::registry::unregisterSIDContainer(this->get_id() + "-genericScene");

    sight::service::remove(m_cameraSrv);
    sight::service::remove(m_meshSrv);
    sight::service::remove(m_interactorSrv);
    sight::service::remove(m_renderSrv);

    m_cameraSrv.reset();
    m_meshSrv.reset();
    m_interactorSrv.reset();
    m_renderSrv.reset();

    m_connections.disconnect();
    m_cameraTransform.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void mesh::updateCamPosition(data::matrix4::sptr _transform)
{
    m_cameraTransform->shallow_copy(_transform);
    m_cameraSrv->update().wait();
}

//------------------------------------------------------------------------------

void mesh::updateCamTransform()
{
    {
        core::com::connection::blocker block(m_sigCamUpdated->get_connection(this->slot(UPDATE_CAM_TRANSFORM_SLOT)));
        m_sigCamUpdated->async_emit(m_cameraTransform);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::sample.
