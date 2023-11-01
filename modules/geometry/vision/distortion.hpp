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

#pragma once

#include "modules/geometry/vision/config.hpp"

#include <data/camera.hpp>
#include <data/image.hpp>

#include <service/filter.hpp>

#ifdef OPENCV_CUDA_SUPPORT
#include <opencv2/cudawarping.hpp>
#else
#include <opencv2/core/mat.hpp>
#endif // OPENCV_CUDA_SUPPORT

namespace sight::module::geometry::vision
{

/**
 * @brief   Distort or undistort an image according to camera intrinsics and distortion coefficients.
 *
 * The service can be used either to transform an image or to output the distortion map. The latter case is used when
 * you want to perform the distortion directly in the rendering pipeline.
 *
 * @section Slots Slots
 * - \b change_state() : enabled/disabled the distortion correction.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::geometry::vision::distortion">
        <in key="camera" uid="..." />
        <in key="input" uid="..." />
        <inout key="output" uid="..." />
        <mode>distort</mode>
    </service>
   @endcode
 * Or
 * @code{.xml}
    <service type="sight::module::geometry::vision::distortion">
        <in key="camera" uid="..." />
        <inout key="map" uid="..." />
        <mode>distort</mode>
    </service>
   @endcode
 * @subsection Input Input
 * - \b camera [sight::data::camera]: camera containing calibration information.
 * - \b input [sight::data::image]: input image to distort.
 * @subsection In-Out In-Out
 * - \b output [sight::data::image]: output image.
 * - \b map [sight::data::image]: distortion map. Useful mainly if you want to perform the distortion directly with the
 * renderer.
 * @subsection Configuration Configuration:
 * - \b mode(optional) : "distort" or "undistort" the output image (default: "distort").
 */
class MODULE_GEOMETRY_VISION_CLASS_API distortion : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(distortion, sight::service::filter);

    /**
     * @name Slots API
     * @{
     */
    MODULE_GEOMETRY_VISION_API static const core::com::slots::key_t CHANGE_STATE_SLOT;
    using change_state_slot_t = core::com::slot<void ()>;
    ///@}

    /// Constructor.
    MODULE_GEOMETRY_VISION_API distortion() noexcept;

    /// Destructor. Does nothing
    MODULE_GEOMETRY_VISION_API ~distortion() noexcept override;

    /**
     * @brief Connect data::image::MODIFIED_SIG to service::slots::UPDATE
     * and data::image::BUFFER_MODIFIED_SIG to service::slots::UPDATE
     */
    MODULE_GEOMETRY_VISION_API service::connections_t auto_connections() const override;

protected:

    /// Does nothing
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /// Retrieve the camera.
    MODULE_GEOMETRY_VISION_API void starting() override;

    /// Do nothing.
    MODULE_GEOMETRY_VISION_API void stopping() override;

    /// Distort the image.
    MODULE_GEOMETRY_VISION_API void updating() override;

private:

    /// Distort or undistort the video.
    void remap();

    /// Slot: enable/disable the distort correction.
    void change_state();

    /// Slot: compute the distortion map.
    void calibrate();

    /// True if the undistortion is enabled.
    bool m_is_enabled {false};

    /// If true, distort the output image, otherwise we undistort it
    bool m_distort {true};

    /// True when a calibration mismatch is detected, this avoids to pop-up the error at each update
    bool m_calibration_mismatch {false};

    /// This is used to reset m_calibrationMismatch when the image resolution changes
    data::image::size_t m_prev_image_size {0, 0, 0};

#if OPENCV_CUDA_SUPPORT
    cv::cuda::GpuMat m_map_x;
    cv::cuda::GpuMat m_map_y;
#else
    cv::Mat m_map_x;
    cv::Mat m_map_y;
#endif // OPENCV_CUDA_SUPPORT

    static constexpr std::string_view CAMERA_INPUT = "camera";
    static constexpr std::string_view IMAGE_INPUT  = "input";
    static constexpr std::string_view IMAGE_INOUT  = "output";
    static constexpr std::string_view MAP_INOUT    = "map";

    sight::data::ptr<sight::data::camera, sight::data::access::in> m_camera {this, CAMERA_INPUT};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_INPUT};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_output {this, IMAGE_INOUT};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_map {this, MAP_INOUT};
};

} // namespace sight::module::geometry::vision
