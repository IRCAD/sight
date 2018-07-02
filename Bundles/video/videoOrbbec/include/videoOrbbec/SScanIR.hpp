/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoOrbbec/ColorFrameWorker.hpp"
#include "videoOrbbec/config.hpp"
#include "videoOrbbec/SScanBase.hpp"

#include <arData/CameraSeries.hpp>
#include <arData/FrameTL.hpp>

#include <arServices/IGrabber.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>

#include <fwThread/Worker.hpp>

#include <fwVideoQt/Player.hpp>

#include <OpenNI.h>
#include <QThread>

namespace videoOrbbec
{

/**
 * @brief   Orbbec Astra Pro camera (InfraRed version)
 *
 * Service to grab video frames from the Orbbec Astra Pro camera.
 *
 * \b Tags: DEVICE
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t): Unimplemented.
 * - \b durationModified(std::int64_t): Unimplemented.
 *
 * @section Slots Slots
 * - \b startCamera(): Start capturing frames from the camera.
 * - \b stopCamera(): Stop capturing frames from the camera.
 * - \b pauseCamera(): Unimplemented.
 * - \b toggleLoopMode(): Unimplemented.
 * - \b setPosition(int): Unimplemented.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="videoGrabber" type="::videoOrbbec::SScanIR">
            <inout key="depthTL" uid="..." />
            <inout key="frameTL" uid="..." />
            <inout key="irTL" uid="..." />
            <videoMode>VGA</videoMode>
        </service>
 * @endcode
 * @subsection In-Out In-Out
 * - \b depthTL [::arData::FrameTL]: timeline containing depth frames.
 * - \b frameTL [::arData::FrameTL]: timeline containing color frames.
 * - \b irTL    [::arData::FrameTL]: timeline containing infrared frames.
 *
 * @subsection Configuration Configuration
 * \see SScanBase
 */
class VIDEOORBBEC_CLASS_API SScanIR : public QObject,
                                      public SScanBase
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro((SScanIR)(::arServices::IRGBDGrabber))

    /**
     * @name Data API
     * @{
     */
    VIDEOORBBEC_CLASS_API static const ::fwServices::IService::KeyType s_IRTL_INOUT;
    /** @} */

    /**
     * @brief SScanIR constructor
     */
    VIDEOORBBEC_CLASS_API SScanIR() noexcept;

    /**
     * @brief SScanIR destructor
     */
    VIDEOORBBEC_CLASS_API ~SScanIR() noexcept;

protected:
    /**
     * @brief Starts the service
     *
     * @throw ::fwTools::Failed
     */
    void starting() override;

    /**
     * @brief Starts capturing RGBD frames through a connected Astra camera.
     * Resets the stream if capture is already started.
     */
    void startCamera() override;

    /**
     * @brief Stops the capture.
     */
    void stopCamera() override;

    /**
     * @brief Try detecting the cameras and setting them up, raise an exception if unable to.
     */
    void tryInitialize();

    /**
     * @brief Check whether the service is ready to read from the camera, ie all sensors are detected.
     */
    bool isReady() const noexcept;

protected Q_SLOTS:
    /**
     * @brief Qt slot called when a new RGB frame is ready.
     */
    //void presentColorFrame(QVideoFrame const&);
    void workerStarted() const;
    void workerStopped() const;

private:
    // Timelines
    /// Timeline containing depth frames.
    ::arData::FrameTL::sptr m_depthTL;
    /// Timeline containing color frames.
    ::arData::FrameTL::sptr m_colorTL;
    /// Timeline containing infrared frames.
    ::arData::FrameTL::sptr m_IRTL;

    /// RGB sensor settings.
    ::arData::Camera::sptr m_rgbCamera;

    // OpenNI types
    /// Error code of the last OpenNI call.
    ::openni::Status m_oniStatus;
    /// OpenNI stream representing the depth data stream.
    ::openni::VideoStream m_depthStream;
    /// OpenNI stream representing the IR data stream.
    ::openni::VideoStream m_irStream;

    /// Worker grabbing ir frames.
    ::fwThread::Worker::sptr m_workerIR;
    /// Worker grabbing depth frames.
    ::fwThread::Worker::sptr m_workerDepth;
    /// Qt thread owning the color worker.
    QThread m_colorWorkerThread;
    /// Grabs color frames from the camera.
    ::fwVideoQt::Player* m_qtPlayer;
    /// Worker grabbing color frames.
    ColorFrameWorker* m_workerColor;

    // Slots
    /// Type of the presentDepthFrame slot.
    using PresentDepthFrameSlotType = ::fwCom::Slot<void()>;
    /// Slot for the presentDepthFrame method.
    PresentDepthFrameSlotType::sptr m_slotPresentDepthFrame;
    /**
     * @brief Slot: grabs depth frames from the camera and pushes them to their timeline.
     */
    void presentDepthFrame();

    /// Type of the presentIRFrame slot.
    using PresentIRFrameSlotType = ::fwCom::Slot<void()>;
    /// Slot for the presentDepthFrame method.
    PresentIRFrameSlotType::sptr m_slotPresentIRFrame;
    /**
     * @brief Slot: grabs depth frames from the camera and pushes them to their timeline.
     */
    void presentIRFrame();
};

} // videoOrbbec
