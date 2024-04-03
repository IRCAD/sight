/************************************************************************
 *
 * Copyright (C) 2015-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include <core/clock.hpp>

#include <array>
#include <cstdint>

namespace sight::data::tools
{

/// Structure to store picking information
struct  picking_info
{
    enum modifier
    {
        none  = 0,
        ctrl  = 1,
        shift = 2
    };

    enum class event : std::int8_t
    {
        mouse_left_up = 0,
        mouse_right_up,
        mouse_middle_up,
        mouse_wheelforward,
        mouse_left_down,
        mouse_right_down,
        mouse_middle_down,
        mouse_wheelbackward,
        mouse_move,
        key_press,
    };

    /// Position clicked in world coordinates
    std::array<double, 3> m_world_pos {};
    /// Id of the cell
    int m_cell_id = -1;
    /// Id of the closest point
    int m_closest_point_id = -1;
    /// Mouse event
    event m_event_id = event::mouse_left_up;
    /// Modifier mask
    std::int8_t m_modifier_mask = 0;
    /// Key event
    char m_key_pressed = 0;

    core::clock::type m_timestamp {};
};

} // namespace sight::data::tools
