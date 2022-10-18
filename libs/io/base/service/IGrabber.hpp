/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "io/base/config.hpp"

#include <data/Camera.hpp>
#include <data/FrameTL.hpp>

#include <service/IService.hpp>

namespace sight::io::base::service
{

/**
 * @brief   This interface defines grabber service API.
 *          Must be implemented for services that grabs frames.
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t): Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t): Emitted when the duration of the video is modified.
 * - \b cameraStarted(): Emitted when camera is started.
 * - \b cameraStopped(): Emitted when camera is stopped.
 * - \b framePresented(): Emitted when a frame is presented.
 * - \b boolChanged(): Emitted when a named boolean grabber parameter is changed.
 * - \b doubleChanged(): Emitted when a named double grabber parameter is changed.
 * - \b intChanged(): Emitted when a named integer grabber parameter is changed.
 * - \b enumChanged(): Emitted when a named list element parameter is changed.
 * - \b enumValuesChanged(): Emitted a named data list parameter is changed for the grabber.
 *
 * @section Slots Slots
 * - \b startCamera(): Start playing the camera or the video.
 * - \b stopCamera(): Stop playing the camera or the video.
 * - \b pauseCamera(): Pause the video, it has no effect when playing a camera.
 * - \b playPauseCamera(): Pauses or unpauses camera if it is started, if not, it starts it.
 * - \b loopVideo(): Toggle the loop of the playing.
 * - \b setPositionVideo(int): Force the current time in the video.
 * - \b nextImage(): display the next image in step by step mode. Does nothing if not overridden.
 * - \b previousImage(): display the previous image in step by step mode. Does nothing if not overridden.
 * - \b setStep(): set the step value between two images when calling nextImage/previousImage. Does nothing if not
 * overridden.
 * - \b setBoolParameter(): Sets a named bool parameter.
 * - \b setDoubleParameter(): Sets a named double parameter.
 * - \b setIntParameter(): Sets a named integer parameter.
 * - \b setEnumParameter(): Sets a named enum value parameter.
 * - \b setEnumValuesParameter(): Sets a named list of enum values.
 * - \b requestSettings(): Requests the grabber internal settings.
 */
class IO_BASE_CLASS_API IGrabber : public sight::service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IGrabber, sight::service::IService);

    /**
     * @name Slots API
     * @{
     */
    IO_BASE_API static const core::com::Slots::SlotKeyType s_START_CAMERA_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_STOP_CAMERA_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_PAUSE_CAMERA_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_PLAY_PAUSE_CAMERA_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SELECT_CAMERA_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_LOOP_VIDEO_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_POSITION_VIDEO_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_PRESENT_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_PREVIOUS_IMAGE_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_NEXT_IMAGE_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_STEP_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_ENUM_VALUES_PARAMETER_SLOT;
    IO_BASE_API static const core::com::Slots::SlotKeyType s_REQUEST_SETTINGS_SLOT;
    ///@}

    /**
     * @name Signals API
     * @{
     */

    IO_BASE_API static const core::com::Signals::SignalKeyType s_POSITION_MODIFIED_SIG;
    using PositionModifiedSignalType = core::com::Signal<void (int64_t)>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_DURATION_MODIFIED_SIG;
    using DurationModifiedSignalType = core::com::Signal<void (int64_t)>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_FRAME_PRESENTED_SIG;
    using FramePresentedSignalType = core::com::Signal<void ()>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_CAMERA_STARTED_SIG;
    using CameraStartedSignalType = core::com::Signal<void ()>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_CAMERA_STOPPED_SIG;
    using CameraStoppedSignalType = core::com::Signal<void ()>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_BOOL_CHANGED_SIG;
    using BoolChangedSignalType = core::com::Signal<void (bool, std::string)>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_DOUBLE_CHANGED_SIG;
    using DoubleChangedSignalType = core::com::Signal<void (double, std::string)>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_INT_CHANGED_SIG;
    using IntChangedSignalType = core::com::Signal<void (int, std::string)>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_ENUM_CHANGED_SIG;
    using EnumChangedSignalType = core::com::Signal<void (std::string, std::string)>;

    IO_BASE_API static const core::com::Signals::SignalKeyType s_ENUM_VALUES_CHANGED_SIG;
    using EnumValuesChangedSignalType = core::com::Signal<void (std::string, std::string)>;

    /** @} */

    /**
     * @name Data API
     * @{
     */
    static constexpr std::string_view s_CAMERA_INPUT  = "camera";
    static constexpr std::string_view s_FRAMETL_INOUT = "frameTL";
    /** @} */
    /**
     * @brief Constructor.
     */
    IO_BASE_API IGrabber() noexcept;

    /**
     * @brief Destructor.
     */
    IO_BASE_API ~IGrabber() noexcept override;

    /**
     * @brief API for starting a camera. Needs to be reimplemented in child classes.
     */
    IO_BASE_API virtual void startCamera() = 0;

    /**
     * @brief API for stopping a camera. Needs to be reimplemented in child classes.
     */
    IO_BASE_API virtual void stopCamera() = 0;

    /**
     * @brief API for pausing a camera. Needs to be reimplemented in child classes.
     */
    IO_BASE_API virtual void pauseCamera() = 0;

    /**
     * @brief API for enable/disable the loop mode in video. Needs to be reimplemented in child classes.
     */
    IO_BASE_API virtual void toggleLoopMode() = 0;

    /**
     * @brief API for setting a new position in the video. Needs to be reimplemented in child classes
     */
    IO_BASE_API virtual void setPosition(int64_t position) = 0;

    /**
     * @brief API to get the previous image in frame by frame mode.
     */
    IO_BASE_API virtual void previousImage();

    /**
     * @brief API to get the next image in frame by frame mode.
     */
    IO_BASE_API virtual void nextImage();

    /**
     * @brief API to set step used on readPrevious/readNext slots in frame by frame mode.
     */
    IO_BASE_API virtual void setStep(int step, std::string key);

    /// Sets an internal bool value.
    IO_BASE_API virtual void setBoolParameter(bool value, std::string key);

    /// Sets an internal double value.
    IO_BASE_API virtual void setDoubleParameter(double value, std::string key);

    /// Sets an internal int value.
    IO_BASE_API virtual void setIntParameter(int value, std::string key);

    /// Sets an internal enum value.
    IO_BASE_API virtual void setEnumParameter(std::string value, std::string key);

    /// Sets an internal list of enum values.
    IO_BASE_API virtual void setEnumValuesParameter(std::string value, std::string key);

    /// Requests the grabber internal settings.
    IO_BASE_API virtual void requestSettings();

protected:

    /**
     * @brief Helper function intended to be used in derived services. Clears the supplied timeline, emits the cleared
     * signal, pushes a black frame and emits the object pushed signal.
     */
    IO_BASE_API static void clearTimeline(data::FrameTL& tl);

    /**
     * @brief sets the current start state of the grabber.
     */
    IO_BASE_API void setStartState(bool state);
    bool started() const;

    data::ptr<data::FrameTL, data::Access::inout> m_frame {this, s_FRAMETL_INOUT};

private:

    /**
     * @brief Pauses or unpauses camera if it is started, if not, it starts it. This slot shouldn't be used when play
     * or pause slots are used, this is why we add a boolean state, to ensure correct interaction with a GUI element.
     */
    IO_BASE_API void playPauseCamera();

    /// Determines whether the grabber has been started, note : this does not mean it is playing, as it could be paused.
    bool m_isStarted {false};
};

//------------------------------------------------------------------------------

inline bool IGrabber::started() const
{
    return m_isStarted;
}

} //namespace sight::io::base::service
