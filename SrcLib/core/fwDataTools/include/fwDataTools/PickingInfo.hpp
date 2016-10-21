/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_PICKINGINFO_HPP__
#define __FWDATATOOLS_PICKINGINFO_HPP__

#include "fwDataTools/config.hpp"

#include <fwCore/HiResClock.hpp>

#include <cstdint>

namespace fwDataTools
{

/// Structure to store picking information
struct  PickingInfo
{
    enum Modifier
    {
        NONE  = 0,
        CTRL  = 1,
        SHIFT = 2,
    };

    enum class Event : std::int8_t
    {
        MOUSE_LEFT_UP = 0,
        MOUSE_RIGHT_UP,
        MOUSE_MIDDLE_UP,
        MOUSE_WHEELFORWARD,
        MOUSE_LEFT_DOWN,
        MOUSE_RIGHT_DOWN,
        MOUSE_MIDDLE_DOWN,
        MOUSE_WHEELBACKWARD,
        MOUSE_MOVE,
        KEY_PRESS,
    };

    /// Position clicked in world coordinates
    double m_worldPos[3];
    /// Id of the cell
    int m_cellId = -1;
    /// Id of the closest point
    int m_closestPointId = -1;
    /// Mouse event
    Event m_eventId = Event::MOUSE_LEFT_UP;
    /// Modifier mask
    std::int8_t m_modifierMask = 0;
    /// Key event
    char m_keyPressed = 0;

    ::fwCore::HiResClock::HiResClockType m_timestamp;

    FWDATATOOLS_API PickingInfo();

};

}

#endif // __FWDATATOOLS_PICKINGINFO_HPP__

