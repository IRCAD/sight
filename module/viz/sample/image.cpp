/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include "module/viz/sample/image.hpp"

#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

namespace sight::module::viz::sample
{

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
        this->get_container()
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

    m_render_srv = sight::service::add("sight::viz::scene3d::render");
    m_render_srv->set_config(render_config);
    m_render_srv->set_id(generic_scene_id);

    m_render_srv->configure();

    m_interactor_srv = sight::service::add("sight::module::viz::scene3d::adaptor::trackball_camera");
    m_interactor_srv->set_id(this->get_id() + "interactorAdaptor");
    m_interactor_srv->configure();

    // Create default transfer function
    m_tf = data::transfer_function::create_default_tf();

    auto image = m_image.lock();
    service::config_t negato_config;
    negato_config.put("config.<xmlattr>.interactive", "true");
    m_negato_srv = sight::service::add("sight::module::viz::scene3d::adaptor::negato3d");
    m_negato_srv->set_config(negato_config);
    m_negato_srv->set_input(image.get_shared(), "image", true);
    m_negato_srv->set_inout(m_tf, "tf", true);
    m_negato_srv->set_id(this->get_id() + "negato3DAdaptor");
    m_negato_srv->configure();

    m_render_srv->start().wait();
    m_interactor_srv->start().wait();
    m_negato_srv->start().wait();
}

//------------------------------------------------------------------------------

void image::updating()
{
}

//------------------------------------------------------------------------------

void image::stopping()
{
    m_negato_srv->stop().wait();
    m_interactor_srv->stop().wait();
    m_render_srv->stop().wait();

    sight::ui::registry::unregister_sid_container(this->get_id() + "-genericScene");

    sight::service::remove(m_negato_srv);
    sight::service::remove(m_interactor_srv);
    sight::service::remove(m_render_srv);

    m_negato_srv.reset();
    m_interactor_srv.reset();
    m_render_srv.reset();
    m_tf.reset();

    this->destroy();
}

} // namespace sight::module::viz::sample.
