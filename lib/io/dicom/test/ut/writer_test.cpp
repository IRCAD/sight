/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "helper.hpp"

#include <core/os/temp_path.hpp>

#include <io/bitmap/backend.hpp>
#include <io/dicom/reader/file.hpp>
#include <io/dicom/writer/file.hpp>

#include <utest/profiling.hpp>

#include <doctest/doctest.h>

// cspell: ignore orthogonalize

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::dicom::writer")
{
    TEST_CASE("write_enhanced_us_volume")
    {
        {
            core::os::temp_dir tmp_dir;
            const auto& expected = get_us_volume_image(0);

            // Write a single RGB uint8 frame image
            {
                auto series_set = std::make_shared<data::series_set>();
                series_set->push_back(expected);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);

                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
            }

            // Read the previously written single frame image
            {
                auto series_set = std::make_shared<data::series_set>();
                auto reader     = std::make_shared<io::dicom::reader::file>();
                reader->set_object(series_set);
                reader->set_folder(tmp_dir);

                auto read_observer = std::make_shared<core::notification::observer>("Test read");
                CHECK_NOTHROW(reader->read(read_observer));
                CHECK_EQ(std::size_t(1), series_set->size());

                compare_enhanced_us_volume(
                    expected,
                    std::dynamic_pointer_cast<data::image_series>(series_set->front()),
                    true
                );
            }
        }

        {
            core::os::temp_dir tmp_dir;
            const auto& expected = get_us_volume_image(1, 4);

            // Write a 4 frames RGB uint8 image
            {
                auto series_set = std::make_shared<data::series_set>();
                series_set->push_back(expected);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);

                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
            }

            // Read the previously written 4 frames image
            {
                auto series_set = std::make_shared<data::series_set>();
                auto reader     = std::make_shared<io::dicom::reader::file>();
                reader->set_object(series_set);
                reader->set_folder(tmp_dir);

                auto read_observer = std::make_shared<core::notification::observer>("Test read");
                CHECK_NOTHROW(reader->read(read_observer));
                CHECK_EQ(std::size_t(1), series_set->size());

                compare_enhanced_us_volume(
                    expected,
                    std::dynamic_pointer_cast<data::image_series>(series_set->front()),
                    true
                );
            }
        }

        {
            core::os::temp_dir tmp_dir;
            const auto& expected = get_us_volume_image(
                2,
                4,
                core::type::UINT16,
                data::image::pixel_format_t::gray_scale
            );

            // Write a 4 frames monochrome uint16 image
            {
                auto series_set = std::make_shared<data::series_set>();
                series_set->push_back(expected);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);

                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
            }

            // Read the previously written 4 frames image
            {
                auto series_set = std::make_shared<data::series_set>();
                auto reader     = std::make_shared<io::dicom::reader::file>();
                reader->set_object(series_set);
                reader->set_folder(tmp_dir);

                auto read_observer = std::make_shared<core::notification::observer>("Test read");
                CHECK_NOTHROW(reader->read(read_observer));
                CHECK_EQ(std::size_t(1), series_set->size());

                compare_enhanced_us_volume(
                    expected,
                    std::dynamic_pointer_cast<data::image_series>(series_set->front()),
                    true
                );
            }
        }

        {
            core::os::temp_dir tmp_dir;
            const auto& expected = get_us_volume_image(2, 4);

            // Write a 4 frames RGB uint8 image, with a custom filename
            {
                auto series_set = std::make_shared<data::series_set>();
                series_set->push_back(expected);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);
                writer->set_file("custom_filename.dcm");

                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
                CHECK(std::filesystem::exists(tmp_dir / "custom_filename.dcm"));
            }

            // Read the previously written 4 frames image
            {
                auto series_set = std::make_shared<data::series_set>();
                auto reader     = std::make_shared<io::dicom::reader::file>();
                reader->set_object(series_set);
                reader->set_folder(tmp_dir);

                auto read_observer = std::make_shared<core::notification::observer>("Test read");
                CHECK_NOTHROW(reader->read(read_observer));
                CHECK_EQ(std::size_t(1), series_set->size());

                compare_enhanced_us_volume(
                    expected,
                    std::dynamic_pointer_cast<data::image_series>(series_set->front()),
                    true
                );
            }
        }

        {
            core::os::temp_dir tmp_dir;
            const auto& expected0 = get_us_volume_image(0, 4);
            const auto& expected1 = get_us_volume_image(1, 4);
            const auto& expected2 = get_us_volume_image(2, 4);

            // Write a three 4 frames RGB uint8 image, with a custom filename
            {
                auto series_set = std::make_shared<data::series_set>();
                series_set->push_back(expected0);
                series_set->push_back(expected1);
                series_set->push_back(expected2);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);
                writer->set_file("custom_filename.dcm");

                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
                CHECK(std::filesystem::exists(tmp_dir / "000-custom_filename.dcm"));
                CHECK(std::filesystem::exists(tmp_dir / "001-custom_filename.dcm"));
                CHECK(std::filesystem::exists(tmp_dir / "002-custom_filename.dcm"));
            }
        }

        // test resized ImageSeries
        {
            core::os::temp_dir tmp_dir;
            const auto& expected = get_us_volume_image(2, 4);

            // Write a 4 frames RGB uint8 image, with a custom filename
            {
                auto series_set = std::make_shared<data::series_set>();

                auto resized = std::make_shared<data::image_series>();
                resized->deep_copy(expected);

                // Resize and set a value at the end...
                auto new_size = expected->size();
                new_size[2] += 1;
                resized->resize(new_size, expected->type(), expected->pixel_format());
                resized->set_frame_acquisition_date_time(
                    format_date_time(std::chrono::system_clock::now()),
                    new_size[2] - 1
                );

                // Resize back to original size
                resized->resize(expected->size(), expected->type(), expected->pixel_format());
                series_set->push_back(resized);

                auto writer = std::make_shared<io::dicom::writer::file>();
                writer->set_object(series_set);
                writer->set_folder(tmp_dir);
                writer->set_file("custom_filename.dcm");

                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
                CHECK(std::filesystem::exists(tmp_dir / "custom_filename.dcm"));
            }

            // Read the previously written 4 frames image
            {
                auto series_set = std::make_shared<data::series_set>();
                auto reader     = std::make_shared<io::dicom::reader::file>();
                reader->set_object(series_set);
                reader->set_folder(tmp_dir);

                auto read_observer = std::make_shared<core::notification::observer>("Test read");
                CHECK_NOTHROW(reader->read(read_observer));
                CHECK_EQ(std::size_t(1), series_set->size());

                compare_enhanced_us_volume(
                    expected,
                    std::dynamic_pointer_cast<data::image_series>(series_set->front()),
                    true
                );
            }
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("force_cpu")
    {
        core::os::temp_dir tmp_dir;
        const auto& expected = get_us_volume_image(0);

        // Write a single RGB uint8 frame image
        {
            auto series_set = std::make_shared<data::series_set>();
            series_set->push_back(expected);

            auto writer = std::make_shared<io::dicom::writer::file>();
            writer->set_object(series_set);
            writer->set_folder(tmp_dir);

            writer->force_cpu(false);

            if(io::bitmap::nvjpeg2k())
            {
                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_NOTHROW(writer->write(write_observer));
            }

#ifdef SIGHT_ENABLE_NVJPEG2K
            else
            {
                auto write_observer = std::make_shared<core::notification::observer>("Test write");
                CHECK_THROWS(writer->write(write_observer));
            }
#endif

            writer->force_cpu(true);

            auto write_observer = std::make_shared<core::notification::observer>("Test write");
            CHECK_NOTHROW(writer->write(write_observer));
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("transfer_syntax")
    {
        const auto& test =
            [](io::dicom::writer::file::transfer_syntax _transfer_syntax)
            {
                core::os::temp_dir tmp_dir;
                const auto& expected = get_us_volume_image(0xFFFF, 6);

                // Write a 4 frames RGB uint8 image
                {
                    auto series_set = std::make_shared<data::series_set>();
                    series_set->push_back(expected);

                    auto writer = std::make_shared<io::dicom::writer::file>();
                    writer->set_object(series_set);
                    writer->set_folder(tmp_dir);
                    writer->set_transfer_syntax(_transfer_syntax);

                    SIGHT_PROFILE_FUNC(
                        [&](std::size_t)
                    {
                        auto write_observer = std::make_shared<core::notification::observer>("Test write");
                        CHECK_NOTHROW(writer->write(write_observer));
                    },
                        3,
                        "Write (" + std::string(
                            io::dicom::writer::file::transfer_syntax_to_string(
                                _transfer_syntax
                            )
                        ) + "): ",
                        0.1
                    );
                }

                // Read the previously written 4 frames image
                {
                    auto series_set = std::make_shared<data::series_set>();
                    auto reader     = std::make_shared<io::dicom::reader::file>();
                    reader->set_object(series_set);
                    reader->set_folder(tmp_dir);

                    SIGHT_PROFILE_FUNC(
                        [&](std::size_t)
                    {
                        auto read_observer = std::make_shared<core::notification::observer>("Test read");
                        CHECK_NOTHROW(reader->read(read_observer));
                    },
                        3,
                        "Read (" + std::string(
                            io::dicom::writer::file::transfer_syntax_to_string(
                                _transfer_syntax
                            )
                        ) + "): ",
                        0.1
                    );

                    CHECK_EQ(std::size_t(1), series_set->size());

                    compare_enhanced_us_volume(
                        expected,
                        std::dynamic_pointer_cast<data::image_series>(series_set->front()),
                        true
                    );
                }

                for(auto const& entry : std::filesystem::directory_iterator {tmp_dir})
                {
                    if(entry.is_regular_file())
                    {
                        const auto size = entry.file_size();

                        SIGHT_INFO(
                            "File size (" << io::dicom::writer::file::transfer_syntax_to_string(
                                _transfer_syntax
                            ) << "): " << size
                        );

                        return size;
                    }
                }

                return static_cast<std::size_t>(0);
            };

        // First the biggest file size
        const auto raw_size = test(io::dicom::writer::file::transfer_syntax::raw);

        for(const auto& transfer_syntax : {
                io::dicom::writer::file::transfer_syntax::rle,
                io::dicom::writer::file::transfer_syntax::jpeg_lossless,
                io::dicom::writer::file::transfer_syntax::jpeg_ls_lossless,
                io::dicom::writer::file::transfer_syntax::jpeg_2000_lossless
            })
        {
            // Then others (which should be smaller)
            const auto size       = test(transfer_syntax);
            const std::string msg = "RAW (" + std::to_string(raw_size) + ") < "
                                    + std::string(io::dicom::writer::file::transfer_syntax_to_string(transfer_syntax))
                                    + " (" + std::to_string(size) + ")";
            CHECK_MESSAGE(raw_size >= size, msg);
        }
    }
}

} // namespace sight::io::dicom::ut
