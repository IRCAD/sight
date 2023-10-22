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

#include "ImageDiffCommandTest.hpp"

#include <ui/history/ImageDiffCommand.hpp>

#include <utest_data//generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::history::ut::ImageDiffCommandTest);

namespace sight::ui::history::ut
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
    const data::image::Size size          = {{32, 32, 32}};
    const data::image::Spacing spacing    = {{1., 1., 1.}};
    const data::image::Origin origin      = {{0., 0., 0.}};
    const core::type type                 = core::type::UINT8;
    const data::image::PixelFormat format = data::image::GRAY_SCALE;

    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(image, size, spacing, origin, type, format);

    const auto dump_lock = image->dump_lock();

    filter::image::image_diff diff(image->getType().size());

    std::uint8_t newvalue = 1;

    auto* new_buffer_value = reinterpret_cast<data::image::buffer_t*>(&newvalue);

    const std::vector<data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff. Write new values to the image.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::index_t index = indices[i];

        const data::image::buffer_t* pix_buf =
            reinterpret_cast<data::image::buffer_t*>(image->getPixel(index));

        diff.addDiff(index, pix_buf, new_buffer_value);
        image->setPixel(index, new_buffer_value);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Create an imageDiffCommand to test
    ui::history::ImageDiffCommand image_diff_command(image, diff);

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    CPPUNIT_ASSERT(image_diff_command.undo());

    for(std::size_t it = 0 ; it < image->getSizeInBytes() ; ++it)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->getPixel(it)));
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    CPPUNIT_ASSERT(image_diff_command.redo());

    for(std::size_t i = 0 ; i < image->getSizeInBytes() ; ++i)
    {
        // Check if 'i' is an index
        auto index_it = std::find(indices.begin(), indices.end(), i);

        if(index_it != indices.end())
        {
            CPPUNIT_ASSERT_EQUAL(newvalue, *reinterpret_cast<std::uint8_t*>(image->getPixel(i)));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->getPixel(i)));
        }
    }
}

//------------------------------------------------------------------------------

void ImageDiffCommandTest::getSizeTest()
{
    const data::image::Size size          = {{32, 32, 32}};
    const data::image::Spacing spacing    = {{1., 1., 1.}};
    const data::image::Origin origin      = {{0., 0., 0.}};
    const core::type type                 = core::type::UINT8;
    const data::image::PixelFormat format = data::image::GRAY_SCALE;

    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generateImage(image, size, spacing, origin, type, format);

    const auto dump_lock = image->dump_lock();

    filter::image::image_diff diff(image->getType().size() * 64);

    std::uint8_t newvalue = 1;

    auto* new_buffer_value = reinterpret_cast<data::image::buffer_t*>(&newvalue);

    const std::vector<data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff. Write new values to the image.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::index_t index = indices[i];

        const data::image::buffer_t* pix_buf =
            reinterpret_cast<data::image::buffer_t*>(image->getPixel(index));

        diff.addDiff(index, pix_buf, new_buffer_value);
        image->setPixel(index, new_buffer_value);

        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), diff.numElements());
        CPPUNIT_ASSERT_EQUAL(index, diff.getElementDiffIndex(i));
    }

    // Create an imageDiffCommand to test
    ui::history::ImageDiffCommand image_diff_command(image, diff);

    // Ensure that the real size is at least bigger than the naive sizeof
    CPPUNIT_ASSERT(image_diff_command.size() > sizeof(image_diff_command));

    // Ensure that the real size is at least bigger than the size of the diff
    CPPUNIT_ASSERT(image_diff_command.size() > diff.size());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::history::ut
