/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2016 IHU Strasbourg
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

#include "data/config.hpp"

#include <core/hires_clock.hpp>

#include <array>
#include <cstdint>

namespace sight::data::tools
{

/// Structure to store picking information
struct  PickingInfo
{
    enum Modifier
    {
        NONE  = 0,
        CTRL  = 1,
        SHIFT = 2
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
    std::array<double, 3> m_worldPos {};
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

    core::hires_clock::type m_timestamp {};
};

} // namespace sight::data::tools
