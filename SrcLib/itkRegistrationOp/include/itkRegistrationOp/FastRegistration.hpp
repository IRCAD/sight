/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__
#define __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__

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
class FastRegistration;

/**
 * @brief Helper type containing the parameters and return values from FastRegistration::registerImage() for use with
 * the Dispatcher.
 */
struct RegistrationDispatch {
    struct Parameters {
        ::fwData::Image::csptr source;
        ::fwData::Image::csptr target;
        ::fwData::TransformationMatrix3D::sptr transform;
    };

    //------------------------------------------------------------------------------

    template<typename PIXELTYPE>
    void operator()(Parameters& params)
    {
        ::itkRegistrationOp::FastRegistration<PIXELTYPE>::registerImage(params.target, params.source, params.transform);
    }
};

/**
 * @brief Compute a fast registration, composed of pure translation, between two 3-dimensional images of the given
 * pixel type.
 *
 * @tparam PIX Subpixel type of the images.
 */
template <typename PIX>
class FastRegistration
{
public:

    /**
     * @brief Compute a fast registration containing translation only.
     * @param[in] _target target, i.e. the fixed image.
     * @param[in] _source source, i.e. the image that will be transformed to the target.
     * @param[inout] _tgt2SrcTrf initial transform applied to the target, updated after registration.
     */
    static void registerImage(const ::fwData::Image::csptr& _target,
                              const ::fwData::Image::csptr& _source,
                              fwData::TransformationMatrix3D::sptr& _targetTransform);

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
void FastRegistration<PIX>::registerImage(const ::fwData::Image::csptr& _target,
                                          const ::fwData::Image::csptr& _source,
                                          ::fwData::TransformationMatrix3D::sptr& _targetTransform)
{
    const double sourceVoxelVolume = std::accumulate(_source->getSpacing().begin(), _source->getSpacing().end(), 1.,
                                                     std::multiplies<double>());

    const double targetVoxelVolume = std::accumulate(_target->getSpacing().begin(), _target->getSpacing().end(), 1.,
                                                     std::multiplies<double>());

    ::fwData::Image::csptr src = _source,
    tgt                        = _target;

    // Resample the image with the smallest voxels to match the other's voxel size.
    if(sourceVoxelVolume < targetVoxelVolume)
    {
        auto inverseTransform = ::fwData::TransformationMatrix3D::New();
        ::fwDataTools::TransformationMatrix3D::invert(_targetTransform, inverseTransform);

        src = ::itkRegistrationOp::Resampler::resample(_source, inverseTransform, _target->getSpacing());
    }
    else
    {
        tgt = ::itkRegistrationOp::Resampler::resample(_target, _targetTransform, _source->getSpacing());
    }

    Image3DPtrType target = castTo<PIX>(tgt);
    Image3DPtrType source = castTo<PIX>(src);

    auto targetSpacing = target->GetSpacing();
    auto sourceSpacing = source->GetSpacing();
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[0] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[1] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[2] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[0] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[1] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[2] != 0.);

    auto targetMipX = computeMIP(target, Direction::X);
    auto targetMipY = computeMIP(target, Direction::Y);
    auto sourceMipX = computeMIP(source, Direction::X);
    auto sourceMipY = computeMIP(source, Direction::Y);

    auto transX = matchTemplate(sourceMipX, targetMipX);
    auto transY = matchTemplate(sourceMipY, targetMipY);

    std::array<double, 3> res {{ transY[0], transX[1], transY[1] }};

    ::fwData::TransformationMatrix3D::sptr translation = ::fwData::TransformationMatrix3D::New();
    ::fwDataTools::TransformationMatrix3D::identity(translation);
    for(std::uint8_t i = 0; i != 3; ++i)
    {
        translation->setCoefficient(i, 3, res[i]);
    }

    ::fwDataTools::TransformationMatrix3D::multiply(translation, _targetTransform, _targetTransform);
}

//------------------------------------------------------------------------------

template <class PIX>
typename FastRegistration<PIX>::Image2DPtrType FastRegistration<PIX>::computeMIP(Image3DPtrType const& img, Direction d)
{
    auto filter = MIPFilterType::New();
    filter->SetInput(img);
    filter->SetProjectionDimension(static_cast<unsigned int>(d));
    filter->Update();
    return filter->GetOutput();
}

//------------------------------------------------------------------------------

template <class PIX>
typename FastRegistration<PIX>::Image2DType::PointType
FastRegistration<PIX>::matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img)
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
    p[0] = maxIdx[0] * spacing[0];
    p[1] = maxIdx[1] * spacing[1];
    p   += tOrigin;

    return p;
}

} // itkRegistrationOp

#endif // __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__
