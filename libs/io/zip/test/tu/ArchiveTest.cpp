/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ArchiveTest.hpp"

#include <core/tools/System.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>
#include <io/zip/exception/Read.hpp>

#include <iostream>
#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::zip::ut::ArchiveTest);

namespace sight::io::zip
{

namespace ut
{

//------------------------------------------------------------------------------

void ArchiveTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ArchiveTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ArchiveTest::newTest()
{
    // Nominal test
    {
        // Create a temporary file
        const std::filesystem::path folder_path = core::tools::System::getTemporaryFolder();
        std::filesystem::create_directories(folder_path);
        const std::filesystem::path archive_path = folder_path / "test.zip";

        {
            auto archive_writer = ArchiveWriter::shared(archive_path);
        }

        {
            auto archive_reader = ArchiveReader::shared(archive_path);
        }
    }

    // Error test
    {
        CPPUNIT_ASSERT_THROW_MESSAGE(
            "A wrong filename should trigger an exception.",
            ArchiveReader::shared("__wrong_file__"),
            io::zip::exception::Read
        );
    }
}

//------------------------------------------------------------------------------

void ArchiveTest::cacheTest()
{
    // Create a temporary file
    const std::filesystem::path folder_path = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(folder_path);
    const std::filesystem::path archive_path = folder_path / "test.zip";

    // Nominal test
    {
        // Test writer
        std::uintptr_t old_archive_ptr = 0;
        {
            auto archive_writer = ArchiveWriter::shared(archive_path);

            CPPUNIT_ASSERT_MESSAGE(
                "New ArchiveWriter shared_ptr, with same path, should share the same handle",
                archive_writer.get() == ArchiveWriter::shared(archive_path).get()
            );

            // Store the pointed address
            old_archive_ptr = reinterpret_cast<std::uintptr_t>(archive_writer.get());
        }

        // Test reader
        {
            auto archive_reader = ArchiveReader::shared(archive_path);

            CPPUNIT_ASSERT_MESSAGE(
                "New ArchiveWriter shared_ptr, with same path, should share the same handle",
                archive_reader.get() == ArchiveReader::shared(archive_path).get()
            );

            // Store the pointed address
            old_archive_ptr = reinterpret_cast<std::uintptr_t>(archive_reader.get());
        }
    }

    // Error test
    {
        // Due to internal limitation of minizip, you should not be able to open the same archive in both mode
        auto archive_writer = ArchiveWriter::shared(archive_path);
        CPPUNIT_ASSERT_THROW_MESSAGE(
            "Open the same archive in writing and in reading at the same time, should trigger an exception.",
            ArchiveReader::shared(archive_path),
            io::zip::exception::Read
        );
    }
}

//------------------------------------------------------------------------------

void ArchiveTest::openTest()
{
    // Create a temporary file
    const std::filesystem::path folder_path = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(folder_path);
    const std::filesystem::path archive_path = folder_path / "test.zip";

    // Test default parameters
    const std::string unencrypted_zstd_default("unencrypted_zstd_default");
    {
        // Create the archive writer
        auto archive_writer = ArchiveWriter::shared(archive_path);

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->openFile(unencrypted_zstd_default);
        ostream->write(unencrypted_zstd_default.data(), static_cast<std::streamsize>(unencrypted_zstd_default.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::shared(archive_path);

        // Read the stream
        std::string buffer(unencrypted_zstd_default.size(), 0);
        auto istream = archive_reader->openFile(unencrypted_zstd_default);
        istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_zstd_default.size()));

        CPPUNIT_ASSERT_EQUAL(unencrypted_zstd_default, buffer);
    }

    // Test best encrypted ZSTD parameters
    const core::crypto::secure_string encrypted_zstd_best("encrypted_zstd_best");
    {
        // Create the archive writer
        auto archive_writer = ArchiveWriter::shared(archive_path);

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->openFile(
            encrypted_zstd_best,
            encrypted_zstd_best,
            Method::ZSTD,
            Level::BEST
        );

        ostream->write(encrypted_zstd_best.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::shared(archive_path);

        // Read the stream
        std::string buffer(encrypted_zstd_best.size(), 0);
        auto istream = archive_reader->openFile(encrypted_zstd_best, encrypted_zstd_best);
        istream->read(buffer.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));

        CPPUNIT_ASSERT_EQUAL(encrypted_zstd_best, core::crypto::secure_string(buffer));
    }

    // Test fast unencrypted deflate parameters
    const std::string unencrypted_deflate_fast("unencrypted_deflate_fast");
    {
        // Create the archive writer
        auto archive_writer = ArchiveWriter::shared(archive_path);

        // Write a new file in the archive with default parameters
        auto ostream = archive_writer->openFile(
            unencrypted_deflate_fast,
            "",
            Method::DEFLATE,
            Level::FAST
        );

        ostream->write(unencrypted_deflate_fast.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::shared(archive_path);

        // Read the stream
        std::string buffer(unencrypted_deflate_fast.size(), 0);
        auto istream = archive_reader->openFile(unencrypted_deflate_fast);
        istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));

        CPPUNIT_ASSERT_EQUAL(unencrypted_deflate_fast, buffer);
    }
}

} // namespace ut

} // namespace sight::io::zip
