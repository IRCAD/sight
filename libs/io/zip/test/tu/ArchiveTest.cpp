/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/os/TempPath.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>
#include <io/zip/exception/Read.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::zip::ut::ArchiveTest);

namespace sight::io::zip::ut
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
        core::os::TempDir tmpDir;
        const std::filesystem::path archivePath = tmpDir / "newTest.zip";

        {
            auto archiveWriter = ArchiveWriter::get(archivePath);

            // check the archive path getter
            CPPUNIT_ASSERT_EQUAL(
                archivePath,
                archiveWriter->getArchivePath()
            );
        }

        {
            auto archiveReader = ArchiveReader::get(archivePath);

            // check the archive path getter
            CPPUNIT_ASSERT_EQUAL(
                archivePath,
                archiveReader->getArchivePath()
            );
        }
    }

    // Error test
    {
        CPPUNIT_ASSERT_THROW_MESSAGE(
            "A wrong filename should trigger an exception.",
            ArchiveReader::get("__wrong_file__"),
            io::zip::exception::Read
        );
    }
}

//------------------------------------------------------------------------------

void ArchiveTest::singletonTest()
{
    // Create a temporary file
    core::os::TempDir tmpDir;
    const std::filesystem::path archivePath = tmpDir / "singletonTest.zip";

    // Error test
    {
        // Due to internal limitation of minizip, you should not be able to open the same archive in both mode
        auto archiveWriter = ArchiveWriter::get(archivePath);
        CPPUNIT_ASSERT_THROW_MESSAGE(
            "Open the same archive in writing and in reading at the same time, should trigger an exception.",
            ArchiveReader::get(archivePath),
            core::Exception
        );
    }
}

//------------------------------------------------------------------------------

void ArchiveTest::openTest()
{
    // Create a temporary file
    core::os::TempDir tmpDir;
    const std::filesystem::path archivePath = tmpDir / "openTest.zip";

    // Test default parameters
    const std::string unencrypted_zstd_default("unencrypted_zstd_default");
    {
        // Create the archive writer
        auto archiveWriter = ArchiveWriter::get(archivePath);

        // Write a new file in the archive with default parameters
        auto ostream = archiveWriter->openFile(unencrypted_zstd_default);
        ostream->write(unencrypted_zstd_default.data(), static_cast<std::streamsize>(unencrypted_zstd_default.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::get(archivePath);

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
        auto archiveWriter = ArchiveWriter::get(archivePath);

        // Write a new file in the archive with default parameters
        auto ostream = archiveWriter->openFile(
            encrypted_zstd_best,
            encrypted_zstd_best,
            Method::ZSTD,
            Level::BEST
        );

        ostream->write(encrypted_zstd_best.data(), static_cast<std::streamsize>(encrypted_zstd_best.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::get(archivePath);

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
        auto archiveWriter = ArchiveWriter::get(archivePath);

        // Write a new file in the archive with default parameters
        auto ostream = archiveWriter->openFile(
            unencrypted_deflate_fast,
            "",
            Method::DEFLATE,
            Level::FAST
        );

        ostream->write(unencrypted_deflate_fast.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::get(archivePath);

        // Read the stream
        std::string buffer(unencrypted_deflate_fast.size(), 0);
        auto istream = archive_reader->openFile(unencrypted_deflate_fast);
        istream->read(buffer.data(), static_cast<std::streamsize>(unencrypted_deflate_fast.size()));

        CPPUNIT_ASSERT_EQUAL(unencrypted_deflate_fast, buffer);
    }
}

//------------------------------------------------------------------------------

void ArchiveTest::rawTest()
{
    // Create a temporary file
    core::os::TempDir tmpDir;

    // use a complex uncreated path
    auto folderPath = tmpDir / std::filesystem::path("a/b/c/d");

    // Test default parameters
    const std::string raw_text("raw_text");
    {
        // Create the archive writer
        auto archiveWriter = ArchiveWriter::get(folderPath, Archive::ArchiveFormat::FILESYSTEM);

        // Write a new file in the archive with default parameters
        auto ostream = archiveWriter->openFile(raw_text);
        ostream->write(raw_text.data(), static_cast<std::streamsize>(raw_text.size()));
    }

    {
        // Create the archive reader
        auto archive_reader = ArchiveReader::get(folderPath, Archive::ArchiveFormat::FILESYSTEM);

        // Read the stream
        std::string buffer(raw_text.size(), 0);
        auto istream = archive_reader->openFile(raw_text);
        istream->read(buffer.data(), static_cast<std::streamsize>(raw_text.size()));

        CPPUNIT_ASSERT_EQUAL(raw_text, buffer);
    }

    std::filesystem::remove_all(folderPath);
}

//------------------------------------------------------------------------------

void ArchiveTest::archiveFormatToStringTest()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(name) \
    CPPUNIT_ASSERT_EQUAL( \
        boost::to_lower_copy(std::string(#name)), \
        std::string(Archive::archiveFormatToString(Archive::ArchiveFormat::name)) \
    )
    TEST(FILESYSTEM);
    TEST(COMPATIBLE);
    TEST(OPTIMIZED);
    #undef TEST
}

//------------------------------------------------------------------------------

void ArchiveTest::stringToArchiveFormat()
{
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(name) \
    CPPUNIT_ASSERT_EQUAL( \
        Archive::ArchiveFormat::name, \
        Archive::stringToArchiveFormat(boost::to_lower_copy(std::string(#name))) \
    )
    TEST(FILESYSTEM);
    TEST(COMPATIBLE);
    TEST(OPTIMIZED);
    #undef TEST
}

} // namespace sight::io::zip::ut
