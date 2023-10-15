/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "sop_class_uid_splitter_test.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/Filter.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::sop_class_uid_splitter_test);

namespace sight::filter::dicom::ut
{

//------------------------------------------------------------------------------

void sop_class_uid_splitter_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void sop_class_uid_splitter_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void sop_class_uid_splitter_test::simpleApplication()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "71-CT-DICOM_SEG";
    const std::filesystem::path path = utest_data::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve DicomSeries
    data::dicom_series::sptr dicomSeries = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector<data::dicom_series::sptr> dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::splitter::sop_class_uid_splitter"
    );
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), dicomSeriesContainer.size());
    data::dicom_series::sptr dicomSeriesA = dicomSeriesContainer[0];
    data::dicom_series::sptr dicomSeriesB = dicomSeriesContainer[1];

    // Check SOP Class UIDs
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.2"), *dicomSeriesA->getSOPClassUIDs().begin());    // CT
                                                                                                                 // Image
                                                                                                                 // Storage
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.66.5"), *dicomSeriesB->getSOPClassUIDs().begin()); // Surface
                                                                                                                 // Segmentation
                                                                                                                 // Storage
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
