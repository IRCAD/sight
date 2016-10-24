/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "InstanceNumberSorterTest.hpp"

#include <fwDicomIOFilter/IFilter.hpp>
#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <gdcmScanner.h>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::InstanceNumberSorterTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void InstanceNumberSorterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void InstanceNumberSorterTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void InstanceNumberSorterTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "01-CT-DICOM_LIVER";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    // Read DicomSeries
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::sorter::InstanceNumberSorter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(1), dicomSeriesContainer.size());
    dicomSeries = dicomSeriesContainer[0];

    // Create the list of files
    ::fwMedData::DicomSeries::DicomPathContainerType dicomPaths = dicomSeries->getLocalDicomPaths();
    std::vector< std::string > filenames;
    ::fwMedData::DicomSeries::DicomPathContainerType::iterator itPath = dicomPaths.begin();
    for(; itPath != dicomPaths.end(); ++itPath)
    {
        filenames.push_back(itPath->second.string());
    }

    // Read the instance number of each file
    ::gdcm::Scanner scanner;
    scanner.AddTag(::gdcm::Tag(0x0020, 0x0013));
    CPPUNIT_ASSERT(scanner.Scan(filenames));

    // Loop through instance number
    ::gdcm::Directory::FilenamesType keys = scanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator itFilename;
    int oldInstanceNumber = -1;
    for(itFilename = keys.begin(); itFilename != keys.end(); ++itFilename)
    {
        const std::string filename = itFilename->c_str();

        // Retrieve instance number
        const std::string instanceNumberStr = scanner.GetValue(filename.c_str(), ::gdcm::Tag(0x0020, 0x0013));
        CPPUNIT_ASSERT(!instanceNumberStr.empty());
        int instanceNumber = ::boost::lexical_cast<unsigned int>(instanceNumberStr);

        // Check that the instance number is bigger that the previous one
        std::stringstream ss;
        ss << instanceNumber << " is not lower than " << oldInstanceNumber;
        CPPUNIT_ASSERT_MESSAGE(ss.str(), instanceNumber > oldInstanceNumber);
        oldInstanceNumber = instanceNumber;
    }

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
