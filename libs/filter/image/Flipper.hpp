/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "filter/image/config.hpp"

#include <data/Image.hpp>
#include <data/Matrix4.hpp>

namespace sight::filter::image
{
/**
 * @brief The Flipper class
 */
class FILTER_IMAGE_CLASS_API Flipper
{

public:

    /**
     * @brief flips an image according to the axis parameter.
     * @param _inImage image to flip.
     * @param _outImage flipped image.
     * @param _flipAxis axes on which to flip the image.
     */
    static FILTER_IMAGE_API void flip(const data::Image::csptr& _inImage,
                                      const data::Image::sptr& _outImage,
                                      const std::array<bool, 3>& _flipAxis);

};

}// namespace sight::filter::image
