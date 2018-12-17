/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ImagePositionPatientSplitterTest.hpp"

#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/IFilter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::ImagePositionPatientSplitterTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void ImagePositionPatientSplitterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImagePositionPatientSplitterTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ImagePositionPatientSplitterTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "08-CT-PACS";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

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
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesContainer.size());
    ::fwMedData::DicomSeries::sptr dicomSeriesA = dicomSeriesContainer[0];
    ::fwMedData::DicomSeries::sptr dicomSeriesB = dicomSeriesContainer[1];

    // Check number of instances in series
    CPPUNIT_ASSERT_EQUAL(size_t(233), dicomSeriesA->getDicomContainer().size());
    CPPUNIT_ASSERT_EQUAL(size_t(275), dicomSeriesB->getDicomContainer().size());

}

//-----------------------------------------------------------------------------

void ImagePositionPatientSplitterTest::negativeSpacingApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "04-CT-DICOM_SCRAT_CORRUPTED/46140000";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

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
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(37), dicomSeriesContainer.size());

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
