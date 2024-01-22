/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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
#include "filter/image/image_diff.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <filter/image/bresenham_line.hpp>

namespace sight::filter::image
{

/**
 * @brief Flood fills an image as long as the neighboring voxels are greater than the smallest seed value.
 */
class FILTER_IMAGE_CLASS_API min_max_propagation
{
public:

    enum mode
    {
        min,
        max,
        minmax
    };

    using coordinates_t = filter::image::bresenham_line::coordinates_t;

    using orientation_t = filter::image::bresenham_line::Orientation;

    using seeds_t = std::vector<coordinates_t>;

    /**
     * @brief Constructor.
     * @param[in] _in_image image in which we propagate.
     * @param[in,out] _out_image mask image containing the written values.
     * @param[in] _roi region of interest.
     */
    FILTER_IMAGE_API min_max_propagation(
        data::image::csptr _in_image,
        data::image::sptr _out_image,
        data::image::csptr _roi
    );

    /**
     * @brief propagates through the image starting from each seed as along as the encountered voxel value
     * is greater than smallest seed value.
     * @param[in] _seeds points where propagation is started.
     * @param[in] _value the value propagated through the buffer.
     * @param[in] _radius maximum propagation distance.
     * @param[in] _overwrite overwrite non zero voxels.
     * @param[in] _mode propagation mode (min, max, or minmax).
     * @return the differences in the image before and after propagation.
     */
    FILTER_IMAGE_API image_diff propagate(
        seeds_t& _seeds,
        data::image::buffer_t* _value,
        double _radius,
        bool _overwrite,
        mode _mode
    );

private:

    /// Seed list.
    seeds_t m_seeds;

    /// Input image. Where the voxel values are read from.
    data::image::csptr m_in_image;

    /// Region of interest.
    data::image::csptr m_roi;

    /// Output image. Where we write the flooded voxels.
    data::image::sptr m_out_image;
};

} // namespace sight::filter::image.
