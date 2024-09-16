/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "modules/viz/scene3d/adaptor/predefined_camera.hpp"

#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <viz/scene3d/utils.hpp>

namespace sight::module::viz::scene3d::adaptor
{

predefined_camera::predefined_camera() noexcept
{
    new_slot(slots::SET_PARAMETER, &predefined_camera::set_parameter, this);
    new_slot(
        slots::NEXT_POSITION,
        [this]()
        {
            if(m_interactor)
            {
                m_interactor->next_position();
            }
        });

    new_slot(
        slots::PREVIOUS_POSITION,
        [this]()
        {
            if(m_interactor)
            {
                m_interactor->previous_position();
            }
        });
    new_slot(slots::UPDATE_TRANSFORM, &predefined_camera::update_transform, this);
}

//-----------------------------------------------------------------------------

void predefined_camera::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    m_priority              = config.get(CONFIG + "priority", m_priority);
    m_layer_order_dependant = config.get(CONFIG + "layerOrderDependant", m_layer_order_dependant);

    m_manual_rotation = config.get(CONFIG + "mouseRotation", m_manual_rotation);

    const auto& default_position = config.get_optional<std::string>(CONFIG + "defaultPosition");
    m_default_position = default_position ? std::make_optional(*default_position) : std::nullopt;

    m_animate = config.get(CONFIG + "animate", m_animate);
    m_zoom    = config.get(CONFIG + "zoom", m_zoom);

    const auto positions = config.get_child("positions");

    const auto pos_cfg = positions.equal_range("position");

    // Configure tracked tool
    for(auto t = pos_cfg.first ; t != pos_cfg.second ; ++t)
    {
        predefined_position_t position;

        const auto posattr = t->second.get_child("<xmlattr>");

        // 1. Name (should be unique).
        position.name = posattr.get<std::string>("name");

        position.rx = posattr.get<float>("rx", 0.F);
        position.ry = posattr.get<float>("ry", 0.F);
        position.rz = posattr.get<float>("rz", 0.F);

        m_camera_positions.push_back(position);
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::starting()
{
    this->initialize();

    const auto layer = this->layer();

    m_interactor =
        std::make_shared<sight::viz::scene3d::interactor::predefined_position_interactor>(
            layer,
            m_layer_order_dependant,
            m_camera_positions,
            m_default_position,
            m_animate,
            m_zoom
        );

    m_interactor->set_mouse_rotation(m_manual_rotation);

    layer->add_interactor(m_interactor, m_priority);

    if(const auto& transform = m_transform.const_lock(); transform)
    {
        const auto ogre_mat = ::sight::viz::scene3d::utils::to_ogre_matrix(transform.get_shared());

        m_interactor->set_transform(ogre_mat);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void predefined_camera::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void predefined_camera::stopping()
{
    const auto layer = this->layer();
    layer->remove_interactor(m_interactor);
    m_interactor.reset();
}

//------------------------------------------------------------------------------

void predefined_camera::set_parameter(ui::parameter_t _value, std::string _key)
{
    if(m_interactor)
    {
        m_interactor->set_parameter(_value, _key);
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::update_transform()
{
    if(const auto& transform = m_transform.const_lock(); transform)
    {
        const auto ogre_mat = ::sight::viz::scene3d::utils::to_ogre_matrix(transform.get_shared());

        m_interactor->set_transform(ogre_mat);
    }
}

//-----------------------------------------------------------------------------

predefined_camera::connections_t predefined_camera::auto_connections() const
{
    service::connections_t connections = {
        {REGISTRATION_TRANSFORM_IN, sight::data::matrix4::MODIFIED_SIG, slots::UPDATE_TRANSFORM}
    };
    return connections + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
