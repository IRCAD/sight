/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/filter/image/config.hpp>

#include <data/image.hpp>

#include <vector>

namespace sight::filter::image
{

/**
 * @brief Overlays a sequence of 3D scalar images, with zero treated as transparent.
 *
 * Each nonzero pixel replaces the accumulated value; zero pixels leave it unchanged. The last nonzero pixel in the
 * sequence wins.
 * @param _images Images to overlay, in priority order.
 * @param _output_image Output image.
 */
SIGHT_FILTER_IMAGE_API void over(
    const std::vector<data::image::csptr>& _images,
    data::image& _output_image
);

} // namespace sight::filter::image
