/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "imageFilterOp/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <cstdint>

namespace imageFilterOp
{

/// Labels filter without template.
IMAGEFILTEROP_API ::fwData::Image::sptr labeling(::fwData::Image::sptr image, unsigned int _numLabels);

/// Labels filter with centroids.
IMAGEFILTEROP_API void computeCentroids(::fwData::Image::sptr image,
                                        std::vector< ::fwData::PointList::sptr > pointListCentroids,
                                        std::vector< std::vector<size_t> > pointListLabels);

} //namespace imageFilterOp.
