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

#include "SliceThicknessModifierTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/Filter.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/Data.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::SliceThicknessModifierTest);

namespace sight::filter::dicom::ut
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
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "09-CT-PACS";
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
    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    CPPUNIT_ASSERT(dicom_series);
    std::vector<data::dicom_series::sptr> dicom_series_container;
    dicom_series_container.push_back(dicom_series);

    // Sort instances according to instance number
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::sorter::InstanceNumberSorter"
    );
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicom_series_container, filter, true);

    // Apply filter
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::modifier::SliceThicknessModifier");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicom_series_container, filter, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dicom_series_container.size());
    dicom_series = dicom_series_container[0];

    // Check that the computed value has been added to the DicomSeries
    const double delta         = 1e-8;
    const auto slice_thickness =
        boost::lexical_cast<double>(dicom_series->getComputedTagValues().at("SliceThickness"));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8, slice_thickness, delta);
}

//------------------------------------------------------------------------------

void SliceThicknessModifierTest::applyFilterOn2DImage()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
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
    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    CPPUNIT_ASSERT(dicom_series);
    std::vector<data::dicom_series::sptr> dicom_series_container;
    dicom_series_container.push_back(dicom_series);

    // Apply filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::modifier::SliceThicknessModifier"
    );
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT(!sight::filter::dicom::helper::Filter::applyFilter(dicom_series_container, filter, false));
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
