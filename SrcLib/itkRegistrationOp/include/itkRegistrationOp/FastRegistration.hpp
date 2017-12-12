/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__
#define __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__

#include "itkRegistrationOp/config.hpp"
#include "itkRegistrationOp/ItkImageCaster.hpp"
#include "itkRegistrationOp/Metric.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwItkIO/itk.hpp>

#include <itkFFTNormalizedCorrelationImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMaximumProjectionImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

namespace itkRegistrationOp
{

/**
 * @brief Compute a fast registration, composed of pure translation, between two 3-dimensional images of the given
 * pixel type.
 *
 * @tparam PIX Subpixel type of the images.
 */
template <class PIX>
class ITKREGISTRATIONOP_CLASS_API FastRegistration
{
public:

    /**
     * @brief Compute a fast registration containing translation only.
     * @param[in] _target target, i.e. the fixed image.
     * @param[in] _source source, i.e. the image that will be transformed to the target.
     *
     * @return The real world coordinates translation which transforms the source to the target image.
     */
    static ITKREGISTRATIONOP_CLASS_API std::array<double, 3> registerImage(const ::fwData::Image::csptr& _target,
                                                                           const ::fwData::Image::csptr& _source);

private:
    enum class Direction : unsigned int { X = 0, Y = 1, Z = 2 };

    using Image3DType    = ::itk::Image<typename PIX, 3>;
    using Image2DType    = ::itk::Image<typename PIX, 2>;
    using Image3DPtrType = typename ::itk::Image<typename PIX, 3>::Pointer;
    using Image2DPtrType = typename ::itk::Image<typename PIX, 2>::Pointer;

    using MIPFilterType           = ::itk::MaximumProjectionImageFilter<Image3DType, Image2DType>;
    using CorrelationFilterType   = ::itk::FFTNormalizedCorrelationImageFilter<Image2DType, Image2DType>;
    using ExtractFilter3Dto2DType = ::itk::RegionOfInterestImageFilter<Image3DType, Image2DType>;
    using ExtractFilter2Dto2DType = ::itk::RegionOfInterestImageFilter<Image2DType, Image2DType>;
    using MinMaxCalculatorType    = ::itk::MinimumMaximumImageCalculator<Image2DType>;

    static Image2DPtrType computeMIP(Image3DPtrType const& img, Direction d);
    static Image2DPtrType resampleSourceToTarget(Image2DPtrType const& source, Image2DPtrType const& target);
    static typename Image2DType::PointType matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img);
};

//------------------------------------------------------------------------------

template <class PIX>
std::array<double, 3> FastRegistration<PIX>::registerImage(const ::fwData::Image::csptr& _target,
                                                           const ::fwData::Image::csptr& _source)
{
    Image3DPtrType target = castTo<PIX>(_target);
    Image3DPtrType source = castTo<PIX>(_source);

    auto targetSpacing = target->GetSpacing();
    auto sourceSpacing = source->GetSpacing();
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[0] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[1] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", targetSpacing[2] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[0] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[1] != 0.);
    SLM_ASSERT("Spacing must be nonzero for all axes of both images", sourceSpacing[2] != 0.);

    auto targetMipX       = computeMIP(target, Direction::X);
    auto targetMipY       = computeMIP(target, Direction::Y);
    auto targetMipZ       = computeMIP(target, Direction::Z);
    auto sourceMipX       = computeMIP(source, Direction::X);
    auto sourceMipY       = computeMIP(source, Direction::Y);
    auto sourceMipZ       = computeMIP(source, Direction::Z);
    auto resampledSourceX = resampleSourceToTarget(sourceMipX, targetMipX);
    auto resampledSourceY = resampleSourceToTarget(sourceMipY, targetMipY);
    auto resampledSourceZ = resampleSourceToTarget(sourceMipZ, targetMipZ);

    auto transX = matchTemplate(resampledSourceX, targetMipX);
    auto transY = matchTemplate(resampledSourceY, targetMipY);
    auto transZ = matchTemplate(resampledSourceZ, targetMipZ);
    std::array<double, 3> res {{ -transY[0], -transX[1], -transX[0] }};
    return res;
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
typename FastRegistration<PIX>::Image2DPtrType
FastRegistration<PIX>::resampleSourceToTarget(Image2DPtrType const& source, Image2DPtrType const& target)
{
    auto targetSpacing = target->GetSpacing();
    auto sourceSpacing = source->GetSpacing();
    auto sourceSize    = source->GetBufferedRegion().GetSize();
    auto newSize       = Image2DType::SizeType {
        static_cast<Image2DType::SizeValueType>(static_cast<double>(sourceSize[0]) / targetSpacing[0] *
                                                sourceSpacing[0]),
        static_cast<Image2DType::SizeValueType>(static_cast<double>(sourceSize[1]) / targetSpacing[1] *
                                                sourceSpacing[1])
    };

    auto transform    = ::itk::IdentityTransform<double, 2>::New();
    auto interpolator = ::itk::LinearInterpolateImageFunction<Image2DType, double>::New();
    auto resampler    = ::itk::ResampleImageFilter<Image2DType, Image2DType>::New();
    resampler->SetInput(source);
    resampler->SetTransform(transform);
    resampler->SetInterpolator(interpolator);
    resampler->SetOutputSpacing(targetSpacing);
    resampler->SetOutputOrigin(source->GetOrigin());
    resampler->SetSize(newSize);
    resampler->Update();
    return resampler->GetOutput();
}

//------------------------------------------------------------------------------

template <class PIX>
typename FastRegistration<PIX>::Image2DType::PointType
FastRegistration<PIX>::matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img)
{
    Image2DType::PointType origin(0.);
    _template->SetOrigin(origin);
    img->SetOrigin(origin);
    auto templateSize = _template->GetLargestPossibleRegion().GetSize();

    auto correlation = CorrelationFilterType::New();
    correlation->SetFixedImage(img);
    correlation->SetMovingImage(_template);
    correlation->Update();
    auto correlationMap       = correlation->GetOutput();
    auto maxCorrelationFinder = MinMaxCalculatorType::New();
    maxCorrelationFinder->SetImage(correlation->GetOutput());
    maxCorrelationFinder->Compute();
    auto maxIdx = maxCorrelationFinder->GetIndexOfMaximum();

    auto spacing = img->GetSpacing();
    maxIdx[0] -= templateSize[0] - 1;
    maxIdx[1] -= templateSize[1] - 1;
    Image2DType::PointType p;
    p[0] = maxIdx[0] * spacing[0];
    p[1] = maxIdx[1] * spacing[1];

    return p;
}

} // itkRegistrationOp

#endif // __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__
