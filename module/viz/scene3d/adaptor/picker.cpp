/************************************************************************
 *
 * Copyright (C) 2019-2026 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "module/viz/scene3d/adaptor/picker.hpp"

#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/utils.hpp"

#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/ptree.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::signals::key_t PICKED_SIG = "picked";

//-----------------------------------------------------------------------------

picker::picker() noexcept
{
    new_signal<point_clicked_sig_t>(PICKED_SIG);
}

//-----------------------------------------------------------------------------

void picker::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_PRIORITY_CONFIG = CONFIG + "priority";

    m_priority = config.get<int>(s_PRIORITY_CONFIG, m_priority);

    const auto hexa_mask = core::ptree::get_and_deprecate<std::string>(
        config,
        CONFIG + "query_mask",
        CONFIG + "queryMask",
        "26.0",
        ""
    );

    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_mask = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }
}

//-----------------------------------------------------------------------------

void picker::starting()
{
    sight::viz::scene3d::adaptor::init();

    const auto layer = this->layer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);
}

//-----------------------------------------------------------------------------

void picker::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void picker::stopping()
{
    const auto layer = this->layer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->remove_interactor(interactor);

    sight::viz::scene3d::adaptor::deinit();
}

//-----------------------------------------------------------------------------

void picker::button_press_event(mouse_button _button, modifier _mod, int _x, int _y)
{
    m_pressed[_button] = true;
    this->pick(_button, _mod, _x, _y, true);
}

//-----------------------------------------------------------------------------

void picker::button_release_event(mouse_button _button, modifier _mod, int _x, int _y)
{
    m_pressed[_button] = false;
    this->pick(_button, _mod, _x, _y, false);
}

//-----------------------------------------------------------------------------

void picker::mouse_move_event(
    mouse_button _button,
    modifier _mod,
    int _x,
    int _y,
    int /*_dx*/,
    int /*_dy*/
)
{
    if(m_pressed[_button])
    {
        this->pick(_button, _mod, _x, _y, true);
    }
}

//-----------------------------------------------------------------------------

void picker::pick(mouse_button _button, modifier _mod, int _x, int _y, bool _pressed)
{
    if(auto layer = this->layer())
    {
        if(!is_in_layer(_x, _y, layer))
        {
            return;
        }

        if(auto result = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *layer->get_scene_manager());
           result != std::nullopt)
        {
            Ogre::Vector3 click = result->position;
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

            info.m_closest_index_id = static_cast<std::int64_t>(result->index);

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
                    SIGHT_ERROR("Unknown button");
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

            this->async_emit(PICKED_SIG, info);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor
