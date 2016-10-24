/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DicomDirTest.hpp"

#include <fwDcmtkIO/helper/DicomDir.hpp>

#include <fwTest/Data.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDcmtkIO::ut::DicomDirTest );

namespace fwDcmtkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomDirTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomDirTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomDirTest::readDicomDir()
{
    const ::boost::filesystem::path path = ::fwTest::Data::dir() /
                                           "fw4spl/Patient/Dicom/DicomDB/82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::string pathStr = ::boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    std::vector<std::string> filenames;

    // Read DICOMDIR file
    CPPUNIT_ASSERT(::fwDcmtkIO::helper::DicomDir::readDicomDir(path, filenames));
    CPPUNIT_ASSERT_EQUAL( size_t( 84 ), filenames.size());

    // Verify filenames
    unsigned int i = 0;
    for(const std::string& filename: filenames)
    {
        std::string newFilename = ::boost::algorithm::replace_all_copy(filename, "\\", "/");

        std::stringstream ss;
        ss << pathStr << "/DICOM/PT0/ST0/SE0/IM" << i++;
        CPPUNIT_ASSERT_EQUAL(ss.str(), newFilename);
    }

}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwDcmtkIO

