/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "ResamplerTest.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Matrix4.hpp>

#include <filter/image/Resampler.hpp>

#include <utestData/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::ResamplerTest);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void ResamplerTest::setUp()
{
}

//------------------------------------------------------------------------------

void ResamplerTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ResamplerTest::identityTest()
{
    const data::Image::Size SIZE = {{32, 32, 32}};

    // TODO: make it work with an anisotropic spacing.
    const data::Image::Spacing SPACING = {{0.5, 0.5, 0.5}};
    const data::Image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::Type TYPE              = core::Type::INT16;

    data::Image::sptr imageIn = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, data::Image::GRAY_SCALE);
    utestData::generator::Image::randomizeImage(imageIn);

    data::Image::sptr imageOut = data::Image::New();

    // Identity.
    data::Matrix4::sptr idMat = data::Matrix4::New();

    filter::image::Resampler::resample(
        data::Image::csptr(imageIn),
        imageOut,
        data::Matrix4::csptr(idMat),
        std::make_tuple(imageIn->getSize(), imageIn->getOrigin(), imageIn->getSpacing())
    );

    CPPUNIT_ASSERT(imageOut->getSize() == SIZE);
    CPPUNIT_ASSERT(imageOut->getSpacing() == SPACING);
    CPPUNIT_ASSERT(imageOut->getType() == TYPE);

    const auto inDumpLock  = imageIn->dump_lock();
    const auto outDumpLock = imageOut->dump_lock();

    for(std::size_t i = 0 ; i < SIZE[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < SIZE[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < SIZE[2] ; ++k)
            {
                const std::int16_t valueIn  = imageIn->at<std::int16_t>(i, j, k);
                const std::int16_t valueOut = imageOut->at<std::int16_t>(i, j, k);

                // The image shouldn't change.
                std::string msg = std::to_string(valueIn) + " " + std::to_string(valueOut);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, valueIn, valueOut);
            }
        }
    }
}

//------------------------------------------------------------------------------

void ResamplerTest::translateTest()
{
    // Generate a simple image with a white cube at its center.
    const data::Image::Size SIZE       = {{16, 16, 16}};
    const data::Image::Spacing SPACING = {{1., 1., 1.}};
    const data::Image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::Type TYPE              = core::Type::UINT8;

    data::Image::sptr imageIn  = data::Image::New();
    data::Image::sptr imageOut = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, data::Image::GRAY_SCALE);

    std::uint8_t value = 255;

    SPTR(data::Image::BufferType) bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    const auto inDumpLock = imageIn->dump_lock();

    // Draw a tiny 2x2 cube at the center
    imageIn->at<std::uint8_t>(7, 7, 7) = value;
    imageIn->at<std::uint8_t>(7, 7, 8) = value;
    imageIn->at<std::uint8_t>(7, 8, 7) = value;
    imageIn->at<std::uint8_t>(7, 8, 8) = value;
    imageIn->at<std::uint8_t>(8, 7, 7) = value;
    imageIn->at<std::uint8_t>(8, 7, 8) = value;
    imageIn->at<std::uint8_t>(8, 8, 7) = value;
    imageIn->at<std::uint8_t>(8, 8, 8) = value;

    // 5 mm translation along the x axis.
    data::Matrix4::sptr transMat = data::Matrix4::New();
    transMat->setCoefficient(0, 3, 5);

    filter::image::Resampler::resample(
        data::Image::csptr(imageIn),
        imageOut,
        data::Matrix4::csptr(transMat)
    );

    const auto dumpLock = imageOut->dump_lock();

    for(std::size_t i = 0 ; i < SIZE[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < SIZE[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < SIZE[2] ; ++k)
            {
                const uint8_t valueOut = imageOut->at<std::uint8_t>(i, j, k);

                if((i >= 2 && i <= 3) && (j >= 7 && j <= 8) && (k >= 7 && k <= 8))
                {
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), valueOut);
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), valueOut);
                }
            }
        }
    }

    // Check if size and spacing are the same as the input.
    CPPUNIT_ASSERT(imageOut->getSize() == SIZE);
    CPPUNIT_ASSERT(imageOut->getSpacing() == SPACING);
}

//------------------------------------------------------------------------------

void ResamplerTest::rotateTest()
{
    const data::Image::Size SIZE       = {{64, 64, 64}};
    const data::Image::Spacing SPACING = {{1., 1., 1.}};
    const data::Image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::Type TYPE              = core::Type::FLOAT;

    data::Image::sptr imageIn  = data::Image::New();
    data::Image::sptr imageOut = data::Image::New();

    utestData::generator::Image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, data::Image::GRAY_SCALE);

    const float value = 1.F;

    const auto dumpLock = imageIn->dump_lock();

    // draw the back Z face.
    for(std::size_t i = 0 ; i < 64 ; ++i)
    {
        for(std::size_t j = 0 ; j < 64 ; ++j)
        {
            imageIn->at<float>(i, j, 0) = value;
        }
    }

    // FIXME: compute to appropriate matrix to rotate a face from negative Z to negative X.

    data::Matrix4::sptr rotMat = data::Matrix4::New();
    // 90° rotation along the Y axis.
    rotMat->setCoefficient(0, 0, 0);
    rotMat->setCoefficient(0, 2, 1);
    rotMat->setCoefficient(2, 0, -1);
    rotMat->setCoefficient(2, 2, 0);

    // 32 mm translation along the X axis.
    rotMat->setCoefficient(0, 3, double(SIZE[0]) / 2.);

    filter::image::Resampler::resample(
        data::Image::csptr(imageIn),
        imageOut,
        data::Matrix4::csptr(rotMat)
    );

    const auto outDumpLock = imageOut->dump_lock();

    for(std::size_t i = 0 ; i < SIZE[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < SIZE[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < SIZE[2] ; ++k)
            {
                const float valueOut = imageOut->at<float>(i, j, k);

                std::string msg = std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(k);

                if(i == 0)
                {
                    // The negative Z face must be 'white'.
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1.F, valueOut);
                }
                else
                {
                    // Everything else should be 'black'.
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0.F, valueOut);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
