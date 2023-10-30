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

#include "slice_thickness_modifier_test.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/filter.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/Data.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::slice_thickness_modifier_test);

namespace sight::filter::dicom::ut
{

//------------------------------------------------------------------------------

void slice_thickness_modifier_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void slice_thickness_modifier_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void slice_thickness_modifier_test::simple_application()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "09-CT-PACS";
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
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::modifier::slice_thickness_modifier");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dicom_series_container.size());
    dicom_series = dicom_series_container[0];

    // Check that the computed value has been added to the DicomSeries
    const double delta         = 1e-8;
    const auto slice_thickness =
        boost::lexical_cast<double>(dicom_series->get_computed_tag_values().at("SliceThickness"));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8, slice_thickness, delta);
}

//------------------------------------------------------------------------------

void slice_thickness_modifier_test::apply_filter_on_2d_image()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
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

    // Apply filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::modifier::slice_thickness_modifier"
    );
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT(!sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, false));
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
