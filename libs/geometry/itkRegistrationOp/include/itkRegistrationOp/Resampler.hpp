/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "itkRegistrationOp/config.hpp"

#include <data/Image.hpp>
#include <data/TransformationMatrix3D.hpp>

namespace itkRegistrationOp
{
/**
 * @brief The Resampler class
 */
class ITKREGISTRATIONOP_CLASS_API Resampler
{

public:

    /**
     * @brief transforms and resamples an image using ITK.
     * @param[in] _inImg the input data::Image.
     * @param[out] _outImg the resulting transformed image.
     * @param[in] _trf transform applied to the input.
     * @param[in] _targetImg image used to set the desired origin, spacing and size.
     */
    static ITKREGISTRATIONOP_API void resample(const data::Image::csptr& _inImg,
                                               const data::Image::sptr& outImg,
                                               const data::TransformationMatrix3D::csptr& _trf,
                                               const data::Image::csptr& _targetImg = nullptr);

    /**
     * @brief transforms and resamples the image into a new grid big enough so it won't crop the input image.
     * @param _img image to resample.
     * @param _trf transform applied to the image.
     * @param _outputSpacing desired sampling rate.
     * @return resampled image.
     */
    [[deprecated("Will be removed in sigth 22.0")]]
    static ITKREGISTRATIONOP_API data::Image::sptr resample(const data::Image::csptr& _img,
                                                            const data::TransformationMatrix3D::csptr& _trf,
                                                            const data::Image::SpacingType& _outputSpacing);

    /**
     * @brief transforms and resamples the image into a new grid big enough so it won't crop the input image.
     * @param _img image to resample.
     * @param _trf transform applied to the image.
     * @param _outputSpacing desired sampling rate.
     * @return resampled image.
     */
    static ITKREGISTRATIONOP_API data::Image::sptr resample(const data::Image::csptr& _img,
                                                            const data::TransformationMatrix3D::csptr& _trf,
                                                            const data::Image::Spacing& _outputSpacing);

};

}// namespace itkRegistrationOp
