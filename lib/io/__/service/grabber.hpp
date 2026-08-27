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

#pragma once

#include <sight/io/__/config.hpp>

#include <data/frame_tl.hpp>
#include <data/point.hpp>

#include <core/notification/has_monitors.hpp>

#include <service/base.hpp>

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
 * - \b notification_created(sight::core::notification::base::sptr): Emitted when a long running task has been
 * launched.
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
                                   public sight::core::notification::has_monitors
{
public:

    SIGHT_DECLARE_SERVICE(grabber, sight::service::base);

    struct slots
    {
        static inline const slot_key_t START_CAMERA        = "start_camera";
        static inline const slot_key_t STOP_CAMERA         = "stop_camera";
        static inline const slot_key_t PAUSE_CAMERA        = "pause_camera";
        static inline const slot_key_t PLAY_PAUSE_CAMERA   = "playPauseCamera";
        static inline const slot_key_t SELECT_CAMERA       = "select_camera";
        static inline const slot_key_t LOOP_VIDEO          = "loop_video";
        static inline const slot_key_t SET_POSITION_VIDEO  = "set_position_video";
        static inline const slot_key_t PRESENT             = "framePresented";
        static inline const slot_key_t PREVIOUS_IMAGE      = "previous_image";
        static inline const slot_key_t NEXT_IMAGE          = "next_image";
        static inline const slot_key_t SET_STEP            = "set_step";
        static inline const slot_key_t SET_PARAMETER       = "set_parameter";
        static inline const slot_key_t REQUEST_SETTINGS    = "request_settings";
        static inline const slot_key_t OPTIMIZE            = "optimize";
        static inline const slot_key_t ADD_ROI_CENTER      = "add_roi_center";
        static inline const slot_key_t REMOVE_ROI_CENTER   = "remove_roi_center";
        static inline const slot_key_t FORWARD_FPS_CHANGED = "forward_fps_changed";
    };

    struct signals
    {
        using position_modified_t = core::com::signal<void (int64_t)>;
        static inline const signal_key_t POSITION_MODIFIED = "position_modified";

        using duration_modified_t = core::com::signal<void (int64_t)>;
        static inline const signal_key_t DURATION_MODIFIED = "duration_modified";

        using frame_presented_t = core::com::signal<void ()>;
        static inline const signal_key_t FRAME_PRESENTED = "framePresented";

        using camera_started_t = core::com::signal<void ()>;
        static inline const signal_key_t CAMERA_STARTED = "camera_started";

        using camera_stopped_t = core::com::signal<void ()>;
        static inline const signal_key_t CAMERA_STOPPED = "camera_stopped";

        using camera_paused_t = core::com::signal<void ()>;
        static inline const signal_key_t CAMERA_PAUSED = "camera_paused";

        using parameter_changed_t = core::com::signal<void (ui::parameter_t, std::string)>;
        static inline const signal_key_t PARAMETER_CHANGED = "parameter_changed";

        using fps_changed_t = sight::core::com::signal<void (double)>;
        static inline const signal_key_t FPS_CHANGED = "fps_changed";
    };

    /**
     * @name Data API
     * @{
     */
    static constexpr std::string_view CAMERA_INPUT  = "camera";
    static constexpr std::string_view FRAMETL_INOUT = "frame_tl";
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
    void set_start_state(bool _state);
    bool started() const;

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    data::ptr<data::frame_tl, data::access::inout> m_frame {this, FRAMETL_INOUT};

private:

    /**
     * @brief Pauses or unpauses camera if it is started, if not, it starts it. This slot shouldn't be used when play
     * or pause slots are used, this is why we add a boolean state, to ensure correct interaction with a GUI element.
     */
    SIGHT_IO_API void play_pause_camera();

    /// Determines whether the grabber has been started, note : this does not mean it is playing, as it could be paused.
    std::atomic_bool m_is_started {false};
};

//------------------------------------------------------------------------------

inline void grabber::set_start_state(bool _state)
{
    m_is_started.store(_state);
}

//------------------------------------------------------------------------------

inline bool grabber::started() const
{
    return m_is_started.load();
}

} //namespace sight::io::service
