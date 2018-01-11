/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "itkRegistrationOp/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

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
     * @param[in] _inImg the input fwData::Image.
     * @param[out] _outImg the resulting transformed image.
     * @param[in] _trf transform applied to the input.
     * @param[in] _targetImg image used to set the desired origin, spacing and size.
     */
    static ITKREGISTRATIONOP_API void resample(const ::fwData::Image::csptr& _inImg,
                                               const ::fwData::Image::sptr& outImg,
                                               const ::fwData::TransformationMatrix3D::csptr& _trf,
                                               const ::fwData::Image::csptr& _targetImg = nullptr);

    /**
     * @brief transforms and resamples the image into a new grid big enough so it won't crop the input image.
     * @param _img image to resample.
     * @param _trf transform applied to the image.
     * @param _outputSpacing desired sampling rate.
     * @return resampled image.
     */
    static ITKREGISTRATIONOP_API ::fwData::Image::sptr resample(const fwData::Image::csptr& _img,
                                                                const fwData::TransformationMatrix3D::csptr& _trf,
                                                                const fwData::Image::SpacingType& _outputSpacing);

};

}// namespace itkRegistrationOp
