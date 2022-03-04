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

#include "ImageDiffTest.hpp"

#include <core/tools/Type.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <filter/image/Image.hpp>
#include <filter/image/ImageDiff.hpp>

#include <utestData/generator/Image.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::ImageDiffTest);

namespace sight::filter::image
{

namespace ut
{

//------------------------------------------------------------------------------

template<typename T>
static void testDiffEquality(const ImageDiff& diff1, const ImageDiff& diff2)
{
    CPPUNIT_ASSERT_EQUAL(diff1.numElements(), diff2.numElements());

    for(std::size_t i = 0 ; i < diff1.numElements() ; ++i)
    {
        ImageDiff::ElementType diff1Elt = diff1.getElement(i);
        ImageDiff::ElementType diff2Elt = diff2.getElement(i);

        CPPUNIT_ASSERT_EQUAL(diff1Elt.m_index, diff2Elt.m_index);
        CPPUNIT_ASSERT_EQUAL(
            *reinterpret_cast<T*>(diff1Elt.m_oldValue),
            *reinterpret_cast<T*>(diff2Elt.m_oldValue)
        );
        CPPUNIT_ASSERT_EQUAL(
            *reinterpret_cast<T*>(diff1Elt.m_newValue),
            *reinterpret_cast<T*>(diff2Elt.m_newValue)
        );
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

    ImageDiff diff(sizeof(OLDVALUE));

    const data::Image::BufferType* oldBufferValue = reinterpret_cast<const data::Image::BufferType*>(&OLDVALUE);
    const data::Image::BufferType* newBufferValue = reinterpret_cast<const data::Image::BufferType*>(&NEWVALUE);

    const std::vector<data::Image::IndexType> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff.
    for(int i = 0 ; i < 8 ; ++i)
    {
        const data::Image::IndexType index = indices[i];

        diff.addDiff(index, oldBufferValue, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), diff.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(std::size_t(8), diff.numElements());

    // Ensure the elements where inserted.
    for(int i = 0 ; i < 8 ; ++i)
    {
        ImageDiff::ElementType elt = diff.getElement(i);

        CPPUNIT_ASSERT_EQUAL(indices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(OLDVALUE, *reinterpret_cast<std::uint8_t*>(elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<std::uint8_t*>(elt.m_newValue));
    }

    // Create a second diff with 3 elements.
    const std::vector<data::Image::IndexType> indices2 = {{66, 42, 8888}};
    ImageDiff diff2(sizeof(OLDVALUE));

    for(int i = 0 ; i < 3 ; ++i)
    {
        const data::Image::IndexType index = indices2[i];

        diff2.addDiff(index, oldBufferValue, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), diff2.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff2.getElementDiffIndex(i));
    }

    // Merge both diffs. Ensure the total size is 11.
    diff.addDiff(diff2);
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), diff.numElements());

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), diff.numElements());

    std::vector<data::Image::IndexType> mergedIndices;
    mergedIndices.insert(mergedIndices.end(), indices.begin(), indices.end());
    mergedIndices.insert(mergedIndices.end(), indices2.begin(), indices2.end());

    // Ensure the elements where merged.
    for(int i = 0 ; i < 11 ; ++i)
    {
        ImageDiff::ElementType elt = diff.getElement(i);

        CPPUNIT_ASSERT_EQUAL(mergedIndices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(OLDVALUE, *reinterpret_cast<std::uint8_t*>(elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<std::uint8_t*>(elt.m_newValue));
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

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), diff.getSize());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), diff.numElements());
}

//------------------------------------------------------------------------------

void ImageDiffTest::undoRedoTest()
{
    const data::Image::Size SIZE          = {{32, 32, 32}};
    const data::Image::Spacing SPACING    = {{1., 1., 1.}};
    const data::Image::Origin ORIGIN      = {{0., 0., 0.}};
    const core::tools::Type TYPE          = core::tools::Type::s_UINT8;
    const data::Image::PixelFormat FORMAT = data::Image::PixelFormat::GRAY_SCALE;

    data::Image::sptr image = data::Image::New();

    utestData::generator::Image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE, FORMAT);

    const auto dumpLock = image->dump_lock();

    ImageDiff diff(image->getType().sizeOf());

    std::uint8_t NEWVALUE = 1;

    data::Image::BufferType* newBufferValue = reinterpret_cast<data::Image::BufferType*>(&NEWVALUE);

    const std::vector<data::Image::IndexType> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff. Write new values to the image.
    for(int i = 0 ; i < 8 ; ++i)
    {
        const data::Image::IndexType index = indices[i];

        const data::Image::BufferType* pixBuf =
            reinterpret_cast<data::Image::BufferType*>(image->getPixel(index));

        diff.addDiff(index, pixBuf, newBufferValue);
        image->setPixel(index, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), diff.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<std::uint8_t*>(image->getPixel(index)));
    }

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    diff.revertDiff(image);

    for(std::size_t it = 0 ; it < image->getSizeInBytes() ; ++it)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "it: " + std::to_string(it),
            (int) std::uint8_t(0),
            (int) *reinterpret_cast<std::uint8_t*>(image->getPixel(it))
        );
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    diff.applyDiff(image);

    for(std::size_t i = 0 ; i < image->getSizeInBytes() ; ++i)
    {
        // Check if 'i' is an index
        auto indexIt = std::find(indices.begin(), indices.end(), i);

        if(indexIt != indices.end())
        {
            CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<std::uint8_t*>(image->getPixel(i)));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->getPixel(i)));
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::filter::image
