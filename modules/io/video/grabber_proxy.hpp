/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "modules/io/video/config.hpp"

#include <core/com/helper/sig_slot_connection.hpp>

#include <io/__/service/grabber.hpp>

#include <service/has_services.hpp>
#include <service/notifier.hpp>

namespace sight::data
{

class Camera;

} // namespace sight::data

namespace sight::module::io::video
{

/**
 * @brief   This service allows you to select a frame grabber implementation at runtime.
 *
 * Actually as its name indicates it, this service is a proxy and behaves like exactly a video frame grabber. The
 * difference is that it selects a valid video grabber implementation when playing is requested. If there are several
 * implementation available, a dialog proposes the user to make a choice.
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t): Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t): Emitted when the duration of the video is modified.
 *
 * @section Slots Slots
 * - \b startCamera(): Start playing the camera or the video.
 * - \b startTargetCamera(std::string): Start playing the specified camera or the video.
 * - \b stopCamera(): Stop playing the camera or the video.
 * - \b pauseCamera(): Pause the video, it has no effect when playing a camera.
 * - \b loopVideo(): Toggle the loop of the playing.
 * - \b setPositionVideo(int): Force the current time in the video.
 * - \b reconfigure(): Allows to erase the implementation choice, so that the selection routine is ran again, thus,
 * potentially the selection dialog is shown.
 * - \b nextImage(): display the next image in step by step mode. Does nothing if not supported by the selected grabber.
 * - \b previousImage(): display the previous image in step by step mode. Does nothing if not supported by the
 * selected grabber.
 * - \b setStep(): set the step value between two images when calling nextImage/previousImage. Does nothing if not
 * supported by the selected grabber.
 * - \b requestSettings(): Requests the grabber internal settings.
 * - \b addROICenter(sight::data::point::sptr): Adds a new region fo interest center.
 * - \b removeROICenter(sight::data::point::sptr): Removes a region of interest via its center.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::io::video::grabber_proxy">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <inout key="depthTL" uid="..." />
            <config>
                <camera type="RGBD" />
                <selection mode="include" />
                <addSelection service="sight::module::io::video::frame_grabber" />
                <config id="cvGrabberConfig" service="sight::module::io::video::frame_grabber" />
                <gui title="Please select the camera to use" />
            </config>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [sight::data::camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [sight::data::frame_tl]: timeline where to extract the video frames.
 * - \b      [sight::data::frame_tl] (optional): timeline where to extract the depth frames.
 * @subsection Configuration Configuration
 *  - \b type (optional, default="RGB"): allows to filter for RGB or RGBD grabbers
 *  - \b selection
 *      - \b mode (optional) : must be include (to add the selection to selector list ) or exclude (to exclude the
 * selection of the selector list).
 *  - \b addSelection
 *      - \b service (mandatory) : name of the service to include/exclude to the choice list of the selector.
 *  - \b config
 *      - \b id (mandatory) : the id of the configuration to use.
 *      - \b service (mandatory) :  the name of the service.
 *  - \b gui
 *      - \b title (optional) : title of the grabber selector window.
 */
class MODULE_IO_VIDEO_CLASS_API grabber_proxy : public sight::io::service::grabber,
                                                public service::has_services
{
public:

    SIGHT_DECLARE_SERVICE(grabber_proxy, sight::io::service::grabber);

    /// Constructor. Initialize slots and signals
    MODULE_IO_VIDEO_API grabber_proxy() noexcept;

    /// Destructor. Do nothing.
    MODULE_IO_VIDEO_API ~grabber_proxy() noexcept override;

    /// Internal wrapper holding slots keys.
    struct slots
    {
        using key_t = sight::core::com::slots::key_t;

        static inline const key_t RECONFIGURE         = "reconfigure";
        static inline const key_t START_TARGET_CAMERA = "startTargetCamera";

        static inline const key_t MODIFY_POSITION = "modifyPosition";
        static inline const key_t MODIFY_DURATION = "modifyDuration";

        static inline const key_t FWD_START_CAMERA  = "forwardStartCamera";
        static inline const key_t FWD_STOP_CAMERA   = "forwardStopCamera";
        static inline const key_t FWD_NOTIFY        = "forwardNotify";
        static inline const key_t FWD_SET_PARAMETER = "forwardSetParameter";
        static inline const key_t FWD_PRESENT_FRAME = "forwardPresentFrame";
        static inline const key_t FWD_CREATE_JOB    = "forwardCreateJob";
    };

protected:

    /// Does nothing.
    MODULE_IO_VIDEO_API void starting() final;

    /// Stop the underlying grabber, destroy it, and empty the input FrameTl.
    MODULE_IO_VIDEO_API void stopping() final;

    /// Does nothing.
    MODULE_IO_VIDEO_API void updating() final;

    /// Parses the XML configuration of the service.
    MODULE_IO_VIDEO_API void configuring() final;

    /**
     * @name Slots methods
     *
     * @{
     */
    /// Initialize and start camera (restart camera if is already started).
    MODULE_IO_VIDEO_API void start_camera() final;

    /// Initialize and start camera (restart camera if is already started).
    MODULE_IO_VIDEO_API void start_target_camera(std::string _impl);

    /// Stop camera.
    MODULE_IO_VIDEO_API void stop_camera() final;

    /// Pause camera.
    MODULE_IO_VIDEO_API void pause_camera() final;

    /// Enable/disable loop in video.
    MODULE_IO_VIDEO_API void toggle_loop_mode() final;

    /// Set the new position in the video.
    MODULE_IO_VIDEO_API void set_position(std::int64_t _position) final;

    /// Get the previous image in frame by frame mode.
    MODULE_IO_VIDEO_API void previous_image() override;

    /// Get the next image in frame by frame mode.
    MODULE_IO_VIDEO_API void next_image() override;

    /// Set step used on readPrevious/readNext slots.
    MODULE_IO_VIDEO_API void set_step(int _step, std::string _key) override;

    /// Sets internal parameters values.
    MODULE_IO_VIDEO_API void set_parameter(ui::parameter_t _value, std::string _key) final;

    /// SLOT: Requests the grabber internal settings.
    MODULE_IO_VIDEO_API void request_settings() final;

    /// SLOT: Calls optimization functions defined in the grabber (e.g. hardware related).
    MODULE_IO_VIDEO_API void optimize() final;

    /// SLOT: Adds a region of interest center.
    MODULE_IO_VIDEO_API void add_roi_center(sight::data::point::sptr _p) final;

    /// SLOT: Removes a region of interest center.
    MODULE_IO_VIDEO_API void remove_roi_center(sight::data::point::sptr _p) final;
/** @} */

private:

    using service_config_pair = std::pair<std::string, service::config_t>;

    enum class camera_t : std::uint8_t
    {
        rgb,
        rgbd
    };

    /**
     * @name Slots methods
     *
     * @{
     */
    /// Reset the grabber implementation and stop the current playback.
    void reconfigure();

    /// Position in the video has changed.
    void modify_position(int64_t _position);

    /// Duration of the video has changed.
    void modify_duration(int64_t _duration);

    /// The playback has started in the sub-service.
    void fwd_start_camera();

    /// The playback has stopped in the sub-service.
    void fwd_stop_camera();

    /// A frame is presented in the sub-service.
    void fwd_present_frame();

    /// A named parameter has been emitted in the sub-service.
    void fwd_set_parameter(ui::parameter_t _value, std::string _key);

    /// A job has been created in the proxied service.
    void fwd_create_job(sight::core::jobs::base::sptr _job);

    // Forwards notifications
    void fwd_notify(service::notification _notification);
    /** @} */

    /// Forward FPS data
    void forward_fps_changed(double _fps) final;

    /// Camera type (RGB, RGBD,...)
    camera_t m_type {camera_t::rgb};

    /// Grabber implementation chosen by the user.
    std::string m_grabber_impl;

    /// config to use with the current grabber.
    std::string m_grabber_config;

    /// Title of the GUI selector window
    std::string m_gui_title {"Please select a video grabber implementation"};

    /// Actual grabber service.
    std::vector<sight::io::service::grabber::sptr> m_services;

    /// Connections with service signals.
    core::com::helper::sig_slot_connection m_connections;

    /// List of services to be included or excluded.
    std::set<std::string> m_selected_services;

    /// Map that specifies all configuration extensions for a service.
    std::map<std::string, std::vector<std::string> > m_service_to_config;

    /// Configure if selected services are excluded (true) or included (false).
    bool m_exclude {true};

    /// Camera can be either a data::camera or a data::camera_set depending on the implementation
    data::ptr<data::object, data::access::in> m_camera {this, CAMERA_INPUT};
};

} // namespace sight::module::io::video
