/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    const ::fwData::Image::SizeType SIZE       = {{ 32, 32, 32 }};
    const ::fwData::Image::SpacingType SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;

    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE);

    ::fwDataTools::helper::Image helper(image);

    ::fwDataTools::ImageDiff diff(image->getPixelType().sizeOf());

    std::uint8_t NEWVALUE = 1;

    ::fwData::Image::BufferType* newBufferValue = reinterpret_cast< ::fwData::Image::BufferType*>(&NEWVALUE);

    const std::vector< ::fwData::Image::BufferIndexType> indices = {{ 51, 10, 8, 123, 1098, 23456, 6, 9999 }};

    // Add 8 elements to the diff. Write new values to the image.
    for(int i = 0; i < 8; ++i)
    {
        const ::fwData::Image::BufferIndexType index = indices[i];

        const ::fwData::Image::BufferType* pixBuf =
            reinterpret_cast< ::fwData::Image::BufferType* >(helper.getPixelBuffer(index));

        diff.addDiff(index, pixBuf, newBufferValue);
        helper.setPixelBuffer(index, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), diff.getNumberOfElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Create an imageDiffCommand to test
    ::fwCommand::ImageDiffCommand imageDiffCommand( image, diff);

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    CPPUNIT_ASSERT(imageDiffCommand.undo());

    for(size_t it = 0; it < image->getSizeInBytes(); ++it)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(helper.getPixelBuffer(it)));
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    CPPUNIT_ASSERT(imageDiffCommand.redo());

    for(size_t i = 0; i < image->getSizeInBytes(); ++i)
    {
        // Check if 'i' is an index
        auto indexIt = std::find(indices.begin(), indices.end(), i);

        if(indexIt != indices.end())
        {
            CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<std::uint8_t*>(helper.getPixelBuffer(i)));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(helper.getPixelBuffer(i)));
        }
    }
}

//------------------------------------------------------------------------------

void ImageDiffCommandTest::getSizeTest()
{
    const ::fwData::Image::SizeType SIZE       = {{ 32, 32, 32 }};
    const ::fwData::Image::SpacingType SPACING = {{ 1., 1., 1. }};
    const ::fwData::Image::OriginType ORIGIN   = {{ 0., 0., 0. }};
    const ::fwTools::Type TYPE                 = ::fwTools::Type::s_UINT8;

    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwTest::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE);

    ::fwDataTools::helper::Image helper(image);

    ::fwDataTools::ImageDiff diff(image->getPixelType().sizeOf()*64);

    std::uint8_t NEWVALUE = 1;

    ::fwData::Image::BufferType* newBufferValue = reinterpret_cast< ::fwData::Image::BufferType*>(&NEWVALUE);

    const std::vector< ::fwData::Image::BufferIndexType> indices = {{ 51, 10, 8, 123, 1098, 23456, 6, 9999 }};

    // Add 8 elements to the diff. Write new values to the image.
    for(size_t i = 0; i < 8; ++i)
    {
        const ::fwData::Image::BufferIndexType index = indices[i];

        const ::fwData::Image::BufferType* pixBuf =
            reinterpret_cast< ::fwData::Image::BufferType* >(helper.getPixelBuffer(index));

        diff.addDiff(index, pixBuf, newBufferValue);
        helper.setPixelBuffer(index, newBufferValue);

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
