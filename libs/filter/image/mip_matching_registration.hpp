/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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
#include "filter/image/itk_image_caster.hpp"
#include "filter/image/metric.hpp"
#include "filter/image/resampler.hpp"

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <geometry/data/matrix4.hpp>

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
class mip_matching_registration;

/**
 * @brief Helper type containing the parameters and return values from mip_matching_registration::registerImage()
 * for use with the Dispatcher.
 */
struct RegistrationDispatch
{
    struct Parameters
    {
        data::image::csptr fixed;
        data::image::csptr moving;
        data::matrix4::sptr transform;
    };

    //------------------------------------------------------------------------------

    template<typename PIXELTYPE>
    void operator()(Parameters& _params)
    {
        filter::image::mip_matching_registration<PIXELTYPE>::registerImage(
            _params.moving,
            _params.fixed,
            _params.transform
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
class mip_matching_registration
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
        const data::image::csptr& _moving,
        const data::image::csptr& _fixed,
        data::matrix4::sptr& _transform
    );

private:

    enum class Direction : unsigned int
    {
        X = 0,
        Y = 1,
        Z = 2
    };

    using Image3DType    = itk::Image<PIX, 3>;
    using Image2DType    = itk::Image<float, 2>;
    using Image3DPtrType = typename itk::Image<PIX, 3>::Pointer;
    using Image2DPtrType = typename itk::Image<float, 2>::Pointer;

    using MIPFilterType           = itk::MaximumProjectionImageFilter<Image3DType, Image2DType>;
    using correlation_filter_t    = itk::FFTNormalizedCorrelationImageFilter<Image2DType, Image2DType>;
    using ExtractFilter3Dto2DType = itk::RegionOfInterestImageFilter<Image3DType, Image2DType>;
    using ExtractFilter2Dto2DType = itk::RegionOfInterestImageFilter<Image2DType, Image2DType>;
    using flip_filter_t           = itk::FlipImageFilter<Image3DType>;
    using min_max_calculator_t    = itk::MinimumMaximumImageCalculator<Image2DType>;

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
void mip_matching_registration<PIX>::registerImage(
    const data::image::csptr& _moving,
    const data::image::csptr& _fixed,
    data::matrix4::sptr& _transform
)
{
    const double fixed_voxel_volume = std::accumulate(
        _fixed->getSpacing().begin(),
        _fixed->getSpacing().end(),
        1.,
        std::multiplies<double>()
    );

    const double moving_voxel_volume = std::accumulate(
        _moving->getSpacing().begin(),
        _moving->getSpacing().end(),
        1.,
        std::multiplies<double>()
    );

    data::image::csptr fixed  = _fixed;
    data::image::csptr moving = _moving;

    // Resample the image with the smallest voxels to match the other's voxel size.
    if(fixed_voxel_volume < moving_voxel_volume)
    {
        auto inverse_transform = std::make_shared<data::matrix4>();
        geometry::data::invert(*_transform, *inverse_transform);

        fixed = filter::image::resampler::resample(_fixed, inverse_transform, _moving->getSpacing());
    }
    else
    {
        moving = filter::image::resampler::resample(_moving, _transform, _fixed->getSpacing());
    }

    const Image3DPtrType itk_moving = cast_to<PIX>(moving);
    const Image3DPtrType itk_fixed  = cast_to<PIX>(fixed);

    const auto moving_mip_x = computeMIP(itk_moving, Direction::X);
    const auto moving_mip_y = computeMIP(itk_moving, Direction::Y);
    const auto fixed_mip_x  = computeMIP(itk_fixed, Direction::X);
    const auto fixed_mip_y  = computeMIP(itk_fixed, Direction::Y);

    const auto trans_x = matchTemplate(fixed_mip_x, moving_mip_x);
    const auto trans_y = matchTemplate(fixed_mip_y, moving_mip_y);

    const std::array<double, 3> res {{trans_y[0], trans_x[1], trans_y[1]}};

    data::matrix4 translation;
    for(std::uint8_t i = 0 ; i != 3 ; ++i)
    {
        translation(i, 3) = res[i];
    }

    geometry::data::multiply(translation, *_transform, *_transform);
}

//------------------------------------------------------------------------------

template<class PIX>
typename mip_matching_registration<PIX>::Image2DPtrType mip_matching_registration<PIX>::computeMIP(
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
typename mip_matching_registration<PIX>::Image2DType::PointType mip_matching_registration<PIX>::matchTemplate(
    Image2DPtrType const& _template,
    Image2DPtrType const& img
)
{
    // The correlation filter works in pixel space and as such, requires that images occupy the same physical space, ie
    // have the same origin and spacings. Spacing is already OK thanks to the previous resampling, we deal with the
    // origin by setting it to some arbitrary value and adding the translation vector between origins to the computed
    // registration.
    Image2DType::PointType origin(0.);
    itk::Vector<Image2DType::PointValueType, 2> t_origin = img->GetOrigin() - _template->GetOrigin();
    _template->SetOrigin(origin);
    img->SetOrigin(origin);
    auto template_size = _template->GetLargestPossibleRegion().GetSize();

    // Compute normalized correlation between both images.
    auto correlation = correlation_filter_t::New();
    correlation->SetFixedImage(img);
    correlation->SetMovingImage(_template);
    correlation->SetRequiredFractionOfOverlappingPixels(0.2F);
    correlation->Update();

    // Find the position with the best correlation.
    auto max_correlation_finder = min_max_calculator_t::New();
    max_correlation_finder->SetImage(correlation->GetOutput());
    max_correlation_finder->Compute();
    auto max_idx = max_correlation_finder->GetIndexOfMaximum();

    // Go from pixel coordinates back to physical coordinates
    auto spacing = img->GetSpacing();
    max_idx[0] -= static_cast<std::int64_t>(template_size[0] - 1);
    max_idx[1] -= static_cast<std::int64_t>(template_size[1] - 1);
    Image2DType::PointType p;
    p[0] = static_cast<double>(max_idx[0]) * spacing[0];
    p[1] = static_cast<double>(max_idx[1]) * spacing[1];
    p   += t_origin;

    return p;
}

} // namespace sight::filter::image
