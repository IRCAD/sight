/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "InstanceNumberSorterTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/IFilter.hpp>

#include <io/dicom/reader/SeriesDB.hpp>

#include <utestData/Data.hpp>

#include <gdcmScanner.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::InstanceNumberSorterTest);

namespace sight::filter::dicom
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
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "01-CT-DICOM_LIVER";
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
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector<data::DicomSeries::sptr> dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(
        "sight::filter::dicom::sorter::InstanceNumberSorter"
    );
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dicomSeriesContainer.size());
    dicomSeries = dicomSeriesContainer[0];
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::filter::dicom
