/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoCalibration/config.hpp"

#include <arData/Camera.hpp>
#include <fwData/Array.hpp>

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
 * - \b camera [::arData::Camera]: camera containing calibration information.
 * - \b input [::fwData::Image]: input image to distort.
 * @subsection In-Out In-Out
 * - \b output [::fwData::Image]: output image.
 * - \b map [::fwData::Image]: distortion map. Useful mainly if you want to perform the distortion directly with the
 * renderer.
 * @subsection Configuration Configuration:
 * - \b mode(optional) : "distort" or "undistort" the output image (default: "distort").
 */
class VIDEOCALIBRATION_CLASS_API SDistortion : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SDistortion)(::fwServices::IOperator) )

    /**
     * @name Slots API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_STATE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeStateSlotType;
    ///@}

    /// Constructor.
    VIDEOCALIBRATION_API SDistortion() noexcept;

    /// Destructor. Does nothing
    VIDEOCALIBRATION_API virtual ~SDistortion() noexcept;

    /**
     * @brief Connect ::fwData::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     * and ::fwData::Image::s_BUFFER_MODIFIED_SIG to s_UPDATE_SLOT
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

#if OPENCV_CUDA_SUPPORT
    ::cv::cuda::GpuMat m_mapx;
    ::cv::cuda::GpuMat m_mapy;
#else
    ::cv::Mat m_mapx;
    ::cv::Mat m_mapy;
#endif // OPENCV_CUDA_SUPPORT
};

} // videoCalibration
