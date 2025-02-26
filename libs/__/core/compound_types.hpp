/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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
#include <vector>

namespace sight
{

using ivec2_t     = std::array<std::int64_t, 2>;
using ivec3_t     = std::array<std::int64_t, 3>;
using ivec4_t     = std::array<std::int64_t, 4>;
using vec2f_t     = std::array<float, 2>;
using vec3f_t     = std::array<float, 3>;
using vec4f_t     = std::array<float, 4>;
using vec2d_t     = std::array<double, 2>;
using vec3d_t     = std::array<double, 3>;
using vec4d_t     = std::array<double, 4>;
using color_t     = std::array<std::uint8_t, 4>;
using enum_list_t = std::vector<std::string>;

} // namespace sight
