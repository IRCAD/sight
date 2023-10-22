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

#include "modules/viz/sample/image.hpp"

#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

namespace sight::module::viz::sample
{

static const std::string s_IMAGE_INPUT = "image";

//------------------------------------------------------------------------------

image::image() noexcept =
    default;

//------------------------------------------------------------------------------

image::~image() noexcept =
    default;

//------------------------------------------------------------------------------

void image::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void image::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    const auto generic_scene_id = this->get_id() + "-genericScene";
    sight::ui::registry::register_sid_container(generic_scene_id, qt_container);

    // create and register the render service
    service::config_t render_config;
    render_config.put("scene.background.<xmlattr>.color", "#36393E");
    render_config.put("scene.layer.<xmlattr>.id", "default");
    render_config.put("scene.layer.<xmlattr>.order", "1");
    render_config.put("scene.layer.<xmlattr>.transparency", "");

    service::config_t interactor_cfg;
    interactor_cfg.put("<xmlattr>.uid", this->get_id() + "interactorAdaptor");
    service::config_t negato_cfg;
    negato_cfg.put("<xmlattr>.uid", this->get_id() + "negato3DAdaptor");

    render_config.add_child("scene.layer.adaptor", interactor_cfg);
    render_config.add_child("scene.layer.adaptor", negato_cfg);

    m_renderSrv = sight::service::add("sight::viz::scene3d::render");
    m_renderSrv->set_config(render_config);
    m_renderSrv->set_id(generic_scene_id);

    m_renderSrv->configure();

    m_interactorSrv = sight::service::add("sight::module::viz::scene3d::adaptor::trackball_camera");
    m_interactorSrv->set_id(this->get_id() + "interactorAdaptor");
    m_interactorSrv->configure();

    // Create default transfer function
    m_tf = data::transfer_function::createDefaultTF();

    auto image = m_image.lock();
    service::config_t negato_config;
    negato_config.put("config.<xmlattr>.interactive", "true");
    m_negatoSrv = sight::service::add("sight::module::viz::scene3d::adaptor::negato3d");
    m_negatoSrv->set_config(negato_config);
    m_negatoSrv->set_input(image.get_shared(), "image", true);
    m_negatoSrv->set_inout(m_tf, "tf", true);
    m_negatoSrv->set_id(this->get_id() + "negato3DAdaptor");
    m_negatoSrv->configure();

    m_renderSrv->start().wait();
    m_interactorSrv->start().wait();
    m_negatoSrv->start().wait();
}

//------------------------------------------------------------------------------

service::connections_t image::auto_connections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    connections_t connections;
    connections.push(s_IMAGE_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void image::updating()
{
}

//------------------------------------------------------------------------------

void image::stopping()
{
    m_negatoSrv->stop().wait();
    m_interactorSrv->stop().wait();
    m_renderSrv->stop().wait();

    sight::ui::registry::unregister_sid_container(this->get_id() + "-genericScene");

    sight::service::remove(m_negatoSrv);
    sight::service::remove(m_interactorSrv);
    sight::service::remove(m_renderSrv);

    m_negatoSrv.reset();
    m_interactorSrv.reset();
    m_renderSrv.reset();
    m_tf.reset();

    this->destroy();
}

} // namespace sight::module::viz::sample.
