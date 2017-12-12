/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_UT_HELPER_HPP__
#define __ITKREGISTRATIONOP_UT_HELPER_HPP__

#include <fwData/Image.hpp>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkSpatialObjectToImageFilter.h>

template <class P, size_t N>
/**
 * @brief Create a binary test image of 100x100x100 size containing an ellipsoid in the center.
 *
 * @tparam P Pixel type
 * @tparam N Number of channels
 */
::fwData::Image::sptr createSphereImage()
{
    using ImageType                      = ::itk::Image<P, N>;
    using EllipseType                    = ::itk::EllipseSpatialObject< N >;
    using SpatialObjectToImageFilterType = ::itk::SpatialObjectToImageFilter< EllipseType, ImageType >;
    using TransformType                  = EllipseType::TransformType;
    static_assert(std::is_arithmetic<P>::value, "P type must be numeric");

    ImageType::Pointer image                            = ImageType::New();
    SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

    ImageType::SizeType size = { 100, 100, 100 };
    imageFilter->SetSize( size );

    ImageType::SpacingType spacing;
    spacing.Fill(1);
    imageFilter->SetSpacing(spacing);

    EllipseType::Pointer ellipse = EllipseType::New();
    EllipseType::ArrayType radiusArray;
    radiusArray[0] = 10;
    radiusArray[1] = 15;
    radiusArray[2] = 20;
    ellipse->SetRadius(radiusArray);

    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    TransformType::OutputVectorType translation;
    TransformType::CenterType center;

    translation[ 0 ] = 50;
    translation[ 1 ] = 50;
    translation[ 2 ] = 50;
    transform->Translate( translation, false );

    ellipse->SetObjectToParentTransform( transform );

    imageFilter->SetInput(ellipse);

    ellipse->SetDefaultInsideValue(std::numeric_limits<P>::max());
    ellipse->SetDefaultOutsideValue(0);
    imageFilter->SetUseObjectValue( true );
    imageFilter->SetOutsideValue( 0 );

    imageFilter->Update();

    image->Graft(imageFilter->GetOutput());

    ::fwData::Image::sptr outputImage = ::fwData::Image::New();
    ::fwItkIO::itkImageToFwDataImage(image, outputImage);

    return outputImage;
}

#endif // __ITKREGISTRATIONOP_UT_HELPER_HPP__
