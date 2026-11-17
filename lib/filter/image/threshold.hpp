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

#include <cstdint>

namespace sight::filter::image
{

/**
 * @brief Applies an inclusive threshold to a 3D integer image.
 * @param _input_image Image to threshold.
 * @param _lower_threshold Inclusive lower bound.
 * @param _upper_threshold Inclusive upper bound.
 * @param _output_image Output image.
 * @param _binary If true, set values in range to 255; otherwise preserve their values.
 */
SIGHT_FILTER_IMAGE_API void threshold(
    const data::image& _input_image,
    std::int64_t _lower_threshold,
    std::int64_t _upper_threshold,
    data::image& _output_image,
    bool _binary = true
);

} // namespace sight::filter::image
