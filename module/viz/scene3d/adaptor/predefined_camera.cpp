/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "module/viz/scene3d/adaptor/predefined_camera.hpp"

#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <viz/scene3d/utils.hpp>

namespace sight::module::viz::scene3d::adaptor
{

predefined_camera::predefined_camera() noexcept
{
    new_slot(slots::RESET, [this](){lazy_update(update_flags::RESET);});
    new_slot(slots::SET_TRANSFORM, [this](){lazy_update(update_flags::SET_TRANSFORM);});
    new_slot(slots::SET_POSITION, [this](){lazy_update(update_flags::SET_POSITION);});
    new_slot(slots::NEXT_POSITION, [this](){lazy_update(update_flags::NEXT_POSITION);});
    new_slot(slots::PREV_POSITION, [this](){lazy_update(update_flags::PREV_POSITION);});
}

//-----------------------------------------------------------------------------

void predefined_camera::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    m_priority              = config.get(CONFIG + "priority", m_priority);
    m_layer_order_dependant = config.get(CONFIG + "layerOrderDependant", m_layer_order_dependant);

    m_manual_rotation = config.get(CONFIG + "mouseRotation", m_manual_rotation);

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
    adaptor::init();

    const auto layer = this->layer();

    Ogre::Vector3 view_up_axis = sight::viz::scene3d::interactor::DEFAULT_VIEW_UP;
    if(const auto& view_up = m_view_up.const_lock(); view_up)
    {
        const auto view_up_matrix = sight::viz::scene3d::utils::to_ogre_matrix(view_up.get_shared());
        view_up_axis = Ogre::Vector3(view_up_matrix[0][1], view_up_matrix[1][1], view_up_matrix[2][1]);
    }

    m_interactor =
        std::make_shared<sight::viz::scene3d::interactor::predefined_position_interactor>(
            layer,
            m_layer_order_dependant,
            m_camera_positions,
            *m_position,
            m_animate,
            *m_follow_orientation,
            m_zoom,
            view_up_axis
        );

    m_interactor->set_mouse_rotation(m_manual_rotation);

    layer->add_interactor(m_interactor, m_priority);

    this->updating();
}

//------------------------------------------------------------------------------

void predefined_camera::reset()
{
    Ogre::Vector3 view_up_axis = sight::viz::scene3d::interactor::DEFAULT_VIEW_UP;
    if(const auto& view_up = m_view_up.const_lock(); view_up)
    {
        const auto view_up_matrix = sight::viz::scene3d::utils::to_ogre_matrix(view_up.get_shared());
        view_up_axis = Ogre::Vector3(view_up_matrix[0][1], view_up_matrix[1][1], view_up_matrix[2][1]);
    }

    m_interactor->set_view_up(view_up_axis);
    m_interactor->reset();
}

//-----------------------------------------------------------------------------

void predefined_camera::updating() noexcept
{
    if(update_needed(update_flags::RESET))
    {
        this->reset();
    }

    // Do first the transform update, otherwise the position will not calculated on the right target
    if(update_needed(update_flags::SET_TRANSFORM))
    {
        if(const auto& transform = m_transform.const_lock(); transform)
        {
            const auto ogre_mat = sight::viz::scene3d::utils::to_ogre_matrix(transform.get_shared());

            m_interactor->set_transform(ogre_mat);
        }
    }

    if(update_needed(update_flags::SET_POSITION))
    {
        if(m_interactor)
        {
            m_interactor->set_position(*m_position);
        }
    }
    else if(update_needed(update_flags::NEXT_POSITION))
    {
        if(m_interactor)
        {
            m_interactor->next_position();
        }
    }
    else if(update_needed(update_flags::PREV_POSITION))
    {
        if(m_interactor)
        {
            m_interactor->previous_position();
        }
    }

    update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void predefined_camera::stopping()
{
    const auto layer = this->layer();
    layer->remove_interactor(m_interactor);
    m_interactor.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

predefined_camera::connections_t predefined_camera::auto_connections() const
{
    service::connections_t connections = {
        {m_transform, sight::data::matrix4::MODIFIED_SIG, slots::SET_TRANSFORM},
        {m_position, sight::data::string::MODIFIED_SIG, slots::SET_POSITION}
    };
    return connections + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
