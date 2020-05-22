/************************************************************************
 *
 * Copyright (C) 2017-2020 IRCAD France
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

#include "ImageDiffCommandTest.hpp"

#include <fwCommand/ImageDiffCommand.hpp>

#include <fwTest/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCommand::ut::ImageDiffCommandTest );

namespace fwCommand
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageDiffCommandTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageDiffCommandTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageDiffCommandTest::undoredoTest()
{
    const ::fwData::Image::Size SIZE          = {{ 32, 32, 32 }};
    const ::fwData::Image::Spacing SPACING    = {{ 1., 1., 1. }};
    const ::fwData::Image::Origin ORIGIN      = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;

    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE, format);

    const auto dumpLock = image->lock();

    ::fwDataTools::ImageDiff diff(image->getType().sizeOf());

    std::uint8_t NEWVALUE = 1;

    ::fwData::Image::BufferType* newBufferValue = reinterpret_cast< ::fwData::Image::BufferType*>(&NEWVALUE);

    const std::vector< ::fwData::Image::IndexType> indices = {{ 51, 10, 8, 123, 1098, 23456, 6, 9999 }};

    // Add 8 elements to the diff. Write new values to the image.
    for(int i = 0; i < 8; ++i)
    {
        const ::fwData::Image::IndexType index = indices[i];

        const ::fwData::Image::BufferType* pixBuf =
            reinterpret_cast< ::fwData::Image::BufferType* >(image->getPixelBuffer(index));

        diff.addDiff(index, pixBuf, newBufferValue);
        image->setPixelBuffer(index, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), diff.getNumberOfElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Create an imageDiffCommand to test
    ::fwCommand::ImageDiffCommand imageDiffCommand( image, diff);

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    CPPUNIT_ASSERT(imageDiffCommand.undo());

    for(size_t it = 0; it < image->getSizeInBytes(); ++it)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->getPixelBuffer(it)));
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    CPPUNIT_ASSERT(imageDiffCommand.redo());

    for(size_t i = 0; i < image->getSizeInBytes(); ++i)
    {
        // Check if 'i' is an index
        auto indexIt = std::find(indices.begin(), indices.end(), i);

        if(indexIt != indices.end())
        {
            CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<std::uint8_t*>(image->getPixelBuffer(i)));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->getPixelBuffer(i)));
        }
    }
}

//------------------------------------------------------------------------------

void ImageDiffCommandTest::getSizeTest()
{
    const ::fwData::Image::Size SIZE          = {{ 32, 32, 32 }};
    const ::fwData::Image::Spacing SPACING    = {{ 1., 1., 1. }};
    const ::fwData::Image::Origin ORIGIN      = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::PixelFormat format = ::fwData::Image::GRAY_SCALE;

    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE, format);

    const auto dumpLock = image->lock();

    ::fwDataTools::ImageDiff diff(image->getType().sizeOf()*64);

    std::uint8_t NEWVALUE = 1;

    ::fwData::Image::BufferType* newBufferValue = reinterpret_cast< ::fwData::Image::BufferType*>(&NEWVALUE);

    const std::vector< ::fwData::Image::IndexType> indices = {{ 51, 10, 8, 123, 1098, 23456, 6, 9999 }};

    // Add 8 elements to the diff. Write new values to the image.
    for(size_t i = 0; i < 8; ++i)
    {
        const ::fwData::Image::IndexType index = indices[i];

        const ::fwData::Image::BufferType* pixBuf =
            reinterpret_cast< ::fwData::Image::BufferType* >(image->getPixelBuffer(index));

        diff.addDiff(index, pixBuf, newBufferValue);
        image->setPixelBuffer(index, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), diff.getNumberOfElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Create an imageDiffCommand to test
    ::fwCommand::ImageDiffCommand imageDiffCommand( image, diff);

    // Ensure that the real size is at least bigger than the naive sizeof
    CPPUNIT_ASSERT( imageDiffCommand.getSize() > sizeof(imageDiffCommand) );

    // Ensure that the real size is at least bigger than the size of the diff
    CPPUNIT_ASSERT( imageDiffCommand.getSize() > diff.getSize() );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace registrationOp
