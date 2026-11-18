/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <filter/image/spheroid_extraction.hpp>

#include <io/itk/itk.hpp> // NOLINT(misc-include-cleaner): move_from_itk is defined in itk.hxx

#include <doctest/doctest.h>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkOrImageFilter.h>
#include <itkSpatialObjectToImageFilter.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <vector>

TEST_SUITE("sight::filter::image::spheroid_extraction")
{
    using image_t = itk::Image<std::int16_t, 3>;

//------------------------------------------------------------------------------

    static void draw_ellipsoid(
        image_t::Pointer& _image,
        const std::array<double, 3>& _radius,
        const std::array<double, 3>& _center
)
    {
        using ellipse_t = itk::EllipseSpatialObject<3>;

        auto ellipse = ellipse_t::New();
        ellipse_t::ArrayType radius;
        ellipse_t::PointType center;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            radius[i] = _radius[i];
            center[i] = _center[i];
        }

        ellipse->SetRadiusInObjectSpace(radius);
        ellipse->SetCenterInObjectSpace(center);
        ellipse->SetDefaultInsideValue(255);
        ellipse->SetDefaultOutsideValue(0);
        ellipse->Update();

        auto image_filter = itk::SpatialObjectToImageFilter<ellipse_t, image_t>::New();
        image_filter->SetInput(ellipse);
        image_filter->SetUseObjectValue(true);
        image_filter->SetOutsideValue(0);
        image_filter->SetSize(_image->GetLargestPossibleRegion().GetSize());
        image_filter->SetSpacing(_image->GetSpacing());
        image_filter->Update();

        auto or_filter = itk::OrImageFilter<image_t, image_t>::New();
        or_filter->SetInput1(_image);
        or_filter->SetInput2(image_filter->GetOutput());
        or_filter->Update();
        _image->Graft(or_filter->GetOutput());
    }

//------------------------------------------------------------------------------

    TEST_CASE("extraction")
    {
        auto itk_image = image_t::New();
        image_t::SizeType size;
        size.Fill(128);
        itk_image->SetRegions(size);
        itk_image->Allocate();

        // Noise stays below the extraction threshold
        std::default_random_engine generator {std::random_device {}()};
        std::uniform_int_distribution<std::int16_t> distribution(0, 128);
        for(itk::ImageRegionIterator<image_t> it(itk_image, itk_image->GetLargestPossibleRegion()) ; !it.IsAtEnd() ;
            ++it)
        {
            it.Set(distribution(generator));
        }

        const std::vector<std::array<double, 3> > spheroid_centers = {
            {{16., 16., 16.}},
            {{54., 67., 12.}},
            {{113., 87., 98.}}
        };

        const std::vector<std::array<double, 3> > ellipsoid_centers = {
            {{16., 100., 64.}},
            {{64., 64., 64.}},
            {{10., 17., 108.}}
        };

        for(const auto& center : spheroid_centers)
        {
            draw_ellipsoid(itk_image, {5., 5., 5.}, center);
        }

        for(const auto& center : ellipsoid_centers)
        {
            draw_ellipsoid(itk_image, {8., 3., 5.}, center);
        }

        const auto image     = sight::io::itk::move_from_itk<image_t>(itk_image);
        const auto spheroids = sight::filter::image::spheroid_extraction::extract(image, 200., 4., 9., 0.8, 1.2);

        CHECK_EQ(spheroid_centers.size(), spheroids->size());

        for(const auto& center : spheroid_centers)
        {
            const bool found = std::ranges::any_of(
                *spheroids,
                [&center](const auto& _point)
            {
                return std::fabs((*_point)[0] - center[0]) < 1e-1
                       && std::fabs((*_point)[1] - center[1]) < 1e-1
                       && std::fabs((*_point)[2] - center[2]) < 1e-1;
            });

            CHECK(found);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("measures_in_image_units")
    {
        // A sphere in image units is an ellipsoid in voxels, the other way around for the ellipsoid.
        auto itk_image = image_t::New();
        image_t::SizeType size;
        size[0] = 160;
        size[1] = 80;
        size[2] = 40;
        itk_image->SetRegions(size);
        image_t::SpacingType spacing;
        spacing[0] = 0.5;
        spacing[1] = 1.;
        spacing[2] = 2.;
        itk_image->SetSpacing(spacing);
        itk_image->Allocate();
        itk_image->FillBuffer(0);

        const std::array<double, 3> sphere_center {20., 30., 40.};
        draw_ellipsoid(itk_image, {5., 5., 5.}, sphere_center);
        draw_ellipsoid(itk_image, {8., 3., 5.}, {60., 60., 60.});

        constexpr std::array<double, 3> origin {10., -20., 30.};
        image_t::PointType itk_origin;
        std::ranges::copy(origin, itk_origin.Begin());
        itk_image->SetOrigin(itk_origin);

        std::array<double, 3> expected {};
        SUBCASE("without orientation")
        {
            expected = {origin[0] + sphere_center[0], origin[1] + sphere_center[1], origin[2] + sphere_center[2]};
        }

        SUBCASE("with an orientation")
        {
            // A quarter turn around z.
            image_t::DirectionType direction;
            direction(0, 0) = 0.;
            direction(0, 1) = -1.;
            direction(0, 2) = 0.;
            direction(1, 0) = 1.;
            direction(1, 1) = 0.;
            direction(1, 2) = 0.;
            direction(2, 0) = 0.;
            direction(2, 1) = 0.;
            direction(2, 2) = 1.;
            itk_image->SetDirection(direction);
            expected = {origin[0] - sphere_center[1], origin[1] + sphere_center[0], origin[2] + sphere_center[2]};
        }

        const auto image     = sight::io::itk::move_from_itk<image_t>(itk_image);
        const auto spheroids = sight::filter::image::spheroid_extraction::extract(image, 200., 4., 9., 0.8, 1.2);

        REQUIRE_EQ(spheroids->size(), 1);
        const auto& center = *spheroids->front();
        CHECK(std::fabs(center[0] - expected[0]) < 1.);
        CHECK(std::fabs(center[1] - expected[1]) < 1.);
        CHECK(std::fabs(center[2] - expected[2]) < 1.);
    }
} // TEST_SUITE("sight::filter::image::spheroid_extraction")
