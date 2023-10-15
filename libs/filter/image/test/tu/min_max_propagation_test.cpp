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

#include "min_max_propagation_test.hpp"

#include <data/helper/MedicalImage.hpp>

#include <filter/image/min_max_propagation.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::min_max_propagation_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

static std::size_t computeOffset(const std::size_t x, const std::size_t y, const std::size_t z, data::image::sptr image)
{
    const data::image::Size size = image->size();
    return z * size[0] * size[1] + y * size[0] + x;
}

//------------------------------------------------------------------------------

static void drawCube(data::image::sptr image, const std::uint8_t value)
{
    const auto dumpLock = image->dump_lock();

    SPTR(data::image::BufferType) bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(image, value);

    for(std::size_t x = 10 ; x < 20 ; ++x)
    {
        for(std::size_t y = 10 ; y < 20 ; ++y)
        {
            for(std::size_t z = 10 ; z < 20 ; ++z)
            {
                image->setPixel(computeOffset(x, y, z, image), bufferValue.get());
            }
        }
    }
}

//------------------------------------------------------------------------------

void min_max_propagation_test::setUp()
{
}

//------------------------------------------------------------------------------

void min_max_propagation_test::tearDown()
{
}

//------------------------------------------------------------------------------

void min_max_propagation_test::minPropagTest()
{
    // Create two 32*32*32*8 images
    const data::image::Size SIZE       = {{32, 32, 32}};
    const data::image::Spacing SPACING = {{1., 1., 1.}};
    const data::image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::type TYPE              = core::type::UINT8;

    data::image::sptr imageIn  = std::make_shared<data::image>();
    data::image::sptr imageOut = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, data::image::GRAY_SCALE);
    utest_data::generator::image::generateImage(imageOut, SIZE, SPACING, ORIGIN, TYPE, data::image::GRAY_SCALE);

    // Draw a cube at 10,10,10 with a 255 value
    drawCube(imageIn, 255);

    min_max_propagation propagator(imageIn, imageOut, nullptr);

    min_max_propagation::SeedsType seed = {{{15, 15, 15}}};

    // Propagate at 15,15,15 with a 255 value (same as cube)
    std::uint8_t value = 255;

    SPTR(data::image::BufferType) bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    propagator.propagate(seed, bufferValue.get(), 500, true, min_max_propagation::MIN);

    const auto dumpLockIn  = imageIn->dump_lock();
    const auto dumpLockOut = imageOut->dump_lock();

    // Check that the image is not changed because the propagated value is the same
    for(std::size_t index = 0 ; index < imageIn->getSizeInBytes() ; ++index)
    {
        const std::uint8_t valueIn  = imageIn->at<std::uint8_t>(index);
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
    }

    // Check that the cube is changed with a "3" propagated value
    drawCube(imageIn, 1);

    value = 3;

    bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    propagator.propagate(seed, bufferValue.get(), 500, true, min_max_propagation::MIN);

    for(std::size_t index = 0 ; index < imageIn->getSizeInBytes() ; ++index)
    {
        const std::uint8_t valueIn  = imageIn->at<std::uint8_t>(index);
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        if(valueIn == 1)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(valueIn, valueOut);
        }
    }

    // Check that the entire image is completely filled with propagated value
    seed = {{{0, 0, 0}}};

    value = 4;

    bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    propagator.propagate(seed, bufferValue.get(), 500, true, min_max_propagation::MIN);

    for(std::size_t index = 0 ; index < imageIn->getSizeInBytes() ; ++index)
    {
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4), valueOut);
    }
}

//------------------------------------------------------------------------------

void min_max_propagation_test::maxPropagTest()
{
    // Create two 32*32*32*8 images
    const data::image::Size SIZE       = {{32, 32, 32}};
    const data::image::Spacing SPACING = {{1., 1., 1.}};
    const data::image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::type TYPE              = core::type::UINT8;

    data::image::sptr imageIn  = std::make_shared<data::image>();
    data::image::sptr imageOut = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, data::image::GRAY_SCALE);
    utest_data::generator::image::generateImage(imageOut, SIZE, SPACING, ORIGIN, TYPE, data::image::GRAY_SCALE);

    // Draw a cube at 10,10,10 with a 2 value
    drawCube(imageIn, 2);

    min_max_propagation propagator(imageIn, imageOut, nullptr);

    min_max_propagation::SeedsType seed = {{{15, 15, 15}}};

    // Propagate at 15,15,15 with a 3 value
    std::uint8_t value = 3;

    SPTR(data::image::BufferType) bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    propagator.propagate(seed, bufferValue.get(), 500, true, min_max_propagation::MAX);

    const auto dumpLockIn  = imageIn->dump_lock();
    const auto dumpLockOut = imageOut->dump_lock();

    // Check that the entire image is completely filled with propagated value
    for(std::size_t index = 0 ; index < imageIn->getSizeInBytes() ; ++index)
    {
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);
    }

    // Propagate at 0,0,0 with a 2 value (outside cube)
    seed  = {{{0, 0, 0}}};
    value = 2;

    bufferValue = data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    propagator.propagate(seed, bufferValue.get(), 500, true, min_max_propagation::MAX);

    // Check that the entire image is completely filled with propagated value
    for(std::size_t index = 0 ; index < imageIn->getSizeInBytes() ; ++index)
    {
        const std::uint8_t valueIn  = imageIn->at<std::uint8_t>(index);
        const std::uint8_t valueOut = imageOut->at<std::uint8_t>(index);

        if(valueIn == 0)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(2), valueOut);
        }
        else
        {
            // Old value previously propagated
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);
        }
    }
}

//------------------------------------------------------------------------------

void min_max_propagation_test::radiusTest()
{
    // Create two 32*32*32*8 images
    const data::image::Size SIZE       = {{33, 33, 33}};
    const data::image::Spacing SPACING = {{1., 1., 1.}};
    const data::image::Origin ORIGIN   = {{0., 0., 0.}};
    const core::type TYPE              = core::type::UINT8;

    data::image::sptr imageIn  = std::make_shared<data::image>();
    data::image::sptr imageOut = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(imageIn, SIZE, SPACING, ORIGIN, TYPE, data::image::GRAY_SCALE);
    utest_data::generator::image::generateImage(imageOut, SIZE, SPACING, ORIGIN, TYPE, data::image::GRAY_SCALE);

    min_max_propagation propagator(imageIn, imageOut, nullptr);

    min_max_propagation::SeedsType seed = {{{16, 16, 16}}};

    // Propagate at 16,16,16 with a 3 value
    std::uint8_t value = 3;

    SPTR(data::image::BufferType) bufferValue =
        data::helper::MedicalImage::getPixelInImageSpace(imageIn, value);

    propagator.propagate(seed, bufferValue.get(), 3.5, true, min_max_propagation::MIN);

    const auto dumpLockOut = imageOut->dump_lock();

    // Check the voxel at 16,16,12
    std::uint8_t valueOut = imageOut->at<std::uint8_t>(16, 16, 12);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), valueOut);

    // Check the voxel at 16,16,13
    valueOut = imageOut->at<std::uint8_t>(16, 16, 13);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);

    // Check the voxel at 16,16,16
    valueOut = imageOut->at<std::uint8_t>(16, 16, 16);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), valueOut);

    // Check the voxel at 16,16,20
    valueOut = imageOut->at<std::uint8_t>(16, 16, 20);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), valueOut);
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
