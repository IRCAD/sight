/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "image_position_patient_splitter_test.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/filter.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::image_position_patient_splitter_test);

namespace sight::filter::dicom::ut
{

//------------------------------------------------------------------------------

void image_position_patient_splitter_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void image_position_patient_splitter_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void image_position_patient_splitter_test::simple_application()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "08-CT-PACS";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->read_dicom_series());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve DicomSeries
    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    CPPUNIT_ASSERT(dicom_series);
    std::vector<data::dicom_series::sptr> dicom_series_container;
    dicom_series_container.push_back(dicom_series);

    // Sort instances according to instance number
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::sorter::instance_number_sorter"
    );
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, true);

    // Apply filter
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::splitter::image_position_patient_splitter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), dicom_series_container.size());
    data::dicom_series::sptr dicom_series_a = dicom_series_container[0];
    data::dicom_series::sptr dicom_series_b = dicom_series_container[1];

    // Check number of instances in series
    CPPUNIT_ASSERT_EQUAL(std::size_t(233), dicom_series_a->get_dicom_container().size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(275), dicom_series_b->get_dicom_container().size());
}

//-----------------------------------------------------------------------------

void image_position_patient_splitter_test::negative_spacing_application()
{
    auto series_set = std::make_shared<data::series_set>();
    // cspell: ignore SCRAT
    const std::string filename       = "04-CT-DICOM_SCRAT_CORRUPTED/46140000";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;
    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    auto reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->read_dicom_series());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());
    // There is two series in this dicom
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    // Retrieve DicomSeries
    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series_set->at(0));
    CPPUNIT_ASSERT(dicom_series);

    // On Unix, the correct series with 304 elements is placed first and the one with 196 elements is at last position,
    // which is the opposite on windows (???)... Assuming the Dicom is file based, the way the OS sorts files may
    // explain the different behaviors. We should investigate this ....
    // The test is written to assume the one of 304 elements is taken.
    if(dicom_series->get_number_of_instances() != 304)
    {
        dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series_set->at(1));
        CPPUNIT_ASSERT(dicom_series);
    }

    // Just in case we load the wrong series or the data is corrupted.
    CPPUNIT_ASSERT_EQUAL(std::size_t(304), dicom_series->get_number_of_instances());

    std::vector<data::dicom_series::sptr> dicom_series_container;
    dicom_series_container.push_back(dicom_series);

    // Sort instances according to instance number
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::sorter::instance_number_sorter"
    );
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, true);

    // Apply filter
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::splitter::image_position_patient_splitter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(37), dicom_series_container.size());
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
