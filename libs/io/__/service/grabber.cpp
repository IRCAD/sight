/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

namespace sight::io::service
{

const core::com::signals::key_t grabber::POSITION_MODIFIED_SIG = "positionModified";
const core::com::signals::key_t grabber::DURATION_MODIFIED_SIG = "durationModified";

const core::com::signals::key_t grabber::CAMERA_STARTED_SIG = "cameraStarted";
const core::com::signals::key_t grabber::CAMERA_STOPPED_SIG = "cameraStopped";

const core::com::signals::key_t grabber::FRAME_PRESENTED_SIG = "framePresented";

const core::com::signals::key_t grabber::PARAMETER_CHANGED_SIG = "parameterChanged";

const core::com::signals::key_t grabber::JOB_CREATED_SIG = "jobCreated";
const core::com::signals::key_t grabber::FPS_CHANGED_SIG = "fpsChanged";

const core::com::slots::key_t grabber::START_CAMERA_SLOT       = "startCamera";
const core::com::slots::key_t grabber::STOP_CAMERA_SLOT        = "stopCamera";
const core::com::slots::key_t grabber::PAUSE_CAMERA_SLOT       = "pauseCamera";
const core::com::slots::key_t grabber::PLAY_PAUSE_CAMERA_SLOT  = "playPauseCamera";
const core::com::slots::key_t grabber::LOOP_VIDEO_SLOT         = "loopVideo";
const core::com::slots::key_t grabber::SET_POSITION_VIDEO_SLOT = "setPositionVideo";
const core::com::slots::key_t grabber::PREVIOUS_IMAGE_SLOT     = "previousImage";
const core::com::slots::key_t grabber::NEXT_IMAGE_SLOT         = "nextImage";
const core::com::slots::key_t grabber::SET_STEP_SLOT           = "setStep";

const core::com::slots::key_t grabber::SET_PARAMETER_SLOT = "set_parameter";

const core::com::slots::key_t grabber::REQUEST_SETTINGS_SLOT    = "requestSettings";
const core::com::slots::key_t grabber::OPTIMIZE_SLOT            = "optimize";
const core::com::slots::key_t grabber::FORWARD_FPS_CHANGED_SLOT = "forwardFPSChanged";

const std::string grabber::ADD_ROI_CENTER_SLOT    = "addROICenter";
const std::string grabber::REMOVE_ROI_CENTER_SLOT = "removeROICenter";

// ----------------------------------------------------------------------------

grabber::grabber() noexcept :
    notifier(m_signals)
{
    //Declare all signals
    new_signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
    new_signal<duration_modified_signal_t>(DURATION_MODIFIED_SIG);
    new_signal<camera_started_signal_t>(CAMERA_STARTED_SIG);
    new_signal<camera_stopped_signal_t>(CAMERA_STOPPED_SIG);
    new_signal<frame_presented_signal_t>(FRAME_PRESENTED_SIG);

    new_signal<parameter_changed_t>(PARAMETER_CHANGED_SIG);
    new_signal<job_created_signal_t>(JOB_CREATED_SIG);
    new_signal<fps_changed_signal_t>(FPS_CHANGED_SIG);

    new_slot(START_CAMERA_SLOT, &grabber::start_camera, this);
    new_slot(STOP_CAMERA_SLOT, &grabber::stop_camera, this);
    new_slot(PAUSE_CAMERA_SLOT, &grabber::pause_camera, this);
    new_slot(PLAY_PAUSE_CAMERA_SLOT, &grabber::play_pause_camera, this);
    new_slot(LOOP_VIDEO_SLOT, &grabber::toggle_loop_mode, this);
    new_slot(SET_POSITION_VIDEO_SLOT, &grabber::set_position, this);
    new_slot(PREVIOUS_IMAGE_SLOT, &grabber::previous_image, this);
    new_slot(NEXT_IMAGE_SLOT, &grabber::next_image, this);
    new_slot(SET_STEP_SLOT, &grabber::set_step, this);

    new_slot(SET_PARAMETER_SLOT, &grabber::set_parameter, this);

    new_slot(REQUEST_SETTINGS_SLOT, &grabber::request_settings, this);
    new_slot(OPTIMIZE_SLOT, &grabber::optimize, this);

    new_slot(ADD_ROI_CENTER_SLOT, &grabber::add_roi_center, this);
    new_slot(REMOVE_ROI_CENTER_SLOT, &grabber::remove_roi_center, this);

    new_slot(FORWARD_FPS_CHANGED_SLOT, &grabber::forward_fps_changed, this);
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
        const core::hires_clock::type timestamp = _tl.get_newer_timestamp() + 1;

        SPTR(data::frame_tl::buffer_t) buffer = _tl.create_buffer(timestamp);
        auto* dest_buffer = reinterpret_cast<std::uint8_t*>(buffer->add_element(0));

        std::memset(dest_buffer, 0, _tl.get_width() * _tl.get_height() * _tl.num_components());

        // push buffer and notify
        _tl.clear_timeline();
        _tl.push_object(buffer);

        auto sig_tl = _tl.signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        sig_tl->async_emit(timestamp);
    }
}

// ----------------------------------------------------------------------------

void grabber::set_start_state(bool _state)
{
    m_is_started = _state;
}

//------------------------------------------------------------------------------

void grabber::forward_fps_changed(double /* fps */)
{
}

// ----------------------------------------------------------------------------

} // namespace sight::io::service
