/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "io/__/config.hpp"

#include <core/jobs/base.hpp>

#include <data/Camera.hpp>
#include <data/FrameTL.hpp>
#include <data/Point.hpp>

#include <service/base.hpp>
#include <service/notifier.hpp>

#include <ui/__/parameter.hpp>

namespace sight::io::service
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
 * - \b parameterChanged(): Emitted when a named parameter is changed.
 * - \b jobCreated(sight::core::jobs::base::sptr): Emitted when a long running job has been launched.
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
 * - \b setParameters(parameters_t, std::string): Sets a parameters with value (variant) and key.
 * - \b requestSettings(): Requests the grabber internal settings.
 * - \b optimize(): Calls grabber-specific optimization function (e.g. hardware related).
 * - \b addROICenter(sight::data::Point::sptr): Adds a new region fo interest center.
 * - \b removeROICenter(sight::data::Point::sptr): Removes a region of interest via its center.
 */
class IO_CLASS_API grabber : public sight::service::base,
                             public sight::service::notifier
{
public:

    SIGHT_DECLARE_SERVICE(grabber, sight::service::base);

    /**
     * @name Slots API
     * @{
     */
    IO_API static const core::com::slots::key_t START_CAMERA_SLOT;
    IO_API static const core::com::slots::key_t STOP_CAMERA_SLOT;
    IO_API static const core::com::slots::key_t PAUSE_CAMERA_SLOT;
    IO_API static const core::com::slots::key_t PLAY_PAUSE_CAMERA_SLOT;
    IO_API static const core::com::slots::key_t SELECT_CAMERA_SLOT;
    IO_API static const core::com::slots::key_t LOOP_VIDEO_SLOT;
    IO_API static const core::com::slots::key_t SET_POSITION_VIDEO_SLOT;
    IO_API static const core::com::slots::key_t PRESENT_SLOT;
    IO_API static const core::com::slots::key_t PREVIOUS_IMAGE_SLOT;
    IO_API static const core::com::slots::key_t NEXT_IMAGE_SLOT;
    IO_API static const core::com::slots::key_t SET_STEP_SLOT;
    IO_API static const core::com::slots::key_t SET_PARAMETER_SLOT;
    IO_API static const core::com::slots::key_t REQUEST_SETTINGS_SLOT;
    IO_API static const core::com::slots::key_t OPTIMIZE_SLOT;
    IO_API static const core::com::slots::key_t ADD_ROI_CENTER_SLOT;
    IO_API static const core::com::slots::key_t REMOVE_ROI_CENTER_SLOT;
    IO_API static const core::com::slots::key_t FORWARD_FPS_CHANGED_SLOT;
    ///@}

    /**
     * @name Signals API
     * @{
     */
    IO_API static const core::com::signals::key_t POSITION_MODIFIED_SIG;
    using PositionModifiedSignalType = core::com::signal<void (int64_t)>;

    IO_API static const core::com::signals::key_t DURATION_MODIFIED_SIG;
    using DurationModifiedSignalType = core::com::signal<void (int64_t)>;

    IO_API static const core::com::signals::key_t FRAME_PRESENTED_SIG;
    using FramePresentedSignalType = core::com::signal<void ()>;

    IO_API static const core::com::signals::key_t CAMERA_STARTED_SIG;
    using CameraStartedSignalType = core::com::signal<void ()>;

    IO_API static const core::com::signals::key_t CAMERA_STOPPED_SIG;
    using CameraStoppedSignalType = core::com::signal<void ()>;

    IO_API static const core::com::signals::key_t PARAMETER_CHANGED_SIG;
    using ParameterChangedSignalType = core::com::signal<void (ui::parameter_t, std::string)>;

    IO_API static const core::com::signals::key_t JOB_CREATED_SIG;
    using JobCreatedSignalType = core::com::signal<void (core::jobs::base::sptr)>;

    IO_API static const core::com::signals::key_t FPS_CHANGED_SIG;
    using FPSChangedSignalType = sight::core::com::signal<void (double)>;
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
    IO_API grabber() noexcept;

    /**
     * @brief Destructor.
     */
    IO_API ~grabber() noexcept override = default;

    /**
     * @brief API for starting a camera. Needs to be reimplemented in child classes.
     */
    IO_API virtual void startCamera() = 0;

    /**
     * @brief API for stopping a camera. Needs to be reimplemented in child classes.
     */
    IO_API virtual void stopCamera() = 0;

    /**
     * @brief API for pausing a camera. Needs to be reimplemented in child classes.
     */
    IO_API virtual void pauseCamera() = 0;

    /**
     * @brief API for enable/disable the loop mode in video. Needs to be reimplemented in child classes.
     */
    IO_API virtual void toggleLoopMode() = 0;

    /**
     * @brief API for setting a new position in the video. Needs to be reimplemented in child classes
     */
    IO_API virtual void setPosition(int64_t position) = 0;

    /**
     * @brief API to get the previous image in frame by frame mode.
     */
    IO_API virtual void previousImage();

    /**
     * @brief API to get the next image in frame by frame mode.
     */
    IO_API virtual void nextImage();

    /**
     * @brief API to set step used on readPrevious/readNext slots in frame by frame mode.
     */
    IO_API virtual void setStep(int step, std::string key);

    /// SLOT: Requests the grabber internal settings.
    IO_API virtual void requestSettings();

    /// SLOT: Calls optimization functions defined in the grabber (e.g. hardware related function).
    IO_API virtual void optimize();

    /// SLOT: Adds a region of interest center.
    IO_API virtual void addROICenter(sight::data::Point::sptr p);

    /// SLOT: Removes a region of interest center.
    IO_API virtual void removeROICenter(sight::data::Point::sptr p);

    /// SLOT: Sets a parameter value with its key.
    IO_API virtual void setParameter(ui::parameter_t value, std::string key);

    /// SLOT: Forward FPS data
    IO_API virtual void forwardFPSChanged(double fps);

protected:

    /**
     * @brief Helper function intended to be used in derived services. Clears the supplied timeline, emits the cleared
     * signal, pushes a black frame and emits the object pushed signal.
     */
    IO_API static void clearTimeline(data::FrameTL& tl);

    /**
     * @brief sets the current start state of the grabber.
     */
    IO_API void setStartState(bool state);
    bool started() const;

    data::ptr<data::FrameTL, data::Access::inout> m_frame {this, s_FRAMETL_INOUT};

private:

    /**
     * @brief Pauses or unpauses camera if it is started, if not, it starts it. This slot shouldn't be used when play
     * or pause slots are used, this is why we add a boolean state, to ensure correct interaction with a GUI element.
     */
    IO_API void playPauseCamera();

    /// Determines whether the grabber has been started, note : this does not mean it is playing, as it could be paused.
    bool m_isStarted {false};
};

//------------------------------------------------------------------------------

inline bool grabber::started() const
{
    return m_isStarted;
}

} //namespace sight::io::service
