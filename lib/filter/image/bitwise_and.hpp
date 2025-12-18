/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

namespace sight::filter::image
{

/**
 * @brief Perform a bitwise AND operation between two images.
 * @param _in_image     input image.
 * @param _mask_image   mask image.
 * @param _out_image    output image.
 */
SIGHT_FILTER_IMAGE_API void bitwise_and(
    const data::image& _in_image,
    const data::image& _mask_image,
    data::image& _out_image
);

} // namespace sight::filter::image
