/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opDistorter/config.hpp"

#include <arData/Camera.hpp>

#include <fwServices/IOperator.hpp>

#ifdef OPENCV_CUDA_SUPPORT
#include <opencv2/cudawarping.hpp>
#else
#include <opencv2/core/mat.hpp>
#endif // OPENCV_CUDA_SUPPORT

namespace opDistorter
{

/**
 * @brief   Distort an image as if it is viewed by a lens of a camera. We thus need a camera calibration.
 *
 * @section Slots Slots
 * - \b changeState() : enabled/disabled the distort correction.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::opDistorter::SDistortImage">
        <in key="camera" uid="..." />
        <inout key="input" uid="..." />
        <inout key="output" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera containing calibration information.
 * - \b input [::fwData::Image]: input image to distort.
 * @subsection In-Out In-Out
 * - \b output [::fwData::Image]: image distorted.
 */
class OPDISTORTER_CLASS_API SDistortImage : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SDistortImage)(::fwServices::IOperator) )

    /**
     * @name Slots API
     * @{
     */
    OPDISTORTER_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_STATE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeStateSlotType;
    ///@}

    /// Constructor.
    OPDISTORTER_API SDistortImage() noexcept;

    /// Destructor. Does nothing
    OPDISTORTER_API virtual ~SDistortImage() noexcept;

    /**
     * @brief Connect ::fwData::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     * and ::fwData::Image::s_BUFFER_MODIFIED_SIG to s_UPDATE_SLOT
     */
    OPDISTORTER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Does nothing
    OPDISTORTER_API virtual void configuring() override;

    /// Retrieve the camera.
    OPDISTORTER_API virtual void starting() override;

    /// Do nothing.
    OPDISTORTER_API virtual void stopping() override;

    /// Distort the image.
    OPDISTORTER_API virtual void updating() override;

private:

    /// Distort the video.
    void distort();

    /// Slot: enable/disable the distort correction.
    void changeState();

    /// True if the undistortion is enabled.
    bool m_isEnabled { false };

    /// Camera corresponding to the video.
    ::arData::Camera::csptr m_camera;

#if OPENCV_CUDA_SUPPORT
    ::cv::cuda::GpuMat m_mapx;
    ::cv::cuda::GpuMat m_mapy;
#else
    ::cv::Mat m_mapx;
    ::cv::Mat m_mapy;
#endif // OPENCV_CUDA_SUPPORT
};

} // opDistorter
