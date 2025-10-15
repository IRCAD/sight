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
 * @brief Subtracts one image from another.
 * @param _image1      first input image.
 * @param _image2      second input image to subtract from the first.
 * @param _output      output image where the result is stored.
 */
SIGHT_FILTER_IMAGE_API void substract(const data::image& _image1, const data::image& _image2, data::image& _output);

} // namespace sight::filter::image
