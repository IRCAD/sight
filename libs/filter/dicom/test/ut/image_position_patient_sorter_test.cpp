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

#include "image_position_patient_sorter_test.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/filter.hpp>

#include <geometry/data/vector_functions.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include <gdcmImageReader.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::image_position_patient_sorter_test);

namespace sight::filter::dicom::ut
{

//------------------------------------------------------------------------------

void image_position_patient_sorter_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void image_position_patient_sorter_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

double get_instance_z_position(const core::memory::buffer_object::sptr& _buffer_obj)
{
    gdcm::ImageReader reader;
    const core::memory::buffer_manager::stream_info stream_info = _buffer_obj->get_stream_info();
    reader.SetStream(*stream_info.stream);

    if(!reader.Read())
    {
        return 0;
    }

    // Retrieve dataset
    const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

    // Check tags availability
    if(!dataset.FindDataElement(gdcm::Tag(0x0020, 0x0032)) || !dataset.FindDataElement(gdcm::Tag(0x0020, 0x0037)))
    {
        const std::string msg = "Unable to compute the spacing between slices of the series.";
        throw io::dicom::exception::failed(msg);
    }

    // Retrieve image position
    const gdcm::Image& gdcm_image = reader.GetImage();
    const double* gdcm_origin     = gdcm_image.GetOrigin();
    const fw_vec3d image_position = {{gdcm_origin[0], gdcm_origin[1], gdcm_origin[2]}};

    // Retrieve image orientation
    const double* direction_cosines    = gdcm_image.GetDirectionCosines();
    const fw_vec3d image_orientation_u = {{
        std::round(direction_cosines[0]),
        std::round(direction_cosines[1]),
        std::round(direction_cosines[2])
    }
    };
    const fw_vec3d image_orientation_v = {{
        std::round(direction_cosines[3]),
        std::round(direction_cosines[4]),
        std::round(direction_cosines[5])
    }
    };

    //Compute Z direction (cross product)
    const fw_vec3d z_vector = geometry::data::cross(image_orientation_u, image_orientation_v);

    //Compute dot product to get the index
    const double index = geometry::data::dot(image_position, z_vector);

    return index;
}

//-----------------------------------------------------------------------------

void image_position_patient_sorter_test::simple_application()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "01-CT-DICOM_LIVER";
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
    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    CPPUNIT_ASSERT(dicom_series);
    std::vector<data::dicom_series::sptr> dicom_series_container;
    dicom_series_container.push_back(dicom_series);

    // Apply filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::sorter::image_position_patient_sorter"
    );
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT_NO_THROW(sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, true));

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), dicom_series_container.size());
    dicom_series = dicom_series_container[0];

    double old_position = -1.0;

    const data::dicom_series::dicom_container_t& dicom_container = dicom_series->get_dicom_container();
    for(std::size_t index = dicom_series->get_first_instance_number() ; index < dicom_series->num_instances() ; ++index)
    {
        const double position = get_instance_z_position(dicom_container.at(index));

        // Check that the position is bigger that the previous one
        CPPUNIT_ASSERT_LESS(position, old_position);
        old_position = position;
    }
}

//------------------------------------------------------------------------------

void image_position_patient_sorter_test::apply_filter_on_multiple_volume_image()
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
    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    CPPUNIT_ASSERT(dicom_series);
    std::vector<data::dicom_series::sptr> dicom_series_container;
    dicom_series_container.push_back(dicom_series);

    // Apply filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::sorter::image_position_patient_sorter"
    );
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT_THROW(
        sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, false),
        sight::filter::dicom::exceptions::filter_failure
    );
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
