/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

// cspell:ignore Acuson

#include "ReaderWriterTest.hpp"

#include <core/tools/system.hpp>
#include <core/tools/uuid.hpp>

#include <data/ImageSeries.hpp>

#include <io/dicom/Reader.hpp>
#include <io/dicom/Writer.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <TestAssert.h>

#include <chrono>
#include <ctime>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::ReaderWriterTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

inline static sight::data::SeriesSet::sptr read(const std::filesystem::path path)
{
    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    auto seriesSet = std::make_shared<data::SeriesSet>();

    auto reader = std::make_shared<io::dicom::Reader>();
    reader->setObject(seriesSet);
    reader->set_folder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    for(const auto& series : *seriesSet)
    {
        series->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    }

    return seriesSet;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path createTempFolder()
{
    auto tmp_folder = core::tools::system::get_temporary_folder() / core::tools::UUID::generate();
    std::filesystem::remove_all(tmp_folder);
    std::filesystem::create_directories(tmp_folder);

    return tmp_folder;
}

//------------------------------------------------------------------------------

inline static void compareEnhancedUSVolume(
    const data::ImageSeries::sptr& expected,
    const data::ImageSeries::sptr& actual
)
{
    CPPUNIT_ASSERT(expected);
    CPPUNIT_ASSERT(actual);

    // SOP Class UID
    CPPUNIT_ASSERT_EQUAL(expected->getSOPKeyword(), actual->getSOPKeyword());

    // Sizes
    const auto& expected_sizes = expected->size();
    const auto& actual_sizes   = actual->size();
    CPPUNIT_ASSERT_EQUAL(expected_sizes.size(), actual_sizes.size());

    for(std::size_t i = 0 ; i < expected_sizes.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expected_sizes[i], actual_sizes[i]);
    }

    // Type
    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());

    // Pixel format
    CPPUNIT_ASSERT_EQUAL(expected->getPixelFormat(), actual->getPixelFormat());

    // Spacings
    const auto& expected_spacing = expected->getSpacing();
    const auto& actual_spacing   = actual->getSpacing();
    CPPUNIT_ASSERT_EQUAL(expected_spacing.size(), actual_spacing.size());

    for(std::size_t i = 0 ; i < expected_spacing.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_spacing[i], actual_spacing[i], 0.0001);
    }

    // Origins
    const auto& expected_origin = expected->getOrigin();
    const auto& actual_origin   = actual->getOrigin();
    CPPUNIT_ASSERT_EQUAL(expected_origin.size(), actual_origin.size());

    for(std::size_t i = 0 ; i < expected_origin.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_origin[i], actual_origin[i], 0.0001);
    }

    // Get Image Position Patient / Image Orientation Patient
    for(std::size_t frame_index = 0 ; frame_index < actual_sizes[2] ; ++frame_index)
    {
        // Image Position Patient
        const auto& expected_position = expected->getImagePositionPatient(frame_index);
        const auto& actual_position   = actual->getImagePositionPatient(frame_index);
        CPPUNIT_ASSERT_EQUAL(expected_position.size(), actual_position.size());

        for(std::size_t i = 0 ; i < expected_position.size() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_position[i], actual_position[i], 0.0001);
        }

        // Image Orientation Patient
        const auto& expected_orientation = expected->getImageOrientationPatient(frame_index);
        const auto& actual_orientation   = actual->getImageOrientationPatient(frame_index);
        CPPUNIT_ASSERT_EQUAL(expected_orientation.size(), actual_orientation.size());

        for(std::size_t i = 0 ; i < expected_orientation.size() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_orientation[i], actual_orientation[i], 0.0001);
        }
    }

    // Ensure that getting value outside the frame range returns std::nullopts
    CPPUNIT_ASSERT(
        !expected->getFrameAcquisitionDateTime(actual_sizes[2])
        && !actual->getFrameAcquisitionDateTime(actual_sizes[2])
    );

    // Compare buffer
    const auto expected_locked = expected->dump_lock();
    const auto actual_locked   = actual->dump_lock();
    CPPUNIT_ASSERT_EQUAL(0, std::memcmp(expected->buffer(), actual->buffer(), expected->getSizeInBytes()));
}

//------------------------------------------------------------------------------

inline static void compareEnhancedUSVolume(const data::SeriesSet::sptr& expected, const data::SeriesSet::sptr& actual)
{
    CPPUNIT_ASSERT_EQUAL(expected->size(), actual->size());
    for(std::size_t i = 0 ; i < expected->size() ; i++)
    {
        const auto& expectedImageSeries = std::dynamic_pointer_cast<data::ImageSeries>((*expected)[i]);
        const auto& actualImageSeries   = std::dynamic_pointer_cast<data::ImageSeries>((*actual)[i]);
        compareEnhancedUSVolume(expectedImageSeries, actualImageSeries);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::setUp()
{
    // Set up context before running a test.
    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
}

//------------------------------------------------------------------------------

static void testImage(const std::string& name)
{
    const auto& folder   = createTempFolder();
    const auto& expected = read(utestData::Data::dir() / name);

    auto writer = std::make_shared<io::dicom::Writer>();
    writer->setObject(expected);
    writer->set_folder(folder);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    auto actual = std::make_shared<data::SeriesSet>();
    auto reader = std::make_shared<io::dicom::Reader>();
    reader->setObject(actual);
    reader->set_folder(folder);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    compareEnhancedUSVolume(expected, actual);
}

//------------------------------------------------------------------------------

void ReaderWriterTest::basicTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testImage("us/Enhanced US Volume Storage/GE, 3D+t, lossy JPEG");
    testImage("us/Ultrasound Image Storage/Philips, RLE, palette color");
    testImage("us/Ultrasound Multi-frame Image Storage/Acuson, 2D+t, lossy JPEG");
    testImage("us/Ultrasound Multi-frame Image Storage/GE, 2D+t, RLE");
    testImage("us/Ultrasound Multi-frame Image Storage/Philips, 2D+t,  lossy JPEG");
}

} // namespace sight::io::dicom::ut
