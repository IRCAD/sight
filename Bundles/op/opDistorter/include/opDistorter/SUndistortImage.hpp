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
 * @brief   SUndistortImage service undistort a video.
 *
 * @section Slots Slots
 * - \b changeState() : Called to enable/disable the undistortion.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::opDistorter::SUndistortImage">
            <in key="camera" uid="..." />
            <in key="input" uid="..." autoConnect="yes" />
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [::arData::Camera]: camera containing calibration information.
 * - \b input [::fwData::Image]: input image to undistort.
 * @subsection In-Out In-Out:
 * - \b output [::fwData::Image]: image undistorted.
 */

class OPDISTORTER_CLASS_API SUndistortImage : public fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro((SUndistortImage)(fwServices::IOperator))

    /**
     * @name Slots API
     * @{
     */
    OPDISTORTER_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_STATE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeStateSlotType;
    ///@}

    /**
     * @brief Constructor.
     */
    OPDISTORTER_API SUndistortImage() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SUndistortImage() noexcept;

    /**
     * @brief Connect ::fwData::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     * and ::fwData::Image::s_BUFFER_MODIFIED_SIG to s_UPDATE_SLOT
     */
    OPDISTORTER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Do nothing
    OPDISTORTER_API void configuring() override;

    /// Compute the undistortion map
    OPDISTORTER_API void starting() override;

    /// Do nothing
    OPDISTORTER_API void stopping() override;

    /// Undistort the video
    OPDISTORTER_API void updating() override;

private:
    /// Remap the image using the undistortion map
    void undistort();

    /// Slot: enable/disable the undistort correction.
    void changeState();

    /// True if the undistortion is enabled.
    bool m_isEnabled { false };

    /// Camera corresponding to the video.
    ::arData::Camera::csptr m_camera;

#ifdef OPENCV_CUDA_SUPPORT
    ::cv::cuda::GpuMat m_mapx;
    ::cv::cuda::GpuMat m_mapy;
#else
    ::cv::Mat m_mapx;
    ::cv::Mat m_mapy;
#endif // OPENCV_CUDA_SUPPORT
};

} //namespace opDistorter
