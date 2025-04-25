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

// cspell:ignore NOLINTNEXTLINE NOLINT

#include "module/viz/scene3d/adaptor/negato3d.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/tools/color.hpp>

#include <geometry/data/image.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

negato3d::negato3d() noexcept
{
    new_slot(slots::SET_TRANSPARENCY, &negato3d::set_transparency, this);
    m_planes.resize(3);

    for(int orientation_num = 0 ; auto& plane : m_planes)
    {
        plane.second = static_cast<axis_t>(orientation_num++);
    }
}

//------------------------------------------------------------------------------

void negato3d::configuring(const config_t& _config)
{
    negato::configuring(_config);

    static const std::string s_AUTORESET_CAMERA_CONFIG = CONFIG + "autoresetcamera";

    m_auto_reset_camera = _config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);
}

//------------------------------------------------------------------------------

void negato3d::change_slice_type(int /*unused*/, int /*unused*/)
{
}

//-----------------------------------------------------------------------------

void negato3d::set_transparency(double _transparency)
{
    SIGHT_ASSERT("Service not started", this->started());

    const float opacity = 1.F - static_cast<float>(_transparency);
    std::ranges::for_each(m_planes, [opacity](auto& _p){_p.first->set_entity_opacity(opacity);});

    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::set_visible(bool _visible)
{
    std::ranges::for_each(m_planes, [_visible](auto& _p){_p.first->set_visible(_visible);});
    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(m_layer_id);
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::set_planes_query_flags(std::uint32_t _flags)
{
    std::ranges::for_each(m_planes, [_flags](auto& _p){_p.first->set_query_flags(_flags);});
}

//------------------------------------------------------------------------------

void negato3d::mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked_plane)
    {
        if(_button == mouse_button::middle)
        {
            this->move_slices(_x, _y);
        }
        else if(_button == mouse_button::right)
        {
            const auto dx = static_cast<double>(_x - m_initial_pos[0]);
            const auto dy = static_cast<double>(m_initial_pos[1] - _y);

            this->update_windowing(dx, dy);
        }
        else if(_button == mouse_button::left)
        {
            this->pick_intensity(_x, _y);
        }

        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato3d::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    m_picked_plane.reset();
    m_picking_cross->set_visible(false);

    if(_button == mouse_button::middle)
    {
        this->move_slices(_x, _y);
    }
    else if(_button == mouse_button::right)
    {
        if(this->get_picked_slices(_x, _y) != std::nullopt)
        {
            const auto tf = m_tf.const_lock();

            m_initial_level  = tf->level();
            m_initial_window = tf->window();

            m_initial_pos = {_x, _y};
        }
    }
    else if(_button == mouse_button::left)
    {
        this->pick_intensity(_x, _y);
    }

    if(m_picked_plane)
    {
        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato3d::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_picked_plane)
    {
        m_picked_plane->set_render_queuer_group_and_priority(sight::viz::scene3d::rq::SURFACE_ID, 0);
        m_picked_plane.reset();
    }

    m_picking_cross->set_visible(false);
    this->signal<signals::picked_voxel_t>(signals::PICKED_VOXEL)->async_emit("");
    this->set_planes_query_flags(m_query_flags); // Make all planes pickable again.
}

//------------------------------------------------------------------------------

void negato3d::move_slices(int _x, int _y)
{
    const auto pick_res = this->get_picked_slices(
        _x,
        _y
    );

    if(pick_res.has_value())
    {
        const auto image = m_image.lock();

        auto picked_pt = pick_res.value();

        std::ranges::for_each(
            m_planes,
            [this](auto& _p)
            {
                if(_p.first != m_picked_plane)
                {
                    _p.first->set_query_flags(0x0);
                }
            });

        const auto picked_voxel = geometry::data::world_to_image(*image, picked_pt, true, true);

        image->async_emit(
            data::image::SLICE_INDEX_MODIFIED_SIG,
            int(picked_voxel[2]),
            int(picked_voxel[1]),
            int(picked_voxel[0])
        );
    }
}

//------------------------------------------------------------------------------

void negato3d::pick_intensity(int _x, int _y)
{
    auto sig = this->signal<signals::picked_voxel_t>(signals::PICKED_VOXEL);
    if(sig->num_connections() > 0)
    {
        const auto picked_pos = this->get_picked_slices(_x, _y);

        if(picked_pos.has_value())
        {
            const auto image = m_image.lock();

            if(!data::helper::medical_image::check_image_validity(image.get_shared()))
            {
                return;
            }

            const auto cross_lines = m_picked_plane->compute_cross(*picked_pos, *image);

            m_picking_cross->update(cross_lines[0], cross_lines[1], cross_lines[2], cross_lines[3]);

            const auto picking_text = sight::viz::scene3d::utils::pick_image(*image, *picked_pos);
            sig->async_emit(picking_text);

            this->request_render();

            // Render the picked plane before the widget.
            m_picked_plane->set_render_queuer_group_and_priority(sight::viz::scene3d::rq::NEGATO_WIDGET_ID, 0);
        }
    }
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> negato3d::get_picked_slices(int _x, int _y)
{
    Ogre::SceneManager* sm = this->get_scene_manager();
    const auto result      = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_flags, *sm);

    if(result.has_value())
    {
        const auto is_picked = [&result](const auto& _p){return _p.first->get_movable_object() == result->first;};
        const auto it = std::ranges::find_if(m_planes, is_picked); // NOLINT(readability-qualified-auto,llvm-qualified-auto)

        if(it != m_planes.cend())
        {
            m_picked_plane = it->first;
            return result->second;
        }
    }

    return std::nullopt;
}

} // namespace sight::module::viz::scene3d::adaptor.
