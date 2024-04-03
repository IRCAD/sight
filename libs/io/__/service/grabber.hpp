/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#pragma once

#include <sight/io/__/config.hpp>

#include <core/jobs/base.hpp>

#include <data/camera.hpp>
#include <data/frame_tl.hpp>
#include <data/point.hpp>

#include <service/base.hpp>
#include <service/notifier.hpp>

#include <ui/__/parameter.hpp>

namespace sight::io::service
{

/**
 * @brief   This interface defines grabber service API.
 *          Must be implemented for services that grabs frames.
 *
 * @section Signals Signals
 * - \b position_modified(std::int64_t): Emitted when the position in the video is modified during playing.
 * - \b duration_modified(std::int64_t): Emitted when the duration of the video is modified.
 * - \b camera_started(): Emitted when camera is started.
 * - \b camera_stopped(): Emitted when camera is stopped.
 * - \b framePresented(): Emitted when a frame is presented.
 * - \b parameter_changed(): Emitted when a named parameter is changed.
 * - \b job_created(sight::core::jobs::base::sptr): Emitted when a long running job has been launched.
 *
 * @section Slots Slots
 * - \b start_camera(): Start playing the camera or the video.
 * - \b stop_camera(): Stop playing the camera or the video.
 * - \b pause_camera(): Pause the video, it has no effect when playing a camera.
 * - \b playPauseCamera(): Pauses or unpauses camera if it is started, if not, it starts it.
 * - \b loop_video(): Toggle the loop of the playing.
 * - \b set_position_video(int): Force the current time in the video.
 * - \b next_image(): display the next image in step by step mode. Does nothing if not overridden.
 * - \b previous_image(): display the previous image in step by step mode. Does nothing if not overridden.
 * - \b set_step(): set the step value between two images when calling nextImage/previous_image. Does nothing if not
 * overridden.
 * - \b set_parameters(parameters_t, std::string): Sets a parameters with value (variant) and key.
 * - \b request_settings(): Requests the grabber internal settings.
 * - \b optimize(): Calls grabber-specific optimization function (e.g. hardware related).
 * - \b add_roi_center(sight::data::point::sptr): Adds a new region fo interest center.
 * - \b remove_roi_center(sight::data::point::sptr): Removes a region of interest via its center.
 */
class SIGHT_IO_CLASS_API grabber : public sight::service::base,
                                   public sight::service::notifier
{
public:

    SIGHT_DECLARE_SERVICE(grabber, sight::service::base);

    /**
     * @name Slots API
     * @{
     */
    SIGHT_IO_API static const core::com::slots::key_t START_CAMERA_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t STOP_CAMERA_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t PAUSE_CAMERA_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t PLAY_PAUSE_CAMERA_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t SELECT_CAMERA_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t LOOP_VIDEO_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t SET_POSITION_VIDEO_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t PRESENT_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t PREVIOUS_IMAGE_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t NEXT_IMAGE_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t SET_STEP_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t SET_PARAMETER_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t REQUEST_SETTINGS_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t OPTIMIZE_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t ADD_ROI_CENTER_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t REMOVE_ROI_CENTER_SLOT;
    SIGHT_IO_API static const core::com::slots::key_t FORWARD_FPS_CHANGED_SLOT;
    ///@}

    /**
     * @name Signals API
     * @{
     */
    SIGHT_IO_API static const core::com::signals::key_t POSITION_MODIFIED_SIG;
    using position_modified_signal_t = core::com::signal<void (int64_t)>;

    SIGHT_IO_API static const core::com::signals::key_t DURATION_MODIFIED_SIG;
    using duration_modified_signal_t = core::com::signal<void (int64_t)>;

    SIGHT_IO_API static const core::com::signals::key_t FRAME_PRESENTED_SIG;
    using frame_presented_signal_t = core::com::signal<void ()>;

    SIGHT_IO_API static const core::com::signals::key_t CAMERA_STARTED_SIG;
    using camera_started_signal_t = core::com::signal<void ()>;

    SIGHT_IO_API static const core::com::signals::key_t CAMERA_STOPPED_SIG;
    using camera_stopped_signal_t = core::com::signal<void ()>;

    SIGHT_IO_API static const core::com::signals::key_t PARAMETER_CHANGED_SIG;
    using parameter_changed_t = core::com::signal<void (ui::parameter_t, std::string)>;

    SIGHT_IO_API static const core::com::signals::key_t JOB_CREATED_SIG;
    using job_created_signal_t = core::com::signal<void (core::jobs::base::sptr)>;

    SIGHT_IO_API static const core::com::signals::key_t FPS_CHANGED_SIG;
    using fps_changed_signal_t = sight::core::com::signal<void (double)>;
    /** @} */

    /**
     * @name Data API
     * @{
     */
    static constexpr std::string_view CAMERA_INPUT  = "camera";
    static constexpr std::string_view FRAMETL_INOUT = "frameTL";
    /** @} */
    /**
     * @brief Constructor.
     */
    SIGHT_IO_API grabber() noexcept;

    /**
     * @brief Destructor.
     */
    SIGHT_IO_API ~grabber() noexcept override = default;

    /**
     * @brief API for starting a camera. Needs to be reimplemented in child classes.
     */
    SIGHT_IO_API virtual void start_camera() = 0;

    /**
     * @brief API for stopping a camera. Needs to be reimplemented in child classes.
     */
    SIGHT_IO_API virtual void stop_camera() = 0;

    /**
     * @brief API for pausing a camera. Needs to be reimplemented in child classes.
     */
    SIGHT_IO_API virtual void pause_camera() = 0;

    /**
     * @brief API for enable/disable the loop mode in video. Needs to be reimplemented in child classes.
     */
    SIGHT_IO_API virtual void toggle_loop_mode() = 0;

    /**
     * @brief API for setting a new position in the video. Needs to be reimplemented in child classes
     */
    SIGHT_IO_API virtual void set_position(int64_t _position) = 0;

    /**
     * @brief API to get the previous image in frame by frame mode.
     */
    SIGHT_IO_API virtual void previous_image();

    /**
     * @brief API to get the next image in frame by frame mode.
     */
    SIGHT_IO_API virtual void next_image();

    /**
     * @brief API to set step used on readPrevious/readNext slots in frame by frame mode.
     */
    SIGHT_IO_API virtual void set_step(int _step, std::string _key);

    /// SLOT: Requests the grabber internal settings.
    SIGHT_IO_API virtual void request_settings();

    /// SLOT: Calls optimization functions defined in the grabber (e.g. hardware related function).
    SIGHT_IO_API virtual void optimize();

    /// SLOT: Adds a region of interest center.
    SIGHT_IO_API virtual void add_roi_center(sight::data::point::sptr _p);

    /// SLOT: Removes a region of interest center.
    SIGHT_IO_API virtual void remove_roi_center(sight::data::point::sptr _p);

    /// SLOT: Sets a parameter value with its key.
    SIGHT_IO_API virtual void set_parameter(ui::parameter_t _value, std::string _key);

    /// SLOT: Forward FPS data
    SIGHT_IO_API virtual void forward_fps_changed(double _fps);

protected:

    /**
     * @brief Helper function intended to be used in derived services. Clears the supplied timeline, emits the cleared
     * signal, pushes a black frame and emits the object pushed signal.
     */
    SIGHT_IO_API static void clear_timeline(data::frame_tl& _tl);

    /**
     * @brief sets the current start state of the grabber.
     */
    SIGHT_IO_API void set_start_state(bool _state);
    bool started() const;

    data::ptr<data::frame_tl, data::access::inout> m_frame {this, FRAMETL_INOUT};

private:

    /**
     * @brief Pauses or unpauses camera if it is started, if not, it starts it. This slot shouldn't be used when play
     * or pause slots are used, this is why we add a boolean state, to ensure correct interaction with a GUI element.
     */
    SIGHT_IO_API void play_pause_camera();

    /// Determines whether the grabber has been started, note : this does not mean it is playing, as it could be paused.
    bool m_is_started {false};
};

//------------------------------------------------------------------------------

inline bool grabber::started() const
{
    return m_is_started;
}

} //namespace sight::io::service
