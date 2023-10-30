/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "image_picker.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::module::ui::video
{

//-----------------------------------------------------------------------------

const core::com::slots::key_t GET_INTERACTION_SLOT = "getInteraction";

image_picker::image_picker() noexcept
{
    new_slot(GET_INTERACTION_SLOT, &image_picker::get_interaction, this);

    m_video_ref_map["top_left"] = video_reference_t::top_left;
    m_video_ref_map["center"]   = video_reference_t::center;
}

//-----------------------------------------------------------------------------

image_picker::~image_picker() noexcept =
    default;

//-----------------------------------------------------------------------------

void image_picker::starting()
{
}

//-----------------------------------------------------------------------------

void image_picker::stopping()
{
}

//-----------------------------------------------------------------------------

void image_picker::configuring()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");

    const std::string video_ref = config.get<std::string>("videoReference", "center");

    auto it = m_video_ref_map.find(video_ref);

    if(it == m_video_ref_map.end())
    {
        SIGHT_WARN("'videoReference' of value '" + video_ref + "' is not handled.");
    }
    else
    {
        m_video_ref = it->second;
    }

    m_use_ctrl_modifier = config.get<bool>("useCtrlModifier", true);
    m_single_point_mode = config.get<bool>("singlePointMode", false);
}

//-----------------------------------------------------------------------------

void image_picker::updating()
{
}

//-----------------------------------------------------------------------------

void image_picker::get_interaction(data::tools::picking_info _info)
{
    if(_info.m_modifier_mask == data::tools::picking_info::ctrl || !m_use_ctrl_modifier)
    {
        if(_info.m_event_id == data::tools::picking_info::event::mouse_left_down)
        {
            const double x = _info.m_world_pos[0];
            const double y = _info.m_world_pos[1];
            const double z = _info.m_world_pos[2];

            const std::array<double, 3> position = {{x, y, z}};

            if(m_single_point_mode)
            {
                std::size_t n_points = 0;
                {
                    auto point_list = m_point_list.lock();
                    n_points = point_list->get_points().size();
                }

                if(n_points > 0)
                {
                    this->remove_last_point();
                }
                else
                {
                    this->add_point(position);
                }
            }
            else
            {
                this->add_point(position);
            }
        }
        else if(_info.m_event_id == data::tools::picking_info::event::mouse_right_down)
        {
            this->remove_last_point();
        }
    }
}

//-----------------------------------------------------------------------------

void image_picker::add_point(const std::array<double, 3>& _current_point)
{
    // Set z to 0 as it is an image.

    auto point_list       = m_point_list.lock();
    auto pixel_point_list = m_pixel_point_list.lock();

    data::point::sptr point = std::make_shared<data::point>(_current_point[0], _current_point[1], 0.);

    const auto camera = m_camera.lock();

    data::point::sptr pixel;

    switch(m_video_ref)
    {
        case video_reference_t::center:
        {
            const std::array offset {static_cast<float>(camera->get_width()) / 2.F,
                                     static_cast<float>(camera->get_height()) / 2.F
            };

            // Shift point to set reference at top_left corner & inverse y axis.
            pixel = std::make_shared<data::point>(_current_point[0] + offset[0], offset[1] - _current_point[1], 0.);
            break;
        }

        case video_reference_t::top_left:
            pixel = std::make_shared<data::point>(_current_point[0], _current_point[1], 0.);
            break;

        default:
            SIGHT_ERROR("Only video reference CENTER and TOP_LEFT are handled");
            break;
    }

    // "World" points.
    {
        point_list->get_points().push_back(point);
        auto sig = point_list->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
        sig->async_emit();

        auto sig2 = point_list->signal<data::point_list::point_added_signal_t>(data::point_list::POINT_ADDED_SIG);
        sig2->async_emit(point);
    }

    // Pixel points.
    {
        pixel_point_list->get_points().push_back(pixel);
        auto sig = pixel_point_list->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
        sig->async_emit();

        auto sig2 = pixel_point_list->signal<data::point_list::point_added_signal_t>(data::point_list::POINT_ADDED_SIG);
        sig2->async_emit(point);
    }
}

//-----------------------------------------------------------------------------

void image_picker::remove_last_point()
{
    auto point_list       = m_point_list.lock();
    auto pixel_point_list = m_pixel_point_list.lock();
    data::point::sptr point;

    if(!point_list->get_points().empty() && !pixel_point_list->get_points().empty())
    {
        {
            point = point_list->get_points().back();
            point_list->get_points().pop_back();

            auto sig = point_list->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
            sig->async_emit();

            auto sig2 =
                point_list->signal<data::point_list::point_removed_signal_t>(data::point_list::POINT_REMOVED_SIG);
            sig2->async_emit(point);
        }

        {
            point = pixel_point_list->get_points().back();
            pixel_point_list->get_points().pop_back();

            auto sig = pixel_point_list->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
            sig->async_emit();

            auto sig2 = pixel_point_list->signal<data::point_list::point_removed_signal_t>(
                data::point_list::POINT_REMOVED_SIG
            );
            sig2->async_emit(point);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::video
