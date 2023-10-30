/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#if ITK_VERSION_MAJOR == 4 || (ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3)

#include "spheroid_extraction_test.hpp"

#include <filter/image/spheroid_extraction.hpp>

#include <io/itk/itk.hpp>

#include <utest_data/generator/image.hpp>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkOrImageFilter.h>
#include <itkSpatialObjectToImageFilter.h>

#include <random>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::spheroid_extraction_test);

namespace sight::filter::image::ut
{

using image_t = itk::Image<std::int16_t, 3>;

//------------------------------------------------------------------------------

template<typename PIXELTYPE>
static void makeNoise(
    const data::image::sptr& _image,
    PIXELTYPE rangeMin = std::numeric_limits<PIXELTYPE>::min(),
    PIXELTYPE rangeMax = std::numeric_limits<PIXELTYPE>::max()
)
{
    const auto dumpLock = _image->dump_lock();
    auto iter           = _image->begin<PIXELTYPE>();
    const auto end      = _image->end<PIXELTYPE>();

    std::default_random_engine randGenerator {std::random_device {}()};
    std::uniform_int_distribution<PIXELTYPE> distribution(rangeMin, rangeMax);

    for( ; iter != end ; ++iter)
    {
        *iter = distribution(randGenerator);
    }
}

//------------------------------------------------------------------------------

static void plantSphere(
    image_t::Pointer _image,
    const std::array<double, 3> radius,
    const std::array<double, 3> center
)
{
    using ellipse_t = itk::EllipseSpatialObject<3>;

    using SpatialObjectToImageFilterType = itk::SpatialObjectToImageFilter<ellipse_t, image_t>;

    ellipse_t::Pointer ellipse = ellipse_t::New();
    ellipse->SetRadius(radius.data());

    using transform_t = ellipse_t::transform_t;
    transform_t::Pointer transform = transform_t::New();
    transform->SetIdentity();
    transform->Translate(center.data(), false);

    ellipse->SetObjectToParentTransform(transform);

    using SpatialObjectToImageFilterType = itk::SpatialObjectToImageFilter<ellipse_t, image_t>;

    SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

    imageFilter->SetInput(ellipse);

    ellipse->SetDefaultInsideValue(255);
    ellipse->SetDefaultOutsideValue(0);
    imageFilter->SetUseObjectValue(true);
    imageFilter->SetOutsideValue(0);
    imageFilter->SetSize(_image->GetLargestPossibleRegion().GetSize());
    imageFilter->SetSpacing(_image->GetSpacing());

    imageFilter->Update();

    using or_filter_t = typename itk::OrImageFilter<image_t, image_t>;

    typename or_filter_t::Pointer orFilter = or_filter_t::New();
    orFilter->SetInput1(_image);
    orFilter->SetInput2(imageFilter->GetOutput());
    orFilter->Update();
    _image->Graft(orFilter->GetOutput());
}

//------------------------------------------------------------------------------

void spheroid_extraction_test::setUp()
{
}

//------------------------------------------------------------------------------

void spheroid_extraction_test::tearDown()
{
}

//------------------------------------------------------------------------------

void spheroid_extraction_test::extractionTest()
{
    data::image::sptr image = std::make_shared<data::image>();

    const data::image::size_t SIZE = {{128, 128, 128}};

    const data::image::spacing_t SPACING = {{1., 1., 1.}};
    const data::image::origin_t ORIGIN   = {{0., 0., 0.}};
    const core::type TYPE                = core::type::INT16;

    utest_data::generator::image::generateImage(
        image,
        SIZE,
        SPACING,
        ORIGIN,
        TYPE,
        data::image::pixel_format::gray_scale
    );

    makeNoise<std::int16_t>(image, 0, 128);

    image_t::Pointer itkImage = io::itk::move_to_itk<image_t>(image);

    const std::vector<std::array<double, 3> > spheroidCenters = {{{16., 16., 16.}},
        {{54., 67., 12.}},
        {{113., 87., 98.
        }
        }
    };
    const std::array radius = {5., 5., 5.};

    const std::vector<std::array<double, 3> > ellipsoidCenters = {{{16., 100., 64.}},
        {{64., 64., 64.}},
        {{10., 17., 108.
        }
        }
    };

    const std::array ellipseRadius = {8., 3., 5.};

    for(const auto& spheroid : spheroidCenters)
    {
        plantSphere(itkImage, radius, spheroid);
    }

    for(const auto& ellipsoid : ellipsoidCenters)
    {
        plantSphere(itkImage, ellipseRadius, ellipsoid);
    }

    // Rewrite the itk image to the Sight image.
    io::itk::move_from_itk(itkImage, image);

    data::point_list::sptr extractedSpheroids =
        filter::image::spheroid_extraction::extract(image, 200., 4., 9., 0.8, 1.2);

    CPPUNIT_ASSERT_EQUAL(spheroidCenters.size(), extractedSpheroids->getPoints().size());

    for(const auto& spheroid : spheroidCenters)
    {
        bool found = false;
        for(const auto& point : extractedSpheroids->getPoints())
        {
            const double compEps = 1e-1;
            const auto coords    = point->getCoord();
            found  = std::fabs(coords[0] - spheroid[0]) < compEps;
            found &= std::fabs(coords[1] - spheroid[1]) < compEps;
            found &= std::fabs(coords[2] - spheroid[2]) < compEps;

            if(found)
            {
                break;
            }
        }

        CPPUNIT_ASSERT(found);
    }
}

} // namespace sight::filter::image::ut

#endif // ITK_VERSION_MAJOR == 4 || (ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3)
