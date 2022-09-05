/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "FlipTest.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <filter/image/Flipper.hpp>

#include <utestData/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::FlipTest);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void FlipTest::flipAlongXAxisTest()
{
    const data::Image::Size size          = {{3, 3, 3}};
    const data::Image::Spacing spacing    = {{0.1, 0.4, 1.6}};
    const data::Image::Origin origin      = {{0., 0., 0.}};
    const core::Type type                 = core::Type::UINT8;
    const data::Image::PixelFormat format = data::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes {true, false, false};

    data::Image::sptr imageIn  = data::Image::New();
    data::Image::sptr imageOut = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    utestData::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->dump_lock();
    filter::image::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                const uint8_t valueOut = imageOut->at<std::uint8_t>(size[0] - i - 1, j, k);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongYAxisTest()
{
    const data::Image::Size size          = {{3, 3, 3}};
    const data::Image::Spacing spacing    = {{0.5, 0.5, 0.5}};
    const data::Image::Origin origin      = {{8., 4., 2.}};
    const core::Type type                 = core::Type::UINT8;
    const data::Image::PixelFormat format = data::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes {false, true, false};

    data::Image::sptr imageIn  = data::Image::New();
    data::Image::sptr imageOut = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    utestData::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->dump_lock();
    filter::image::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                const uint8_t valueOut = imageOut->at<std::uint8_t>(i, size[1] - j - 1, k);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongZAxisTest()
{
    const data::Image::Size size          = {{3, 3, 3}};
    const data::Image::Spacing spacing    = {{2.0, 2.0, 2.0}};
    const data::Image::Origin origin      = {{0., 0., 0.}};
    const core::Type type                 = core::Type::UINT8;
    const data::Image::PixelFormat format = data::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes {false, false, true};

    data::Image::sptr imageIn  = data::Image::New();
    data::Image::sptr imageOut = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    utestData::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->dump_lock();
    filter::image::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->dump_lock();

    for(std::size_t i = 0 ; i < size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < size[2] ; ++k)
            {
                const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                const uint8_t valueOut = imageOut->at<std::uint8_t>(i, j, size[2] - k - 1);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongMultipleAxesTest()
{
    {
        const data::Image::Size size          = {{3, 3, 3}};
        const data::Image::Spacing spacing    = {{0.5, 0.5, 0.5}};
        const data::Image::Origin origin      = {{0., 0., 0.}};
        const core::Type type                 = core::Type::UINT8;
        const data::Image::PixelFormat format = data::Image::GRAY_SCALE;
        std::array<bool, 3> flipAxes {true, true, false};

        data::Image::sptr imageIn  = data::Image::New();
        data::Image::sptr imageOut = data::Image::New();

        utestData::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
        utestData::generator::Image::randomizeImage(imageIn);

        const auto inDumpLock = imageIn->dump_lock();
        filter::image::Flipper::flip(imageIn, imageOut, flipAxes);
        const auto outDumpLock = imageOut->dump_lock();

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                    const uint8_t valueOut = imageOut->at<std::uint8_t>(size[0] - i - 1, size[1] - j - 1, k);

                    // Static cast to get proper printing of the value (in int and not char) on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
                }
            }
        }
    }

    {
        const data::Image::Size size          = {{3, 3, 3}};
        const data::Image::Spacing spacing    = {{0.5, 0.5, 0.5}};
        const data::Image::Origin origin      = {{0., 0., 0.}};
        const core::Type type                 = core::Type::UINT8;
        const data::Image::PixelFormat format = data::Image::GRAY_SCALE;
        std::array<bool, 3> flipAxes {true, true, true};

        data::Image::sptr imageIn  = data::Image::New();
        data::Image::sptr imageOut = data::Image::New();

        utestData::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
        utestData::generator::Image::randomizeImage(imageIn);

        const auto inDumpLock = imageIn->dump_lock();
        filter::image::Flipper::flip(imageIn, imageOut, flipAxes);
        const auto outDumpLock = imageOut->dump_lock();

        for(std::size_t i = 0 ; i < size[0] ; ++i)
        {
            for(std::size_t j = 0 ; j < size[1] ; ++j)
            {
                for(std::size_t k = 0 ; k < size[2] ; ++k)
                {
                    const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                    const uint8_t valueOut = imageOut->at<std::uint8_t>(
                        size[0] - i - 1,
                        size[1] - j - 1,
                        size[2] - k - 1
                    );

                    // Static cast to get proper printing of the value (in int and not char) on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipEmptyImageTest()
{
    const data::Image::Size size          = {0, 0, 0};
    const data::Image::Spacing spacing    = {0., 0., 0.};
    const data::Image::Origin origin      = {0., 0., 0.};
    const core::Type type                 = core::Type::UINT8;
    const data::Image::PixelFormat format = data::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes {false, true, false};

    data::Image::sptr imageIn  = data::Image::New();
    data::Image::sptr imageOut = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    utestData::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->dump_lock();
    filter::image::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->dump_lock();

    const data::Image::Size imageSize    = imageIn->getSize();
    const data::Image::Size imageOutSize = imageOut->getSize();

    CPPUNIT_ASSERT_EQUAL(imageSize[0], imageOutSize[0]);
    CPPUNIT_ASSERT_EQUAL(imageSize[1], imageOutSize[1]);
    CPPUNIT_ASSERT_EQUAL(imageSize[2], imageOutSize[2]);
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
