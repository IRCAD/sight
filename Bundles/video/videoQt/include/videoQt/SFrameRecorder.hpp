/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "videoQt/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>

#include <fwServices/IController.hpp>

#include <fwTools/fwID.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Image;
}

namespace videoQt
{

/**
 * @brief This service saves the timeline's frame on disk.
 *
 * The service saves the frame when it is pushed in the timeline.
 *
 * @warning In order to start/stop recording, this service must not be auto-connected.
 *
 * @section Slots Slots
 * - \b saveFrame(::fwCore::HiResClock::HiResClockType timestamp) : save the frame with the given timestamp
 * - \b startRecord() : start listening the timeline
 * - \b stopRecord() : stop listening the timeline
 * - \b pauseRecord() : toggle (start or stop) listening the the timeline
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::videoQt::SFrameRecorder">
       <in key="frameTL" uid="..." />
       <path>...</path>
    </service>
   @endcode
 * @subsection Input Input
 * - \b frameTL [ ::arData::FrameTL]: timeline containing the frame to save on disk.
 * @subsection Configuration Configuration
 * - \b path: path to the directory where the frames are saved
 */
class VIDEOQT_CLASS_API SFrameRecorder : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SFrameRecorder, ::fwServices::IController);

    /// Contructor. Do nothing.
    VIDEOQT_API SFrameRecorder() noexcept;

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SFrameRecorder() noexcept;

    /**
     * @name Slots API
     * @{
     */
    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SAVE_FRAME_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> SaveFrameSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_START_RECORD_SLOT;
    typedef ::fwCom::Slot<void ()> StartRecordSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_STOP_RECORD_SLOT;
    typedef ::fwCom::Slot<void ()> StopRecordSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_PAUSE_RECORD_SLOT;
    typedef ::fwCom::Slot<void ()> PauseRecordSlotType;
    /// @}

protected:
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Does nothing.
    VIDEOQT_API virtual void starting() override;

    /// Does nothing.
    VIDEOQT_API virtual void stopping() override;

    /// Does nothing.
    VIDEOQT_API virtual void updating() override;

    /// Parse the configuration
    VIDEOQT_API virtual void configuring() override;

private:

    /// SLOT: saves frame on disk
    void saveFrame(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: start record
    void startRecord();

    /// SLOT: stop record
    void stopRecord();

    /// SLOT: pause record
    void pauseRecord();

    /// Slot to save frames
    SaveFrameSlotType::sptr m_slotSaveFrame;

    /// Slot to start record
    StartRecordSlotType::sptr m_slotStartRecord;

    /// Slot to stop record
    StopRecordSlotType::sptr m_slotStopRecord;

    /// Slot to pause record
    PauseRecordSlotType::sptr m_slotPauseRecord;

    /// directory where the frames are saved
    ::boost::filesystem::path m_path;

    /// Counter of the number of frame
    unsigned int m_count;

    /// True if it is recording
    bool m_isRecording;

    /// True if it is paused
    bool m_isPaused;

    /// Helper to manage connections between the timeline and this service.
    ::fwCom::helper::SigSlotConnection m_connections;
};

} //namespace videoQt
