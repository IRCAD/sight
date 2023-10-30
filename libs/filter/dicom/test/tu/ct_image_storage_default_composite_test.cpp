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

#include "ct_image_storage_default_composite_test.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/filter.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::ct_image_storage_default_composite_test);

namespace sight::filter::dicom::ut
{

//------------------------------------------------------------------------------

void ct_image_storage_default_composite_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ct_image_storage_default_composite_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ct_image_storage_default_composite_test::simple_application()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "08-CT-PACS";
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
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve DicomSeries
    data::dicom_series::sptr dicom_series_a = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    data::dicom_series::sptr dicom_series_b = std::make_shared<data::dicom_series>();
    dicom_series_b->deep_copy(dicom_series_a);
    CPPUNIT_ASSERT(dicom_series_a);
    CPPUNIT_ASSERT(dicom_series_b);
    std::vector<data::dicom_series::sptr> dicom_series_container_a;
    std::vector<data::dicom_series::sptr> dicom_series_container_b;
    dicom_series_container_a.push_back(dicom_series_a);
    dicom_series_container_b.push_back(dicom_series_b);

    // Apply composite filter on the first DicomSeries
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::composite::ct_image_storage_default_composite"
    );
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_a, filter, true);

    // Apply other filters on the second DicomSeries
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::splitter::acquisition_number_splitter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_b, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::sorter::instance_number_sorter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_b, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::sorter::image_position_patient_sorter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_b, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::splitter::image_position_patient_splitter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_b, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::sorter::image_position_patient_sorter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_b, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::modifier::slice_thickness_modifier");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container_b, filter, true);

    // Compare the two series
    CPPUNIT_ASSERT_EQUAL(dicom_series_container_a.size(), dicom_series_container_b.size());
    for(unsigned int j = 0 ; j < dicom_series_container_a.size() ; ++j) // For every series
    {
        for(unsigned int i = 0 ; i < dicom_series_container_a[j]->get_dicom_container().size() ; ++i) // For every
                                                                                                      // instances
        {
            CPPUNIT_ASSERT(
                std::memcmp(
                    dicom_series_container_a[j]->get_dicom_container().at(i)->buffer(),
                    dicom_series_container_b[j]->get_dicom_container().at(i)->buffer(),
                    dicom_series_container_a[j]->get_dicom_container().at(i)->size()
                ) == 0
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
