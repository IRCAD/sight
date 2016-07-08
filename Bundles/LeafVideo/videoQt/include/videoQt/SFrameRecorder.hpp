/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_SFRAMERECORDER_HPP__
#define __VIDEOQT_SFRAMERECORDER_HPP__

#include "videoQt/config.hpp"

#include <extData/FrameTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/IController.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>

#include <boost/filesystem/path.hpp>


namespace fwData
{
class Image;
}

namespace videoQt
{
/**
 * @brief This service saves the timeline's frame on disk.
 */
class VIDEOQT_CLASS_API SFrameRecorder : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SFrameRecorder)(::fwServices::IController) );

    /// Contructor. Do nothing.
    VIDEOQT_API SFrameRecorder() throw();

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SFrameRecorder() throw();

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

    ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

protected:
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Does nothing.
    VIDEOQT_API virtual void starting() throw( ::fwTools::Failed );

    /// Does nothing.
    VIDEOQT_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing.
    VIDEOQT_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @code{.xml}
       <service impl="::videoQt::SFrameRecorder" autoConnect="yes">
           <path>...</path>
       </service>
       @endcode
     * - \b path: path to the directory where the frames are saved
     **/
    VIDEOQT_API virtual void configuring() throw( ::fwTools::Failed );

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
    ::fwServices::helper::SigSlotConnection m_connections;
};

} //namespace videoQt


#endif // __VIDEOQT_SFRAMERECORDER_HPP__
