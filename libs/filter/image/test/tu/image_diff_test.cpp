/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "image_diff_test.hpp"

#include <core/type.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>

#include <filter/image/image.hpp>
#include <filter/image/image_diff.hpp>

#include <utest_data/generator/image.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::image_diff_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

template<typename T>
static void testDiffEquality(const image_diff& diff1, const image_diff& diff2)
{
    CPPUNIT_ASSERT_EQUAL(diff1.numElements(), diff2.numElements());

    for(std::size_t i = 0 ; i < diff1.numElements() ; ++i)
    {
        image_diff::ElementType diff1Elt = diff1.getElement(i);
        image_diff::ElementType diff2Elt = diff2.getElement(i);

        CPPUNIT_ASSERT_EQUAL(diff1Elt.m_index, diff2Elt.m_index);
        CPPUNIT_ASSERT_EQUAL(
            *reinterpret_cast<const T*>(diff1Elt.m_oldValue),
            *reinterpret_cast<const T*>(diff2Elt.m_oldValue)
        );
        CPPUNIT_ASSERT_EQUAL(
            *reinterpret_cast<const T*>(diff1Elt.m_newValue),
            *reinterpret_cast<const T*>(diff2Elt.m_newValue)
        );
    }
}

//------------------------------------------------------------------------------

void image_diff_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_diff_test::tearDown()
{
}

//------------------------------------------------------------------------------

void image_diff_test::storeDiffsTest()
{
    const std::uint8_t OLDVALUE = 0;
    const std::uint8_t NEWVALUE = 1;

    image_diff diff(sizeof(OLDVALUE));

    const auto* oldBufferValue = reinterpret_cast<const data::image::BufferType*>(&OLDVALUE);
    const auto* newBufferValue = reinterpret_cast<const data::image::BufferType*>(&NEWVALUE);

    const std::vector<data::image::IndexType> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::IndexType index = indices[i];

        diff.addDiff(index, oldBufferValue, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(std::size_t(8), diff.numElements());

    // Ensure the elements where inserted.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        image_diff::ElementType elt = diff.getElement(i);

        CPPUNIT_ASSERT_EQUAL(indices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(OLDVALUE, *reinterpret_cast<const std::uint8_t*>(elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<const std::uint8_t*>(elt.m_newValue));
    }

    // Create a second diff with 3 elements.
    const std::vector<data::image::IndexType> indices2 = {{66, 42, 8888}};
    image_diff diff2(sizeof(OLDVALUE));

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        const data::image::IndexType index = indices2[i];

        diff2.addDiff(index, oldBufferValue, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff2.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff2.getElementDiffIndex(i));
    }

    // Merge both diffs. Ensure the total size is 11.
    diff.addDiff(diff2);
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), diff.numElements());

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), diff.numElements());

    std::vector<data::image::IndexType> mergedIndices;
    mergedIndices.insert(mergedIndices.end(), indices.begin(), indices.end());
    mergedIndices.insert(mergedIndices.end(), indices2.begin(), indices2.end());

    // Ensure the elements where merged.
    for(std::size_t i = 0 ; i < 11 ; ++i)
    {
        image_diff::ElementType elt = diff.getElement(i);

        CPPUNIT_ASSERT_EQUAL(mergedIndices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(OLDVALUE, *reinterpret_cast<const std::uint8_t*>(elt.m_oldValue));
        CPPUNIT_ASSERT_EQUAL(NEWVALUE, *reinterpret_cast<const std::uint8_t*>(elt.m_newValue));
    }

    // Copy constructor test.
    image_diff copyDiff(diff);
    testDiffEquality<std::uint8_t>(diff, copyDiff);

    // Copy assignement test.
    image_diff copyDiff2 = diff;
    testDiffEquality<std::uint8_t>(diff, copyDiff2);

    // Move constructor test.
    const image_diff moveDiff(std::move(copyDiff));
    testDiffEquality<std::uint8_t>(diff, moveDiff);

    // Move assignement test.
    image_diff moveDiff2 = std::move(copyDiff2);
    testDiffEquality<std::uint8_t>(diff, moveDiff2);

    // Clear the diff, test if it's really empty.
    diff.clear();

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), diff.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), diff.numElements());
}

//------------------------------------------------------------------------------

void image_diff_test::undoRedoTest()
{
    const data::image::Size SIZE          = {{32, 32, 32}};
    const data::image::Spacing SPACING    = {{1., 1., 1.}};
    const data::image::Origin ORIGIN      = {{0., 0., 0.}};
    const core::type TYPE                 = core::type::UINT8;
    const data::image::PixelFormat FORMAT = data::image::PixelFormat::GRAY_SCALE;

    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(image, SIZE, SPACING, ORIGIN, TYPE, FORMAT);

    const auto dumpLock = image->dump_lock();

    image_diff diff(image->getType().size());

    std::uint8_t NEWVALUE = 1;

    auto* newBufferValue = reinterpret_cast<data::image::BufferType*>(&NEWVALUE);

    const std::vector<data::image::IndexType> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff. Write new values to the image.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::IndexType index = indices[i];

        const data::image::BufferType* pixBuf =
            reinterpret_cast<data::image::BufferType*>(image->getPixel(index));

        diff.addDiff(index, pixBuf, newBufferValue);
        image->setPixel(index, newBufferValue);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff.numElements());
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

} // namespace sight::filter::image::ut
