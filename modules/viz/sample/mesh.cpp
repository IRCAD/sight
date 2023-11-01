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

const core::com::slots::key_t mesh::UPDATE_CAM_POSITION_SLOT   = "update_cam_position";
static const core::com::slots::key_t UPDATE_CAM_TRANSFORM_SLOT = "updateCamTransform";

const core::com::signals::key_t mesh::CAM_UPDATED_SIG = "cam_updated";

static const std::string MESH_INPUT = "mesh";

//------------------------------------------------------------------------------

mesh::mesh() noexcept
{
    new_slot(UPDATE_CAM_POSITION_SLOT, &mesh::update_cam_position, this);
    new_slot(UPDATE_CAM_TRANSFORM_SLOT, &mesh::update_cam_transform, this);

    m_sig_cam_updated = new_signal<cam_updated_signal_t>(CAM_UPDATED_SIG);
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

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    const auto generic_scene_id = this->get_id() + "-genericScene";
    sight::ui::registry::register_sid_container(generic_scene_id, qt_container);

    auto mesh = m_mesh.lock();

    // create and register the render service
    // create the frame configuration
    service::config_t render_config;
    render_config.put("scene.background.<xmlattr>.color", "#36393E");
    render_config.put("scene.layer.<xmlattr>.id", "default");
    render_config.put("scene.layer.<xmlattr>.order", "1");

    service::config_t interactor_cfg;
    interactor_cfg.put("<xmlattr>.uid", this->get_id() + "interactorAdaptor");
    service::config_t negato_cfg;
    negato_cfg.put("<xmlattr>.uid", this->get_id() + "meshAdaptor");
    service::config_t camera_cfg;
    camera_cfg.put("<xmlattr>.uid", this->get_id() + "cameraAdaptor");

    render_config.add_child("scene.layer.adaptor", interactor_cfg);
    render_config.add_child("scene.layer.adaptor", negato_cfg);
    render_config.add_child("scene.layer.adaptor", camera_cfg);

    m_render_srv = sight::service::add("sight::viz::scene3d::render");
    m_render_srv->set_config(render_config);
    m_render_srv->set_id(generic_scene_id);
    m_render_srv->configure();

    service::config_t interactor_config;
    m_interactor_srv = sight::service::add("sight::module::viz::scene3d::adaptor::trackball_camera");
    m_interactor_srv->set_id(this->get_id() + "interactorAdaptor");
    m_interactor_srv->configure();

    m_mesh_srv = sight::service::add("sight::module::viz::scene3d::adaptor::mesh");
    m_mesh_srv->set_input(std::const_pointer_cast<data::object>(mesh->get_const_sptr()), "mesh", true);
    m_mesh_srv->set_id(this->get_id() + "meshAdaptor");
    m_mesh_srv->configure();

    m_camera_transform = std::make_shared<data::matrix4>();
    m_connections.connect(
        m_camera_transform,
        data::object::MODIFIED_SIG,
        this->get_sptr(),
        UPDATE_CAM_TRANSFORM_SLOT
    );

    m_camera_srv = sight::service::add("sight::module::viz::scene3d::adaptor::camera");
    m_camera_srv->set_inout(m_camera_transform->get_sptr(), "transform", true);
    m_camera_srv->set_id(this->get_id() + "cameraAdaptor");
    m_camera_srv->configure();

    m_render_srv->start().wait();
    m_interactor_srv->start().wait();
    m_mesh_srv->start().wait();
    m_camera_srv->start().wait();
}

//------------------------------------------------------------------------------

service::connections_t mesh::auto_connections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    connections_t connections;
    connections.push(MESH_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void mesh::updating()
{
}

//------------------------------------------------------------------------------

void mesh::stopping()
{
    m_camera_srv->stop().wait();
    m_mesh_srv->stop().wait();
    m_interactor_srv->stop().wait();
    m_render_srv->stop().wait();

    sight::ui::registry::unregister_sid_container(this->get_id() + "-genericScene");

    sight::service::remove(m_camera_srv);
    sight::service::remove(m_mesh_srv);
    sight::service::remove(m_interactor_srv);
    sight::service::remove(m_render_srv);

    m_camera_srv.reset();
    m_mesh_srv.reset();
    m_interactor_srv.reset();
    m_render_srv.reset();

    m_connections.disconnect();
    m_camera_transform.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void mesh::update_cam_position(data::matrix4::sptr _transform)
{
    m_camera_transform->shallow_copy(_transform);
    m_camera_srv->update().wait();
}

//------------------------------------------------------------------------------

void mesh::update_cam_transform()
{
    {
        core::com::connection::blocker block(m_sig_cam_updated->get_connection(this->slot(UPDATE_CAM_TRANSFORM_SLOT)));
        m_sig_cam_updated->async_emit(m_camera_transform);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::sample.
