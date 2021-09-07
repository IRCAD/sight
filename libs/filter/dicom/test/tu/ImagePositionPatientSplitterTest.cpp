/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ImagePositionPatientSplitterTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/IFilter.hpp>

#include <io/dicom/reader/SeriesDB.hpp>

#include <utestData/Data.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::filter::dicom::ut::ImagePositionPatientSplitterTest);

namespace sight::filter::dicom
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
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "08-CT-PACS";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector<data::DicomSeries::sptr> dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Sort instances according to instance number
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(
        "sight::filter::dicom::sorter::InstanceNumberSorter"
    );
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);

    // Apply filter
    filter = filter::dicom::factory::New("sight::filter::dicom::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesContainer.size());
    data::DicomSeries::sptr dicomSeriesA = dicomSeriesContainer[0];
    data::DicomSeries::sptr dicomSeriesB = dicomSeriesContainer[1];

    // Check number of instances in series
    CPPUNIT_ASSERT_EQUAL(size_t(233), dicomSeriesA->getDicomContainer().size());
    CPPUNIT_ASSERT_EQUAL(size_t(275), dicomSeriesB->getDicomContainer().size());
}

//-----------------------------------------------------------------------------

void ImagePositionPatientSplitterTest::negativeSpacingApplication()
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "04-CT-DICOM_SCRAT_CORRUPTED/46140000";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());

    // Horos seems also to think that there is two series in this dicom
    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->size());

    // Retrieve DicomSeries
    data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast(seriesDB->at(0));
    CPPUNIT_ASSERT(dicomSeries);

    // On Unix, the correct series with 304 elements is placed first and the one with 196 elements is at last position,
    // which is the opposite on windows (???)... Assuming the Dicom is file based, the way the OS sorts files may
    // explain the different behaviors. We should investigate this ....
    // The test is written to assume the one of 304 elements is taken.
    if(dicomSeries->getNumberOfInstances() != 304)
    {
        dicomSeries = data::DicomSeries::dynamicCast(seriesDB->at(1));
        CPPUNIT_ASSERT(dicomSeries);
    }

    // Just in case we load the wrong series or the data is corrupted.
    CPPUNIT_ASSERT_EQUAL(size_t(304), dicomSeries->getNumberOfInstances());

    std::vector<data::DicomSeries::sptr> dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Sort instances according to instance number
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(
        "sight::filter::dicom::sorter::InstanceNumberSorter"
    );
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);

    // Apply filter
    filter = filter::dicom::factory::New("sight::filter::dicom::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(37), dicomSeriesContainer.size());
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::filter::dicom
