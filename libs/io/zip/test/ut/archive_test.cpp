/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "archive_test.hpp"

#include <core/os/temp_path.hpp>

#include <io/zip/archive_reader.hpp>
#include <io/zip/archive_writer.hpp>
#include <io/zip/exception/read.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::zip::ut::archive_test);

namespace sight::io::zip::ut
{

//------------------------------------------------------------------------------

void archive_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void archive_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void archive_test::new_test()
{
    // Nominal test
    {
        // Create a temporary file
        core::os::temp_dir tmp_dir;
        const std::filesystem::path archive_path = tmp_dir / "newTest.zip";

        {
            auto archive_writer = archive_writer::get(archive_path);
            CPPUNIT_ASSERT(archive_writer->is_a("sight::io::zip::archive_writer"));

            // check the archive path getter
            CPPUNIT_ASSERT_EQUAL(
                archive_path,
                archive_writer->get_archive_path()
            );
        }

        {
            auto archive_reader = archive_reader::get(archive_path);
            CPPUNIT_ASSERT(archive_reader->is_a("sight::io::zip::archive_reader"));

            // check the archive path getter
            CPPUNIT_ASSERT_EQUAL(
                archive_path,
                archive_reader->get_archive_path()
            );
        }
    }

    // Error test
    {
        CPPUNIT_ASSERT_THROW_MESSAGE(
            "A wrong filename should trigger an exception.",
            archive_reader::get("__wrong_file__"),
            io::zip::exception::read
        );
    }
}

//------------------------------------------------------------------------------

void archive_test::singleton_test()
{
    // Create a temporary file
    core::os::temp_dir tmp_dir;
    const std::filesystem::path archive_path = tmp_dir / "singletonTest.zip";

    // Error test
    {
        // Due to internal limitation of minizip, you should not be able to open the same archive in both mode
        auto archive_writer = archive_writer::get(archive_path);
        CPPUNIT_ASSERT_THROW_MESSAGE(
            "Open the same archive in writing and in reading at the same time, should trigger an exception.",
            archive_reader::get(archive_path),
            core::exception
        );
    }
}

//------------------------------------------------------------------------------

void archive_test::open_test()
{
    // Create a temporary file
    core::os::temp_dir tmp_dir;
    const std::filesystem::path archive_path = tmp_dir / "openTest.zip";

    // Test default parameters
    const std::string unencrypted_zstd_default("unencrypted_zstd_default");
    {
        // Create the archive writer
        auto archive_writer = archive_writer::get(archive_path);

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->open_file(unencrypted_zstd_default);
        ostream->write(unencrypted_zstd_default.data(), static_cast<std::streamsize>(unencrypted_zstd_default.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = archive_reader::get(archive_path);

        // Read the stream
        std::string buffer(unencrypted_zstd_default.size(), 0);
        auto istream = archive_reader->open_file(unencrypted_zstd_default);
        istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_zstd_default.size()));

        CPPUNIT_ASSERT_EQUAL(unencrypted_zstd_default, buffer);
    }

    // Test best encrypted ZSTD parameters
    const core::crypto::secure_string encrypted_zstd_best("encrypted_zstd_best");
    {
        // Create the archive writer
        auto archive_writer = archive_writer::get(archive_path);

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->open_file(
            encrypted_zstd_best,
            encrypted_zstd_best,
            method::zstd,
            level::best
        );

        ostream->write(encrypted_zstd_best.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = archive_reader::get(archive_path);

        // Read the stream
        std::string buffer(encrypted_zstd_best.size(), 0);
        auto istream = archive_reader->open_file(encrypted_zstd_best, encrypted_zstd_best);
        istream->read(buffer.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));

        CPPUNIT_ASSERT_EQUAL(encrypted_zstd_best, core::crypto::secure_string(buffer));
    }

    // Test fast unencrypted deflate parameters
    const std::string unencrypted_deflate_fast("unencrypted_deflate_fast");
    {
        // Create the archive writer
        auto archive_writer = archive_writer::get(archive_path);

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->open_file(
            unencrypted_deflate_fast,
            "",
            method::deflate,
            level::fast
        );

        ostream->write(unencrypted_deflate_fast.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = archive_reader::get(archive_path);

        // Read the stream
        std::string buffer(unencrypted_deflate_fast.size(), 0);
        auto istream = archive_reader->open_file(unencrypted_deflate_fast);
        istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));

        CPPUNIT_ASSERT_EQUAL(unencrypted_deflate_fast, buffer);
    }
}

//------------------------------------------------------------------------------

void archive_test::raw_test()
{
    // Create a temporary file
    core::os::temp_dir tmp_dir;

    // use a complex uncreated path
    auto folder_path = tmp_dir / std::filesystem::path("a/b/c/d");

    // Test default parameters
    const std::string raw_text("raw_text");
    {
        // Create the archive writer
        auto archive_writer = archive_writer::get(folder_path, archive::archive_format::filesystem);
        CPPUNIT_ASSERT(archive_writer->is_a("sight::io::zip::archive_writer"));

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->open_file(raw_text);
        ostream->write(raw_text.data(), static_cast<std::streamsize>(raw_text.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = archive_reader::get(folder_path, archive::archive_format::filesystem);
        CPPUNIT_ASSERT(archive_reader->is_a("sight::io::zip::archive_reader"));

        // Read the stream
        std::string buffer(raw_text.size(), 0);
        auto istream = archive_reader->open_file(raw_text);
        istream->read(buffer.data(), static_cast<std::streamsize>(raw_text.size()));

        CPPUNIT_ASSERT_EQUAL(raw_text, buffer);
    }

    std::filesystem::remove_all(folder_path);
}

//------------------------------------------------------------------------------

void archive_test::archive_format_to_string_test()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(name) \
            CPPUNIT_ASSERT_EQUAL( \
                boost::to_lower_copy(std::string(#name)), \
                std::string(archive::archive_format_to_string(archive::archive_format::name)) \
            )
    TEST(filesystem);
    TEST(compatible);
    TEST(optimized);
    #undef TEST
}

//------------------------------------------------------------------------------

void archive_test::string_to_archive_format()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(name) \
            CPPUNIT_ASSERT_EQUAL( \
                archive::archive_format::name, \
                archive::string_to_archive_format(boost::to_lower_copy(std::string(#name))) \
            )
    TEST(filesystem);
    TEST(compatible);
    TEST(optimized);
    #undef TEST
}

} // namespace sight::io::zip::ut
