/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <data/Image.hpp>

#include <io/itk/itk.hpp>
#include <io/itk/itk.hxx>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkSpatialObjectToImageFilter.h>

template<class P, size_t N>
/**
 * @brief Create a binary test image of 100x100x100 size containing an ellipsoid in the center.
 *
 * @tparam P Pixel type
 * @tparam N Number of channels
 */
sight::data::Image::sptr createSphereImage(::itk::Vector<double, N> spacing = ::itk::Vector<double, N>(1.))
{
    using ImageType                      = ::itk::Image<P, N>;
    using EllipseType                    = ::itk::EllipseSpatialObject<N>;
    using SpatialObjectToImageFilterType = ::itk::SpatialObjectToImageFilter<EllipseType, ImageType>;
    using TransformType                  = typename EllipseType::TransformType;
    static_assert(std::is_arithmetic<P>::value, "P type must be numeric");

    typename ImageType::Pointer image                            = ImageType::New();
    typename SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

    typename ImageType::SizeType size = {100, 100, 100};
    imageFilter->SetSize(size);

    imageFilter->SetSpacing(spacing);

    typename EllipseType::Pointer ellipse = EllipseType::New();
    typename EllipseType::ArrayType radiusArray;
    radiusArray[0] = 10;
    radiusArray[1] = 15;
    radiusArray[2] = 20;
    ellipse->SetRadius(radiusArray);

    typename TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    typename TransformType::OutputVectorType translation;
    typename TransformType::CenterType center;

    translation[0] = 50;
    translation[1] = 50;
    translation[2] = 50;
    transform->Translate(translation, false);

    ellipse->SetObjectToParentTransform(transform);

    imageFilter->SetInput(ellipse);

    ellipse->SetDefaultInsideValue(std::numeric_limits<P>::max());
    ellipse->SetDefaultOutsideValue(0);
    imageFilter->SetUseObjectValue(true);
    imageFilter->SetOutsideValue(0);

    imageFilter->Update();

    image->Graft(imageFilter->GetOutput());

    auto outputImage = sight::data::Image::New();
    sight::io::itk::itkImageToFwDataImage(image, outputImage);

    return outputImage;
}
