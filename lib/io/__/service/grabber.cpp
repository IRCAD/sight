/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "grabber.hpp"

namespace sight::io::service
{

//------------------------------------------------------------------------------

grabber::grabber() noexcept :
    notifier(has_signals::signals())
{
    //Declare all signals
    new_signal<signals::position_modified_t>(signals::POSITION_MODIFIED);
    new_signal<signals::duration_modified_t>(signals::DURATION_MODIFIED);
    new_signal<signals::camera_started_t>(signals::CAMERA_STARTED);
    new_signal<signals::camera_stopped_t>(signals::CAMERA_STOPPED);
    new_signal<signals::camera_paused_t>(signals::CAMERA_PAUSED);
    new_signal<signals::frame_presented_t>(signals::FRAME_PRESENTED);

    new_signal<signals::parameter_changed_t>(signals::PARAMETER_CHANGED);
    new_signal<signals::monitor_created_t>(signals::MONITOR_CREATED);
    new_signal<signals::fps_changed_t>(signals::FPS_CHANGED);

    new_slot(slots::START_CAMERA, &grabber::start_camera, this);
    new_slot(slots::STOP_CAMERA, &grabber::stop_camera, this);
    new_slot(slots::PAUSE_CAMERA, &grabber::pause_camera, this);
    new_slot(slots::PLAY_PAUSE_CAMERA, &grabber::play_pause_camera, this);
    new_slot(slots::LOOP_VIDEO, &grabber::toggle_loop_mode, this);
    new_slot(slots::SET_POSITION_VIDEO, &grabber::set_position, this);
    new_slot(slots::PREVIOUS_IMAGE, &grabber::previous_image, this);
    new_slot(slots::NEXT_IMAGE, &grabber::next_image, this);
    new_slot(slots::SET_STEP, &grabber::set_step, this);

    new_slot(slots::SET_PARAMETER, &grabber::set_parameter, this);

    new_slot(slots::REQUEST_SETTINGS, &grabber::request_settings, this);
    new_slot(slots::OPTIMIZE, &grabber::optimize, this);

    new_slot(slots::ADD_ROI_CENTER, &grabber::add_roi_center, this);
    new_slot(slots::REMOVE_ROI_CENTER, &grabber::remove_roi_center, this);

    new_slot(slots::FORWARD_FPS_CHANGED, &grabber::forward_fps_changed, this);
}

// ----------------------------------------------------------------------------

void grabber::play_pause_camera()
{
    if(m_is_started)
    {
        this->pause_camera();
    }
    else
    {
        this->start_camera();
    }
}

// ----------------------------------------------------------------------------
void grabber::previous_image()
{
    SIGHT_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void grabber::next_image()
{
    SIGHT_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void grabber::set_step(int /*step*/, std::string /*key*/)
{
    SIGHT_WARN("Frame by frame mode not implemented for this grabber type.");
}

//-----------------------------------------------------------------------------

void grabber::set_parameter(ui::parameter_t /*unused*/, std::string /*unused*/)
{
}

// ----------------------------------------------------------------------------

void grabber::request_settings()
{
}

// ----------------------------------------------------------------------------

void grabber::optimize()
{
}

//------------------------------------------------------------------------------

void grabber::add_roi_center(sight::data::point::sptr /*unused*/)
{
}

//------------------------------------------------------------------------------

void grabber::remove_roi_center(sight::data::point::sptr /*unused*/)
{
}

//------------------------------------------------------------------------------

void grabber::clear_timeline(data::frame_tl& _tl)
{
    if(_tl.is_allocated())
    {
        // Clear the timeline: send a black frame
        const core::clock::type timestamp = _tl.get_newer_timestamp() + 1;

        SPTR(data::frame_tl::buffer_t) buffer = _tl.create_buffer(timestamp);
        auto* dest_buffer = reinterpret_cast<std::uint8_t*>(buffer->add_element(0));

        std::memset(dest_buffer, 0, _tl.get_width() * _tl.get_height() * _tl.num_components() * _tl.type().size());

        // push buffer and notify
        _tl.clear_timeline();
        _tl.push_object(buffer);

        auto sig_tl = _tl.signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        sig_tl->async_emit(timestamp);
    }
}

//------------------------------------------------------------------------------

void grabber::forward_fps_changed(double /* fps */)
{
}

// ----------------------------------------------------------------------------

} // namespace sight::io::service
