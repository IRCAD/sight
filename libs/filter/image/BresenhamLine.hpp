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

#include "filter/image//config.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

namespace sight::filter::image
{

/**
 * @brief Static class to compute Bresenham lines in 3D images.
 *
 */
class FILTER_IMAGE_CLASS_API BresenhamLine
{
public:

    /// Voxel coordinate type.
    typedef std::array<data::Image::IndexType, 3> CoordinatesType;

    /// Series of coordinates making up a line.
    typedef std::vector<CoordinatesType> PathType;

    /// Computes a Bresenham line in a 3D image slice.
    FILTER_IMAGE_API static PathType draw(
        const data::helper::MedicalImage::Orientation orientation,
        const CoordinatesType& startCoord,
        const CoordinatesType& endCoord
    );
};

}
