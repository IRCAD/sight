/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#pragma once

#include <sight/filter/image//config.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

namespace sight::filter::image
{

/**
 * @brief Static class to compute Bresenham lines in 3D images.
 *
 */
class SIGHT_FILTER_IMAGE_CLASS_API bresenham_line
{
public:

    /// image orientation.
    using Orientation = data::helper::medical_image::axis_t;

    /// Voxel coordinate type.
    using coordinates_t = std::array<data::image::index_t, 3>;

    /// Series of coordinates making up a line.
    using path_t = std::vector<coordinates_t>;

    /// Computes a Bresenham line in a 3D image slice.
    SIGHT_FILTER_IMAGE_API static path_t draw(
        Orientation _orientation,
        const coordinates_t& _start_coord,
        const coordinates_t& _end_coord
    );
};

} // namespace sight::filter::image
