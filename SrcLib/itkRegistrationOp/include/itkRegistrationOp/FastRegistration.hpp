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

#include <cvIO/Image.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwItkIO/itk.hpp>

#include <itkFFTNormalizedCorrelationImageFilter.h>
#include <itkFlipImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkImageKernelOperator.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMaximumProjectionImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkNeighborhoodIterator.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>

#include <opencv2/highgui.hpp>

namespace itkRegistrationOp
{

/**
 * @brief
 *
 * @tparam PIX
 */
template <class PIX>
class ITKREGISTRATIONOP_CLASS_API FastRegistration
{
public:

    /**
     * @brief find a rigid transform matching the reference image with the target image.
     * @param[in] _target target, i.e. the static image.
     * @param[in] _source source, i.e. the image that will be transformed into the target.
     * @param[out] _trf the resulting rigid transform.
     * @param[in] _metric the metric to be used for registration.
     * @param[in] _minStep minimum step for used by optimizer for each iteration.
     * @param[in] _maxIterations the maximum number of iterations
     */
    static ITKREGISTRATIONOP_CLASS_API void registerImage(const ::fwData::Image::csptr& _target,
                                                          const ::fwData::Image::csptr& _source,
                                                          const ::fwData::TransformationMatrix3D::sptr& _trf);

private:
    enum class Direction : unsigned int { X = 0, Y = 1, Z = 2 };
    using Image3DType = ::itk::Image<typename PIX, 3>;
    using Image2DType = ::itk::Image<typename PIX, 2>;

    using Image3DPtrType = typename ::itk::Image<typename PIX, 3>::Pointer;
    using Image2DPtrType = typename ::itk::Image<typename PIX, 2>::Pointer;

    using KernelOperatorType      = ::itk::ImageKernelOperator<typename PIX, 2>;
    using MIPFilterType           = ::itk::MaximumProjectionImageFilter<Image3DType, Image2DType>;
    using CorrelationFilterType   = ::itk::FFTNormalizedCorrelationImageFilter<Image2DType, Image2DType>;
    using ExtractFilter3Dto2DType = ::itk::RegionOfInterestImageFilter<Image3DType, Image2DType>;
    using ExtractFilter2Dto2DType = ::itk::RegionOfInterestImageFilter<Image2DType, Image2DType>;
    using MinMaxCalculatorType    = ::itk::MinimumMaximumImageCalculator<Image2DType>;

    static Image2DPtrType computeMIP(Image3DPtrType const& img, Direction d);

    static Image2DPtrType
    resampleSourceToTarget(Image2DPtrType const& source, typename Image2DType::SpacingType const& spacing, Direction d);

    static ::cv::Mat itkToCV(Image2DPtrType const& img);

    static typename Image2DType::IndexType
    matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img, Direction d);
};

//------------------------------------------------------------------------------

template <class PIX>
void FastRegistration<PIX>::registerImage(const ::fwData::Image::csptr& _target,
                                          const ::fwData::Image::csptr& _source,
                                          const ::fwData::TransformationMatrix3D::sptr& _trf)
{
    Image3DPtrType target = castTo<PIX>(_target);
    Image3DPtrType source = castTo<PIX>(_source);

    //using FlipFilter = itk::FlipImageFilter<Image3DType>;
    //auto flip = FlipFilter::New();
    //FlipFilter::FlipAxesArrayType flipAxes;
    //flipAxes[0] = true;
    //flipAxes[1] = false;
    //flipAxes[2] = false;
    //flip->SetFlipAxes(flipAxes);
    //flip->SetInput(target);
    //auto flippedTarget = flip->GetOutput();
    auto flippedTarget = target;

    auto targetMipX       = computeMIP(flippedTarget, Direction::X);
    auto targetMipY       = computeMIP(flippedTarget, Direction::Y);
    auto targetMipZ       = computeMIP(flippedTarget, Direction::Z);
    auto sourceMipX       = computeMIP(source, Direction::X);
    auto sourceMipY       = computeMIP(source, Direction::Y);
    auto sourceMipZ       = computeMIP(source, Direction::Z);
    auto resampledSourceX = resampleSourceToTarget(sourceMipX, targetMipX->GetSpacing(), Direction::X);
    auto resampledSourceY = resampleSourceToTarget(sourceMipY, targetMipY->GetSpacing(), Direction::Y);
    auto resampledSourceZ = resampleSourceToTarget(sourceMipZ, targetMipZ->GetSpacing(), Direction::Z);

    auto idxMaxX = matchTemplate(resampledSourceX, targetMipX, Direction::X);
    auto idxMaxY = matchTemplate(resampledSourceY, targetMipY, Direction::Y);
    auto idxMaxZ = matchTemplate(resampledSourceZ, targetMipZ, Direction::Z);

    for(auto i = 0; i != 4; ++i)
    {
        for(auto j = 0; j != 3; ++j)
        {
            _trf->setCoefficient(i, j, (i == j) ? 1. : 0.);
        }
    }
    _trf->setCoefficient(0, 3, -idxMaxY[0]);
    _trf->setCoefficient(1, 3, -idxMaxX[1]);
    _trf->setCoefficient(2, 3, -idxMaxX[0]);
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
FastRegistration<PIX>::resampleSourceToTarget(Image2DPtrType const& source,
                                              typename Image2DType::SpacingType const& spacing, Direction d)
{
    auto transform    = ::itk::IdentityTransform<double, 2>::New();
    auto interpolator = ::itk::LinearInterpolateImageFunction<Image2DType, double>::New();
    auto resampler    = ::itk::ResampleImageFilter<Image2DType, Image2DType>::New();

    resampler->SetTransform(transform);
    resampler->SetInterpolator(interpolator);
    resampler->SetOutputSpacing(spacing);
    resampler->SetOutputOrigin(source->GetOrigin());

    auto cSpacing = source->GetSpacing();
    auto cSize    = source->GetBufferedRegion().GetSize();
    auto newSize  =
        Image2DType::SizeType { static_cast<Image2DType::SizeValueType>(static_cast<double>(cSize[0]) / spacing[0] *
                                                                        cSpacing[0]),
                                static_cast<Image2DType::SizeValueType>(static_cast<double>(cSize[1]) / spacing[1] *
                                                                        cSpacing[1]) };
    resampler->SetSize(newSize);
    resampler->SetInput(source);
    resampler->Update();
    return resampler->GetOutput();
}

//------------------------------------------------------------------------------

template <class PIX>
typename FastRegistration<PIX>::Image2DType::IndexType
FastRegistration<PIX>::matchTemplate(Image2DPtrType const& _template, Image2DPtrType const& img, Direction d)
{
    auto cvImg      = itkToCV(img);
    auto cvTemplate = itkToCV(_template);
    ::cv::normalize(cvImg, cvImg, 1., 0., cv::NORM_MINMAX);
    ::cv::normalize(cvTemplate, cvTemplate, 1., 0., cv::NORM_MINMAX);
    ::cv::imshow("Template", cvTemplate);
    ::cv::imshow("Target", cvImg);

    auto templateSize = _template->GetLargestPossibleRegion().GetSize();

    double origin[2] = {0.f, 0.f};
    img->SetOrigin(origin);
    _template->SetOrigin(origin);

    auto correlation = CorrelationFilterType::New();
    correlation->SetFixedImage(img);
    correlation->SetMovingImage(_template);
    //correlation->SetRequiredNumberOfOverlappingPixels(templateSize[0] * templateSize[1]);
    correlation->Update();
    auto correlationMap       = correlation->GetOutput();
    auto maxCorrelationFinder = MinMaxCalculatorType::New();
    maxCorrelationFinder->SetImage(correlation->GetOutput());
    maxCorrelationFinder->Compute();
    auto max = maxCorrelationFinder->GetIndexOfMaximum();

    auto cvCorrel = itkToCV(correlationMap);
    ::cv::normalize(cvCorrel, cvCorrel, 1., 0., cv::NORM_MINMAX);
    ::cv::imshow("Correlation map", cvCorrel);
    //cv::waitKey();

    max[0] -= templateSize[0] - 1;
    max[1] -= templateSize[1] - 1;

    return max;
}

//------------------------------------------------------------------------------

template <class PIX>
::cv::Mat FastRegistration<PIX>::itkToCV(Image2DPtrType const& img)
{
    auto f4sImg = ::fwItkIO::dataImageFactory<Image2DType>(img, false);
    return ::cvIO::Image::copyToCv(f4sImg);
}

} // itkRegistrationOp

#endif // __ITKREGISTRATIONOP_FASTREGISTRATION_HPP__
