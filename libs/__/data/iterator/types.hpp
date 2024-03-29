/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

namespace sight::data::iterator
{

/// Used to iterate through a RGB image of type 'uint8'
struct rgb
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

/// Used to iterate through a RGBA image of type 'uint8'
struct rgba
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

/// Used to iterate through a BGR image of type 'uint8'
struct bgr
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
};

/// Used to iterate through a BGRA image of type 'uint8'
struct bgra
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;
};

/// Used to iterate through a RGBA image of type 'uint8'
struct rgba16
{
    std::uint16_t r;
    std::uint16_t g;
    std::uint16_t b;
    std::uint16_t a;
};

namespace point
/// Stores a representation of a point.
{

struct xyz
{
    float x;
    float y;
    float z;
};

/// Stores a representation of a normal.
struct nxyz
{
    float nx;
    float ny;
    float nz;
};

/// Stores a representation of a texture coordinate.
struct uv
{
    float u;
    float v;
};

/// Used to iterate through colors
using rgba = data::iterator::rgba;

} // namespace point

using cell_t  = std::uint32_t;
using point_t = std::uint32_t;
using size_t  = std::uint32_t;

namespace cell
{

struct point
{
    cell_t pt;
};

struct line
{
    std::array<cell_t, 2> pt;
};

struct triangle
{
    std::array<cell_t, 3> pt;
};

struct quad
{
    std::array<cell_t, 4> pt;
};

struct tetra
{
    std::array<cell_t, 4> pt;
};

/// Stores a representation of a normal.
struct nxyz
{
    float nx;
    float ny;
    float nz;
};

/// Stores a representation of a texture coordinate.
struct uv
{
    float u;
    float v;
};

/// Used to iterate through colors
struct rgba
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

} // namespace cell

} // namespace sight::data::iterator
