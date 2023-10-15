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

#include "line_drawer_test.hpp"

#include <core/type.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>

#include <filter/image/image_diff.hpp>
#include <filter/image/line_drawer.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::line_drawer_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void line_drawer_test::setUp()
{
}

//------------------------------------------------------------------------------

void line_drawer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void line_drawer_test::circleTest()
{
    {
        const data::image::Size SIZE       = {{120, 120, 120}};
        const data::image::Spacing SPACING = {{1., 1., 1.}};
        const data::image::Origin ORIGIN   = {{0., 0., 0.}};
        const core::type TYPE              = core::type::INT16;

        const filter::image::line_drawer::CoordinatesType POINT = {{20, 20, 20}};
        const double THICKNESS                                  = 0.0001;
        const std::int16_t VALUE                                = 152;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generateImage(
            image,
            SIZE,
            SPACING,
            ORIGIN,
            TYPE,
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();
        SPTR(data::image::BufferType) val =
            data::helper::MedicalImage::getPixelInImageSpace(image, VALUE);

        filter::image::line_drawer drawer(image, nullptr);
        image_diff diff = drawer.draw(
            filter::image::bresenham_line::Orientation::Z_AXIS,
            POINT,
            POINT,
            val.get(),
            THICKNESS
        );

        const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

        CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), diff.numElements());
        data::image::IndexType index = (POINT[0] + POINT[1] * SIZE[0] + POINT[2] * SIZE[0] * SIZE[1]);
        CPPUNIT_ASSERT_EQUAL(index, diff.getElement(0).m_index);
        CPPUNIT_ASSERT_EQUAL(std::int16_t(0), *reinterpret_cast<const std::int16_t*>(diff.getElement(0).m_oldValue));
        CPPUNIT_ASSERT_EQUAL(VALUE, *reinterpret_cast<const std::int16_t*>(diff.getElement(0).m_newValue));
    }

    {
        const data::image::Size SIZE       = {{120, 120, 120}};
        const data::image::Spacing SPACING = {{1., 1., 1.}};
        const data::image::Origin ORIGIN   = {{0., 0., 0.}};
        const core::type TYPE              = core::type::INT16;

        const filter::image::line_drawer::CoordinatesType POINT = {{20, 20, 20}};

        const double THICKNESS   = 5;
        const std::int16_t VALUE = 152;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generateImage(
            image,
            SIZE,
            SPACING,
            ORIGIN,
            TYPE,
            data::image::PixelFormat::GRAY_SCALE
        );
        const auto dumpLock = image->dump_lock();

        SPTR(data::image::BufferType) val =
            data::helper::MedicalImage::getPixelInImageSpace(image, VALUE);

        filter::image::line_drawer drawer(image, nullptr);
        drawer.draw(filter::image::bresenham_line::Orientation::Z_AXIS, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const std::int16_t resValue = image->at<std::int16_t>(29, 36, 54);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 2, POINT[1] - 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 2, POINT[1] - 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 2, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 2, POINT[1] + 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 2, POINT[1] + 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 1, POINT[1] - 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 1, POINT[1] - 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 1, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 1, POINT[1] + 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] - 1, POINT[1] + 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1] - 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1] - 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1] + 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1] + 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 1, POINT[1] - 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 1, POINT[1] - 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 1, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 1, POINT[1] + 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 1, POINT[1] + 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 2, POINT[1] - 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 2, POINT[1] - 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 2, POINT[1], POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 2, POINT[1] + 1, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }
        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0] + 2, POINT[1] + 2, POINT[2]);
            CPPUNIT_ASSERT_EQUAL(std::int16_t(0), resValue);
        }
    }
}

//------------------------------------------------------------------------------

void line_drawer_test::ellipseTest()
{
    {
        const data::image::Size SIZE       = {{120, 120, 120}};
        const data::image::Spacing SPACING = {{1., 4., 1.}};
        const data::image::Origin ORIGIN   = {{0., 0., 0.}};
        const core::type TYPE              = core::type::INT16;

        const filter::image::line_drawer::CoordinatesType POINT = {{50, 50, 50}};

        const double THICKNESS   = 10;
        const std::int16_t VALUE = 152;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generateImage(
            image,
            SIZE,
            SPACING,
            ORIGIN,
            TYPE,
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();
        SPTR(data::image::BufferType) val =
            data::helper::MedicalImage::getPixelInImageSpace(image, VALUE);

        filter::image::line_drawer drawer(image, nullptr);
        image_diff diff = drawer.draw(
            filter::image::bresenham_line::Orientation::Z_AXIS,
            POINT,
            POINT,
            val.get(),
            THICKNESS
        );

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const double squareRadius = (THICKNESS / 2.) * (THICKNESS / 2.);
            const std::array p        = {static_cast<double>(POINT[0]),
                                         static_cast<double>(POINT[1]),
                                         static_cast<double>(POINT[2])
            };

            std::size_t diffIndex = 0;

            for(std::size_t j = 0 ; j < SIZE[1] ; ++j)
            {
                for(std::size_t i = 0 ; i < SIZE[0] ; ++i)
                {
                    const std::int16_t resValue = image->at<std::int16_t>(i, j, POINT[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * SPACING[0];
                    const double y    = (static_cast<double>(j) - p[1]) * SPACING[1];
                    const double dist = x * x + y * y;
                    if(dist <= squareRadius)
                    {
                        image_diff::ElementType elt = diff.getElement(diffIndex);

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            VALUE,
                            resValue
                        );
                        CPPUNIT_ASSERT(diffIndex != diff.numElements());
                        const data::image::IndexType index = i + j * SIZE[0] + POINT[2] * SIZE[0] * SIZE[1];

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            index,
                            elt.m_index
                        );

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            VALUE,
                            *reinterpret_cast<const std::int16_t*>(elt.m_newValue)
                        );

                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            *reinterpret_cast<const std::int16_t*>(elt.m_oldValue)
                        );
                        ++diffIndex;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            resValue
                        );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void line_drawer_test::borderTest()
{
    {
        const data::image::Size SIZE       = {{50, 50, 50}};
        const data::image::Spacing SPACING = {{2., 4., 8.}};
        const data::image::Origin ORIGIN   = {{0., 0., 0.}};
        const core::type TYPE              = core::type::INT16;

        const filter::image::line_drawer::CoordinatesType POINT = {{45, 3, 20}};

        const double THICKNESS   = 15;
        const std::int16_t VALUE = 1952;

        data::image::sptr image = std::make_shared<data::image>();

        utest_data::generator::image::generateImage(
            image,
            SIZE,
            SPACING,
            ORIGIN,
            TYPE,
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();
        SPTR(data::image::BufferType) val =
            data::helper::MedicalImage::getPixelInImageSpace(image, VALUE);

        filter::image::line_drawer drawer(image, nullptr);
        drawer.draw(filter::image::bresenham_line::Orientation::Z_AXIS, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const double squareRadius = (THICKNESS / 2.) * (THICKNESS / 2.);
            const std::array p        = {static_cast<double>(POINT[0]),
                                         static_cast<double>(POINT[1]),
                                         static_cast<double>(POINT[2])
            };
            for(std::size_t i = 0 ; i < SIZE[0] ; ++i)
            {
                for(std::size_t j = 0 ; j < SIZE[1] ; ++j)
                {
                    const std::int16_t resValue = image->at<std::int16_t>(i, j, POINT[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * SPACING[0];
                    const double y    = (static_cast<double>(j) - p[1]) * SPACING[1];
                    const double dist = x * x + y * y;

                    if(dist <= squareRadius)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            VALUE,
                            resValue
                        );
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            resValue
                        );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void line_drawer_test::roiTest()
{
    {
        const data::image::Size SIZE       = {{150, 150, 150}};
        const data::image::Spacing SPACING = {{2., 4., 8.}};
        const data::image::Origin ORIGIN   = {{0., 0., 0.}};
        const core::type TYPE              = core::type::INT16;

        const filter::image::line_drawer::CoordinatesType POINT = {{45, 45, 40}};

        const data::helper::MedicalImage::orientation_t ORIENTATION =
            data::helper::MedicalImage::Z_AXIS;
        const double THICKNESS   = 15;
        const std::int16_t VALUE = 1952;

        data::image::sptr image    = std::make_shared<data::image>();
        data::image::sptr roiImage = std::make_shared<data::image>();

        utest_data::generator::image::generateImage(
            image,
            SIZE,
            SPACING,
            ORIGIN,
            TYPE,
            data::image::PixelFormat::GRAY_SCALE
        );
        utest_data::generator::image::generateImage(
            roiImage,
            SIZE,
            SPACING,
            ORIGIN,
            TYPE,
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto roiDumpLock = roiImage->dump_lock();

        // draw a cube in ROI
        const data::image::Size ROI_BEGIN = {{25, 25, 25}};
        const data::image::Size ROI_END   = {{50, 50, 50}};
        const std::int16_t ROI_VALUE      = 1;

        SPTR(data::image::BufferType) roiVal =
            data::helper::MedicalImage::getPixelInImageSpace(roiImage, ROI_VALUE);

        for(std::size_t i = ROI_BEGIN[0] ; i < ROI_END[0] ; ++i)
        {
            for(std::size_t j = ROI_BEGIN[1] ; j < ROI_END[1] ; ++j)
            {
                for(std::size_t k = ROI_BEGIN[2] ; k < ROI_END[2] ; ++k)
                {
                    data::image::IndexType index = i + j * SIZE[0] + k * SIZE[0] * SIZE[1];
                    roiImage->setPixel(index, roiVal.get());
                }
            }
        }

        const auto dumpLock = image->dump_lock();
        SPTR(data::image::BufferType) val =
            data::helper::MedicalImage::getPixelInImageSpace(image, VALUE);

        filter::image::line_drawer drawer(image, roiImage);
        drawer.draw(ORIENTATION, POINT, POINT, val.get(), THICKNESS);

        {
            const std::int16_t resValue = image->at<std::int16_t>(POINT[0], POINT[1], POINT[2]);

            CPPUNIT_ASSERT_EQUAL(VALUE, resValue);
        }

        {
            const double squareRadius = (THICKNESS / 2.) * (THICKNESS / 2.);
            const std::array p        = {static_cast<double>(POINT[0]),
                                         static_cast<double>(POINT[1]),
                                         static_cast<double>(POINT[2])
            };
            for(std::size_t i = 0 ; i < SIZE[0] ; ++i)
            {
                for(std::size_t j = 0 ; j < SIZE[1] ; ++j)
                {
                    const std::int16_t resValue = image->at<std::int16_t>(i, j, POINT[2]);

                    const double x    = (static_cast<double>(i) - p[0]) * SPACING[0];
                    const double y    = (static_cast<double>(j) - p[1]) * SPACING[1];
                    const double dist = x * x + y * y;

                    if(dist <= squareRadius
                       && i >= ROI_BEGIN[0] && i <= ROI_END[0]
                       && j >= ROI_BEGIN[1] && j <= ROI_END[1])
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            VALUE,
                            resValue
                        );
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "p[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                            std::int16_t(0),
                            resValue
                        );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
