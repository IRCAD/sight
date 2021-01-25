/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "videoCalibration/config.hpp"

#include <data/Camera.hpp>
#include <data/Image.hpp>

#include <fwServices/IOperator.hpp>

#ifdef OPENCV_CUDA_SUPPORT
#include <opencv2/cudawarping.hpp>
#else
#include <opencv2/core/mat.hpp>
#endif // OPENCV_CUDA_SUPPORT

namespace videoCalibration
{

/**
 * @brief   Distort or undistort an image according to camera intrinsics and distortion coefficients.
 *
 * The service can be used either to transform an image or to output the distortion map. The latter case is used when
 * you want to perform the distortion directly in the rendering pipeline.
 *
 * @section Slots Slots
 * - \b changeState() : enabled/disabled the distortion correction.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::videoCalibration::SDistortion">
        <in key="camera" uid="..." />
        <in key="input" uid="..." />
        <inout key="output" uid="..." />
        <mode>distort</mode>
    </service>
   @endcode
 * Or
 * @code{.xml}
    <service type="::videoCalibration::SDistortion">
        <in key="camera" uid="..." />
        <inout key="map" uid="..." />
        <mode>distort</mode>
    </service>
   @endcode
 * @subsection Input Input
 * - \b camera [data::Camera]: camera containing calibration information.
 * - \b input [data::Image]: input image to distort.
 * @subsection In-Out In-Out
 * - \b output [data::Image]: output image.
 * - \b map [data::Image]: distortion map. Useful mainly if you want to perform the distortion directly with the
 * renderer.
 * @subsection Configuration Configuration:
 * - \b mode(optional) : "distort" or "undistort" the output image (default: "distort").
 */
class VIDEOCALIBRATION_CLASS_API SDistortion : public ::fwServices::IOperator
{
public:

    fwCoreServiceMacro(SDistortion, ::fwServices::IOperator)

    /**
     * @name Slots API
     * @{
     */
    VIDEOCALIBRATION_API static const core::com::Slots::SlotKeyType s_CHANGE_STATE_SLOT;
    typedef core::com::Slot<void ()> ChangeStateSlotType;
    ///@}

    /// Constructor.
    VIDEOCALIBRATION_API SDistortion() noexcept;

    /// Destructor. Does nothing
    VIDEOCALIBRATION_API virtual ~SDistortion() noexcept;

    /**
     * @brief Connect data::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     * and data::Image::s_BUFFER_MODIFIED_SIG to s_UPDATE_SLOT
     */
    VIDEOCALIBRATION_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Does nothing
    VIDEOCALIBRATION_API virtual void configuring() override;

    /// Retrieve the camera.
    VIDEOCALIBRATION_API virtual void starting() override;

    /// Do nothing.
    VIDEOCALIBRATION_API virtual void stopping() override;

    /// Distort the image.
    VIDEOCALIBRATION_API virtual void updating() override;

private:

    /// Distort or undistort the video.
    void remap();

    /// Slot: enable/disable the distort correction.
    void changeState();

    /// Slot: compute the distortion map.
    void calibrate();

    /// True if the undistortion is enabled.
    bool m_isEnabled { false };

    /// If true, distort the output image, otherwise we undistort it
    bool m_distort { true };

    /// True when a calibration mismatch is detected, this avoids to pop-up the error at each update
    bool m_calibrationMismatch { false };

    /// This is used to reset m_calibrationMismatch when the image resolution changes
    data::Image::Size m_prevImageSize{0, 0, 0};

#if OPENCV_CUDA_SUPPORT
    ::cv::cuda::GpuMat m_mapx;
    ::cv::cuda::GpuMat m_mapy;
#else
    ::cv::Mat m_mapx;
    ::cv::Mat m_mapy;
#endif // OPENCV_CUDA_SUPPORT
};

} // videoCalibration
