/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/os/temp_path.hpp>

#include <io/zip/archive_reader.hpp>
#include <io/zip/archive_writer.hpp>
#include <io/zip/exception/read.hpp>

#include <boost/algorithm/string.hpp>

#include <doctest/doctest.h>

#include <filesystem>
#include <iostream>
#include <string>

TEST_SUITE("sight::io::zip::archive")
{
    TEST_CASE("new")
    {
        // Nominal test
        {
            // Create a temporary file
            sight::core::os::temp_dir tmp_dir;
            const std::filesystem::path archive_path = tmp_dir / "newTest.zip";

            {
                auto archive_writer = sight::io::zip::archive_writer::get(archive_path);
                CHECK(archive_writer->is_a("sight::io::zip::archive_writer"));

                // check the archive path getter
                CHECK_EQ(
                    archive_path,
                    archive_writer->get_archive_path()
                );
            }

            {
                auto archive_reader = sight::io::zip::archive_reader::get(archive_path);
                CHECK(archive_reader->is_a("sight::io::zip::archive_reader"));

                // check the archive path getter
                CHECK_EQ(
                    archive_path,
                    archive_reader->get_archive_path()
                );
            }
        }

        // Error test
        {
            CHECK_THROWS_AS(
                sight::io::zip::archive_reader::get("__wrong_file__"),
                sight::io::zip::exception::read
            );
        }
    }

    TEST_CASE("singleton")
    {
        // Create a temporary file
        sight::core::os::temp_dir tmp_dir;
        const std::filesystem::path archive_path = tmp_dir / "singletonTest.zip";

        // Error test
        {
            // Due to internal limitation of minizip, you should not be able to open the same archive in both mode
            auto archive_writer = sight::io::zip::archive_writer::get(archive_path);
            CHECK_THROWS_AS(
                sight::io::zip::archive_reader::get(archive_path),
                sight::core::exception
            );
        }
    }

    TEST_CASE("open")
    {
        // Create a temporary file
        sight::core::os::temp_dir tmp_dir;
        const std::filesystem::path archive_path = tmp_dir / "openTest.zip";

        // Test default parameters
        const std::string unencrypted_zstd_default("unencrypted_zstd_default");
        {
            // Create the archive writer
            auto archive_writer = sight::io::zip::archive_writer::get(archive_path);

            // Write a new file in the archive with default parameters
            auto ostream = archive_writer->open_file(unencrypted_zstd_default);
            ostream->write(
                unencrypted_zstd_default.data(),
                static_cast<std::streamsize>(unencrypted_zstd_default.size())
            );
        }

        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::archive_reader::get(archive_path);

            // Read the stream
            std::string buffer(unencrypted_zstd_default.size(), 0);
            auto istream = archive_reader->open_file(unencrypted_zstd_default);
            istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_zstd_default.size()));

            CHECK_EQ(unencrypted_zstd_default, buffer);
        }

        // Test best encrypted ZSTD parameters
        const sight::core::crypto::secure_string encrypted_zstd_best("encrypted_zstd_best");
        {
            // Create the archive writer
            auto archive_writer = sight::io::zip::archive_writer::get(archive_path);

            // Write a new file in the archive with default parameters
            auto ostream = archive_writer->open_file(
                encrypted_zstd_best,
                encrypted_zstd_best,
                sight::io::zip::method::zstd,
                sight::io::zip::level::best
            );

            ostream->write(encrypted_zstd_best.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));
        }

        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::archive_reader::get(archive_path);

            // Read the stream
            std::string buffer(encrypted_zstd_best.size(), 0);
            auto istream = archive_reader->open_file(encrypted_zstd_best, encrypted_zstd_best);
            istream->read(buffer.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));

            CHECK_EQ(encrypted_zstd_best, sight::core::crypto::secure_string(buffer));
        }

        const sight::core::crypto::secure_string encrypted_zstd_fast("encrypted_zstd_fast");
        {
            // Create the archive writer
            auto archive_writer = sight::io::zip::archive_writer::get(archive_path);

            // Write a new file in the archive with default parameters
            auto ostream = archive_writer->open_file(
                encrypted_zstd_fast,
                encrypted_zstd_fast,
                sight::io::zip::method::zstd,
                sight::io::zip::level::fast
            );

            ostream->write(encrypted_zstd_fast.data(), static_cast<std::streamsize>(encrypted_zstd_fast.size()));
        }

        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::archive_reader::get(archive_path);

            // Read the stream as string
            std::string buffer = archive_reader->read_file(encrypted_zstd_fast, encrypted_zstd_fast);
            CHECK_EQ(encrypted_zstd_fast, sight::core::crypto::secure_string(buffer));
        }

        // Test fast unencrypted deflate parameters
        const std::string unencrypted_deflate_fast("unencrypted_deflate_fast");
        {
            // Create the archive writer
            auto archive_writer = sight::io::zip::archive_writer::get(archive_path);

            // Write a new file in the archive with default parameters
            auto ostream = archive_writer->open_file(
                unencrypted_deflate_fast,
                "",
                sight::io::zip::method::deflate,
                sight::io::zip::level::fast
            );

            ostream->write(
                unencrypted_deflate_fast.data(),
                static_cast<std::streamsize>(unencrypted_deflate_fast.size())
            );
        }

        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::archive_reader::get(archive_path);

            // Read the stream
            std::string buffer(unencrypted_deflate_fast.size(), 0);
            auto istream = archive_reader->open_file(unencrypted_deflate_fast);
            istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));

            CHECK_EQ(unencrypted_deflate_fast, buffer);
        }
    }

    TEST_CASE("raw")
    {
        // Create a temporary file
        sight::core::os::temp_dir tmp_dir;

        // use a complex uncreated path
        auto folder_path = tmp_dir / std::filesystem::path("a/b/c/d");

        // Test default parameters
        const std::string raw_text("raw_text");
        {
            // Create the archive writer
            auto archive_writer = sight::io::zip::archive_writer::get(
                folder_path,
                sight::io::zip::archive::archive_format::filesystem
            );
            CHECK(archive_writer->is_a("sight::io::zip::archive_writer"));

            // Write a new file in the archive with default parameters
            auto ostream = archive_writer->open_file(raw_text);
            ostream->write(raw_text.data(), static_cast<std::streamsize>(raw_text.size()));
        }

        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::archive_reader::get(
                folder_path,
                sight::io::zip::archive::archive_format::filesystem
            );
            CHECK(archive_reader->is_a("sight::io::zip::archive_reader"));

            // Read the stream
            std::string buffer(raw_text.size(), 0);
            auto istream = archive_reader->open_file(raw_text);
            istream->read(buffer.data(), static_cast<std::streamsize>(raw_text.size()));

            CHECK_EQ(raw_text, buffer);
        }

        std::filesystem::remove_all(folder_path);
    }

    TEST_CASE("archive_format_to_string")
    {
        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
        #define TEST(name) \
                CHECK_EQ( \
                    boost::to_lower_copy(std::string(#name)), \
                    std::string( \
                        sight::io::zip::archive::archive_format_to_string( \
                            sight::io::zip::archive::archive_format::name \
                        ) \
                    ) \
                )
        TEST(filesystem);
        TEST(compatible);
        TEST(optimized);
        #undef TEST
    }

    TEST_CASE("string_to_archive_format")
    {
        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
        #define TEST(name) \
                CHECK_EQ( \
                    sight::io::zip::archive::archive_format::name, \
                    sight::io::zip::archive::string_to_archive_format(boost::to_lower_copy(std::string(#name))) \
                )
        TEST(filesystem);
        TEST(compatible);
        TEST(optimized);
        #undef TEST
    }
} // TEST_SUITE
