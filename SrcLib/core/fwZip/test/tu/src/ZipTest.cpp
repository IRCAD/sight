/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ZipTest.hpp"

#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <fwTest/Data.hpp>

#include <fwTools/System.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwZip::ut::ZipTest );

namespace fwZip
{
namespace ut
{

//------------------------------------------------------------------------------

void ZipTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ZipTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ZipTest::commentTest()
{
    const std::string writerComment = "Example of comment";

    const std::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwZipTest";
    std::filesystem::create_directories( dirPath );
    const std::filesystem::path path        = dirPath / "jambon.zip";
    const std::filesystem::path sourceFile  = ::fwTest::Data::dir() / "sight/image/jpg/makao01.jpg";
    const std::filesystem::path archiveFile = "makao.jpg";

    CPPUNIT_ASSERT_MESSAGE("The file '" + sourceFile.string() + "' does not exist",
                           std::filesystem::exists(sourceFile));

    SPTR(WriteZipArchive) writer = std::make_shared<WriteZipArchive>(path, writerComment);
    writer->putFile(sourceFile, archiveFile);

    SPTR(ReadZipArchive) reader = std::make_shared<ReadZipArchive>(path);
    std::string readerComment = reader->getComment();

    std::filesystem::remove_all( path );

    CPPUNIT_ASSERT_EQUAL(writerComment, readerComment);
}

//------------------------------------------------------------------------------

void ZipTest::cryptTest()
{
    const std::string writerComment = "Example of comment";

    const std::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwZipTest";
    std::filesystem::create_directories( dirPath );
    const std::filesystem::path path        = dirPath / "jambon.zip";
    const std::filesystem::path sourceFile  = ::fwTest::Data::dir() / "sight/image/jpg/makao01.jpg";
    const std::filesystem::path archiveFile = "makao.jpg";

    CPPUNIT_ASSERT_MESSAGE("The file '" + sourceFile.string() + "' does not exist",
                           std::filesystem::exists(sourceFile));

    // The password to use for encryption
    const std::string password = "The little blue kitty is very sick...";

    // Put sourceFile into archiveFile
    SPTR(WriteZipArchive) writer = std::make_shared<WriteZipArchive>(path, writerComment, password);
    writer->putFile(sourceFile, archiveFile);

    // Read comment
    SPTR(ReadZipArchive) reader = std::make_shared<ReadZipArchive>(path, password);
    std::string readerComment = reader->getComment();

    CPPUNIT_ASSERT_EQUAL(writerComment, readerComment);

    // Compare the first 1024 bytes of the actual file and the archived one
    char sourceBuffer[1024];
    std::ifstream fileStream(sourceFile, std::ifstream::binary);
    fileStream.read(sourceBuffer, sizeof(sourceBuffer));

    char archiveBuffer[sizeof(sourceBuffer)];
    auto archiveStream = reader->getFile(archiveFile);
    archiveStream->read(archiveBuffer, sizeof(archiveBuffer));

    for(size_t i = 0; i < sizeof(sourceBuffer); i++)
    {
        CPPUNIT_ASSERT_EQUAL(sourceBuffer[i], archiveBuffer[i]);
    }
}

} // namespace ut
} // namespace fwZip
