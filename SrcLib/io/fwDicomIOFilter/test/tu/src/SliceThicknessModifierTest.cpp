/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SliceThicknessModifierTest.hpp"

#include <fwDicomIOFilter/IFilter.hpp>
#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::SliceThicknessModifierTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void SliceThicknessModifierTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SliceThicknessModifierTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void SliceThicknessModifierTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "09-CT-PACS";
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

    // Sort instances according to instance number
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::sorter::InstanceNumberSorter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);

    // Apply filter
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::modifier::SliceThicknessModifier");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(1), dicomSeriesContainer.size());
    dicomSeries = dicomSeriesContainer[0];

    // Check that the computed value has been added to the DicomSeries
    const double delta          = 1e-8;
    const double sliceThickness =
        ::boost::lexical_cast<double>(dicomSeries->getComputedTagValues().at("SliceThickness"));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8, sliceThickness, delta);

}

//------------------------------------------------------------------------------

void SliceThicknessModifierTest::applyFilterOn2DImage()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "46-MR-BARRE-MONO2-12-shoulder";
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
        "::fwDicomIOFilter::modifier::SliceThicknessModifier");
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT(!::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, false));
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
