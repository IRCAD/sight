/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include <cstdint>

namespace sight::filter::image
{

/// Labels filter without template.
FILTER_IMAGE_API data::Image::sptr labeling(data::Image::sptr image, unsigned int _numLabels);

/// Labels filter with centroids.
FILTER_IMAGE_API void computeCentroids(
    data::Image::sptr image,
    std::vector<data::PointList::sptr> pointListCentroids,
    std::vector<std::vector<std::size_t> > pointListLabels
);

} //namespace sight::filter::image.
