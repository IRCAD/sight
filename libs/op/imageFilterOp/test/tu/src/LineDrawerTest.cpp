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

#include "LineDrawerTest.hpp"

#include <imageFilterOp/LineDrawer.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/ImageDiff.hpp>

#include <fwTest/generator/Image.hpp>

#include <fwTools/Type.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::imageFilterOp::ut::LineDrawerTest );

namespace imageFilterOp
{
namespace ut
{

//------------------------------------------------------------------------------

void LineDrawerTest::setUp()
{
}

//------------------------------------------------------------------------------

void LineDrawerTest::tearDown()
{
}

//------------------------------------------------------------------------------

void LineDrawerTest::circleTest()
{
    {
        const ::fwData::Image::Size SIZE       = {{ 120, 120, 120 }};
        const ::fwData::Image::Spacing SPACING = {{ 1., 1., 1. }};
        const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
        const ::fwTools::Type TYPE             = ::fwTools::Type::s_INT16;

        const ::imageFilterOp::LineDrawer::CoordinatesType POINT = {{ 20, 20, 20 }};

        const ::fwDataTools::helper::MedicalImage::Orientation ORIENTATION =
            ::fwDataTools::helper::MedicalImage::Z_AXIS;
        const double THICKNESS   = 0.0001;
        const std::int16_t VALUE = 152;

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE,
                                                  ::fwData::Image::PixelFormat::GRAY_SCALE);

        const auto dumpLock = image->lock();
        SPTR(::fwData::Image::BufferType) val =
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, VALUE);

        ::imageFilterOp::LineDrawer drawer(image, nullptr);
        ::fwDataTools::ImageDiff diff = drawer.draw(ORIENTATION, POINT, POINT, val.get(), THICKNESS);

        const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

        CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        CPPUNIT_ASSERT_EQUAL(size_t(1), diff.getNumberOfElements());
        ::fwData::Image::IndexType index = (POINT[0] + POINT[1]*SIZE[0] + POINT[2]*SIZE[0]*SIZE[1]);
        CPPUNIT_ASSERT_EQUAL(index, diff.getElement(0).m_index);
        CPPUNIT_ASSERT_EQUAL(std::int16_t(0), *reinterpret_cast<std::int16_t*>(diff.getElement(0).m_oldValue));
        CPPUNIT_ASSERT_EQUAL(VALUE, *reinterpret_cast<std::int16_t*>(diff.getElement(0).m_newValue));
    }

    {
        const ::fwData::Image::Size SIZE       = {{ 120, 120, 120 }};
        const ::fwData::Image::Spacing SPACING = {{ 1., 1., 1. }};
        const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
        const ::fwTools::Type TYPE             = ::fwTools::Type::s_INT16;

        const ::imageFilterOp::LineDrawer::CoordinatesType POINT = {{ 20, 20, 20 }};

        const ::fwDataTools::helper::MedicalImage::Orientation ORIENTATION =
            ::fwDataTools::helper::MedicalImage::Z_AXIS;
        const double THICKNESS   = 5;
        const std::int16_t VALUE = 152;

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE,
                                                  ::fwData::Image::PixelFormat::GRAY_SCALE);
        const auto dumpLock = image->lock();

        SPTR(::fwData::Image::BufferType) val =
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, VALUE);

        ::imageFilterOp::LineDrawer drawer(image, nullptr);
        drawer.draw(ORIENTATION, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const std::int16_t resValue = image->at<std::int16_t>(29, 36, 54);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-2, POINT[1]-2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-2, POINT[1]-1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-2, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-2, POINT[1]+1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-2, POINT[1]+2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-1, POINT[1]-2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-1, POINT[1]-1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-1, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-1, POINT[1]+1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]-1, POINT[1]+2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1]-2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1]-1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1]+1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1]+2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+1, POINT[1]-2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+1, POINT[1]-1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+1, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+1, POINT[1]+1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+1, POINT[1]+2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+2, POINT[1]-2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+2, POINT[1]-1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+2, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+2, POINT[1]+1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0]+2, POINT[1]+2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
    }
}

//------------------------------------------------------------------------------

void LineDrawerTest::ellipseTest()
{
    {
        const ::fwData::Image::Size SIZE       = {{ 120, 120, 120 }};
        const ::fwData::Image::Spacing SPACING = {{ 1., 4., 1. }};
        const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
        const ::fwTools::Type TYPE             = ::fwTools::Type::s_INT16;

        const ::imageFilterOp::LineDrawer::CoordinatesType POINT = {{ 50, 50, 50 }};

        const ::fwDataTools::helper::MedicalImage::Orientation ORIENTATION =
            ::fwDataTools::helper::MedicalImage::Z_AXIS;
        const double THICKNESS   = 10;
        const std::int16_t VALUE = 152;

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE,
                                                  ::fwData::Image::PixelFormat::GRAY_SCALE);

        const auto dumpLock = image->lock();
        SPTR(::fwData::Image::BufferType) val =
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, VALUE);

        ::imageFilterOp::LineDrawer drawer(image, nullptr);
        ::fwDataTools::ImageDiff diff = drawer.draw(ORIENTATION, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const double squareRadius = (THICKNESS/2.)*(THICKNESS/2.);
            const double p[3]         = { static_cast<double>(POINT[0]),
                                          static_cast<double>(POINT[1]),
                                          static_cast<double>(POINT[2]) };

            size_t diffIndex = 0;

            for (size_t j = 0; j < SIZE[1]; ++j)
            {
                for (size_t i = 0; i < SIZE[0]; ++i)
                {
                    const std::int16_t resValue = image->at<std::int16_t>(i, j, POINT[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * SPACING[0];
                    const double y    = (static_cast<double>(j) - p[1]) * SPACING[1];
                    const double dist = x*x + y*y;
                    if (dist <= squareRadius )
                    {
                        ::fwDataTools::ImageDiff::ElementType elt = diff.getElement(diffIndex);

                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     VALUE, resValue);
                        CPPUNIT_ASSERT(diffIndex != diff.getNumberOfElements());
                        const ::fwData::Image::IndexType index = i + j*SIZE[0] + POINT[2]*SIZE[0]*SIZE[1];

                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     index, elt.m_index);

                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     VALUE, *reinterpret_cast<std::int16_t*>(elt.m_newValue));

                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     std::int16_t(0),
                                                     *reinterpret_cast<std::int16_t*>(elt.m_oldValue));
                        ++diffIndex;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     std::int16_t(0), resValue);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void LineDrawerTest::borderTest()
{
    {
        const ::fwData::Image::Size SIZE       = {{ 50, 50, 50 }};
        const ::fwData::Image::Spacing SPACING = {{ 2., 4., 8. }};
        const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
        const ::fwTools::Type TYPE             = ::fwTools::Type::s_INT16;

        const ::imageFilterOp::LineDrawer::CoordinatesType POINT = {{ 45, 3, 20 }};

        const ::fwDataTools::helper::MedicalImage::Orientation ORIENTATION =
            ::fwDataTools::helper::MedicalImage::Z_AXIS;
        const double THICKNESS   = 15;
        const std::int16_t VALUE = 1952;

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE,
                                                  ::fwData::Image::PixelFormat::GRAY_SCALE);

        const auto dumpLock = image->lock();
        SPTR(::fwData::Image::BufferType) val =
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, VALUE);

        ::imageFilterOp::LineDrawer drawer(image, nullptr);
        drawer.draw(ORIENTATION, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const double squareRadius = (THICKNESS/2.)*(THICKNESS/2.);
            const double p[3]         = { static_cast<double>(POINT[0]),
                                          static_cast<double>(POINT[1]),
                                          static_cast<double>(POINT[2]) };
            for (size_t i = 0; i < SIZE[0]; ++i)
            {
                for (size_t j = 0; j < SIZE[1]; ++j)
                {
                    const std::int16_t resValue = image->at<std::int16_t>(i, j, POINT[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * SPACING[0];
                    const double y    = (static_cast<double>(j) - p[1]) * SPACING[1];
                    const double dist = x*x + y*y;

                    if (dist <= squareRadius )
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     VALUE, resValue);
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     std::int16_t(0), resValue);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void LineDrawerTest::roiTest()
{
    {
        const ::fwData::Image::Size SIZE       = {{ 150, 150, 150 }};
        const ::fwData::Image::Spacing SPACING = {{ 2., 4., 8. }};
        const ::fwData::Image::Origin ORIGIN   = {{ 0., 0., 0. }};
        const ::fwTools::Type TYPE             = ::fwTools::Type::s_INT16;

        const ::imageFilterOp::LineDrawer::CoordinatesType POINT = {{ 45, 45, 40 }};

        const ::fwDataTools::helper::MedicalImage::Orientation ORIENTATION =
            ::fwDataTools::helper::MedicalImage::Z_AXIS;
        const double THICKNESS   = 15;
        const std::int16_t VALUE = 1952;

        ::fwData::Image::sptr image    = ::fwData::Image::New();
        ::fwData::Image::sptr roiImage = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE,
                                                  ::fwData::Image::PixelFormat::GRAY_SCALE);
        ::fwTest::generator::Image::generateImage(roiImage, SIZE, SPACING, ORIGIN, TYPE,
                                                  ::fwData::Image::PixelFormat::GRAY_SCALE);

        const auto roiDumpLock = roiImage->lock();

        // draw a cube in ROI
        const ::fwData::Image::Size ROI_BEGIN = {{ 25, 25, 25 }};
        const ::fwData::Image::Size ROI_END   = {{ 50, 50, 50 }};
        const std::int16_t ROI_VALUE          = 1;

        SPTR(::fwData::Image::BufferType) roiVal =
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(roiImage, ROI_VALUE);

        for (size_t i = ROI_BEGIN[0]; i < ROI_END[0]; ++i)
        {
            for (size_t j = ROI_BEGIN[1]; j < ROI_END[1]; ++j)
            {
                for (size_t k = ROI_BEGIN[2]; k < ROI_END[2]; ++k)
                {
                    ::fwData::Image::IndexType index = i + j*SIZE[0] + k * SIZE[0]*SIZE[1];
                    roiImage->setPixelBuffer(index, roiVal.get());
                }
            }
        }

        const auto dumpLock = image->lock();
        SPTR(::fwData::Image::BufferType) val =
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, VALUE);

        ::imageFilterOp::LineDrawer drawer(image, roiImage);
        drawer.draw(ORIENTATION, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const double squareRadius = (THICKNESS/2.)*(THICKNESS/2.);
            const double p[3]         = { static_cast<double>(POINT[0]),
                                          static_cast<double>(POINT[1]),
                                          static_cast<double>(POINT[2]) };
            for (size_t i = 0; i < SIZE[0]; ++i)
            {
                for (size_t j = 0; j < SIZE[1]; ++j)
                {
                    const std::int16_t resValue = image->at<std::int16_t>(i, j, POINT[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * SPACING[0];
                    const double y    = (static_cast<double>(j) - p[1]) * SPACING[1];
                    const double dist = x*x + y*y;

                    if (dist <= squareRadius
                        && i >= ROI_BEGIN[0] && i <= ROI_END[0]
                        && j >= ROI_BEGIN[1] && j <= ROI_END[1])
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     VALUE, resValue);
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("p["+std::to_string(i)+"]["+std::to_string(j)+"]",
                                                     std::int16_t(0), resValue);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace imageFilterOp
