/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/adaptor.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <viz/scene3d/registry/adaptor.hpp>
#include <viz/scene3d/utils.hpp>

namespace sight::viz::scene3d
{

const std::string adaptor::CONFIG = "config.<xmlattr>.";

//------------------------------------------------------------------------------

adaptor::adaptor() noexcept
{
    new_slot(slots::UPDATE_VISIBILITY, &adaptor::update_visibility, this);
    new_slot(slots::APPLY_VISIBILITY, &adaptor::apply_visibility, this);
    new_slot(slots::TOGGLE_VISIBILITY, &adaptor::toggle_visibility, this);
    new_slot(slots::SHOW, &adaptor::show, this);
    new_slot(slots::HIDE, &adaptor::hide, this);
    new_slot(slots::LAZY_UPDATE, [this](){this->lazy_update();});
}

//------------------------------------------------------------------------------

void adaptor::info(std::ostream& _sstream)
{
    _sstream << "adaptor : " << this->get_id();
    this->service::base::info(_sstream);
}

//------------------------------------------------------------------------------

void adaptor::configure_params()
{
    const config_t config = this->get_config();
    m_cfg_layer_id = config.get<std::string>("config.<xmlattr>.layer", "");

    if(auto properties = config.get_child_optional("properties"); not properties.has_value())
    {
        const auto visible = m_visible.lock();
        *visible = config.get<bool>("config.<xmlattr>.visible", true);
    }

    SIGHT_WARN_IF(
        "In [" + this->get_id() + "] adaptor, specifying a layer is now deprecated. "
                                  "Please place the adaptor in the layer tag in the render scene configuration.",
        !m_cfg_layer_id.empty()
    );
}

//------------------------------------------------------------------------------

void adaptor::init()
{
    if(m_render_service.expired())
    {
        auto services_vector = sight::service::get_services("sight::viz::scene3d::render");

        auto& registry = viz::scene3d::registry::get_adaptor_registry();
        auto layer_cfg = registry[this->get_id()];

        auto result =
            std::find_if(
                services_vector.begin(),
                services_vector.end(),
                [layer_cfg](const service::base::sptr& _srv)
            {
                return _srv->get_id() == layer_cfg.render;
            });
        SIGHT_ASSERT("Can't find '" + layer_cfg.render + "' render service.", result != services_vector.end());

        m_render_service = std::dynamic_pointer_cast<viz::scene3d::render>(*result);

        m_layer_id = layer_cfg.layer.empty() ? m_cfg_layer_id : layer_cfg.layer;
    }

    m_render_service.lock()->register_adaptor(dynamic_pointer_cast<self_t>(shared_from_this()));
}

//------------------------------------------------------------------------------

void adaptor::deinit()
{
    auto render_service = m_render_service.lock();
    SIGHT_ASSERT("Render service is null", render_service);
    render_service->unregister_adaptor(dynamic_pointer_cast<self_t>(shared_from_this()));
}

//------------------------------------------------------------------------------

void adaptor::set_layer_id(const std::string& _id)
{
    m_layer_id = _id;
}

//------------------------------------------------------------------------------

const std::string& adaptor::layer_id() const
{
    return m_layer_id;
}

//------------------------------------------------------------------------------

layer::sptr adaptor::layer() const
{
    return this->render_service()->layer(m_layer_id);
}

//------------------------------------------------------------------------------

void adaptor::set_render_service(render::sptr _service)
{
    SIGHT_ASSERT("service not instanced", _service);
    SIGHT_ASSERT("The adaptor ('" + this->get_id() + "') is not stopped", this->stopped());

    m_render_service = _service;
}

//------------------------------------------------------------------------------

render::sptr adaptor::render_service() const
{
    return m_render_service.lock();
}

//------------------------------------------------------------------------------

Ogre::SceneManager* adaptor::get_scene_manager()
{
    return m_render_service.lock()->get_scene_manager(m_layer_id);
}

//------------------------------------------------------------------------------

void adaptor::request_render()
{
    auto render_service = this->render_service();
    if(*m_visible || !m_visibility_applied)
    {
        render_service->request_render();
    }

    m_visibility_applied = true;
}

//-----------------------------------------------------------------------------

void adaptor::update_visibility(bool _visible)
{
    {
        const auto visible = m_visible.lock();
        *visible = _visible;
    }
    m_visibility_applied = false;
    this->set_visible(_visible);
}

//-----------------------------------------------------------------------------

void adaptor::apply_visibility()
{
    m_visibility_applied = false;
    this->set_visible(*m_visible);
}

//------------------------------------------------------------------------------

bool adaptor::visible() const
{
    return *m_visible;
}

//------------------------------------------------------------------------------

void adaptor::toggle_visibility()
{
    this->update_visibility(not * m_visible);
}

//------------------------------------------------------------------------------

void adaptor::show()
{
    this->update_visibility(true);
}

//------------------------------------------------------------------------------

void adaptor::hide()
{
    this->update_visibility(false);
}

//------------------------------------------------------------------------------

void adaptor::set_visible(bool /*unused*/)
{
    SIGHT_WARN("This adaptor has no method 'set_visible(bool)', it needs to be overridden to be called.");
}

//-----------------------------------------------------------------------------

service::connections_t adaptor::auto_connections() const
{
    return {{m_visible, data::object::MODIFIED_SIG, slots::APPLY_VISIBILITY}};
}

//-----------------------------------------------------------------------------

void adaptor::do_update()
{
    updating();
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
