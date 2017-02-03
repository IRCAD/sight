/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKER_ITRACKER_HPP__
#define __TRACKER_ITRACKER_HPP__

#include "tracker/config.hpp"

#include <fwCore/HiResClock.hpp>

#include <fwServices/IService.hpp>

namespace tracker
{

/**
 * @brief  This interface defines Tracker service API.
 *
 * The sub-classes must implements the method 'tracking(timestamp)' that is called by 'track' slot.
 *
 * If 'dropObj' is enabled, the sub-class 'tracking' method is called with the last timestamp in the timeline if the
 * received timestamp is greater than the last processed timestamp.
 *
 * @section Slots Slots
 * - \b track(::fwCore::HiResClock::HiResClockType  timestamp) : performs the tracking, does nothing if the tracking is
 *   not started.
 * - \b startTracking() : start the tracking
 * - \b stopTracking() : stop the tracking
 *
 * @subsection Input Input
 * - \b timeline : timeline used to retrieve the tracked objects
 * @subsection Configuration Configuration
 * - \b dropObj(optional, default=true) : defines if the tracker should drop few objects from the timeline (and always
 *   get the last one) or not.
 */
class TRACKER_CLASS_API ITracker : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (ITracker)(::fwServices::IService) );

    TRACKER_API static const ::fwCom::Slots::SlotKeyType s_TRACK_SLOT;
    TRACKER_API static const ::fwCom::Slots::SlotKeyType s_START_TRACKING_SLOT;
    TRACKER_API static const ::fwCom::Slots::SlotKeyType s_STOP_TRACKING_SLOT;

    TRACKER_API static const ::fwServices::IService::KeyType s_TIMELINE_INPUT;

    /// Defines the auto-connection between the timeline and the 'track' slot
    TRACKER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Return true if the tracking is started.
    bool isTracking() const
    {
        return m_isTracking;
    }

    /// Enable/Disable drop
    void enableDrop(bool enable)
    {
        m_dropObj = enable;
    }

    /// Return true if tracker drop frames
    bool isDropping() const
    {
        return m_dropObj;
    }

protected:

    ///@brief ITracker constructor. Do nothing.
    TRACKER_API ITracker();

    ///@brief ITracker destructor. Do nothing.
    TRACKER_API virtual ~ITracker();

    TRACKER_API virtual void configuring() throw (::fwTools::Failed);

    /**
     * @brief This method calls tracking.
     * If m_dropObj is true, tracking is called only if the given timestamp is greater than m_lastTimestamp.
     * @warning If tracking is stopped, this method does nothing.
     * @note You should connect this method to the input timeline
     */
    TRACKER_API virtual void track(::fwCore::HiResClock::HiResClockType timestamp);

    /// start the tracking
    TRACKER_API virtual void startTracking();

    /// stop the tracking
    TRACKER_API virtual void stopTracking();

    /**
     * @brief process the tracking
     * @param[in,out] timestamp the timestamp of the processes object of the timeline
     */
    TRACKER_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp) = 0;

    /// timestamp of the last tracking
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// If true, only last object is retrived
    bool m_dropObj;

    /// If false, the trackMethod does nothing
    bool m_isTracking;

};
} // tracker

#endif // __TRACKER_ITRACKER_HPP__
