/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <array>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace sight::ui
{

using int2_t      = std::array<int, 2>;
using int3_t      = std::array<int, 3>;
using double2_t   = std::array<double, 2>;
using double3_t   = std::array<double, 3>;
using color_t     = std::array<std::uint8_t, 4>;
using enum_list_t = std::vector<std::string>;

using parameter_t = std::variant<bool, double, double2_t, double3_t, int, int2_t, int3_t, color_t, std::string,
                                 enum_list_t>;

} // sight::ui
