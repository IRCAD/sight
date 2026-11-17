/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <array>
#include <optional>

namespace sight::filter::image
{

/**
 * @brief Remaps pixel values in an image based on a dictionary mapping.
 *
 * Applies a value mapping to all pixels in an image. The mapping is defined by a dictionary
 * where index [0-255] maps to a target value (-1 means unmapped). For unmapped pixels,
 * the default value is used if provided.
 *
 * @param _image                Input/output image to relabel (modified in-place)
 * @param _dictionary           Mapping dictionary from input value (0-255) to output value
 *                              (-1 means unmapped)
 * @param _default_value        Default value for unmapped pixels (std::nullopt to leave unchanged)
 *
 * @note Works with any pixel type via template dispatcher
 */
SIGHT_FILTER_IMAGE_API void relabel(
    data::image& _image,
    const std::map<std::int32_t, std::int32_t>& _dictionary,
    std::optional<std::uint16_t> _default_value = std::nullopt
);

} // namespace sight::filter::image
