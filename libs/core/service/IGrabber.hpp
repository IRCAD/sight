/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "service/config.hpp"
#include <service/IService.hpp>

#include <data/Camera.hpp>
#include <data/FrameTL.hpp>

namespace sight::service
{

/**
 * @brief   This interface defines grabber service API.
 *          Must be implemented for services that grabs frames.
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t) : Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t) : Emitted when the duration of the video is modified.
 * - \b cameraStarted() : Emitted when camera is started.
 * - \b cameraStopped() : Emitted when camera is stopped.
 * - \b framePresented() : Emitted when a frame is presented.
 *
 * @section Slots Slots
 * - \b startCamera() : Start playing the camera or the video.
 * - \b stopCamera() : Stop playing the camera or the video.
 * - \b pauseCamera() : Pause the video, it has no effect when playing a camera.
 * - \b playPauseCamera() : Pauses or unpauses camera if it is started, if not, it starts it.
 * - \b loopVideo() : Toggle the loop of the playing.
 * - \b setPositionVideo(int) : Force the current time in the video.
 * - \b nextImage(): display the next image in step by step mode. Does nothing if not overridden.
 * - \b previousImage(): display the previous image in step by step mode. Does nothing if not overridden.
 * - \b setStep(): set the step value between two images when calling nextImage/previousImage. Does nothing if not
 * overridden.
 */
class SERVICE_CLASS_API IGrabber : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IGrabber, service::IService);

    /**
     * @name Slots API
     * @{
     */
    SERVICE_API static const core::com::Slots::SlotKeyType s_START_CAMERA_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_STOP_CAMERA_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_PAUSE_CAMERA_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_PLAY_PAUSE_CAMERA_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_SELECT_CAMERA_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_LOOP_VIDEO_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_SET_POSITION_VIDEO_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_PRESENT_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_PREVIOUS_IMAGE_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_NEXT_IMAGE_SLOT;
    SERVICE_API static const core::com::Slots::SlotKeyType s_SET_STEP_SLOT;
    ///@}

    /**
     * @name Signals API
     * @{
     */

    SERVICE_API static const core::com::Signals::SignalKeyType s_POSITION_MODIFIED_SIG;
    typedef core::com::Signal<void (int64_t)> PositionModifiedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_DURATION_MODIFIED_SIG;
    typedef core::com::Signal<void (int64_t)> DurationModifiedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_FRAME_PRESENTED_SIG;
    typedef core::com::Signal<void ()> FramePresentedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_CAMERA_STARTED_SIG;
    typedef core::com::Signal<void ()> CameraStartedSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_CAMERA_STOPPED_SIG;
    typedef core::com::Signal<void ()> CameraStoppedSignalType;

    /** @} */

    /**
     * @name Data API
     * @{
     */
    SERVICE_API static const service::IService::KeyType s_CAMERA_INPUT;
    SERVICE_API static const service::IService::KeyType s_FRAMETL_INOUT;
    /** @} */
    /**
     * @brief Constructor.
     */
    SERVICE_API IGrabber() noexcept;

    /**
     * @brief Destructor.
     */
    SERVICE_API virtual ~IGrabber() noexcept;

    /**
     * @brief API for starting a camera. Needs to be reimplemented in child classes.
     */
    SERVICE_API virtual void startCamera() = 0;

    /**
     * @brief API for stopping a camera. Needs to be reimplemented in child classes.
     */
    SERVICE_API virtual void stopCamera() = 0;

    /**
     * @brief API for pausing a camera. Needs to be reimplemented in child classes.
     */
    SERVICE_API virtual void pauseCamera() = 0;

    /**
     * @brief API for enable/disable the loop mode in video. Needs to be reimplemented in child classes.
     */
    SERVICE_API virtual void toggleLoopMode() = 0;

    /**
     * @brief API for setting a new position in the video. Needs to be reimplemented in child classes
     */
    SERVICE_API virtual void setPosition(int64_t position) = 0;

    /**
     * @brief API to get the previous image in frame by frame mode.
     */
    SERVICE_API virtual void previousImage();

    /**
     * @brief API to get the next image in frame by frame mode.
     */
    SERVICE_API virtual void nextImage();

    /**
     * @brief API to set step used on readPrevious/readNext slots in frame by frame mode.
     */
    SERVICE_API virtual void setStep(int step, std::string key);

protected:

    /**
     * @brief Helper function intended to be used in derived services. Clears the supplied timeline, emits the cleared
     * signal, pushes a black frame and emits the object pushed signal.
     */
    SERVICE_API static void clearTimeline(SPTR(data::FrameTL) const&);

    /**
     * @brief sets the current start state of the grabber.
     */
    SERVICE_API void setStartState(bool state);

    data::ptr<data::FrameTL, data::Access::inout> m_frame {this, s_FRAMETL_INOUT};

private:

    /**
     * @brief Pauses or unpauses camera if it is started, if not, it starts it. This slot shouldn't be used when play
     * or pause slots are used, this is why we add a boolean state, to ensure correct interaction with a GUI element.
     */
    SERVICE_API void playPauseCamera();

    /// Determines whether the grabber has been started, note : this does not mean it is playing, as it could be paused.
    bool m_isStarted {false};
};

} //namespace sight::service
