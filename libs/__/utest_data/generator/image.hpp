/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "utest_data/config.hpp"

#include <core/base.hpp>
#include <core/type.hpp>

#include <data/image.hpp>

#include <optional>

namespace sight::utest_data::generator
{

/**
 * @brief   This class contains helper to generate images.
 */
class UTEST_DATA_CLASS_API image
{
public:

    /// Generate an image with random information (size, spacing, ...). Buffer is filled with random values.
    UTEST_DATA_API static void generateRandomImage(
        data::image::sptr _image,
        core::type _type,
        std::uint32_t _seed = 0
    );

    /// Generate an image.
    /// @param [inout] image image to generate
    /// @param [in] sizes image sizes
    /// @param [in] type image component type
    /// @param [in] format image pixel format
    /// @param [in] spacing image spacing
    /// @param [in] origin image origin
    /// @param [in] seed [optional] random seed used to generate the buffer. If not set, fill the buffer with 0.
    UTEST_DATA_API static void generateImage(
        data::image::sptr _image,
        const data::image::Size& _sizes           = {64, 64, 64},
        const data::image::Spacing& _spacing      = {0, 0, 0},
        const data::image::Origin& _origin        = {0, 0, 0},
        const core::type& _type                   = core::type::UINT8,
        const data::image::PixelFormat& _format   = data::image::PixelFormat::GRAY_SCALE,
        const std::optional<std::uint32_t>& _seed = std::nullopt
    );

    /// Fill image array with random value
    UTEST_DATA_API static void randomizeImage(data::image::sptr _image, std::uint32_t _seed = 0);

    /// Fill array with random value
    UTEST_DATA_API static void randomizeArray(data::array::sptr _array, std::uint32_t _seed = 0);
};

} // namespace sight::utest_data::generator
