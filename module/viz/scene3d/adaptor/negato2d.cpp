/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "module/viz/scene3d/adaptor/negato2d.hpp"

#include <core/com/signals.hpp>
#include <core/com/slots.hxx>
#include <core/ptree.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OgreSceneNode.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

negato2d::negato2d() noexcept
{
    new_signal<signals::slice_index_changed_t>(signals::SLICE_INDEX_CHANGED);
    m_planes.resize(1);
}

//------------------------------------------------------------------------------

void negato2d::configuring(const config_t& _config)
{
    negato::configuring(_config);

    const auto axis = core::ptree::get_and_deprecate<std::string>(
        _config,
        CONFIG + "orientation",
        CONFIG + "sliceIndex",
        "26.0",
        "axial"
    );
    if(axis == "axial")
    {
        m_planes[0].second = axis_t::z_axis;
    }
    else if(axis == "frontal")
    {
        m_planes[0].second = axis_t::y_axis;
    }
    else if(axis == "sagittal")
    {
        m_planes[0].second = axis_t::x_axis;
    }

    m_slices_cross = core::ptree::get_and_deprecate<bool>(
        _config,
        CONFIG + "cross",
        CONFIG + "slicesCross",
        "26.0",
        true
    );
}

//------------------------------------------------------------------------------

void negato2d::change_slice_type(int _from, int _to)
{
    const auto image = m_image.lock();

    const auto to_axis   = static_cast<axis_t>(_to);
    const auto from_axis = static_cast<axis_t>(_from);

    const auto plane_axis = m_planes[0].second;
    const auto new_axis   = m_planes[0].second == to_axis ? from_axis
                                                          : plane_axis
                            == from_axis ? to_axis : plane_axis;

    if(plane_axis != new_axis)
    {
        m_planes[0].second = new_axis;
        this->render_service()->make_current();

        const auto spacing = sight::viz::scene3d::utils::get_ogre_spacing(*image);
        m_planes[0].first->update(m_planes[0].second, spacing, m_enable_alpha);

        // Update threshold if necessary
        this->update_tf();

        m_planes[0].first->change_slice(m_current_slice_index);

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void negato2d::mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked)
    {
        if(_button == mouse_button::left)
        {
            this->pick_intensity(_x, _y);
        }
        else if(_button == mouse_button::right)
        {
            const auto dx = static_cast<double>(_x - m_initial_pos[0]);
            const auto dy = static_cast<double>(m_initial_pos[1] - _y);

            this->update_windowing(dx, dy);
        }

        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato2d::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    namespace interactor_3d = sight::viz::scene3d::interactor;

    m_picking_cross->set_visible(false);
    if(_button == mouse_button::left)
    {
        this->pick_intensity(_x, _y);
    }
    else if(_button == mouse_button::right && interactor_3d::base::is_in_layer(
                _x,
                _y,
                this->layer(),
                m_layer_order_dependant
    ))
    {
        const auto tf = m_tf.const_lock();

        m_initial_level  = tf->level();
        m_initial_window = tf->window();
        m_initial_pos    = {_x, _y};
        m_picked         = true;
    }

    if(m_picked)
    {
        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato2d::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    m_picked = false;
    m_picking_cross->set_visible(false);
    this->signal<signals::picked_voxel_t>(signals::PICKED_VOXEL)->async_emit("");
}

//------------------------------------------------------------------------------
void negato2d::set_visible(bool _visible)
{
    if(m_negato_scene_node != nullptr)
    {
        m_negato_scene_node->setVisible(_visible);

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void negato2d::pick_intensity(int _x, int _y)
{
    Ogre::SceneManager* sm = this->get_scene_manager();

    const auto result = sight::viz::scene3d::utils::pick_object(_x, _y, Ogre::SceneManager::ENTITY_TYPE_MASK, *sm);

    if(result.has_value())
    {
        if(m_planes[0].first->get_movable_object() == result->first)
        {
            m_picked = true;
            const auto image = m_image.lock();

            if(!data::helper::medical_image::check_image_validity(image.get_shared()))
            {
                return;
            }

            const auto cross_lines = m_planes[0].first->compute_cross(result->second, *image);

            m_picking_cross->update(cross_lines[0], cross_lines[1], cross_lines[2], cross_lines[3]);

            const auto picking_text = sight::viz::scene3d::utils::pick_image(*image, result->second);
            this->signal<signals::picked_voxel_t>(signals::PICKED_VOXEL)->async_emit(picking_text);

            this->request_render();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
