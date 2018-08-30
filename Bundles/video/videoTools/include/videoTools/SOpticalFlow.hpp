/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoTools/config.hpp"

#include <fwServices/IController.hpp>

#include <opencv2/core.hpp>

namespace videoTools
{

/**
 * @brief Service used to compute optical flow (Lukas-Kanade) to detect if camera is moving or not.
 *  \b Note: this service is not detecting if something is moving in a video,
 * due to internal parameters it only detects if camera is moving globally.
 *
 * @section Signals Signals
 * - \b cameraMoved: send when camera has moved.
 *  -\b cameraRemained: send when camera is detected as stable.
 *
 * @section Slots Slots
 * - \b updating: called when a new frame is pushed into the timeline. Detected if camera has moved or not.

 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::videoTools::SOpticalFlow" worker="ofWorker  >
        <in key="timeline" uid="..." autoConnect="yes" />
        <config imgToDrop="10" scaleFactor="3.6" />
     </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [::arData::FrameTL]: timeline on which optical flow will be computed.
 * @subsection Configuration Configuration:
 * - \b imgToDrop (optionnal): number of image to drop (default: 10), usually you don't need to change the value.
 * - \b scaleFactor (optionnal): if image is > 640 x 480 the scaleFactor is applied to downscale image
 * to keep a good balance between computation time and feature tracking quality (default 3,6),
 *  usually you don't need to change the value.
 */
class VIDEOTOOLS_CLASS_API SOpticalFlow : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpticalFlow)(::fwServices::IController));

    typedef ::fwCom::Signal<void ()> MotionSignalType;
    typedef ::fwCom::Signal<void ()> NoMotionSignalType;

    /// Constructor
    VIDEOTOOLS_CLASS_API SOpticalFlow() noexcept;

    /// Destructor
    VIDEOTOOLS_CLASS_API ~SOpticalFlow() noexcept override;

    /// Connects ::arData:FrameTL::s_OBJECT_PUSHED_SIG to s_UPDATE_SLOT
    VIDEOTOOLS_CLASS_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Does nothing.
    VIDEOTOOLS_CLASS_API void configuring() override;

    /// Does nothing.
    VIDEOTOOLS_CLASS_API void starting() override;

    /// Computes Optical flow.
    VIDEOTOOLS_CLASS_API void updating() override;

    /// Does nothing.
    VIDEOTOOLS_CLASS_API void stopping() override;

private:
    /// Handles concurrency.
    std::mutex m_mainMutex;
    /// Signal send when motion is detected.
    MotionSignalType::sptr m_motionSignal;
    /// Signal send when no motion is detected.
    NoMotionSignalType::sptr m_noMotionSignal;
    /// Stores last image.
    ::cv::Mat m_lastGrayImg;
    /// Stores last corners.
    ::cv::Mat m_lastCorners;
    /// Number of frame to drop
    unsigned int m_nbDropFrame;
    /// Factor of re-scale: resize image to keep good balance between computation time and feature tracking quality.
    float m_imageScaleFactor;
    /// Optical flow can only be computed if a frame is already present (see: m_lastGrayImg).
    /// True if it is, False otherwise
    bool m_initialization;
    /// Stores last behavior (true if motion, false otherwise).
    bool m_motion;
};

} //namespace videoTools
