/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageDiffTest.hpp"

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/Image.hpp>
#include <fwDataTools/ImageDiff.hpp>

#include <fwData/Image.hpp>

#include <fwTest/generator/Image.hpp>

#include <fwTools/Type.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::ImageDiffTest );

namespace fwDataTools
{
namespace ut
{

//------------------------------------------------------------------------------

template<typename T>
static void testDiffEquality(const ImageDiff& diff1, const ImageDiff& diff2)
{
    CPPUNIT_ASSERT_EQUAL(diff1.getNumberOfElements(), diff2.getNumberOfElements());

    for(int i = 0; i < diff1.getNumberOfElements(); ++i)
    {
        ::fwDataTools::ImageDiff::ElementType diff1Elt = diff1.getElement(i);
        ::fwDataTools::ImageDiff::ElementType diff2Elt = diff2.getElement(i);

        CPPUNIT_ASSERT_EQUAL(diff1Elt.m_index, diff2Elt.m_index);
        CPPUNIT_ASSERT_EQUAL(*reinterpret_cast< T* >(diff1Elt.m_oldValue),
                             *reinterpret_cast< T* >(diff2Elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(*reinterpret_cast< T* >(diff1Elt.m_newValue),
                             *reinterpret_cast< T* >(diff2Elt.m_newValue));
    }
}

//------------------------------------------------------------------------------

void ImageDiffTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageDiffTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageDiffTest::storeDiffsTest()
{
    const std::uint8_t OLDVALUE = 0;
    const std::uint8_t NEWVALUE = 1;

    ::fwDataTools::ImageDiff diff(sizeof(OLDVALUE));

    const ::fwData::Image::BufferType* oldBufferValue = reinterpret_cast<const ::fwData::Image::BufferType*>(&OLDVALUE);
    const ::fwData::Image::BufferType* newBufferValue = reinterpret_cast<const ::fwData::Image::BufferType*>(&NEWVALUE);

    const std::vector< ::fwData::Image::BufferIndexType > indices = {{ 51, 10, 8, 123, 1098, 23456, 6, 9999 }};

    // Add 8 elements to the diff.
    for(int i = 0; i < 8; ++i)
    {
        const ::fwData::Image::BufferIndexType index = indices[i];

        diff.addDiff(index, oldBufferValue, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), diff.getNumberOfElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(size_t(8), diff.getNumberOfElements());

    // Ensure the elements where inserted.
    for(int i = 0; i < 8; ++i)
    {
        ::fwDataTools::ImageDiff::ElementType elt = diff.getElement(i);

        CPPUNIT_ASSERT_EQUAL(indices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(OLDVALUE, *reinterpret_cast< std::uint8_t* >(elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast< std::uint8_t* >(elt.m_newValue));
    }

    // Create a second diff with 3 elements.
    const std::vector< ::fwData::Image::BufferIndexType> indices2 = {{ 66, 42, 8888 }};
    ::fwDataTools::ImageDiff diff2(sizeof(OLDVALUE));

    for(int i = 0; i < 3; ++i)
    {
        const ::fwData::Image::BufferIndexType index = indices2[i];

        diff2.addDiff(index, oldBufferValue, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), diff2.getNumberOfElements());
        CPPUNIT_ASSERT_EQUAL(index, diff2.getElementDiffIndex(i));
    }

    // Merge both diffs. Ensure the total size is 11.
    diff.addDiff(diff2);
    CPPUNIT_ASSERT_EQUAL(size_t(11), diff.getNumberOfElements());

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(size_t(11), diff.getNumberOfElements());

    std::vector< ::fwData::Image::BufferIndexType> mergedIndices;
    mergedIndices.insert(mergedIndices.end(), indices.begin(), indices.end());
    mergedIndices.insert(mergedIndices.end(), indices2.begin(), indices2.end());

    // Ensure the elements where merged.
    for(int i = 0; i < 11; ++i)
    {
        ::fwDataTools::ImageDiff::ElementType elt = diff.getElement(i);

        CPPUNIT_ASSERT_EQUAL(mergedIndices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(OLDVALUE, *reinterpret_cast< std::uint8_t* >(elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast< std::uint8_t* >(elt.m_newValue));
    }

    // Copy constructor test.
    const ImageDiff copyDiff(diff);
    testDiffEquality<std::uint8_t>(diff, copyDiff);

    // Copy assignement test.
    ImageDiff copyDiff2 = diff;
    testDiffEquality<std::uint8_t>(diff, copyDiff2);

    // Move constructor test.
    const ImageDiff moveDiff(std::move(copyDiff));
    testDiffEquality<std::uint8_t>(diff, moveDiff);

    // Move assignement test.
    ImageDiff moveDiff2 = std::move(copyDiff2);
    testDiffEquality<std::uint8_t>(diff, moveDiff2);

    // Clear the diff, test if it's really empty.
    diff.clear();

    CPPUNIT_ASSERT_EQUAL(size_t(0), diff.getSize());
    CPPUNIT_ASSERT_EQUAL(size_t(0), diff.getNumberOfElements());
}

//------------------------------------------------------------------------------

void ImageDiffTest::undoRedoTest()
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

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    diff.revertDiff(image);

    for(size_t it = 0; it < image->getSizeInBytes(); ++it)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(helper.getPixelBuffer(it)));
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    diff.applyDiff(image);

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

} //namespace ut
} //namespace fwDataTools
