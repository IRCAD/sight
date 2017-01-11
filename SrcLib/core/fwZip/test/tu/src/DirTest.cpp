/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DirTest.hpp"

#include <fwTest/Data.hpp>

#include <fwTools/System.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwZip::ut::DirTest );

namespace fwZip
{
namespace ut
{

void DirTest::setUp()
{
    // Set up context before running a test.
}

void DirTest::tearDown()
{
    // Clean up after the test run.
}

void DirTest::writeReadFileTest()
{
    const ::boost::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwDirTest";
    ::boost::filesystem::create_directories( dirPath );

    SPTR(WriteDirArchive) writer = std::make_shared<WriteDirArchive>(dirPath);
    SPTR(ReadDirArchive) reader  = std::make_shared<ReadDirArchive>(dirPath);

    const ::boost::filesystem::path testFile = "test.txt";

    CPPUNIT_ASSERT_NO_THROW(writer->createFile(testFile));

    CPPUNIT_ASSERT_NO_THROW(reader->getFile(testFile));
    ::boost::filesystem::remove_all( dirPath );

}

void DirTest::writeDirTest()
{
    const ::boost::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwDirTest";
    ::boost::filesystem::create_directories( dirPath );

    SPTR(WriteDirArchive) writer = std::make_shared<WriteDirArchive>(dirPath);

    const ::boost::filesystem::path testDir = "test";

    CPPUNIT_ASSERT_NO_THROW(writer->createDir(testDir));

    CPPUNIT_ASSERT(::boost::filesystem::exists(dirPath/testDir));

    ::boost::filesystem::remove_all( dirPath );
}

void DirTest::putFileTest()
{
    const ::boost::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwDirTest";
    ::boost::filesystem::create_directories( dirPath );

    SPTR(WriteDirArchive) writer = std::make_shared<WriteDirArchive>(dirPath);

    const ::boost::filesystem::path testDir  = "test";
    const ::boost::filesystem::path testFile = ::fwTest::Data::dir() / "fw4spl/image/jpg/makao01.jpg";


    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           ::boost::filesystem::exists(testFile));

    CPPUNIT_ASSERT_NO_THROW(writer->putFile(testFile, "image.jpg"));

    CPPUNIT_ASSERT(::boost::filesystem::exists(dirPath/"image.jpg"));

    ::boost::filesystem::remove_all( dirPath );
}


} // namespace ut
} // namespace fwZip
