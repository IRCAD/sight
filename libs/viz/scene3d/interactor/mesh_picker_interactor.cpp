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

#include "viz/scene3d/interactor/mesh_picker_interactor.hpp"

#include <core/com/signal.hxx>

#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/registry/macros.hpp>

namespace sight::viz::scene3d::interactor
{

mesh_picker_interactor::mesh_picker_interactor(layer::sptr _layer, bool _layer_order_dependant) noexcept :
    base(_layer, _layer_order_dependant)
{
}

//------------------------------------------------------------------------------

mesh_picker_interactor::~mesh_picker_interactor() noexcept =
    default;

//------------------------------------------------------------------------------

void mesh_picker_interactor::set_point_clicked_sig(const point_clicked_sig_t::sptr& _sig)
{
    m_point_clicked_sig = _sig;
}

//------------------------------------------------------------------------------

void mesh_picker_interactor::set_query_mask(std::uint32_t _query_mask)
{
    m_query_mask = _query_mask;
}

//------------------------------------------------------------------------------

void mesh_picker_interactor::pick(mouse_button _button, modifier _mod, int _x, int _y, bool _pressed)
{
    if(auto layer = m_layer.lock())
    {
        if(!is_in_layer(_x, _y, layer, m_layer_order_dependant))
        {
            return;
        }

        if(auto result = viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *layer->get_scene_manager());
           result != std::nullopt)
        {
            Ogre::Vector3 click = result->second;
            data::tools::picking_info info;

            const Ogre::Camera* const cam = layer->get_default_camera();
            const auto* const vp          = cam->getViewport();

            info.m_pixel_pos[0] = static_cast<double>(_x);
            info.m_pixel_pos[1] = static_cast<double>(_y);

            info.m_world_pos[0] = static_cast<double>(click.x);
            info.m_world_pos[1] = static_cast<double>(click.y);
            info.m_world_pos[2] = static_cast<double>(click.z);

            info.m_viewport_size[0] = static_cast<double>(vp->getActualWidth());
            info.m_viewport_size[1] = static_cast<double>(vp->getActualHeight());

            using picking_event_t = data::tools::picking_info::event;
            switch(_button)
            {
                case mouse_button::left:
                    info.m_event_id = _pressed ? picking_event_t::mouse_left_down : picking_event_t::mouse_left_up;
                    break;

                case mouse_button::right:
                    info.m_event_id = _pressed ? picking_event_t::mouse_right_down : picking_event_t::mouse_right_up;
                    break;

                case mouse_button::middle:
                    info.m_event_id = _pressed ? picking_event_t::mouse_middle_down : picking_event_t::mouse_middle_up;
                    break;

                default:
                    SIGHT_ERROR("Unknow button");
                    break;
            }

            if(static_cast<bool>(_mod & modifier::control))
            {
                info.m_modifier_mask |= data::tools::picking_info::ctrl;
            }

            if(static_cast<bool>(_mod & modifier::shift))
            {
                info.m_modifier_mask |= data::tools::picking_info::shift;
            }

            if(m_point_clicked_sig)
            {
                m_point_clicked_sig->async_emit(info);
            }
            else
            {
                SIGHT_ERROR(
                    "You must first set the signal sent using 'mesh_picker_interactor::setPointClickedSig'"
                    " for this interactor to work."
                );
            }
        }
    }
}

//------------------------------------------------------------------------------

void mesh_picker_interactor::button_press_event(mouse_button _button, modifier _mod, int _x, int _y)
{
    this->pick(_button, _mod, _x, _y, true);
}

//------------------------------------------------------------------------------

void mesh_picker_interactor::button_release_event(mouse_button _button, modifier _mod, int _x, int _y)
{
    this->pick(_button, _mod, _x, _y, false);
}

//------------------------------------------------------------------------------

} //namespace sight::viz::scene3d::interactor.
