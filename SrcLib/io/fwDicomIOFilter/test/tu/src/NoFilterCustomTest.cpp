/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "NoFilterCustomTest.hpp"

#include <fwDicomIOFilter/IFilter.hpp>
#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::NoFilterCustomTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void NoFilterCustomTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void NoFilterCustomTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void NoFilterCustomTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "71-CT-DICOM_SEG";
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
    ::fwMedData::DicomSeries::sptr dicomSeriesA = ::fwMedData::DicomSeries::dynamicCast((*seriesDB)[0]);
    ::fwMedData::DicomSeries::sptr dicomSeriesB = ::fwMedData::DicomSeries::New();
    dicomSeriesB->deepCopy(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesB);
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainerA;
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainerB;
    dicomSeriesContainerA.push_back(dicomSeriesA);
    dicomSeriesContainerB.push_back(dicomSeriesB);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::custom::NoFilter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerA, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(1), dicomSeriesContainerA.size());
    dicomSeriesA = dicomSeriesContainerA[0];

    // Check Number of SOP Class UID
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesA->getSOPClassUIDs().size());

    // Compare the two series
    CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA.size(), dicomSeriesContainerB.size());
    for(unsigned int j = 0; j < dicomSeriesContainerA.size(); ++j)  // For every series
    {
        for(unsigned int i = 0; i < dicomSeriesContainerA[j]->getLocalDicomPaths().size(); ++i) // For every instances
        {
            CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA[j]->getLocalDicomPaths().at(i),
                                 dicomSeriesContainerB[j]->getLocalDicomPaths().at(i));
        }
    }

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
