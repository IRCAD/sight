/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "helper.hpp"

#include <core/os/temp_path.hpp>

#include <io/dicom/reader/file.hpp>
#include <io/dicom/writer/file.hpp>

#include <doctest/doctest.h>

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::dicom::fiducials_reader_writer")
{
    TEST_CASE("basic")
    {
        auto original_image_series = get_us_volume_image_copy(0xFFFF, 2);
        CHECK(original_image_series);

        auto original_fiducials_series = original_image_series->get_fiducials();
        CHECK(original_fiducials_series->get_fiducial_sets().empty());

        original_fiducials_series->set_content_date("1");
        original_fiducials_series->set_content_time("2");
        original_fiducials_series->set_instance_number(3);
        original_fiducials_series->set_content_label("4");
        original_fiducials_series->set_content_description("5");
        original_fiducials_series->set_content_creator_name("6");

        data::fiducials_series::fiducial_set fiducial_set;

        data::fiducials_series::referenced_image referenced_image;
        referenced_image.referenced_sop_class_uid =
            data::dicom::sop::get(original_image_series->get_sop_keyword()).m_uid;
        referenced_image.referenced_sop_instance_uid = original_image_series->get_sop_instance_uid();
        referenced_image.referenced_frame_number     = {7};
        referenced_image.referenced_segment_number   = {8};
        fiducial_set.referenced_image_sequence       = {referenced_image};

        fiducial_set.frame_of_reference_uid = "9";

        data::fiducials_series::fiducial fiducial;
        fiducial.shape_type           = data::fiducials_series::shape::point;
        fiducial.fiducial_description = "10";
        fiducial.fiducial_identifier  = "11";

        data::fiducials_series::graphic_coordinates_data graphic_coordinates_data;
        graphic_coordinates_data.referenced_image_sequence.referenced_sop_class_uid = data::dicom::sop::get(
            original_image_series->get_sop_keyword()
        ).m_uid;
        graphic_coordinates_data.referenced_image_sequence.referenced_sop_instance_uid =
            original_image_series->get_sop_instance_uid();
        graphic_coordinates_data.referenced_image_sequence.referenced_frame_number   = {12};
        graphic_coordinates_data.referenced_image_sequence.referenced_segment_number = {13};
        graphic_coordinates_data.graphic_data                                        = {{.x = 14, .y = 15}};
        fiducial.graphic_coordinates_data_sequence                                   = {graphic_coordinates_data};

        fiducial.fiducial_uid = "16";
        fiducial.contour_data = {{.x = 17, .y = 18, .z = 19}};
        fiducial_set.fiducial_sequence.push_back(fiducial);

        fiducial_set.group_name = "20";

        original_fiducials_series->set_sop_instance_uid(
            std::string(
                data::dicom::sop::get(
                    data::dicom::sop::Keyword::
                    SpatialFiducialsStorage
                ).m_uid
            ) + ".0"
        );
        original_fiducials_series->append_fiducial_set(fiducial_set);

        auto original = std::make_shared<data::series_set>();
        original->push_back(original_image_series);

        auto writer = std::make_shared<io::dicom::writer::file>();
        writer->set_object(original);

        const core::os::temp_dir folder;
        writer->set_folder(folder);
        {
            const auto observer = std::make_shared<core::progress::observer>("FIDUCIALS Writer Test");
            CHECK_NOTHROW(writer->write(observer));
        }

        auto actual = std::make_shared<data::series_set>();
        auto reader = std::make_shared<io::dicom::reader::file>();
        reader->set_object(actual);
        reader->set_folder(folder);

        {
            const auto observer = std::make_shared<core::progress::observer>("FIDUCIALS Reader Test");
            CHECK_NOTHROW(reader->read(observer));
        }
        CHECK_EQ(std::size_t(1), actual->size());

        auto actual_image_series = std::dynamic_pointer_cast<data::image_series>(actual->at(0));
        CHECK(actual_image_series);
        auto actual_fiducials_series = actual_image_series->get_fiducials();
        CHECK(actual_fiducials_series);
        std::vector<data::fiducials_series::fiducial_set> actual_fiducial_sets =
            actual_fiducials_series->get_fiducial_sets();
        REQUIRE_EQ(std::size_t(1), actual_fiducial_sets.size());

        CHECK_EQ(original_fiducials_series->get_content_date(), actual_fiducials_series->get_content_date());
        CHECK_EQ(original_fiducials_series->get_content_time(), actual_fiducials_series->get_content_time());
        CHECK(original_fiducials_series->get_instance_number() == actual_fiducials_series->get_instance_number());
        CHECK_EQ(original_fiducials_series->get_content_label(), actual_fiducials_series->get_content_label());
        CHECK_EQ(
            original_fiducials_series->get_content_description(),
            actual_fiducials_series->get_content_description()
        );
        CHECK_EQ(
            original_fiducials_series->get_content_creator_name(),
            actual_fiducials_series->get_content_creator_name()
        );
        CHECK(fiducial_set == actual_fiducial_sets[0]);
    }
}

} // namespace sight::io::dicom::ut
