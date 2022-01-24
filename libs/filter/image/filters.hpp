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

#include <itkImage.h>

#include <cstdint>

namespace sight::filter::image
{

/// Threshold filter
template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<std::uint8_t, DIM>::Pointer threshold(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    IMAGE_TYPE _lowerThreshold,
    IMAGE_TYPE _upperThreshold
);

/// Median filter
template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<IMAGE_TYPE, DIM>::Pointer median(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    size_t _x,
    size_t _y,
    size_t _z
);

/// Labeling filter
template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<std::uint8_t, DIM>::Pointer labeling(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _numLabels
);

/// FillHole2D filter
template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<IMAGE_TYPE, DIM>::Pointer fillHole2D(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _direction,
    IMAGE_TYPE _background,
    IMAGE_TYPE _foreground
);

/// Closing filter
template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<IMAGE_TYPE, DIM>::Pointer closing(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    size_t _x,
    size_t _y,
    size_t _z
);

} //namespace sight::filter::image.
