/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DicomDirTest.hpp"

#include <fwGdcmIO/helper/DicomDir.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwTest/Data.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::DicomDirTest );

namespace fwGdcmIO
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


    std::vector< ::fwMedData::DicomSeries::sptr > seriesDB;

    ::fwLog::Logger::sptr logger = ::fwLog::Logger::New();

    // Read DICOMDIR file
    ::fwGdcmIO::helper::DicomDir::retrieveDicomSeries(path / "DICOMDIR", seriesDB, logger);
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB.size());
    CPPUNIT_ASSERT( logger->empty() );

    auto series = *seriesDB.begin();
    CPPUNIT_ASSERT_EQUAL( size_t( 84 ), series->getLocalDicomPaths().size());

    // Verify filenames
    for(auto entry: series->getLocalDicomPaths())
    {
        CPPUNIT_ASSERT(::boost::filesystem::exists(entry.second));
    }

}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwGdcmIO

