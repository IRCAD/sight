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

#include "DefaultDicomFilterCustomTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/IFilter.hpp>

#include <io/dicom/reader/SeriesDB.hpp>

#include <utestData/Data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include <gdcmScanner.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::filter::dicom::ut::DefaultDicomFilterCustomTest);

namespace sight::filter::dicom
{

namespace ut
{

//------------------------------------------------------------------------------

void DefaultDicomFilterCustomTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DefaultDicomFilterCustomTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void DefaultDicomFilterCustomTest::simpleApplication()
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "71-CT-DICOM_SEG";
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

    // Apply filter
    filter::dicom::IFilter::sptr filter =
        filter::dicom::factory::New("sight::filter::dicom::custom::DefaultDicomFilter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesContainer.size());
    data::DicomSeries::sptr dicomSeriesA = dicomSeriesContainer[0];
    data::DicomSeries::sptr dicomSeriesB = dicomSeriesContainer[1];

    // Check SOP Class UIDs
    // CT Image Storage Surface Segmentation Storage
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.2"), *dicomSeriesA->getSOPClassUIDs().begin());
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.66.5"), *dicomSeriesB->getSOPClassUIDs().begin());
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::filter::dicom
