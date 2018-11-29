/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "arServices/config.hpp"

#include <fwCore/HiResClock.hpp>

#include <fwServices/IService.hpp>

namespace arServices
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
class ARSERVICES_CLASS_API ITracker : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (ITracker)(::fwServices::IService) );

    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_TRACK_SLOT;
    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_START_TRACKING_SLOT;
    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_STOP_TRACKING_SLOT;

    ARSERVICES_API static const ::fwServices::IService::KeyType s_TIMELINE_INPUT;
    ARSERVICES_API static const ::fwServices::IService::KeyType s_FRAME_INOUT;

    /// Defines the auto-connection between the timeline and the 'track' slot
    ARSERVICES_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

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
    ARSERVICES_API ITracker();

    ///@brief ITracker destructor. Do nothing.
    ARSERVICES_API virtual ~ITracker();

    ARSERVICES_API virtual void configuring() override;

    /**
     * @brief This method calls tracking.
     * If m_dropObj is true, tracking is called only if the given timestamp is greater than m_lastTimestamp.
     * @warning If tracking is stopped, this method does nothing.
     * @note You should connect this method to the input timeline
     */
    ARSERVICES_API virtual void track(::fwCore::HiResClock::HiResClockType timestamp);

    /// start the tracking
    ARSERVICES_API virtual void startTracking();

    /// stop the tracking
    ARSERVICES_API virtual void stopTracking();

    /**
     * @brief process the tracking
     * @param[in,out] timestamp the timestamp of the processes object of the timeline
     */
    ARSERVICES_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp) = 0;

    /// timestamp of the last tracking
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// If true, only last object is retrived
    bool m_dropObj;

    /// If false, the trackMethod does nothing
    bool m_isTracking;

};
} // arServices
