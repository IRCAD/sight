/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "ruler_fiducials_reader_writer_test.hpp"

#include <core/os/temp_path.hpp>

#include <data/image_series.hpp>

#include <io/dicom/reader/file.hpp>
#include <io/dicom/writer/file.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::ruler_fiducials_reader_writer_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void ruler_fiducials_reader_writer_test::ruler_basic_test()
{
    auto original = std::make_shared<data::series_set>();
    auto reader   = std::make_shared<io::dicom::reader::file>();
    reader->set_object(original);
    reader->set_folder(utest_data::dir() / "us/Enhanced US Volume Storage/GE, 3D+t, lossy JPEG");
    CPPUNIT_ASSERT_NO_THROW(reader->read());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), original->size());

    auto original_image_series = std::dynamic_pointer_cast<data::image_series>(original->at(0));
    CPPUNIT_ASSERT(original_image_series);

    auto original_fiducials_series = original_image_series->get_fiducials();
    CPPUNIT_ASSERT(original_fiducials_series->get_fiducial_sets().empty());

    original_fiducials_series->set_content_date("1");
    original_fiducials_series->set_content_time("2");
    original_fiducials_series->set_instance_number(3);
    original_fiducials_series->set_content_label("4");
    original_fiducials_series->set_content_description("5");
    original_fiducials_series->set_content_creator_name("6");

    // Create a fiducial
    data::fiducials_series::fiducial fiducial;
    fiducial.shape_type           = data::fiducials_series::shape::ruler;
    fiducial.fiducial_identifier  = "8";
    fiducial.fiducial_description = "Distance";

    fiducial.fiducial_uid = "9";
    fiducial.contour_data = {
        {.x               = 20, .y = 21, .z = 22},
        {.x               = 30, .y = 31, .z = 32}
    };

    fiducial.graphic_coordinates_data_sequence = std::nullopt;

    // Create referenced_image
    sight::data::fiducials_series::referenced_image ri;
    ri.referenced_sop_class_uid    = original_image_series->get_sop_class_uid();
    ri.referenced_sop_instance_uid = original_image_series->get_sop_instance_uid();

    const int slice_index = 10;

    ri.referenced_frame_number.push_back(static_cast<std::int32_t>(slice_index));
    ri.referenced_segment_number.push_back(0);

    // Add fiducial and referenced_image data to fiducial_set
    data::fiducials_series::fiducial_set fiducial_set;
    fiducial_set.frame_of_reference_uid = "7";
    fiducial_set.fiducial_sequence.push_back(fiducial);

    fiducial_set.referenced_image_sequence = std::vector<sight::data::fiducials_series::referenced_image>();
    fiducial_set.referenced_image_sequence->push_back(ri);

    fiducial_set.group_name = "11";

    original_fiducials_series->append_fiducial_set(fiducial_set);

    const core::os::temp_dir folder;

    auto writer = std::make_shared<io::dicom::writer::file>();
    writer->set_object(original);
    writer->set_folder(folder);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    auto actual = std::make_shared<data::series_set>();
    reader->set_object(actual);
    reader->set_folder(folder);
    CPPUNIT_ASSERT_NO_THROW(reader->read());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), actual->size());

    auto actual_image_series = std::dynamic_pointer_cast<data::image_series>(actual->at(0));
    CPPUNIT_ASSERT(actual_image_series);
    auto actual_fiducials_series = actual_image_series->get_fiducials();
    CPPUNIT_ASSERT(actual_fiducials_series);
    std::vector<data::fiducials_series::fiducial_set> actual_fiducial_sets =
        actual_fiducials_series->get_fiducial_sets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), actual_fiducial_sets.size());

    CPPUNIT_ASSERT_EQUAL(original_fiducials_series->get_content_date(), actual_fiducials_series->get_content_date());
    CPPUNIT_ASSERT_EQUAL(original_fiducials_series->get_content_time(), actual_fiducials_series->get_content_time());
    CPPUNIT_ASSERT(original_fiducials_series->get_instance_number() == actual_fiducials_series->get_instance_number());
    CPPUNIT_ASSERT_EQUAL(original_fiducials_series->get_content_label(), actual_fiducials_series->get_content_label());
    CPPUNIT_ASSERT_EQUAL(
        original_fiducials_series->get_content_description(),
        actual_fiducials_series->get_content_description()
    );
    CPPUNIT_ASSERT_EQUAL(
        original_fiducials_series->get_content_creator_name(),
        actual_fiducials_series->get_content_creator_name()
    );
    CPPUNIT_ASSERT(fiducial_set == actual_fiducial_sets[0]);
}

} // namespace sight::io::dicom::ut
