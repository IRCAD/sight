/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ZipTest.hpp"

#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <fwTools/System.hpp>
#include <fwTest/Data.hpp>

#include <boost/make_shared.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwZip::ut::ZipTest );

namespace fwZip
{
namespace ut
{

void ZipTest::setUp()
{
    // Set up context before running a test.
}

void ZipTest::tearDown()
{
    // Clean up after the test run.
}

void ZipTest::commentTest()
{
    const std::string writerComment = "Example of comment";

    const ::boost::filesystem::path dirPath = ::fwTools::System::getTemporaryFolder() / "fwZipTest";
    ::boost::filesystem::create_directories( dirPath );
    const ::boost::filesystem::path path        = dirPath / "jambon.zip";
    const ::boost::filesystem::path sourceFile  = ::fwTest::Data::dir() / "fw4spl/image/jpg/makao01.jpg";
    const ::boost::filesystem::path archiveFile = "makao.jpg";


    SPTR(WriteZipArchive) writer = ::boost::make_shared<WriteZipArchive>(path, writerComment);
    writer->putFile(sourceFile, archiveFile);
    writer->createFile("");

    SPTR(ReadZipArchive) reader = ::boost::make_shared<ReadZipArchive>(path);
    std::string readerComment = reader->getComment();

    ::boost::filesystem::remove_all( path );

    CPPUNIT_ASSERT_EQUAL(writerComment, readerComment);

}

} // namespace ut
} // namespace fwZip
