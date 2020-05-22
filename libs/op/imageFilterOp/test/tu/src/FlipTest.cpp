/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
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

#include <imageFilterOp/Flipper.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwTest/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(::imageFilterOp::ut::FlipTest);

namespace imageFilterOp
{
namespace ut
{

//------------------------------------------------------------------------------

void FlipTest::flipAlongXAxisTest()
{
    const ::fwData::Image::Size size          = {{ 3, 3, 3 }};
    const ::fwData::Image::Spacing spacing    = {{ 0.1, 0.4, 1.6 }};
    const ::fwData::Image::Origin origin      = {{ 0., 0., 0. }};
    const ::fwTools::Type type                = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes{true, false, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    ::fwTest::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->lock();
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->lock();

    for(size_t i = 0; i < size[0]; ++i)
    {
        for(size_t j = 0; j < size[1]; ++j )
        {
            for(size_t k = 0; k < size[2]; ++k)
            {
                const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                const uint8_t valueOut = imageOut->at<std::uint8_t>(size[0]-i-1, j, k);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongYAxisTest()
{
    const ::fwData::Image::Size size          = {{ 3, 3, 3 }};
    const ::fwData::Image::Spacing spacing    = {{ 0.5, 0.5, 0.5 }};
    const ::fwData::Image::Origin origin      = {{ 8., 4., 2. }};
    const ::fwTools::Type type                = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes{false, true, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    ::fwTest::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->lock();
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->lock();

    for(size_t i = 0; i < size[0]; ++i)
    {
        for(size_t j = 0; j < size[1]; ++j )
        {
            for(size_t k = 0; k < size[2]; ++k)
            {
                const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                const uint8_t valueOut = imageOut->at<std::uint8_t>(i, size[1]-j-1, k);

                // Static cast to get proper printing of the value (in int and not char) on stdout
                CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
            }
        }
    }
}

//------------------------------------------------------------------------------

void FlipTest::flipAlongZAxisTest()
{
    const ::fwData::Image::Size size          = {{ 3, 3, 3 }};
    const ::fwData::Image::Spacing spacing    = {{ 2.0, 2.0, 2.0 }};
    const ::fwData::Image::Origin origin      = {{ 0., 0., 0. }};
    const ::fwTools::Type type                = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes{false, false, true};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    ::fwTest::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->lock();
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->lock();

    for(size_t i = 0; i < size[0]; ++i)
    {
        for(size_t j = 0; j < size[1]; ++j )
        {
            for(size_t k = 0; k < size[2]; ++k)
            {
                const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                const uint8_t valueOut = imageOut->at<std::uint8_t>(i, j, size[2]-k-1);

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
        const ::fwData::Image::Size size          = {{ 3, 3, 3 }};
        const ::fwData::Image::Spacing spacing    = {{ 0.5, 0.5, 0.5 }};
        const ::fwData::Image::Origin origin      = {{ 0., 0., 0. }};
        const ::fwTools::Type type                = ::fwTools::Type::s_UINT8;
        const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;
        std::array<bool, 3> flipAxes{true, true, false };

        ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
        ::fwData::Image::sptr imageOut = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
        ::fwTest::generator::Image::randomizeImage(imageIn);

        const auto inDumpLock = imageIn->lock();
        ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
        const auto outDumpLock = imageOut->lock();

        for(size_t i = 0; i < size[0]; ++i)
        {
            for(size_t j = 0; j < size[1]; ++j )
            {
                for(size_t k = 0; k < size[2]; ++k)
                {
                    const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                    const uint8_t valueOut = imageOut->at<std::uint8_t>(size[0]-i-1, size[1]-j-1, k);

                    // Static cast to get proper printing of the value (in int and not char) on stdout
                    CPPUNIT_ASSERT_EQUAL(static_cast<int>(valueIn), static_cast<int>(valueOut));
                }
            }
        }
    }

    {
        const ::fwData::Image::Size size          = {{ 3, 3, 3 }};
        const ::fwData::Image::Spacing spacing    = {{ 0.5, 0.5, 0.5 }};
        const ::fwData::Image::Origin origin      = {{ 0., 0., 0. }};
        const ::fwTools::Type type                = ::fwTools::Type::s_UINT8;
        const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;
        std::array<bool, 3> flipAxes{true, true, true};

        ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
        ::fwData::Image::sptr imageOut = ::fwData::Image::New();

        ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
        ::fwTest::generator::Image::randomizeImage(imageIn);

        const auto inDumpLock = imageIn->lock();
        ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
        const auto outDumpLock = imageOut->lock();

        for(size_t i = 0; i < size[0]; ++i)
        {
            for(size_t j = 0; j < size[1]; ++j )
            {
                for(size_t k = 0; k < size[2]; ++k)
                {
                    const uint8_t valueIn  = imageIn->at<std::uint8_t>(i, j, k);
                    const uint8_t valueOut = imageOut->at<std::uint8_t>(size[0]-i-1, size[1]-j-1, size[2]-k-1);

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
    const ::fwData::Image::Size size          = {0, 0, 0};
    const ::fwData::Image::Spacing spacing    = {0., 0., 0.};
    const ::fwData::Image::Origin origin      = {0., 0., 0.};
    const ::fwTools::Type type                = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;
    std::array<bool, 3> flipAxes{false, true, false};

    ::fwData::Image::sptr imageIn  = ::fwData::Image::New();
    ::fwData::Image::sptr imageOut = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(imageIn, size, spacing, origin, type, format);
    ::fwTest::generator::Image::randomizeImage(imageIn);

    const auto inDumpLock = imageIn->lock();
    ::imageFilterOp::Flipper::flip(imageIn, imageOut, flipAxes);
    const auto outDumpLock = imageOut->lock();

    const ::fwData::Image::Size imageSize    = imageIn->getSize2();
    const ::fwData::Image::Size imageOutSize = imageOut->getSize2();

    CPPUNIT_ASSERT_EQUAL(imageSize[0], imageOutSize[0]);
    CPPUNIT_ASSERT_EQUAL(imageSize[1], imageOutSize[1]);
    CPPUNIT_ASSERT_EQUAL(imageSize[2], imageOutSize[2]);
}

//------------------------------------------------------------------------------
} // ut
} // imageFilterOp
