/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "filter/image/bresenham_line.hpp"

#include <cmath>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

bresenham_line::path_t bresenham_line::draw(
    const Orientation _orientation,
    const coordinates_t& _start_coord,
    const coordinates_t& _end_coord
)
{
    std::size_t dim0 = 0;
    std::size_t dim1 = 1;

    switch(_orientation)
    {
        case Orientation::X_AXIS:
            dim0 = 1;
            dim1 = 2;
            break;

        case Orientation::Y_AXIS:
            dim0 = 0;
            dim1 = 2;
            break;

        case Orientation::Z_AXIS:
            dim0 = 0;
            dim1 = 1;
            break;
    }

    const coordinates_t::value_type begin_x = _start_coord[dim0];
    const coordinates_t::value_type begin_y = _start_coord[dim1];
    const coordinates_t::value_type end_x   = _end_coord[dim0];
    const coordinates_t::value_type end_y   = _end_coord[dim1];

    const std::int64_t dx = std::abs(
        static_cast<std::int64_t>(end_x) - static_cast<std::int64_t>(begin_x)
    );
    const std::int64_t dy = std::abs(
        static_cast<std::int64_t>(end_y) - static_cast<std::int64_t>(begin_y)
    );

    const int sx = (end_x < begin_x) ? -1 : 1;
    const int sy = (end_y < begin_y) ? -1 : 1;

    std::int64_t e = dx - dy;

    coordinates_t current_coord = _start_coord;

    path_t result;
    auto path_length = static_cast<coordinates_t::value_type>(std::max(dx, dy));
    result.reserve(path_length);

    result.push_back(current_coord);

    while(current_coord != _end_coord)
    {
        const std::int64_t e2 = e * 2;
        auto cx               = static_cast<std::int64_t>(current_coord[dim0]);
        auto cy               = static_cast<std::int64_t>(current_coord[dim1]);

        if(e2 > -static_cast<std::int64_t>(dy))
        {
            e  -= dy;
            cx += sx;
        }

        if(e2 < static_cast<std::int64_t>(dx))
        {
            e  += dx;
            cy += sy;
        }

        current_coord[dim0] = static_cast<coordinates_t::value_type>(cx);
        current_coord[dim1] = static_cast<coordinates_t::value_type>(cy);
        result.push_back(current_coord);
    }

    return result;
}

} // namespace sight::filter::image
