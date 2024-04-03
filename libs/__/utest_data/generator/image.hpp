/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <sight/utest_data/config.hpp>

#include <core/base.hpp>
#include <core/type.hpp>

#include <data/image.hpp>

#include <optional>

namespace sight::utest_data::generator
{

/**
 * @brief   This class contains helper to generate images.
 */
class SIGHT_UTEST_DATA_CLASS_API image
{
public:

    /// Generate an image with random information (size, spacing, ...). Buffer is filled with random values.
    SIGHT_UTEST_DATA_API static void generate_random_image(
        data::image::sptr _image,
        core::type _type,
        std::uint32_t _seed = 0
    );

    /// Generate an image.
    /// @param [inout] _image image to generate
    /// @param [in] _sizes image sizes
    /// @param [in] _spacing image spacing
    /// @param [in] _origin image origin
    /// @param [in] _type image component type
    /// @param [in] _format image pixel format
    /// @param [in] _seed [optional] random seed used to generate the buffer. If not set, fill the buffer with 0.
    SIGHT_UTEST_DATA_API static void generate_image(
        data::image::sptr _image,
        const data::image::size_t& _sizes             = {64, 64, 64},
        const data::image::spacing_t& _spacing        = {0, 0, 0},
        const data::image::origin_t& _origin          = {0, 0, 0},
        const core::type& _type                       = core::type::UINT8,
        const enum data::image::pixel_format& _format = data::image::pixel_format::gray_scale,
        const std::optional<std::uint32_t>& _seed     = std::nullopt
    );

    /// Fill image array with random value
    SIGHT_UTEST_DATA_API static void randomize_image(data::image::sptr _image, std::uint32_t _seed = 0);

    /// Fill array with random value
    SIGHT_UTEST_DATA_API static void randomize_array(data::array::sptr _array, std::uint32_t _seed = 0);
};

} // namespace sight::utest_data::generator
