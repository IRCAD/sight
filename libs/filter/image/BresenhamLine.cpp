/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "filter/image/BresenhamLine.hpp"

#include <cmath>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

BresenhamLine::PathType BresenhamLine::draw(const data::tools::helper::MedicalImage::Orientation orientation,
                                            const CoordinatesType& startCoord, const CoordinatesType& endCoord)
{
    size_t dim0 = 0, dim1 = 1;

    switch(orientation)
    {
        case data::tools::helper::MedicalImage::X_AXIS:
            dim0 = 1;
            dim1 = 2;
            break;
        case data::tools::helper::MedicalImage::Y_AXIS:
            dim0 = 0;
            dim1 = 2;
            break;
        case data::tools::helper::MedicalImage::Z_AXIS:
            dim0 = 0;
            dim1 = 1;
            break;
    }

    const CoordinatesType::value_type beginX = startCoord[dim0];
    const CoordinatesType::value_type beginY = startCoord[dim1];
    const CoordinatesType::value_type endX   = endCoord[dim0];
    const CoordinatesType::value_type endY   = endCoord[dim1];

    const std::int64_t dx = std::abs(
        static_cast< std::int64_t >(endX) - static_cast< std::int64_t >(beginX));
    const std::int64_t dy = std::abs(
        static_cast< std::int64_t >(endY) - static_cast< std::int64_t >(beginY));

    const int sx = (endX < beginX) ? -1 : 1;
    const int sy = (endY < beginY) ? -1 : 1;

    std::int64_t e = dx - dy;

    CoordinatesType currentCoord = startCoord;

    PathType result;
    CoordinatesType::value_type pathLength = static_cast<CoordinatesType::value_type>(std::max(dx, dy));
    result.reserve(pathLength);

    result.push_back(currentCoord);

    while(currentCoord != endCoord)
    {
        const std::int64_t e2 = e * 2;
        std::int64_t cx       = static_cast<std::int64_t>(currentCoord[dim0]);
        std::int64_t cy       = static_cast<std::int64_t>(currentCoord[dim1]);

        if(e2 > -static_cast< std::int64_t >(dy))
        {
            e  -= dy;
            cx += sx;
        }
        if(e2 < static_cast< std::int64_t >(dx))
        {
            e  += dx;
            cy += sy;
        }

        currentCoord[dim0] = static_cast<CoordinatesType::value_type>(cx);
        currentCoord[dim1] = static_cast<CoordinatesType::value_type>(cy);
        result.push_back(currentCoord);
    }

    return result;
}

} // namespace sight::filter::image
