/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include <data/image.hpp>

#include <io/itk/itk.hpp>
#include <io/itk/itk.hxx>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkSpatialObjectToImageFilter.h>

template<class P, std::size_t N>
/**
 * @brief Create a binary test image of 100x100x100 size containing an ellipsoid in the center.
 *
 * @tparam P Pixel type
 * @tparam N Number of channels
 */
sight::data::image::sptr create_sphere_image(itk::Vector<double, N> _spacing = itk::Vector<double, N>(1.))
{
    using image_t                          = itk::Image<P, N>;
    using ellipse_t                        = itk::EllipseSpatialObject<N>;
    using spatial_object_to_image_filter_t = itk::SpatialObjectToImageFilter<ellipse_t, image_t>;
    using transform_t                      = typename ellipse_t::TransformType;
    static_assert(std::is_arithmetic<P>::value, "P type must be numeric");

    auto image        = image_t::New();
    auto image_filter = spatial_object_to_image_filter_t::New();

    typename image_t::SizeType size = {100, 100, 100};
    image_filter->SetSize(size);

    image_filter->SetSpacing(_spacing);

    auto ellipse = ellipse_t::New();
    typename ellipse_t::ArrayType radius_array;
    radius_array[0] = 10;
    radius_array[1] = 15;
    radius_array[2] = 20;
    ellipse->SetRadiusInObjectSpace(radius_array);

    auto transform = transform_t::New();
    transform->SetIdentity();

    typename transform_t::OutputVectorType translation;

    translation[0] = 50;
    translation[1] = 50;
    translation[2] = 50;
    transform->Translate(translation, false);

    ellipse->SetObjectToParentTransform(transform);

    image_filter->SetInput(ellipse);

    ellipse->SetDefaultInsideValue(std::numeric_limits<P>::max());
    ellipse->SetDefaultOutsideValue(0);
    image_filter->SetUseObjectValue(true);
    image_filter->SetOutsideValue(0);

    image_filter->Update();

    image->Graft(image_filter->GetOutput());

    auto output_image = std::make_shared<sight::data::image>();
    sight::io::itk::move_from_itk(image, output_image);

    return output_image;
}
