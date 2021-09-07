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

#include "filter/image/config.hpp"
#include "filter/image/ItkImageCaster.hpp"
#include "filter/image/Metric.hpp"
#include "filter/image/Resampler.hpp"

#include <data/Image.hpp>
#include <data/Matrix4.hpp>

#include <geometry/data/Matrix4.hpp>

#include <io/itk/itk.hpp>

#include <itkFFTNormalizedCorrelationImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMaximumProjectionImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

#include <numeric>

namespace sight::filter::image
{

template<class PIX>
class MIPMatchingRegistration;

/**
 * @brief Helper type containing the parameters and return values from MIPMatchingRegistration::registerImage()
 * for use with the Dispatcher.
 */
struct RegistrationDispatch
{
    struct Parameters
    {
        data::Image::csptr fixed;
        data::Image::csptr moving;
        data::Matrix4::sptr transform;
    };

    //------------------------------------------------------------------------------

    template<typename PIXELTYPE>
    void operator()(Parameters& params)
    {
        filter::image::MIPMatchingRegistration<PIXELTYPE>::registerImage(
            params.moving,
            params.fixed,
            params.transform
        );
    }
};

/**
 * @brief Fast approximate registration, made of pure translation roughly matching two 3-dimensional images
 * of the given pixel type.
 *
 * @tparam PIX Subpixel type of the images.
 */
template<typename PIX>
class MIPMatchingRegistration
{
public:

    /**
     * @brief Compute a fast registration containing translation only.
     * @param[in] _moving image that will be transformed
     * @param[in] _fixed fixed image
     * @param[inout] _transform initial transform applied to the moving image, updated after registration.
     *
     * @pre The transformed moving image must approximately match
     */
    static void registerImage(
        const data::Image::csptr& _moving,
        const data::Image::csptr& _fixed,
        data::Matrix4::sptr& _transform
    );

private:

    enum class Direction : unsigned int
    {
        X = 0,
        Y = 1,
        Z = 2
    };

    using Image3DType    = ::itk::Image<PIX, 3>;
    using Image2DType    = ::itk::Image<float, 2>;
    using Image3DPtrType = typename ::itk::Image<PIX, 3>::Pointer;
    using Image2DPtrType = typename ::itk::Image<float, 2>::Pointer;

    using MIPFilterType           = ::itk::MaximumProjectionImageFilter<Image3DType, Image2DType>;
    using CorrelationFilterType   = ::itk::FFTNormalizedCorrelationImageFilter<Image2DType, Image2DType>;
    using ExtractFilter3Dto2DType = ::itk::RegionOfInterestImageFilter<Image3DType, Image2DType>;
    using ExtractFilter2Dto2DType = ::itk::RegionOfInterestImageFilter<Image2DType, Image2DType>;
    using FlipFilterType          = ::itk::FlipImageFilter<Image3DType>;
    using MinMaxCalculatorType    = ::itk::MinimumMaximumImageCalculator<Image2DType>;

    /**
     * @brief Compute the maximum intensity projection (MIP) of an image along the specified axis.
     */
    static Image2DPtrType computeMIP(Image3DPtrType const& img, Direction d);

    /**
     * @brief Perform the template matching.
     *
     * @return Translation from the template to the target.
     */
    static typename Image2DType::PointType matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img);
};

//------------------------------------------------------------------------------

template<class PIX>
void MIPMatchingRegistration<PIX>::registerImage(
    const data::Image::csptr& _moving,
    const data::Image::csptr& _fixed,
    data::Matrix4::sptr& _transform
)
{
    const double fixedVoxelVolume = std::accumulate(
        _fixed->getSpacing2().begin(),
        _fixed->getSpacing2().end(),
        1.,
        std::multiplies<double>()
    );

    const double movingVoxelVolume = std::accumulate(
        _moving->getSpacing2().begin(),
        _moving->getSpacing2().end(),
        1.,
        std::multiplies<double>()
    );

    data::Image::csptr fixed  = _fixed,
                       moving = _moving;

    // Resample the image with the smallest voxels to match the other's voxel size.
    if(fixedVoxelVolume < movingVoxelVolume)
    {
        auto inverseTransform = data::Matrix4::New();
        geometry::data::invert(*_transform, *inverseTransform);

        fixed = filter::image::Resampler::resample(_fixed, inverseTransform, _moving->getSpacing2());
    }
    else
    {
        moving = filter::image::Resampler::resample(_moving, _transform, _fixed->getSpacing2());
    }

    const Image3DPtrType itkMoving = castTo<PIX>(moving);
    const Image3DPtrType itkFixed  = castTo<PIX>(fixed);

    const auto movingMipX = computeMIP(itkMoving, Direction::X);
    const auto movingMipY = computeMIP(itkMoving, Direction::Y);
    const auto fixedMipX  = computeMIP(itkFixed, Direction::X);
    const auto fixedMipY  = computeMIP(itkFixed, Direction::Y);

    const auto transX = matchTemplate(fixedMipX, movingMipX);
    const auto transY = matchTemplate(fixedMipY, movingMipY);

    const std::array<double, 3> res {{transY[0], transX[1], transY[1]}};

    data::Matrix4 translation;
    for(std::uint8_t i = 0 ; i != 3 ; ++i)
    {
        translation.setCoefficient(i, 3, res[i]);
    }

    geometry::data::multiply(translation, *_transform, *_transform);
}

//------------------------------------------------------------------------------

template<class PIX>
typename MIPMatchingRegistration<PIX>::Image2DPtrType MIPMatchingRegistration<PIX>::computeMIP(
    Image3DPtrType const& img,
    Direction d
)
{
    auto filter = MIPFilterType::New();
    filter->SetInput(img);
    filter->SetProjectionDimension(static_cast<unsigned int>(d));
    filter->Update();
    return filter->GetOutput();
}

//------------------------------------------------------------------------------

template<class PIX>
typename MIPMatchingRegistration<PIX>::Image2DType::PointType MIPMatchingRegistration<PIX>::matchTemplate(
    Image2DPtrType const& _template,
    Image2DPtrType const& img
)
{
    // The correlation filter works in pixel space and as such, requires that images occupy the same physical space, ie
    // have the same origin and spacings. Spacing is already OK thanks to the previous resampling, we deal with the
    // origin by setting it to some arbitrary value and adding the translation vector between origins to the computed
    // registration.
    Image2DType::PointType origin(0.);
    ::itk::Vector<Image2DType::PointValueType, 2> tOrigin = img->GetOrigin() - _template->GetOrigin();
    _template->SetOrigin(origin);
    img->SetOrigin(origin);
    auto templateSize = _template->GetLargestPossibleRegion().GetSize();

    // Compute normalized correlation between both images.
    auto correlation = CorrelationFilterType::New();
    correlation->SetFixedImage(img);
    correlation->SetMovingImage(_template);
    correlation->SetRequiredFractionOfOverlappingPixels(0.2f);
    correlation->Update();

    // Find the position with the best correlation.
    auto maxCorrelationFinder = MinMaxCalculatorType::New();
    maxCorrelationFinder->SetImage(correlation->GetOutput());
    maxCorrelationFinder->Compute();
    auto maxIdx = maxCorrelationFinder->GetIndexOfMaximum();

    // Go from pixel coordinates back to physical coordinates
    auto spacing = img->GetSpacing();
    maxIdx[0] -= (templateSize[0] - 1);
    maxIdx[1] -= (templateSize[1] - 1);
    Image2DType::PointType p;
    p[0] = static_cast<double>(maxIdx[0]) * spacing[0];
    p[1] = static_cast<double>(maxIdx[1]) * spacing[1];
    p   += tOrigin;

    return p;
}

} // itkRegistrationOp
