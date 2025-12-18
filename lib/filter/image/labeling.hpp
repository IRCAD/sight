/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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

#include <sight/filter/image/config.hpp>

#include <data/image.hpp>
#include <data/point_list.hpp>

#include <cstdint>

namespace sight::filter::image
{

/// Labels filter without template.
SIGHT_FILTER_IMAGE_API data::image::sptr labeling(data::image::sptr _image, unsigned int _num_labels);

/// Labels filter with centroids.
SIGHT_FILTER_IMAGE_API void compute_centroids(
    data::image::sptr _image,
    std::vector<data::point_list::sptr> _point_list_centroids,
    std::vector<std::vector<std::size_t> > _point_list_labels
);

/// Convert a label image to a binary mask image.
SIGHT_FILTER_IMAGE_API void convert_label_image_to_binary_mask(
    const data::image& label_image,
    data::image& mask_image,
    const std::string& label_field_name
);

} //namespace sight::filter::image.
