/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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
#ifndef WIN32

#include "SpheroidExtractionTest.hpp"

#include <filter/image/SpheroidExtraction.hpp>

#include <io/itk/itk.hpp>

#include <utestData/generator/Image.hpp>

#include <itkEllipseSpatialObject.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkOrImageFilter.h>
#include <itkSpatialObjectToImageFilter.h>

#include <random>

CPPUNIT_TEST_SUITE_REGISTRATION(::sight::filter::image::ut::SpheroidExtractionTest);

namespace sight::filter::image
{

namespace ut
{

typedef ::itk::Image<std::int16_t, 3> ImageType;

//------------------------------------------------------------------------------

template<typename PIXELTYPE>
static void makeNoise(
    const data::Image::sptr& _image,
    PIXELTYPE rangeMin = std::numeric_limits<PIXELTYPE>::min(),
    PIXELTYPE rangeMax = std::numeric_limits<PIXELTYPE>::max()
)
{
    const auto dumpLock = _image->lock();
    auto iter           = _image->begin<PIXELTYPE>();
    const auto end      = _image->end<PIXELTYPE>();

    std::default_random_engine randGenerator;
    std::uniform_int_distribution<PIXELTYPE> distribution(rangeMin, rangeMax);

    for( ; iter != end ; ++iter)
    {
        *iter = distribution(randGenerator);
    }
}

//------------------------------------------------------------------------------

static void plantSphere(ImageType::Pointer _image, const double radius[3], const double center[3])
{
    typedef itk::EllipseSpatialObject<3> EllipseType;

    typedef itk::SpatialObjectToImageFilter<EllipseType, ImageType> SpatialObjectToImageFilterType;

    EllipseType::Pointer ellipse = EllipseType::New();
    ellipse->SetRadius(radius);

    typedef EllipseType::TransformType TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    transform->Translate(center, false);

    ellipse->SetObjectToParentTransform(transform);

    typedef itk::SpatialObjectToImageFilter<
            EllipseType, ImageType> SpatialObjectToImageFilterType;

    SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

    imageFilter->SetInput(ellipse);

    ellipse->SetDefaultInsideValue(255);
    ellipse->SetDefaultOutsideValue(0);
    imageFilter->SetUseObjectValue(true);
    imageFilter->SetOutsideValue(0);
    imageFilter->SetSize(_image->GetLargestPossibleRegion().GetSize());
    imageFilter->SetSpacing(_image->GetSpacing());

    imageFilter->Update();

    typedef typename ::itk::OrImageFilter<ImageType, ImageType> OrFilterType;

    typename OrFilterType::Pointer orFilter = OrFilterType::New();
    orFilter->SetInput1(_image);
    orFilter->SetInput2(imageFilter->GetOutput());
    orFilter->Update();
    _image->Graft(orFilter->GetOutput());
}

//------------------------------------------------------------------------------

void SpheroidExtractionTest::setUp()
{
}

//------------------------------------------------------------------------------

void SpheroidExtractionTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SpheroidExtractionTest::extractionTest()
{
    data::Image::sptr image = data::Image::New();

    const data::Image::Size SIZE = {{128, 128, 128}};

    const data::Image::Spacing SPACING = {{1., 1., 1.}};
    const data::Image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::tools::Type TYPE       = core::tools::Type::s_INT16;

    utestData::generator::Image::generateImage(
        image,
        SIZE,
        SPACING,
        ORIGIN,
        TYPE,
        data::Image::PixelFormat::GRAY_SCALE
    );

    makeNoise<std::int16_t>(image, 0, 128);

    ImageType::Pointer itkImage = io::itk::itkImageFactory<ImageType>(image);

    const std::vector<std::array<double, 3> > spheroidCenters = {{{16., 16., 16.}},
        {{54., 67., 12.}},
        {{113., 87., 98.
        }
        }
    };
    const double radius[3] = {5, 5, 5};

    const std::vector<std::array<double, 3> > ellipsoidCenters = {{{16., 100., 64.}},
        {{64., 64., 64.}},
        {{10., 17., 108.
        }
        }
    };

    const double ellipseRadius[3] = {8, 3, 5};

    for(const auto& spheroid : spheroidCenters)
    {
        plantSphere(itkImage, radius, spheroid.data());
    }

    for(const auto& ellipsoid : ellipsoidCenters)
    {
        plantSphere(itkImage, ellipseRadius, ellipsoid.data());
    }

    // Rewrite the itk image to the Sight image.
    io::itk::itkImageToFwDataImage(itkImage, image);

    data::PointList::sptr extractedSpheroids =
        filter::image::SpheroidExtraction::extract(image, 200., 4., 9., 0.8, 1.2);

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

} //namespace ut.

} //namespace sight::filter::image.

#endif // ifndef WIN32
