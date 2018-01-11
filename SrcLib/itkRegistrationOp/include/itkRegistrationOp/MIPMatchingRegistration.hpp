/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "itkRegistrationOp/config.hpp"
#include "itkRegistrationOp/ItkImageCaster.hpp"
#include "itkRegistrationOp/Metric.hpp"
#include "itkRegistrationOp/Resampler.hpp"

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwItkIO/itk.hpp>

#include <itkFFTNormalizedCorrelationImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMaximumProjectionImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

#include <numeric>

namespace itkRegistrationOp
{

template <class PIX>
class MIPMatchingRegistration;

/**
 * @brief Helper type containing the parameters and return values from MIPMatchingRegistration::registerImage()
 * for use with the Dispatcher.
 */
struct RegistrationDispatch {
    struct Parameters {
        ::fwData::Image::csptr fixed;
        ::fwData::Image::csptr moving;
        ::fwData::TransformationMatrix3D::sptr transform;
    };

    //------------------------------------------------------------------------------

    template<typename PIXELTYPE>
    void operator()(Parameters& params)
    {
        ::itkRegistrationOp::MIPMatchingRegistration<PIXELTYPE>::registerImage(params.moving, params.fixed,
                                                                               params.transform);
    }
};

/**
 * @brief Fast approximate registration, made of pure translation roughly matching two 3-dimensional images
 * of the given pixel type.
 *
 * @tparam PIX Subpixel type of the images.
 */
template <typename PIX>
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
    static void registerImage(const ::fwData::Image::csptr& _moving,
                              const ::fwData::Image::csptr& _fixed,
                              fwData::TransformationMatrix3D::sptr& _transform);

private:
    enum class Direction : unsigned int { X = 0, Y = 1, Z = 2 };

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

template <class PIX>
void MIPMatchingRegistration<PIX>::registerImage(const ::fwData::Image::csptr& _moving,
                                                 const ::fwData::Image::csptr& _fixed,
                                                 ::fwData::TransformationMatrix3D::sptr& _transform)
{
    const double fixedVoxelVolume = std::accumulate(_fixed->getSpacing().begin(), _fixed->getSpacing().end(), 1.,
                                                    std::multiplies<double>());

    const double movingVoxelVolume = std::accumulate(_moving->getSpacing().begin(), _moving->getSpacing().end(), 1.,
                                                     std::multiplies<double>());

    ::fwData::Image::csptr fixed = _fixed,
    moving                       = _moving;

    // Resample the image with the smallest voxels to match the other's voxel size.
    if(fixedVoxelVolume < movingVoxelVolume)
    {
        auto inverseTransform = ::fwData::TransformationMatrix3D::New();
        ::fwDataTools::TransformationMatrix3D::invert(_transform, inverseTransform);

        fixed = ::itkRegistrationOp::Resampler::resample(_fixed, inverseTransform, _moving->getSpacing());
    }
    else
    {
        moving = ::itkRegistrationOp::Resampler::resample(_moving, _transform, _fixed->getSpacing());
    }

    Image3DPtrType itkMoving = castTo<PIX>(moving);
    Image3DPtrType itkFixed  = castTo<PIX>(fixed);

    auto targetSpacing = itkMoving->GetSpacing();
    auto sourceSpacing = itkFixed->GetSpacing();
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[0] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[1] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[2] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[0] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[1] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[2] != 0.);

    auto movingMipX = computeMIP(itkMoving, Direction::X);
    auto movingMipY = computeMIP(itkMoving, Direction::Y);
    auto fixedMipX  = computeMIP(itkFixed, Direction::X);
    auto fixedMipY  = computeMIP(itkFixed, Direction::Y);

    auto transX = matchTemplate(fixedMipX, movingMipX);
    auto transY = matchTemplate(fixedMipY, movingMipY);

    std::array<double, 3> res {{ transY[0], transX[1], transY[1] }};

    ::fwData::TransformationMatrix3D::sptr translation = ::fwData::TransformationMatrix3D::New();
    for(std::uint8_t i = 0; i != 3; ++i)
    {
        translation->setCoefficient(i, 3, res[i]);
    }

    ::fwDataTools::TransformationMatrix3D::multiply(translation, _transform, _transform);
}

//------------------------------------------------------------------------------

template <class PIX>
typename MIPMatchingRegistration<PIX>::Image2DPtrType MIPMatchingRegistration<PIX>::computeMIP(
    Image3DPtrType const& img, Direction d)
{
    auto filter = MIPFilterType::New();
    filter->SetInput(img);
    filter->SetProjectionDimension(static_cast<unsigned int>(d));
    filter->Update();
    return filter->GetOutput();
}

//------------------------------------------------------------------------------

template <class PIX>
typename MIPMatchingRegistration<PIX>::Image2DType::PointType
MIPMatchingRegistration<PIX>::matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img)
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

