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
#include "filter/image/ImageDiff.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <filter/image/BresenhamLine.hpp>

namespace sight::filter::image
{

/**
 * @brief Flood fills an image as long as the neighboring voxels are greater than the smallest seed value.
 */
class FILTER_IMAGE_CLASS_API MinMaxPropagation
{
public:

    enum Mode
    {
        MIN,
        MAX,
        MINMAX
    };

    typedef filter::image::BresenhamLine::CoordinatesType CoordinatesType;

    typedef data::helper::MedicalImage::Orientation OrientationType;

    typedef std::vector<CoordinatesType> SeedsType;

    /**
     * @brief Constructor.
     * @param[in] inImage image in which we propagate.
     * @param[in,out] outImage mask image containing the written values.
     * @param[in] roi region of interest.
     */
    FILTER_IMAGE_API MinMaxPropagation(
        const data::Image::csptr& inImage,
        const data::Image::sptr& outImage,
        const data::Image::csptr& roi
    );

    /**
     * @brief propagates through the image starting from each seed as along as the encountered voxel value
     * is greater than smallest seed value.
     * @param[in] seeds points where propagation is started.
     * @param[in] value the value propagated through the buffer.
     * @param[in] radius maximum propagation distance.
     * @param[in] overwrite overwrite non zero voxels.
     * @param[in] mode propagation mode (min, max, or minmax).
     * @return the differences in the image before and after propagation.
     */
    FILTER_IMAGE_API ImageDiff propagate(
        SeedsType& seeds,
        data::Image::BufferType* value,
        const double radius,
        const bool overwrite,
        const Mode mode
    );

private:

    /// Seed list.
    SeedsType m_seeds;

    /// Input image. Where the voxel values are read from.
    data::Image::csptr m_inImage;

    /// Region of interest.
    data::Image::csptr m_roi;

    /// Output image. Where we write the flooded voxels.
    data::Image::sptr m_outImage;
};

} // namespace sight::filter::image.
