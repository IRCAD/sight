/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "DirTest.hpp"

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>

#include <fwTest/Data.hpp>

#include <fwTools/System.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwZip::ut::DirTest );

namespace fwZip
{
namespace ut
{

//------------------------------------------------------------------------------

void DirTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DirTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DirTest::writeReadFileTest()
{
    const std::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwDirTest";
    std::filesystem::create_directories( dirPath );

    SPTR(WriteDirArchive) writer = std::make_shared<WriteDirArchive>(dirPath);
    SPTR(ReadDirArchive) reader  = std::make_shared<ReadDirArchive>(dirPath);

    const std::filesystem::path testFile = "test.txt";

    CPPUNIT_ASSERT_NO_THROW(writer->createFile(testFile));

    CPPUNIT_ASSERT_NO_THROW(reader->getFile(testFile));
    std::filesystem::remove_all( dirPath );

}

//------------------------------------------------------------------------------

void DirTest::writeDirTest()
{
    const std::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwDirTest";
    std::filesystem::create_directories( dirPath );

    SPTR(WriteDirArchive) writer = std::make_shared<WriteDirArchive>(dirPath);

    const std::filesystem::path testDir = "test";

    CPPUNIT_ASSERT_NO_THROW(writer->createDir(testDir));

    CPPUNIT_ASSERT(std::filesystem::exists(dirPath/testDir));

    std::filesystem::remove_all( dirPath );
}

//------------------------------------------------------------------------------

void DirTest::putFileTest()
{
    const std::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwDirTest";
    std::filesystem::create_directories( dirPath );

    SPTR(WriteDirArchive) writer = std::make_shared<WriteDirArchive>(dirPath);

    const std::filesystem::path testDir  = "test";
    const std::filesystem::path testFile = ::fwTest::Data::dir() / "sight/image/jpg/makao01.jpg";

    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    CPPUNIT_ASSERT_NO_THROW(writer->putFile(testFile, "image.jpg"));

    CPPUNIT_ASSERT(std::filesystem::exists(dirPath/"image.jpg"));

    std::filesystem::remove_all( dirPath );
}

} // namespace ut
} // namespace fwZip
