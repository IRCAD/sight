/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/filter/vision/config.hpp"

#include <data/frame_tl.hpp>

#include <service/controller.hpp>

#include <opencv2/core.hpp>

namespace sight::module::filter::vision
{

/**
 * @brief Service used to compute optical flow (Lukas-Kanade) to detect if camera is moving or not.
 *  \b Note: this service is not detecting if something is moving in a video,
 * due to internal parameters it only detects if camera is moving globally.
 *
 * @section Signals Signals
 * - \b camera_moved: send when camera has moved.
 *  -\b camera_remained: send when camera is detected as stable.
 *
 * @section Slots Slots
 * - \b updating: called when a new frame is pushed into the timeline. Detected if camera has moved or not.

 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="sight::module::filter::vision::optical_flow" worker="ofWorker  >
        <in key="timeline" uid="..." auto_connect="true" />
        <config latency="333" scaleFactor="3.6" />
     </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [sight::data::frame_tl]: timeline on which optical flow will be computed.
 * @subsection Configuration Configuration:
 * - \b latency (optional): There is no need to process every frames coming in.
 * Use `latency` to wait between two consecutive frame. Ex: if set to 333 (in ms), it will compute ~1/10 frames at 30fps
 * (Default: 333 ms).
 * Usually you don't need to change the value.
 * - \b scaleFactor (optional): if image is > 640 x 480 the scaleFactor is applied to downscale image
 * to keep a good balance between computation time and feature tracking quality (default 3,6),
 *  usually you don't need to change the value.
 */
class MODULE_FILTER_VISION_CLASS_API optical_flow : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(optical_flow, sight::service::controller);

    using motion_signal_t    = core::com::signal<void ()>;
    using no_motion_signal_t = core::com::signal<void ()>;

    /// Constructor
    MODULE_FILTER_VISION_CLASS_API optical_flow() noexcept;

    /// Destructor
    MODULE_FILTER_VISION_CLASS_API ~optical_flow() noexcept override;

    /// Connects ::arData:FrameTL::signals::PUSHED to service::slots::UPDATE
    MODULE_FILTER_VISION_CLASS_API service::connections_t auto_connections() const override;

protected:

    /// Does nothing.
    MODULE_FILTER_VISION_CLASS_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_VISION_CLASS_API void starting() override;

    /// Computes Optical flow.
    MODULE_FILTER_VISION_CLASS_API void updating() override;

    /// Does nothing.
    MODULE_FILTER_VISION_CLASS_API void stopping() override;

private:

    /// Handles concurrency.
    std::mutex m_main_mutex;

    /// Signal send when motion is detected.
    motion_signal_t::sptr m_motion_signal;

    /// Signal send when no motion is detected.
    no_motion_signal_t::sptr m_no_motion_signal;

    /// Stores last image.
    cv::Mat m_last_gray_img;

    /// Stores last corners.
    cv::Mat m_last_corners;

    /// Waiting time between to frames.
    unsigned int m_latency {333};

    /// Factor of re-scale: resize image to keep good balance between computation time and feature tracking quality.
    float m_image_scale_factor {3.6F};

    /// Optical flow can only be computed if a frame is already present (see: m_lastGrayImg).
    /// True if it is, False otherwise
    bool m_initialization {false};

    /// Stores last behavior (true if motion, false otherwise).
    bool m_motion {false};

    /// Stores last processed frame timestamp.
    core::clock::type m_last_timestamp {0};

    static constexpr std::string_view FRAME_TIMELINE_INPUT = "timeline";

    sight::data::ptr<sight::data::frame_tl, sight::data::access::in> m_timeline {this, FRAME_TIMELINE_INPUT};
};

} //namespace sight::module::filter::vision
