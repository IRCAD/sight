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

#pragma once

#include "filter/image/config.hpp"

#include <data/Image.hpp>
#include <data/PointList.hpp>

namespace sight::filter::image
{

/**
 * @brief Detects sphere-like features in images.
 */
class FILTER_IMAGE_CLASS_API SpheroidExtraction
{
public:

    /**
     * @brief Extracts spheres centers in an image with a given radius and above a given intensity value.
     * @param[in] _image image in which to search for spheres
     * @param[in] _threshold minimum intensity value of the spheres
     * @param[in] _radiusMin minimum sphere radius
     * @param[in] _radiusMax maximum sphere radius
     * @param[in] _elongationMin minimum elongation to recognize a sphere, perfect spheres have an elongation of one.
     * @param[in] _elongationMax maximum elongation to recognize a sphere, perfect spheres have an elongation of one.
     * @return detected spheres' center
     */
    FILTER_IMAGE_API static data::PointList::sptr extract(
        const data::Image::csptr& _image,
        const double _threshold,
        const double _radiusMin,
        const double _radiusMax,
        const double _elongationMin,
        const double _elongationMax
    );
};

} // namespace sight::filter::image.
