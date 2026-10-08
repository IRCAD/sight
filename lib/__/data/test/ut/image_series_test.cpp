/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/type.hpp>

#include <data/image_series.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <gdcmTagKeywords.h>

TEST_SUITE("sight::data::image_series")
{
//------------------------------------------------------------------------------

    TEST_CASE("image")
    {
        const auto series = std::make_shared<sight::data::image_series>();

        sight::utest_data::generator::image::generate_random_image(series, sight::core::type::FLOAT32);

        auto series2 = std::make_shared<sight::data::image_series>();
        CHECK(*series2 != *series);

        series2->shallow_copy(series);
        CHECK(*series2 == *series);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto series1 = std::make_shared<sight::data::image_series>();
        auto series2 = std::make_shared<sight::data::image_series>();

        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_origin({1.0, 2.0, 3.0});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_origin(series1->origin());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_orientation({0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_orientation(series1->orientation());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Also test origin and orientation with a different SOP.
        // A different SOP means that the data may be stored in a different location in DICOM data
        series1->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        series2->set_sop_keyword(series1->get_sop_keyword());
        series1->set_origin({4.0, 5.0, 6.0});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_origin(series1->origin());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));
        series1->set_orientation({0, 1, 0, -1, 0, 0, 0, 0, 1});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_orientation(series1->orientation());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_modality(sight::data::dicom::modality_t::us);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_modality(series1->get_modality());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_series_instance_uid("2");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_series_instance_uid(series1->get_series_instance_uid());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_series_number(3);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_series_number(series1->get_series_number());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_laterality("4");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_laterality(series1->get_laterality());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_series_date("5");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_series_date(series1->get_series_date());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_series_time("6");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_series_time(series1->get_series_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performing_physician_name("7\\8\\9");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performing_physician_name(series1->get_performing_physician_name());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_protocol_name("10");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_protocol_name(series1->get_protocol_name());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_series_description("11");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_series_description(series1->get_series_description());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_body_part_examined("12");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_body_part_examined(series1->get_body_part_examined());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_position("13");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_position(series1->get_patient_position());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_anatomical_orientation_type("14");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_anatomical_orientation_type(series1->get_anatomical_orientation_type());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performed_procedure_step_id("15");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performed_procedure_step_id(series1->get_performed_procedure_step_id());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performed_procedure_step_start_date("16");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performed_procedure_step_start_date(series1->get_performed_procedure_step_start_date());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performed_procedure_step_start_time("17");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performed_procedure_step_start_time(series1->get_performed_procedure_step_start_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performed_procedure_step_end_date("18");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performed_procedure_step_end_date(series1->get_performed_procedure_step_end_date());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performed_procedure_step_end_time("19");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performed_procedure_step_end_time(series1->get_performed_procedure_step_end_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_performed_procedure_step_description("20");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_performed_procedure_step_description(series1->get_performed_procedure_step_description());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_comments_on_the_performed_procedure_step("21");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_comments_on_the_performed_procedure_step(series1->get_comments_on_the_performed_procedure_step());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_id("22");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_id(series1->get_patient_id());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_study_instance_uid("23");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_study_instance_uid(series1->get_study_instance_uid());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_institution_name("24");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_institution_name(series1->get_institution_name());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        sight::utest_data::generator::image::generate_random_image(series1, sight::core::type::FLOAT32);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->shallow_copy(series1);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_agent("26");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_agent(series1->get_contrast_bolus_agent());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_route("27");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_route(series1->get_contrast_bolus_route());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_volume(28.0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_volume(series1->get_contrast_bolus_volume());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_start_time("29");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_start_time(series1->get_contrast_bolus_start_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_stop_time("30");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_stop_time(series1->get_contrast_bolus_stop_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_total_dose(31.0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_total_dose(series1->get_contrast_bolus_total_dose());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_flow_rate("32.0\\32.1\\32.2");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_flow_rate(series1->get_contrast_flow_rate());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_flow_duration("33.0\\33.1\\33.2");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_flow_duration(series1->get_contrast_flow_duration());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_ingredient("34");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_ingredient(series1->get_contrast_bolus_ingredient());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_contrast_bolus_ingredient_concentration(35.0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_contrast_bolus_ingredient_concentration(series1->get_contrast_bolus_ingredient_concentration());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_acquisition_date("36");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_acquisition_date(series1->get_acquisition_date());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_acquisition_time("37");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_acquisition_time(series1->get_acquisition_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_window_center({38, 39});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_window_center(series1->window_center());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_window_width({40, 41});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_window_width(series1->window_width());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_rows(static_cast<std::uint16_t>(42));
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_rows(series1->get_rows());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_columns(static_cast<std::uint16_t>(43));
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_columns(series1->get_columns());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Test also deepcopy, just for fun
        auto series3 = std::make_shared<sight::data::image_series>();
        series3->deep_copy(series1);
        CHECK(*series1 == *series3);
        CHECK(!(*series1 != *series3));
    }

//------------------------------------------------------------------------------

    TEST_CASE("resize")
    {
        auto series1 = std::make_shared<sight::data::image_series>();
        series1->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        do
        {
            sight::utest_data::generator::image::generate_random_image(series1, sight::core::type::INT8);
        }
        while(series1->size()[2] <= 2);

        auto size = series1->size();

        for(std::size_t i = 0, end = size[2] ; i < end ; ++i)
        {
            series1->set_image_position_patient(
                {0.1 * static_cast<double>(i), 0.2 * static_cast<double>(i), 0.3 * static_cast<double>(i)
                },
                i
            );
            series1->set_frame_comments("Comments" + std::to_string(i), i);
            series1->set_frame_label("Label" + std::to_string(i), i);
        }

        auto series2 = std::make_shared<sight::data::image_series>();
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));

        series2->deep_copy(series1);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Resize to 2 slices
        size[2] = 2;
        series1->resize(size, series1->type(), series1->pixel_format());

        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));

        CHECK(series2->get_image_position_patient(0) == series1->get_image_position_patient(0));
        CHECK(series2->get_image_position_patient(1) == series1->get_image_position_patient(1));
        CHECK(series2->get_image_position_patient(2) != series1->get_image_position_patient(2));
        CHECK(series1->get_image_position_patient(2).empty());

        CHECK_EQ(*series2->get_frame_comments(0), *series1->get_frame_comments(0));
        CHECK_EQ(*series2->get_frame_comments(1), *series1->get_frame_comments(1));
        CHECK(series2->get_frame_comments(2));
        CHECK(!series1->get_frame_comments(2));

        CHECK_EQ(*series2->get_frame_label(0), *series1->get_frame_label(0));
        CHECK_EQ(*series2->get_frame_label(1), *series1->get_frame_label(1));
        CHECK(series2->get_frame_label(2));
        CHECK(!series1->get_frame_label(2));

        series2->deep_copy(series1);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));
    }

//------------------------------------------------------------------------------

    TEST_CASE("origin_orientation")
    {
        // In row major order
        static constexpr sight::data::image::origin_t s_ORIGIN {1.0, 2.0, 3.0};
        static constexpr sight::data::image::orientation_t s_ORIENTATION {
            0.36, -0.8, 0.48,
            0.48, 0.6, 0.64,
            -0.8, 0.0, 0.6
        };

        // Test no defined SOP
        {
            auto image_series = std::make_shared<sight::data::image_series>();

            // Set values using standard setters and check standard getters
            image_series->set_origin(s_ORIGIN);
            CHECK(sight::core::is_equal(image_series->origin(), s_ORIGIN));

            image_series->set_orientation(s_ORIENTATION);
            CHECK(sight::core::is_equal(image_series->orientation(), s_ORIENTATION));

            // Check also DICOM values
            const auto& dataset = image_series->get_data_set();

            gdcm::Keywords::ImagePositionPatient position_attribute;
            position_attribute.SetFromDataSet(dataset);
            const auto position = std::vector(
                position_attribute.GetValues(),
                position_attribute.GetValues() + position_attribute.GetNumberOfValues()
            );

            CHECK(sight::core::is_equal(s_ORIGIN, position));

            gdcm::Keywords::ImageOrientationPatient orientation_attribute;
            orientation_attribute.SetFromDataSet(dataset);
            const auto orientation = std::vector(
                orientation_attribute.GetValues(),
                orientation_attribute.GetValues() + orientation_attribute.GetNumberOfValues()
            );

            // DICOM only uses 6 cosines in column-major order
            CHECK_EQ(s_ORIENTATION[0], doctest::Approx(orientation[0]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[1], doctest::Approx(orientation[3]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[3], doctest::Approx(orientation[1]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[4], doctest::Approx(orientation[4]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[6], doctest::Approx(orientation[2]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[7], doctest::Approx(orientation[5]).epsilon(1e-6));
        }

        // Test EnhancedUSVolumeStorage with no acquisition geometry
        {
            auto image_series = std::make_shared<sight::data::image_series>();
            image_series->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

            // Set values using standard setters and check standard getters
            image_series->set_origin(s_ORIGIN);
            CHECK(sight::core::is_equal(image_series->origin(), s_ORIGIN));

            image_series->set_orientation(s_ORIENTATION);
            CHECK(sight::core::is_equal(image_series->orientation(), s_ORIENTATION));

            // Check also DICOM values
            const auto& dataset = image_series->get_data_set();

            gdcm::Keywords::ImagePositionPatient position_attribute;
            position_attribute.SetFromDataSet(dataset);
            const auto position = std::vector(
                position_attribute.GetValues(),
                position_attribute.GetValues() + position_attribute.GetNumberOfValues()
            );

            CHECK(sight::core::is_equal(s_ORIGIN, position));

            gdcm::Keywords::ImageOrientationPatient orientation_attribute;
            orientation_attribute.SetFromDataSet(dataset);
            const auto orientation = std::vector(
                orientation_attribute.GetValues(),
                orientation_attribute.GetValues() + orientation_attribute.GetNumberOfValues()
            );

            // DICOM only uses 6 cosines in column-major order
            CHECK_EQ(s_ORIENTATION[0], doctest::Approx(orientation[0]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[1], doctest::Approx(orientation[3]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[3], doctest::Approx(orientation[1]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[4], doctest::Approx(orientation[4]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[6], doctest::Approx(orientation[2]).epsilon(1e-6));
            CHECK_EQ(s_ORIENTATION[7], doctest::Approx(orientation[5]).epsilon(1e-6));
        }

        // Test EnhancedUSVolumeStorage in APEX acquisition geometry
        {
            auto image_series = std::make_shared<sight::data::image_series>();
            image_series->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
            image_series->set_ultrasound_acquisition_geometry(
                sight::data::dicom::ultrasound_acquisition_geometry_t::apex
            );

            // Set values using standard setters and check standard getters
            image_series->set_origin(s_ORIGIN);
            CHECK(sight::core::is_equal(image_series->origin(), s_ORIGIN));

            image_series->set_orientation(s_ORIENTATION);
            CHECK(sight::core::is_equal(image_series->orientation(), s_ORIENTATION));

            // Check also DICOM values
            const auto& dataset = image_series->get_data_set();

            gdcm::Keywords::VolumeToTransducerMappingMatrix attribute;
            attribute.SetFromDataSet(dataset);

            const auto values = std::vector(
                attribute.GetValues(),
                attribute.GetValues() + attribute.GetNumberOfValues()
            );

            sight::data::matrix4 mapping(values);
            CHECK(sight::core::is_equal(s_ORIGIN, mapping.position()));
            CHECK(sight::core::is_equal(s_ORIENTATION, mapping.orientation()));
        }

        // Test EnhancedUSVolumeStorage in patient acquisition geometry
        {
            auto image_series = std::make_shared<sight::data::image_series>();
            image_series->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
            image_series->set_ultrasound_acquisition_geometry(
                sight::data::dicom::ultrasound_acquisition_geometry_t::patient
            );

            // Set values using standard setters and check standard getters
            image_series->set_origin(s_ORIGIN);
            CHECK(sight::core::is_equal(image_series->origin(), s_ORIGIN));

            image_series->set_orientation(s_ORIENTATION);
            CHECK(sight::core::is_equal(image_series->orientation(), s_ORIENTATION));

            // Check also DICOM values
            const auto& dataset = image_series->get_data_set();

            gdcm::Keywords::VolumeToTableMappingMatrix attribute;
            attribute.SetFromDataSet(dataset);

            const auto values = std::vector(
                attribute.GetValues(),
                attribute.GetValues() + attribute.GetNumberOfValues()
            );

            sight::data::matrix4 mapping(values);
            CHECK(sight::core::is_equal(s_ORIGIN, mapping.position()));
            CHECK(sight::core::is_equal(s_ORIENTATION, mapping.orientation()));
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::image_series")
