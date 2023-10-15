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

#include "WriterTest.hpp"

#include <core/os/temp_path.hpp>

#include <data/image_series.hpp>

#include <io/bitmap/backend.hpp>
#include <io/dicom/Reader.hpp>
#include <io/dicom/Writer.hpp>

#include <utest/Filter.hpp>
#include <utest/profiling.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>

#include <chrono>
#include <ctime>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::WriterTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

/// @note this function is only needed because std::format (c++20) is not supported by clang 14 / gcc 12
/// It is bold C API to format a date / time to something like "YYYYMMDDHHMMSS.FFFFFF" (DICOM DateTime format)
inline static std::string formatDateTime(const std::chrono::time_point<std::chrono::system_clock>& now)
{
    time_t now_time = std::chrono::system_clock::to_time_t(now);

    struct tm timeinfo {};

#ifdef WIN32
    localtime_s(&timeinfo, &now_time);
#else
    localtime_r(&now_time, &timeinfo);
#endif

    const auto truncated_now = std::chrono::system_clock::from_time_t(now_time);
    const auto ms            = (now - truncated_now).count();

    std::string buffer("YYYYMMDDHHMMSS.");
    const auto size = strftime(buffer.data(), buffer.size(), "%Y%m%d%H%M%S", &timeinfo);
    buffer.resize(size);

    return buffer + "." + std::to_string(ms).substr(0, 6);
}

//------------------------------------------------------------------------------

inline static data::image_series::sptr getUSVolumeImage(
    std::uint32_t seed              = 0,
    std::size_t num_frames          = 1,
    core::type type                 = core::type::UINT8,
    data::image::PixelFormat format = data::image::RGB
)
{
    using key_t = std::tuple<std::size_t, core::type, data::image::PixelFormat, std::uint32_t>;
    static std::map<key_t, data::image_series::sptr> generated;

    const key_t key {num_frames, type, format, seed};

    const auto& it = generated.find(key);

    if(it == generated.end())
    {
        auto image = std::make_shared<data::image_series>();

        utest_data::generator::image::generateImage(
            image,
            {64, 64, num_frames},
            {1.0, 1.0, 1.0},
            {0, 0, 0},
            type,
            format,
            seed
        );

        if(seed == 0xFFFF)
        {
            // De-randomize a bit the image to allow compression, otherwise we cannot check the effectiveness.
            const auto dump_lock = image->dump_lock();

            const auto& sizes     = image->size();
            std::size_t index     = 0;
            std::size_t max_index = (sizes[0] * sizes[1] * sizes[2] * image->numComponents() / 10);
            auto image_it         = image->begin<std::uint8_t>();
            const auto& end       = image->end<std::uint8_t>();

            for( ; image_it != end ; ++image_it)
            {
                std::uint8_t value = 0;

                if(index++ < max_index)
                {
                    // This is for RLE
                    value = 0xA0;
                }
                else
                {
                    value = (*image_it % 0x20) + 0xA0;
                }

                *image_it = value;
            }
        }

        // We want an Enhanced US Volume
        image->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // Set Image Position Patient / Image Orientation Patient
        for(std::size_t frame_index = 0 ; frame_index < num_frames ; ++frame_index)
        {
            // ..Image Position / Orientation Patient is what we want
            image->setImagePositionPatient(
                {
                    double(seed + 1) * 0.1,
                    double(seed + 1) * 0.2,
                    double(seed + 1) * 0.3
                },
                frame_index
            );

            image->setImageOrientationPatient(
                {
                    double(seed + 1) * 0.4,
                    double(seed + 1) * 0.5,
                    double(seed + 1) * 0.6,
                    double(seed + 1) * 0.7,
                    double(seed + 1) * 0.8,
                    double(seed + 1) * 0.9,
                },
                frame_index
            );

            // set the Frame Acquisition Date Time, which is our "timestamp"
            auto now = std::chrono::system_clock::now();
            now += std::chrono::milliseconds(frame_index);

            //YYYYMMDDHHMMSS.FFFFFF
            image->setFrameAcquisitionDateTime(formatDateTime(now), frame_index);

            // Add a private custom attribute
            image->setMultiFramePrivateValue(
                formatDateTime(now),
                0x42,
                frame_index
            );
        }

        generated[key] = image;

        return image;
    }

    return it->second;
}

//------------------------------------------------------------------------------

inline static void compareEnhancedUSVolume(
    const data::image_series::sptr& expected,
    const data::image_series::sptr& actual
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

        // FrameAcquisitionDateTime
        CPPUNIT_ASSERT_EQUAL(
            *expected->getFrameAcquisitionDateTime(frame_index),
            *actual->getFrameAcquisitionDateTime(frame_index)
        );

        // Private custom attribute
        CPPUNIT_ASSERT_EQUAL(
            *expected->getMultiFramePrivateValue(0x42, frame_index),
            *actual->getMultiFramePrivateValue(0x42, frame_index)
        );
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

void WriterTest::setUp()
{
    // Set up context before running a test.
    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
}

//------------------------------------------------------------------------------

void WriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void WriterTest::writeEnhancedUSVolumeTest()
{
    {
        core::os::temp_dir tmpDir;
        const auto& expected = getUSVolumeImage(0);

        // Write a single RGB uint8 frame image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            seriesSet->push_back(expected);

            auto writer = std::make_shared<io::dicom::Writer>();
            writer->set_object(seriesSet);
            writer->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written single frame image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            auto reader    = std::make_shared<io::dicom::Reader>();
            reader->set_object(seriesSet);
            reader->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, std::dynamic_pointer_cast<data::image_series>(seriesSet->front()));
        }
    }

    {
        core::os::temp_dir tmpDir;
        const auto& expected = getUSVolumeImage(1, 4);

        // Write a 4 frames RGB uint8 image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            seriesSet->push_back(expected);

            auto writer = std::make_shared<io::dicom::Writer>();
            writer->set_object(seriesSet);
            writer->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            auto reader    = std::make_shared<io::dicom::Reader>();
            reader->set_object(seriesSet);
            reader->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, std::dynamic_pointer_cast<data::image_series>(seriesSet->front()));
        }
    }

    {
        core::os::temp_dir tmpDir;
        const auto& expected = getUSVolumeImage(2, 4, core::type::UINT16, data::image::PixelFormat::GRAY_SCALE);

        // Write a 4 frames monochrome uint16 image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            seriesSet->push_back(expected);

            auto writer = std::make_shared<io::dicom::Writer>();
            writer->set_object(seriesSet);
            writer->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            auto reader    = std::make_shared<io::dicom::Reader>();
            reader->set_object(seriesSet);
            reader->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, std::dynamic_pointer_cast<data::image_series>(seriesSet->front()));
        }
    }

    {
        core::os::temp_dir tmpDir;
        const auto& expected = getUSVolumeImage(2, 4);

        // Write a 4 frames RGB uint8 image, with a custom filename
        {
            auto seriesSet = std::make_shared<data::series_set>();
            seriesSet->push_back(expected);

            auto writer = std::make_shared<io::dicom::Writer>();
            writer->set_object(seriesSet);
            writer->set_folder(tmpDir);
            writer->set_file("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / "custom_filename.dcm"));
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            auto reader    = std::make_shared<io::dicom::Reader>();
            reader->set_object(seriesSet);
            reader->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, std::dynamic_pointer_cast<data::image_series>(seriesSet->front()));
        }
    }

    {
        core::os::temp_dir tmpDir;
        const auto& expected0 = getUSVolumeImage(0, 4);
        const auto& expected1 = getUSVolumeImage(1, 4);
        const auto& expected2 = getUSVolumeImage(2, 4);

        // Write a three 4 frames RGB uint8 image, with a custom filename
        {
            auto seriesSet = std::make_shared<data::series_set>();
            seriesSet->push_back(expected0);
            seriesSet->push_back(expected1);
            seriesSet->push_back(expected2);

            auto writer = std::make_shared<io::dicom::Writer>();
            writer->set_object(seriesSet);
            writer->set_folder(tmpDir);
            writer->set_file("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / "000-custom_filename.dcm"));
            CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / "001-custom_filename.dcm"));
            CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / "002-custom_filename.dcm"));
        }
    }

    // test resized ImageSeries
    {
        core::os::temp_dir tmpDir;
        const auto& expected = getUSVolumeImage(2, 4);

        // Write a 4 frames RGB uint8 image, with a custom filename
        {
            auto seriesSet = std::make_shared<data::series_set>();

            auto resized = std::make_shared<data::image_series>();
            resized->deep_copy(expected);

            // Resize and set a value at the end...
            auto new_size = expected->size();
            new_size[2] += 1;
            resized->resize(new_size, expected->getType(), expected->getPixelFormat());
            resized->setFrameAcquisitionDateTime(formatDateTime(std::chrono::system_clock::now()), new_size[2] - 1);

            // Resize back to original size
            resized->resize(expected->size(), expected->getType(), expected->getPixelFormat());
            seriesSet->push_back(resized);

            auto writer = std::make_shared<io::dicom::Writer>();
            writer->set_object(seriesSet);
            writer->set_folder(tmpDir);
            writer->set_file("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / "custom_filename.dcm"));
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = std::make_shared<data::series_set>();
            auto reader    = std::make_shared<io::dicom::Reader>();
            reader->set_object(seriesSet);
            reader->set_folder(tmpDir);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, std::dynamic_pointer_cast<data::image_series>(seriesSet->front()));
        }
    }
}

//------------------------------------------------------------------------------

void WriterTest::forceCPUTest()
{
    core::os::temp_dir tmpDir;
    const auto& expected = getUSVolumeImage(0);

    // Write a single RGB uint8 frame image
    {
        auto seriesSet = std::make_shared<data::series_set>();
        seriesSet->push_back(expected);

        auto writer = std::make_shared<io::dicom::Writer>();
        writer->set_object(seriesSet);
        writer->set_folder(tmpDir);

        writer->forceCPU(false);

        if(io::bitmap::nvJPEG2K())
        {
            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

#ifdef SIGHT_ENABLE_NVJPEG2K
        else
        {
            CPPUNIT_ASSERT_THROW(writer->write(), core::exception);
        }
#endif

        writer->forceCPU(true);

        CPPUNIT_ASSERT_NO_THROW(writer->write());
    }
}

//------------------------------------------------------------------------------

void WriterTest::transferSyntaxTest()
{
    const auto& test =
        [](io::dicom::Writer::TransferSyntax transferSyntax)
        {
            core::os::temp_dir tmpDir;
            const auto& expected = getUSVolumeImage(0xFFFF, 6);

            // Write a 4 frames RGB uint8 image
            {
                auto seriesSet = std::make_shared<data::series_set>();
                seriesSet->push_back(expected);

                auto writer = std::make_shared<io::dicom::Writer>();
                writer->set_object(seriesSet);
                writer->set_folder(tmpDir);
                writer->setTransferSyntax(transferSyntax);

                SIGHT_PROFILE_FUNC(
                    [&](std::size_t)
                {
                    CPPUNIT_ASSERT_NO_THROW(writer->write());
                },
                    3,
                    "Write (" + std::string(io::dicom::Writer::transferSyntaxToString(transferSyntax)) + "): ",
                    0.1
                );
            }

            // Read the previously written 4 frames image
            {
                auto seriesSet = std::make_shared<data::series_set>();
                auto reader    = std::make_shared<io::dicom::Reader>();
                reader->set_object(seriesSet);
                reader->set_folder(tmpDir);

                SIGHT_PROFILE_FUNC(
                    [&](std::size_t)
                {
                    CPPUNIT_ASSERT_NO_THROW(reader->read());
                },
                    3,
                    "Read (" + std::string(io::dicom::Writer::transferSyntaxToString(transferSyntax)) + "): ",
                    0.1
                );

                CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

                compareEnhancedUSVolume(expected, std::dynamic_pointer_cast<data::image_series>(seriesSet->front()));
            }

            for(auto const& entry : std::filesystem::directory_iterator {tmpDir})
            {
                if(entry.is_regular_file())
                {
                    const auto size = entry.file_size();

                    SIGHT_INFO(
                        "File size (" << io::dicom::Writer::transferSyntaxToString(transferSyntax) << "): " << size
                    );

                    return size;
                }
            }

            return std::size_t(0);
        };

    // First the biggest file size
    const auto raw_size = test(io::dicom::Writer::TransferSyntax::RAW);

    for(const auto& transferSyntax : {
            io::dicom::Writer::TransferSyntax::RLE,
            io::dicom::Writer::TransferSyntax::JPEG_LOSSLESS,
            io::dicom::Writer::TransferSyntax::JPEG_LS_LOSSLESS,
            io::dicom::Writer::TransferSyntax::JPEG2000_LOSSLESS
        })
    {
        // Then others (which should be smaller)
        const auto size = test(transferSyntax);
        CPPUNIT_ASSERT_MESSAGE(
            "RAW (" + std::to_string(raw_size) + ") < "
            + std::string(io::dicom::Writer::transferSyntaxToString(transferSyntax))
            + " (" + std::to_string(size) + ")",
            raw_size >= size
        );
    }
}

} // namespace sight::io::dicom::ut
