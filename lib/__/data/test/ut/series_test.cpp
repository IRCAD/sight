/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/compare.hpp>
#include <core/tools/uuid.hpp>

#include <data/dicom/attribute.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <boost/algorithm/string/join.hpp>

#include <doctest/doctest.h>

using uuid = sight::core::tools::uuid;

namespace
{

struct fixture
{
    fixture()
    {
        m_series.push_back(std::make_shared<sight::data::image_series>());
        m_series.push_back(std::make_shared<sight::data::model_series>());
    }

    std::vector<sight::data::series::sptr> m_series;
};

} // namespace

TEST_SUITE("sight::data::series")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_patient")
    {
        const std::string patient_name       = "John DOE";
        const std::string patient_id         = "42";
        const std::string patient_birth_date = "02-14-2015 11:28";
        const std::string patient_sex        = "M";

        for(const auto& series : m_series)
        {
            series->set_patient_name(patient_name);
            CHECK(series->get_patient_name() == patient_name);

            series->set_patient_id(patient_id);
            CHECK(series->get_patient_id() == patient_id);

            series->set_patient_birth_date(patient_birth_date);
            CHECK(series->get_patient_birth_date() == patient_birth_date);

            series->set_patient_sex(patient_sex);
            CHECK(series->get_patient_sex() == patient_sex);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_study")
    {
        const std::string instance_uid = "1346357.1664.643101.421337.4123403";
        const std::string date         = "02-14-2015";
        const std::string time         = "11:59";
        const std::string accession    = "2819497684894126";
        const std::string rpn          = "Dr^Jekyl";
        const std::string desc         = "Say 33.";
        const std::string age          = "42";

        for(const auto& series : m_series)
        {
            series->set_study_instance_uid(instance_uid);
            CHECK(series->get_study_instance_uid() == instance_uid);

            series->set_study_date(date);
            CHECK(series->get_study_date() == date);

            series->set_study_time(time);
            CHECK(series->get_study_time() == time);

            series->set_accession_number(accession);
            CHECK(series->get_accession_number() == accession);

            series->set_referring_physician_name(rpn);
            CHECK(series->get_referring_physician_name() == rpn);

            series->set_study_description(desc);
            CHECK(series->get_study_description() == desc);

            series->set_patient_age(age);
            CHECK(series->get_patient_age() == age);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_equipment")
    {
        constexpr auto institution_name  = "IRCAD";
        constexpr auto manufacturer_name = "IRCAD";
        constexpr auto model_name        = "device-0";
        constexpr auto serial_number     = "S/N:N/A";
        std::vector<std::string> software_versions {"Sight-X.Y"};

        for(const auto& series : m_series)
        {
            series->set_institution_name(institution_name);
            CHECK(series->get_institution_name() == institution_name);

            series->set_equipment_manufacturer(manufacturer_name);
            CHECK(series->get_equipment_manufacturer() == manufacturer_name);

            series->set_equipment_manufacturer_model_name(model_name);
            CHECK(series->get_equipment_manufacturer_model_name() == model_name);

            series->set_equipment_device_serial_number(serial_number);
            CHECK(series->get_equipment_device_serial_number() == serial_number);

            series->set_software_versions(software_versions);
            CHECK(series->get_software_versions() == software_versions);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_instance_uid")
    {
        const std::string instance_uid = "1337.1664.42";

        for(const auto& series : m_series)
        {
            series->set_series_instance_uid(instance_uid);
            CHECK_EQ(instance_uid, series->get_series_instance_uid());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_modality")
    {
        for(const auto& series : m_series)
        {
            series->set_modality(sight::data::dicom::modality_t::mr);

            CHECK_EQ(sight::data::dicom::modality_t::mr, series->get_modality());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_date")
    {
        const std::string date = "02-14-2015";

        for(const auto& series : m_series)
        {
            series->set_series_date(date);
            CHECK_EQ(date, series->get_series_date());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_time")
    {
        const std::string time = "14:02";

        for(const auto& series : m_series)
        {
            series->set_series_time(time);
            CHECK_EQ(time, series->get_series_time());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_performing_physician_name")
    {
        const std::string performing_physician_name =
            "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John";

        for(const auto& series : m_series)
        {
            series->set_performing_physician_name(performing_physician_name);
            CHECK(performing_physician_name == series->get_performing_physician_name());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_description")
    {
        const std::string description = "Series";

        for(const auto& series : m_series)
        {
            series->set_series_description(description);
            CHECK_EQ(description, series->get_series_description());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_enhanced_us_image")
    {
        int depth_mm = 150;
        std::vector<double> focus_depths_mm {8, 64};
        std::string processing_function     = "gain: 54;";
        auto position_measuring_device_used = sight::data::dicom::position_measuring_device_used_t::tracked;

        for(const auto& series : m_series)
        {
            series->set_depth_of_scan_field_mm(depth_mm);
            CHECK(depth_mm == series->get_depth_of_scan_field_mm());

            series->set_depths_of_focus_mm(focus_depths_mm);
            CHECK(focus_depths_mm.size() == series->get_depths_of_focus_mm().size());
            for(std::size_t i = 0 ; i < focus_depths_mm.size() ; i++)
            {
                CHECK_EQ(focus_depths_mm.at(i), doctest::Approx(series->get_depths_of_focus_mm().at(i)).epsilon(1.e-5));
            }

            series->set_processing_function(processing_function);
            CHECK(processing_function == series->get_processing_function());

            position_measuring_device_used = sight::data::dicom::position_measuring_device_used_t::tracked;
            CHECK_NOTHROW(series->set_position_measuring_device_used(position_measuring_device_used));
            CHECK(position_measuring_device_used == series->get_position_measuring_device_used());

            position_measuring_device_used = sight::data::dicom::position_measuring_device_used_t::rigid;
            CHECK_NOTHROW(series->set_position_measuring_device_used(position_measuring_device_used));
            CHECK(position_measuring_device_used == series->get_position_measuring_device_used());

            position_measuring_device_used = sight::data::dicom::position_measuring_device_used_t::freehand;
            CHECK_NOTHROW(series->set_position_measuring_device_used(position_measuring_device_used));
            CHECK(position_measuring_device_used == series->get_position_measuring_device_used());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_general_acquisition")
    {
        double duration = 123.456;

        for(const auto& series : m_series)
        {
            series->set_acquisition_duration(duration);
            auto dt = series->get_acquisition_duration();
            CHECK(dt.has_value());
            CHECK_EQ(duration, doctest::Approx(*dt).epsilon(0.0001));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "attr_ultrasound_frame_of_reference")
    {
        sight::data::matrix4 volume_to_transducer_mapping_matrix {
            10.0, 0.0, 0.0, 0.0,
            0.0, 15.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        };

        for(const auto& series : m_series)
        {
            series->set_volume_to_transducer_mapping_matrix(volume_to_transducer_mapping_matrix);
            auto matrix = series->get_volume_to_transducer_mapping_matrix();
            CHECK(matrix.has_value());
            for(std::size_t i = 0 ; i < volume_to_transducer_mapping_matrix.size() ; i++)
            {
                CHECK_EQ(volume_to_transducer_mapping_matrix.at(i), doctest::Approx((*matrix).at(i)).epsilon(0.0001));
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "equality")
    {
        auto series1 = std::make_shared<sight::data::series>();
        auto series2 = std::make_shared<sight::data::series>();

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

        series1->set_patient_birth_date("22");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_birth_date(series1->get_patient_birth_date());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_name("23");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_name(series1->get_patient_name());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_sex("24");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_sex(series1->get_patient_sex());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_id("25");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_id(series1->get_patient_id());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_study_instance_uid("26");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_study_instance_uid(series1->get_study_instance_uid());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_study_date("28");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_study_date(series1->get_study_date());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_study_description("29");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_study_description(series1->get_study_description());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_age("30");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_age(series1->get_patient_age());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_size(32.0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_size(series1->get_patient_size());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_patient_weight(33.0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_weight(series1->get_patient_weight());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_referring_physician_name("34");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_referring_physician_name(series1->get_referring_physician_name());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_study_id("36");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_study_id(series1->get_study_id());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_study_time("37");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_study_time(series1->get_study_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_institution_name("38");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_institution_name(series1->get_institution_name());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_sop_keyword(series1->get_sop_keyword());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_sop_instance_uid("40");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_sop_instance_uid(series1->get_sop_instance_uid());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_specific_character_set("41");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_specific_character_set(series1->get_specific_character_set());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_content_time("42");
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_content_time(series1->get_content_time());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_instance_number(43);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_instance_number(series1->get_instance_number());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_acquisition_number(44);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_acquisition_number(series1->get_acquisition_number());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_rescale_intercept(45);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_rescale_intercept(series1->get_rescale_intercept());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_rescale_slope(46);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_rescale_slope(series1->get_rescale_slope());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_image_position_patient({47, 48, 49}, 0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_image_position_patient(series1->get_image_position_patient(0), 0);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_image_position_patient({50, 51, 52}, std::nullopt);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_image_position_patient(series1->get_image_position_patient(std::nullopt), std::nullopt);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_image_orientation_patient({50, 51, 52, 53, 54, 55}, 0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_image_orientation_patient(series1->get_image_orientation_patient(0), 0);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_image_orientation_patient({56, 57, 58, 59, 60, 61}, std::nullopt);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_image_orientation_patient(series1->get_image_orientation_patient(std::nullopt), std::nullopt);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_slice_thickness(56);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_slice_thickness(series1->get_slice_thickness());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        CHECK_NOTHROW(
            series1->set_ultrasound_acquisition_geometry(
                sight::data::dicom::ultrasound_acquisition_geometry_t::patient
            )
        );

        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_ultrasound_acquisition_geometry(series1->get_ultrasound_acquisition_geometry());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        CHECK_NOTHROW(
            series1->set_patient_frame_of_reference_source(
                sight::data::dicom::patient_frame_of_reference_source_t::table
            )
        );

        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_patient_frame_of_reference_source(series1->get_patient_frame_of_reference_source());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        CHECK_NOTHROW(
            series1->set_dimension_organization_type(
                sight::data::dicom::dimension_organization_t::volume
            )
        );

        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_dimension_organization_type(series1->get_dimension_organization_type());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        CHECK_NOTHROW(series1->set_referenced_sop_class_uid("54b"));
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_referenced_sop_class_uid(series1->get_referenced_sop_class_uid());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        CHECK_NOTHROW(series1->set_referenced_sop_instance_uid("54c"));
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_referenced_sop_instance_uid(series1->get_referenced_sop_instance_uid());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_slice_thickness(0.55);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_slice_thickness(series1->get_slice_thickness());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_frame_acquisition_date_time("57", 0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_frame_acquisition_date_time(series1->get_frame_acquisition_date_time(0), 0);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_frame_acquisition_date_time("57b", std::nullopt);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_frame_acquisition_date_time(series1->get_frame_acquisition_date_time(std::nullopt), std::nullopt);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_frame_comments("58", 0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_frame_comments(series1->get_frame_comments(0), 0);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_frame_comments("58b", std::nullopt);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_frame_comments(series1->get_frame_comments(std::nullopt), std::nullopt);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Frame label
        series1->set_frame_label("59", 0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_frame_label(series1->get_frame_label(0), 0);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_frame_label("59b", std::nullopt);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_frame_label(series1->get_frame_label(std::nullopt), std::nullopt);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Spacing between slices
        series1->set_spacing_between_slices(1.0, 0);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_spacing_between_slices(series1->get_spacing_between_slices(0), 0);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_spacing_between_slices(1.0, std::nullopt);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_spacing_between_slices(series1->get_spacing_between_slices(std::nullopt), std::nullopt);
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Apex position
        series1->set_apex_position({60.0, 61.0, 62.0});
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_apex_position(series1->get_apex_position());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        sight::data::matrix4 matrix;
        series1->set_volume_to_transducer_mapping_matrix(matrix);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_volume_to_transducer_mapping_matrix(series1->get_volume_to_transducer_mapping_matrix());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_volume_to_table_mapping_matrix(matrix);
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_volume_to_table_mapping_matrix(series1->get_volume_to_table_mapping_matrix());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        series1->set_image_type(
        {
            .pixel_data_characteristics          = sight::data::dicom::pixel_data_characteristics_t::derived,
            .patient_examination_characteristics = sight::data::dicom::patient_examination_characteristics_t::secondary,
            .other_values                        = {"AXIAL"}
        });
        CHECK(*series1 != *series2);
        CHECK(!(*series1 == *series2));
        series2->set_image_type(series1->get_image_type());
        CHECK(*series1 == *series2);
        CHECK(!(*series1 != *series2));

        // Test also deepcopy, just for fun
        auto series3 = std::make_shared<sight::data::series>();
        series3->deep_copy(series1);
        CHECK(*series1 == *series3);
        CHECK(!(*series1 != *series3));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "sop_class_uid")
    {
        static const sight::data::dicom::sop::Keyword s_KEYWORD = sight::data::dicom::sop::Keyword::CTImageStorage;

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_sop_keyword(s_KEYWORD);
            CHECK_EQ(s_KEYWORD, series->get_sop_keyword());
            CHECK_EQ(
                s_KEYWORD,
                sight::data::dicom::sop::get(
                    series->get_byte_value(
                        sight::data::dicom::attribute::Keyword::SOPClassUID
                    )
                ).m_keyword
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::SOPClassUID,
                std::string(sight::data::dicom::sop::get(s_KEYWORD).m_uid)
            );
            CHECK_EQ(s_KEYWORD, series->get_sop_keyword());
            CHECK_EQ(
                s_KEYWORD,
                sight::data::dicom::sop::get(
                    series->get_byte_value(
                        sight::data::dicom::attribute::Keyword::SOPClassUID
                    )
                ).m_keyword
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "sop_instance_uid")
    {
        static const std::string s_SOP_INSTANCE_UID(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_sop_instance_uid(s_SOP_INSTANCE_UID);
            CHECK_EQ(s_SOP_INSTANCE_UID, series->get_sop_instance_uid());
            CHECK_EQ(
                s_SOP_INSTANCE_UID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SOPInstanceUID)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::SOPInstanceUID, s_SOP_INSTANCE_UID);
            CHECK_EQ(s_SOP_INSTANCE_UID, series->get_sop_instance_uid());
            CHECK_EQ(
                s_SOP_INSTANCE_UID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SOPInstanceUID)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "specific_character_set")
    {
        static const std::string s_SPECIFIC_CHARACTER_SET("ISO 2022\\IR 100");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_specific_character_set(s_SPECIFIC_CHARACTER_SET);
            CHECK_EQ(s_SPECIFIC_CHARACTER_SET, series->get_specific_character_set());
            CHECK_EQ(
                s_SPECIFIC_CHARACTER_SET,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SpecificCharacterSet)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::SpecificCharacterSet,
                s_SPECIFIC_CHARACTER_SET
            );
            CHECK_EQ(s_SPECIFIC_CHARACTER_SET, series->get_specific_character_set());
            CHECK_EQ(
                s_SPECIFIC_CHARACTER_SET,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SpecificCharacterSet)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "encoding")
    {
        static const std::array<std::pair<std::string, std::string>, 19> s_CHARACTER_SET {{
            {"GB18030", "GB18030"},
            {"GBK", "GBK"},
            {"ISO 2022\\IR 6", "UTF-8"},
            {"ISO 2022\\IR 100", "ISO-8859-1"},
            {"ISO 2022\\IR 101", "ISO-8859-2"},
            {"ISO 2022\\IR 109", "ISO-8859-3"},
            {"ISO 2022\\IR 110", "ISO-8859-4"},
            {"ISO 2022\\IR 144", "ISO-8859-5"},
            {"ISO 2022\\IR 127", "ISO-8859-6"},
            {"ISO 2022\\IR 126", "ISO-8859-7"},
            {"ISO 2022\\IR 138", "ISO-8859-8"},
            {"ISO 2022\\IR 148", "ISO-8859-9"},
            {"ISO 2022\\IR 13", "Shift_JIS"},
            {"ISO 2022\\IR 166", "TIS-620"},
            {"ISO 2022\\IR 192", "UTF-8"},
            {"ISO 2022\\IR 87", "ISO-2022-JP-1"},
            {"ISO 2022\\IR 159", "ISO-2022-JP-2"},
            {"ISO 2022\\IR 149", "ISO-2022-KR"},
            {"ISO 2022\\IR 58", "ISO-2022-CN"}
        }
        };

        {
            auto series = std::make_shared<sight::data::series>();

            for(const auto& character_set : s_CHARACTER_SET)
            {
                series->set_specific_character_set(character_set.first);
                CHECK_EQ(character_set.second, series->get_encoding());
            }
        }

        {
            // By default, UTF-8 should be used...
            auto series = std::make_shared<sight::data::series>();
            CHECK_EQ(std::string("UTF-8"), series->get_encoding());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "series_date")
    {
        static const std::string s_SERIES_DATE("20180201");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_series_date(s_SERIES_DATE);
            CHECK_EQ(s_SERIES_DATE, series->get_series_date());
            CHECK_EQ(s_SERIES_DATE, series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesDate));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::SeriesDate, s_SERIES_DATE);
            CHECK_EQ(s_SERIES_DATE, series->get_series_date());
            CHECK_EQ(s_SERIES_DATE, series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesDate));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "series_time")
    {
        static const std::string s_SERIES_TIME("165649.123456");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_series_time(s_SERIES_TIME);
            CHECK_EQ(s_SERIES_TIME, series->get_series_time());
            CHECK_EQ(s_SERIES_TIME, series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesTime));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::SeriesTime, s_SERIES_TIME);
            CHECK_EQ(s_SERIES_TIME, series->get_series_time());
            CHECK_EQ(s_SERIES_TIME, series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesTime));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "modality")
    {
        static const sight::data::dicom::modality_t s_MODALITY = sight::data::dicom::modality_t::us;
        static const std::string s_MODALITY_STRING(*sight::data::dicom::to_string(s_MODALITY));

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_modality(s_MODALITY);
            CHECK_EQ(s_MODALITY, series->get_modality());
            CHECK_EQ(s_MODALITY_STRING, series->get_byte_value(sight::data::dicom::attribute::Keyword::Modality));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::Modality, s_MODALITY_STRING);
            CHECK_EQ(s_MODALITY, series->get_modality());
            CHECK_EQ(s_MODALITY_STRING, series->get_byte_value(sight::data::dicom::attribute::Keyword::Modality));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "series_description")
    {
        static const std::string s_SERIES_DESCRIPTION(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_series_description(s_SERIES_DESCRIPTION);
            CHECK_EQ(s_SERIES_DESCRIPTION, series->get_series_description());
            CHECK_EQ(
                s_SERIES_DESCRIPTION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesDescription)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::SeriesDescription, s_SERIES_DESCRIPTION);
            CHECK_EQ(s_SERIES_DESCRIPTION, series->get_series_description());
            CHECK_EQ(
                s_SERIES_DESCRIPTION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesDescription)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performing_physician_name")
    {
        static const std::vector<std::string> s_PERFORMING_PHYSICIAN_NAMES = {
            uuid::generate(),
            uuid::generate(),
            uuid::generate()
        };

        static const std::string s_PERFORMING_PHYSICIAN_NAME = boost::join(s_PERFORMING_PHYSICIAN_NAMES, "\\");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performing_physician_names(s_PERFORMING_PHYSICIAN_NAMES);
            CHECK(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
            CHECK(
                s_PERFORMING_PHYSICIAN_NAMES
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );

            CHECK_EQ(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
            CHECK_EQ(
                s_PERFORMING_PHYSICIAN_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_values(
                sight::data::dicom::attribute::Keyword::PerformingPhysicianName,
                s_PERFORMING_PHYSICIAN_NAMES
            );
            CHECK(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
            CHECK(
                s_PERFORMING_PHYSICIAN_NAMES
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );

            CHECK_EQ(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
            CHECK_EQ(
                s_PERFORMING_PHYSICIAN_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performing_physician_name(s_PERFORMING_PHYSICIAN_NAME);
            CHECK(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
            CHECK(
                s_PERFORMING_PHYSICIAN_NAMES
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );

            CHECK_EQ(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
            CHECK_EQ(
                s_PERFORMING_PHYSICIAN_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformingPhysicianName,
                s_PERFORMING_PHYSICIAN_NAME
            );
            CHECK(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
            CHECK(
                s_PERFORMING_PHYSICIAN_NAMES
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );

            CHECK_EQ(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
            CHECK_EQ(
                s_PERFORMING_PHYSICIAN_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformingPhysicianName)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "anatomical_orientation_type")
    {
        static const std::string s_ANATOMICAL_ORIENTATION_TYPE("QUADRUPED");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_anatomical_orientation_type(s_ANATOMICAL_ORIENTATION_TYPE);
            CHECK_EQ(s_ANATOMICAL_ORIENTATION_TYPE, series->get_anatomical_orientation_type());
            CHECK_EQ(
                s_ANATOMICAL_ORIENTATION_TYPE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AnatomicalOrientationType)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::AnatomicalOrientationType,
                s_ANATOMICAL_ORIENTATION_TYPE
            );
            CHECK_EQ(s_ANATOMICAL_ORIENTATION_TYPE, series->get_anatomical_orientation_type());
            CHECK_EQ(
                s_ANATOMICAL_ORIENTATION_TYPE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AnatomicalOrientationType)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "body_part_examined")
    {
        static const std::string s_BODY_PART_EXAMINED("ABDOMEN");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_body_part_examined(s_BODY_PART_EXAMINED);
            CHECK_EQ(s_BODY_PART_EXAMINED, series->get_body_part_examined());
            CHECK_EQ(
                s_BODY_PART_EXAMINED,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::BodyPartExamined)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::BodyPartExamined, s_BODY_PART_EXAMINED);
            CHECK_EQ(s_BODY_PART_EXAMINED, series->get_body_part_examined());
            CHECK_EQ(
                s_BODY_PART_EXAMINED,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::BodyPartExamined)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "protocol_name")
    {
        static const std::string s_PROTOCOL_NAME(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_protocol_name(s_PROTOCOL_NAME);
            CHECK_EQ(s_PROTOCOL_NAME, series->get_protocol_name());
            CHECK_EQ(s_PROTOCOL_NAME, series->get_byte_value(sight::data::dicom::attribute::Keyword::ProtocolName));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::ProtocolName, s_PROTOCOL_NAME);
            CHECK_EQ(s_PROTOCOL_NAME, series->get_protocol_name());
            CHECK_EQ(s_PROTOCOL_NAME, series->get_byte_value(sight::data::dicom::attribute::Keyword::ProtocolName));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_position")
    {
        static const std::string s_PATIENT_POSITION("FFP");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_position(s_PATIENT_POSITION);
            CHECK_EQ(s_PATIENT_POSITION, series->get_patient_position());
            CHECK_EQ(
                s_PATIENT_POSITION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientPosition)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientPosition, s_PATIENT_POSITION);
            CHECK_EQ(s_PATIENT_POSITION, series->get_patient_position());
            CHECK_EQ(
                s_PATIENT_POSITION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientPosition)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "series_instance_uid")
    {
        static const std::string s_SERIES_INSTANCE_UID(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_series_instance_uid(s_SERIES_INSTANCE_UID);
            CHECK_EQ(s_SERIES_INSTANCE_UID, series->get_series_instance_uid());
            CHECK_EQ(
                s_SERIES_INSTANCE_UID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesInstanceUID)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::SeriesInstanceUID, s_SERIES_INSTANCE_UID);
            CHECK_EQ(s_SERIES_INSTANCE_UID, series->get_series_instance_uid());
            CHECK_EQ(
                s_SERIES_INSTANCE_UID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesInstanceUID)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "series_number")
    {
        static const std::int32_t s_SERIES_NUMBER = 1;

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_series_number(s_SERIES_NUMBER);
            CHECK_EQ(s_SERIES_NUMBER, *series->get_series_number());
            CHECK_EQ(
                std::to_string(s_SERIES_NUMBER),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesNumber)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::SeriesNumber,
                std::to_string(s_SERIES_NUMBER)
            );
            CHECK_EQ(s_SERIES_NUMBER, *series->get_series_number());
            CHECK_EQ(
                std::to_string(s_SERIES_NUMBER),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesNumber)
            );
        }

        {
            // Test emtpy value
            auto series = std::make_shared<sight::data::series>();
            series->set_series_number();

            CHECK(!series->get_series_number());
            CHECK_EQ(std::string(), series->get_byte_value(sight::data::dicom::attribute::Keyword::SeriesNumber));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "laterality")
    {
        static const std::string s_LATERALITY("R");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_laterality(s_LATERALITY);
            CHECK_EQ(s_LATERALITY, series->get_laterality());
            CHECK_EQ(s_LATERALITY, series->get_byte_value(sight::data::dicom::attribute::Keyword::Laterality));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::Laterality, s_LATERALITY);
            CHECK_EQ(s_LATERALITY, series->get_laterality());
            CHECK_EQ(s_LATERALITY, series->get_byte_value(sight::data::dicom::attribute::Keyword::Laterality));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performed_procedure_step_start_date")
    {
        static const std::string s_PERFORMED_PROCEDURE_STEP_START_DATE("20000101");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performed_procedure_step_start_date(s_PERFORMED_PROCEDURE_STEP_START_DATE);
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_START_DATE, series->get_performed_procedure_step_start_date());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_START_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepStartDate)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformedProcedureStepStartDate,
                s_PERFORMED_PROCEDURE_STEP_START_DATE
            );
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_START_DATE, series->get_performed_procedure_step_start_date());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_START_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepStartDate)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performed_procedure_step_start_time")
    {
        static const std::string s_PERFORMED_PROCEDURE_STEP_START_TIME("165649.123456");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performed_procedure_step_start_time(s_PERFORMED_PROCEDURE_STEP_START_TIME);
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_START_TIME, series->get_performed_procedure_step_start_time());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_START_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepStartTime)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformedProcedureStepStartTime,
                s_PERFORMED_PROCEDURE_STEP_START_TIME
            );
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_START_TIME, series->get_performed_procedure_step_start_time());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_START_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepStartTime)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performed_procedure_step_end_date")
    {
        static const std::string s_PERFORMED_PROCEDURE_STEP_END_DATE("20000101");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performed_procedure_step_end_date(s_PERFORMED_PROCEDURE_STEP_END_DATE);
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_END_DATE, series->get_performed_procedure_step_end_date());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_END_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepEndDate)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformedProcedureStepEndDate,
                s_PERFORMED_PROCEDURE_STEP_END_DATE
            );
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_END_DATE, series->get_performed_procedure_step_end_date());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_END_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepEndDate)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performed_procedure_step_end_time")
    {
        static const std::string s_PERFORMED_PROCEDURE_STEP_END_TIME("165649.123456");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performed_procedure_step_end_time(s_PERFORMED_PROCEDURE_STEP_END_TIME);
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_END_TIME, series->get_performed_procedure_step_end_time());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_END_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepEndTime)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformedProcedureStepEndTime,
                s_PERFORMED_PROCEDURE_STEP_END_TIME
            );
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_END_TIME, series->get_performed_procedure_step_end_time());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_END_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepEndTime)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performed_procedure_step_id")
    {
        static const std::string s_PERFORMED_PROCEDURE_STEP_ID("B");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performed_procedure_step_id(s_PERFORMED_PROCEDURE_STEP_ID);
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_ID, series->get_performed_procedure_step_id());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_ID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepID)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformedProcedureStepID,
                s_PERFORMED_PROCEDURE_STEP_ID
            );
            CHECK_EQ(s_PERFORMED_PROCEDURE_STEP_ID, series->get_performed_procedure_step_id());
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_ID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepID)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "performed_procedure_step_description")
    {
        static const std::string s_PERFORMED_PROCEDURE_STEP_DESCRIPTION(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_performed_procedure_step_description(s_PERFORMED_PROCEDURE_STEP_DESCRIPTION);
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
                series->get_performed_procedure_step_description()
            );
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepDescription)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PerformedProcedureStepDescription,
                s_PERFORMED_PROCEDURE_STEP_DESCRIPTION
            );
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
                series->get_performed_procedure_step_description()
            );
            CHECK_EQ(
                s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PerformedProcedureStepDescription)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "comments_on_the_performed_procedure_step")
    {
        static const std::string s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_comments_on_the_performed_procedure_step(s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP);
            CHECK_EQ(
                s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
                series->get_comments_on_the_performed_procedure_step()
            );
            CHECK_EQ(
                s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep,
                s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP
            );
            CHECK_EQ(
                s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
                series->get_comments_on_the_performed_procedure_step()
            );
            CHECK_EQ(
                s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "institution_name")
    {
        static const std::string s_INSTITUTION_NAME(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_institution_name(s_INSTITUTION_NAME);
            CHECK_EQ(s_INSTITUTION_NAME, series->get_institution_name());
            CHECK_EQ(
                s_INSTITUTION_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::InstitutionName)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::InstitutionName, s_INSTITUTION_NAME);
            CHECK_EQ(s_INSTITUTION_NAME, series->get_institution_name());
            CHECK_EQ(
                s_INSTITUTION_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::InstitutionName)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_name")
    {
        static const std::string s_PATIENT_NAME(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_name(s_PATIENT_NAME);
            CHECK_EQ(s_PATIENT_NAME, series->get_patient_name());
            CHECK_EQ(s_PATIENT_NAME, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientName));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientName, s_PATIENT_NAME);
            CHECK_EQ(s_PATIENT_NAME, series->get_patient_name());
            CHECK_EQ(s_PATIENT_NAME, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientName));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_id")
    {
        static const std::string s_PATIENT_ID(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_id(s_PATIENT_ID);
            CHECK_EQ(s_PATIENT_ID, series->get_patient_id());
            CHECK_EQ(s_PATIENT_ID, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientID));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientID, s_PATIENT_ID);
            CHECK_EQ(s_PATIENT_ID, series->get_patient_id());
            CHECK_EQ(s_PATIENT_ID, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientID));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_birth_date")
    {
        static const std::string s_PATIENT_BIRTH_DATE("20000101");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_birth_date(s_PATIENT_BIRTH_DATE);
            CHECK_EQ(s_PATIENT_BIRTH_DATE, series->get_patient_birth_date());
            CHECK_EQ(
                s_PATIENT_BIRTH_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientBirthDate)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientBirthDate, s_PATIENT_BIRTH_DATE);
            CHECK_EQ(s_PATIENT_BIRTH_DATE, series->get_patient_birth_date());
            CHECK_EQ(
                s_PATIENT_BIRTH_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientBirthDate)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_sex")
    {
        static const std::string s_PATIENT_SEX("M");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_sex(s_PATIENT_SEX);
            CHECK_EQ(s_PATIENT_SEX, series->get_patient_sex());
            CHECK_EQ(s_PATIENT_SEX, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientSex));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientSex, s_PATIENT_SEX);
            CHECK_EQ(s_PATIENT_SEX, series->get_patient_sex());
            CHECK_EQ(s_PATIENT_SEX, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientSex));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "study_date")
    {
        static const std::string s_STUDY_DATE("20011012");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_study_date(s_STUDY_DATE);
            CHECK_EQ(s_STUDY_DATE, series->get_study_date());
            CHECK_EQ(s_STUDY_DATE, series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyDate));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::StudyDate, s_STUDY_DATE);
            CHECK_EQ(s_STUDY_DATE, series->get_study_date());
            CHECK_EQ(s_STUDY_DATE, series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyDate));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "study_time")
    {
        static const std::string s_STUDY_TIME("224513.123456");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_study_time(s_STUDY_TIME);
            CHECK_EQ(s_STUDY_TIME, series->get_study_time());
            CHECK_EQ(s_STUDY_TIME, series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyTime));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::StudyTime, s_STUDY_TIME);
            CHECK_EQ(s_STUDY_TIME, series->get_study_time());
            CHECK_EQ(s_STUDY_TIME, series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyTime));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "referring_physician_name")
    {
        static const std::string s_REFERRING_PHYSICIAN_NAME(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_referring_physician_name(s_REFERRING_PHYSICIAN_NAME);
            CHECK_EQ(s_REFERRING_PHYSICIAN_NAME, series->get_referring_physician_name());
            CHECK_EQ(
                s_REFERRING_PHYSICIAN_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ReferringPhysicianName)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ReferringPhysicianName,
                s_REFERRING_PHYSICIAN_NAME
            );
            CHECK_EQ(s_REFERRING_PHYSICIAN_NAME, series->get_referring_physician_name());
            CHECK_EQ(
                s_REFERRING_PHYSICIAN_NAME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ReferringPhysicianName)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "study_description")
    {
        static const std::string s_STUDY_DESCRIPTION(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_study_description(s_STUDY_DESCRIPTION);
            CHECK_EQ(s_STUDY_DESCRIPTION, series->get_study_description());
            CHECK_EQ(
                s_STUDY_DESCRIPTION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyDescription)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::StudyDescription, s_STUDY_DESCRIPTION);
            CHECK_EQ(s_STUDY_DESCRIPTION, series->get_study_description());
            CHECK_EQ(
                s_STUDY_DESCRIPTION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyDescription)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "study_instance_uid")
    {
        static const std::string s_STUDY_INSTANCE_UID(uuid::generate());

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_study_instance_uid(s_STUDY_INSTANCE_UID);
            CHECK_EQ(s_STUDY_INSTANCE_UID, series->get_study_instance_uid());
            CHECK_EQ(
                s_STUDY_INSTANCE_UID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyInstanceUID)
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::StudyInstanceUID, s_STUDY_INSTANCE_UID);
            CHECK_EQ(s_STUDY_INSTANCE_UID, series->get_study_instance_uid());
            CHECK_EQ(
                s_STUDY_INSTANCE_UID,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyInstanceUID)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "study_id")
    {
        static const std::string s_STUDY_ID("A");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_study_id(s_STUDY_ID);
            CHECK_EQ(s_STUDY_ID, series->get_study_id());
            CHECK_EQ(s_STUDY_ID, series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyID));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::StudyID, s_STUDY_ID);
            CHECK_EQ(s_STUDY_ID, series->get_study_id());
            CHECK_EQ(s_STUDY_ID, series->get_byte_value(sight::data::dicom::attribute::Keyword::StudyID));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_age")
    {
        static const std::string s_PATIENT_AGE("015Y");

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_age(s_PATIENT_AGE);
            CHECK_EQ(s_PATIENT_AGE, series->get_patient_age());
            CHECK_EQ(s_PATIENT_AGE, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientAge));
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientAge, s_PATIENT_AGE);
            CHECK_EQ(s_PATIENT_AGE, series->get_patient_age());
            CHECK_EQ(s_PATIENT_AGE, series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientAge));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_size")
    {
        static const double s_PATIENT_SIZE = 188.53;

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_size(s_PATIENT_SIZE);
            CHECK_EQ(s_PATIENT_SIZE, *series->get_patient_size());
            CHECK_EQ(
                doctest::Approx(s_PATIENT_SIZE).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientSize))
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::PatientSize, std::to_string(s_PATIENT_SIZE));
            CHECK_EQ(s_PATIENT_SIZE, *series->get_patient_size());
            CHECK_EQ(
                doctest::Approx(s_PATIENT_SIZE).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientSize))
            );
        }

        {
            // Test emtpy value
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_size();

            CHECK(!series->get_patient_size());
            CHECK_EQ(std::string(), series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientSize));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "patient_weight")
    {
        static const double s_PATIENT_WEIGHT = 115.07;

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_weight(s_PATIENT_WEIGHT);
            CHECK_EQ(s_PATIENT_WEIGHT, *series->get_patient_weight());
            CHECK_EQ(
                doctest::Approx(s_PATIENT_WEIGHT).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientWeight))
            );
        }

        {
            auto series = std::make_shared<sight::data::series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::PatientWeight,
                std::to_string(s_PATIENT_WEIGHT)
            );
            CHECK_EQ(s_PATIENT_WEIGHT, *series->get_patient_weight());
            CHECK_EQ(
                doctest::Approx(s_PATIENT_WEIGHT).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientWeight))
            );
        }

        {
            // Test emtpy value
            auto series = std::make_shared<sight::data::series>();
            series->set_patient_weight();

            CHECK(!series->get_patient_weight());
            CHECK_EQ(std::string(), series->get_byte_value(sight::data::dicom::attribute::Keyword::PatientWeight));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "acquisition_date")
    {
        static const std::string s_ACQUISITION_DATE("20011012");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_acquisition_date(s_ACQUISITION_DATE);
            CHECK_EQ(s_ACQUISITION_DATE, series->get_acquisition_date());
            CHECK_EQ(
                s_ACQUISITION_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionDate)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionDate, s_ACQUISITION_DATE);
            CHECK_EQ(s_ACQUISITION_DATE, series->get_acquisition_date());
            CHECK_EQ(
                s_ACQUISITION_DATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionDate)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "acquisition_time")
    {
        static const std::string s_ACQUISITION_TIME("224513.123456");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_acquisition_time(s_ACQUISITION_TIME);
            CHECK_EQ(s_ACQUISITION_TIME, series->get_acquisition_time());
            CHECK_EQ(
                s_ACQUISITION_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionTime)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionTime, s_ACQUISITION_TIME);
            CHECK_EQ(s_ACQUISITION_TIME, series->get_acquisition_time());
            CHECK_EQ(
                s_ACQUISITION_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionTime)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "acquisition_number")
    {
        static const std::int32_t s_ACQUISITION_NUMBER = 0x00200012;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_acquisition_number(s_ACQUISITION_NUMBER);
            CHECK_EQ(s_ACQUISITION_NUMBER, series->get_acquisition_number().value_or(0));
            CHECK_EQ(
                std::to_string(s_ACQUISITION_NUMBER),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionNumber)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::AcquisitionNumber,
                std::to_string(s_ACQUISITION_NUMBER)
            );
            CHECK_EQ(s_ACQUISITION_NUMBER, series->get_acquisition_number().value_or(0));
            CHECK_EQ(
                std::to_string(s_ACQUISITION_NUMBER),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::AcquisitionNumber)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "instance_number")
    {
        static const std::int32_t s_INSTANCE_NUMBER = 0x00200013;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_instance_number(s_INSTANCE_NUMBER);
            CHECK_EQ(s_INSTANCE_NUMBER, series->get_instance_number().value_or(0));
            CHECK_EQ(
                std::to_string(s_INSTANCE_NUMBER),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::InstanceNumber)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::InstanceNumber,
                std::to_string(s_INSTANCE_NUMBER)
            );
            CHECK_EQ(s_INSTANCE_NUMBER, series->get_instance_number().value_or(0));
            CHECK_EQ(
                std::to_string(s_INSTANCE_NUMBER),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::InstanceNumber)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "content_time")
    {
        static const std::string s_CONTENT_TIME("224513.123456");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_content_time(s_CONTENT_TIME);
            CHECK_EQ(s_CONTENT_TIME, series->get_content_time());
            CHECK_EQ(s_CONTENT_TIME, series->get_byte_value(sight::data::dicom::attribute::Keyword::ContentTime));
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::ContentTime, s_CONTENT_TIME);
            CHECK_EQ(s_CONTENT_TIME, series->get_content_time());
            CHECK_EQ(s_CONTENT_TIME, series->get_byte_value(sight::data::dicom::attribute::Keyword::ContentTime));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_agent")
    {
        static const std::string s_CONTRAST_BOLUS_AGENT(uuid::generate());

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_agent(s_CONTRAST_BOLUS_AGENT);
            CHECK_EQ(s_CONTRAST_BOLUS_AGENT, series->get_contrast_bolus_agent());
            CHECK_EQ(
                s_CONTRAST_BOLUS_AGENT,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusAgent)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusAgent, s_CONTRAST_BOLUS_AGENT);
            CHECK_EQ(s_CONTRAST_BOLUS_AGENT, series->get_contrast_bolus_agent());
            CHECK_EQ(
                s_CONTRAST_BOLUS_AGENT,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusAgent)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_route")
    {
        static const std::string s_CONTRAST_BOLUS_ROUTE(uuid::generate());

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_route(s_CONTRAST_BOLUS_ROUTE);
            CHECK_EQ(s_CONTRAST_BOLUS_ROUTE, series->get_contrast_bolus_route());
            CHECK_EQ(
                s_CONTRAST_BOLUS_ROUTE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusRoute)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusRoute, s_CONTRAST_BOLUS_ROUTE);
            CHECK_EQ(s_CONTRAST_BOLUS_ROUTE, series->get_contrast_bolus_route());
            CHECK_EQ(
                s_CONTRAST_BOLUS_ROUTE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusRoute)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_volume")
    {
        static const double s_CONTRAST_BOLUS_VOLUME = 188.53;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_volume(s_CONTRAST_BOLUS_VOLUME);
            CHECK_EQ(s_CONTRAST_BOLUS_VOLUME, *series->get_contrast_bolus_volume());
            CHECK_EQ(
                doctest::Approx(s_CONTRAST_BOLUS_VOLUME).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusVolume))
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastBolusVolume,
                std::to_string(s_CONTRAST_BOLUS_VOLUME)
            );
            CHECK_EQ(s_CONTRAST_BOLUS_VOLUME, *series->get_contrast_bolus_volume());
            CHECK_EQ(
                doctest::Approx(s_CONTRAST_BOLUS_VOLUME).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusVolume))
            );
        }

        {
            // Test emtpy value
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_volume();

            CHECK(!series->get_contrast_bolus_volume());
            CHECK_EQ(
                std::string(),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusVolume)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_start_time")
    {
        static const std::string s_CONTRAST_BOLUS_START_TIME("224513.123456");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_start_time(s_CONTRAST_BOLUS_START_TIME);
            CHECK_EQ(s_CONTRAST_BOLUS_START_TIME, series->get_contrast_bolus_start_time());
            CHECK_EQ(
                s_CONTRAST_BOLUS_START_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusStartTime)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastBolusStartTime,
                s_CONTRAST_BOLUS_START_TIME
            );
            CHECK_EQ(s_CONTRAST_BOLUS_START_TIME, series->get_contrast_bolus_start_time());
            CHECK_EQ(
                s_CONTRAST_BOLUS_START_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusStartTime)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_stop_time")
    {
        static const std::string s_CONTRAST_BOLUS_STOP_TIME("224513.123456");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_stop_time(s_CONTRAST_BOLUS_STOP_TIME);
            CHECK_EQ(s_CONTRAST_BOLUS_STOP_TIME, series->get_contrast_bolus_stop_time());
            CHECK_EQ(
                s_CONTRAST_BOLUS_STOP_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusStopTime)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastBolusStopTime,
                s_CONTRAST_BOLUS_STOP_TIME
            );
            CHECK_EQ(s_CONTRAST_BOLUS_STOP_TIME, series->get_contrast_bolus_stop_time());
            CHECK_EQ(
                s_CONTRAST_BOLUS_STOP_TIME,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusStopTime)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_total_dose")
    {
        static const double s_CONTRAST_BOLUS_TOTAL_DOSE = 123.45;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_total_dose(s_CONTRAST_BOLUS_TOTAL_DOSE);
            CHECK_EQ(s_CONTRAST_BOLUS_TOTAL_DOSE, *series->get_contrast_bolus_total_dose());
            CHECK_EQ(
                doctest::Approx(s_CONTRAST_BOLUS_TOTAL_DOSE).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusTotalDose))
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastBolusTotalDose,
                std::to_string(s_CONTRAST_BOLUS_TOTAL_DOSE)
            );
            CHECK_EQ(s_CONTRAST_BOLUS_TOTAL_DOSE, *series->get_contrast_bolus_total_dose());
            CHECK_EQ(
                doctest::Approx(s_CONTRAST_BOLUS_TOTAL_DOSE).epsilon(0.00001),
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusTotalDose))
            );
        }

        {
            // Test emtpy value
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_total_dose();

            CHECK(!series->get_contrast_bolus_total_dose());
            CHECK_EQ(
                std::string(),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusTotalDose)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_flow_rate")
    {
        static const std::vector<std::string> s_CONTRAST_FLOW_RATE_STRINGS = {
            "11.1",
            "22.2",
            "33.3"
        };

        static const std::vector<double> s_CONTRAST_FLOW_RATES = {
            std::stod(s_CONTRAST_FLOW_RATE_STRINGS[0]),
            std::stod(s_CONTRAST_FLOW_RATE_STRINGS[1]),
            std::stod(s_CONTRAST_FLOW_RATE_STRINGS[2])
        };

        static const std::string s_CONTRAST_FLOW_RATE = boost::join(s_CONTRAST_FLOW_RATE_STRINGS, "\\");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_flow_rates(s_CONTRAST_FLOW_RATES);
            CHECK(sight::core::is_equal(s_CONTRAST_FLOW_RATES, series->get_contrast_flow_rates()));
            CHECK(
                s_CONTRAST_FLOW_RATE_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );

            CHECK_EQ(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
            CHECK_EQ(
                s_CONTRAST_FLOW_RATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_values(
                sight::data::dicom::attribute::Keyword::ContrastFlowRate,
                s_CONTRAST_FLOW_RATE_STRINGS
            );
            CHECK(s_CONTRAST_FLOW_RATES == series->get_contrast_flow_rates());
            CHECK(
                s_CONTRAST_FLOW_RATE_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );

            CHECK_EQ(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
            CHECK_EQ(
                s_CONTRAST_FLOW_RATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_flow_rate(s_CONTRAST_FLOW_RATE);
            CHECK(s_CONTRAST_FLOW_RATES == series->get_contrast_flow_rates());
            CHECK(
                s_CONTRAST_FLOW_RATE_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );

            CHECK_EQ(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
            CHECK_EQ(
                s_CONTRAST_FLOW_RATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowRate, s_CONTRAST_FLOW_RATE);
            CHECK(s_CONTRAST_FLOW_RATES == series->get_contrast_flow_rates());
            CHECK(
                s_CONTRAST_FLOW_RATE_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );

            CHECK_EQ(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
            CHECK_EQ(
                s_CONTRAST_FLOW_RATE,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowRate)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_flow_duration")
    {
        static const std::vector<std::string> s_CONTRAST_FLOW_DURATION_STRINGS = {
            "44.4",
            "55.5",
            "66.6"
        };

        static const std::vector<double> s_CONTRAST_FLOW_DURATIONS = {
            std::stod(s_CONTRAST_FLOW_DURATION_STRINGS[0]),
            std::stod(s_CONTRAST_FLOW_DURATION_STRINGS[1]),
            std::stod(s_CONTRAST_FLOW_DURATION_STRINGS[2])
        };

        static const std::string s_CONTRAST_FLOW_DURATION = boost::join(s_CONTRAST_FLOW_DURATION_STRINGS, "\\");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_flow_durations(s_CONTRAST_FLOW_DURATIONS);
            CHECK(sight::core::is_equal(s_CONTRAST_FLOW_DURATIONS, series->get_contrast_flow_durations()));
            CHECK(
                s_CONTRAST_FLOW_DURATION_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );

            CHECK_EQ(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
            CHECK_EQ(
                s_CONTRAST_FLOW_DURATION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_values(
                sight::data::dicom::attribute::Keyword::ContrastFlowDuration,
                s_CONTRAST_FLOW_DURATION_STRINGS
            );
            CHECK(s_CONTRAST_FLOW_DURATIONS == series->get_contrast_flow_durations());
            CHECK(
                s_CONTRAST_FLOW_DURATION_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );

            CHECK_EQ(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
            CHECK_EQ(
                s_CONTRAST_FLOW_DURATION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_flow_duration(s_CONTRAST_FLOW_DURATION);
            CHECK(s_CONTRAST_FLOW_DURATIONS == series->get_contrast_flow_durations());
            CHECK(
                s_CONTRAST_FLOW_DURATION_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );

            CHECK_EQ(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
            CHECK_EQ(
                s_CONTRAST_FLOW_DURATION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastFlowDuration,
                s_CONTRAST_FLOW_DURATION
            );
            CHECK(s_CONTRAST_FLOW_DURATIONS == series->get_contrast_flow_durations());
            CHECK(
                s_CONTRAST_FLOW_DURATION_STRINGS
                == series->get_byte_values(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );

            CHECK_EQ(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
            CHECK_EQ(
                s_CONTRAST_FLOW_DURATION,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastFlowDuration)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_ingredient")
    {
        static const std::string s_CONTRAST_BOLUS_INGREDIENT("GADOLINIUM");

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_ingredient(s_CONTRAST_BOLUS_INGREDIENT);
            CHECK_EQ(s_CONTRAST_BOLUS_INGREDIENT, series->get_contrast_bolus_ingredient());
            CHECK_EQ(
                s_CONTRAST_BOLUS_INGREDIENT,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusIngredient)
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastBolusIngredient,
                s_CONTRAST_BOLUS_INGREDIENT
            );
            CHECK_EQ(s_CONTRAST_BOLUS_INGREDIENT, series->get_contrast_bolus_ingredient());
            CHECK_EQ(
                s_CONTRAST_BOLUS_INGREDIENT,
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusIngredient)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "contrast_bolus_ingredient_concentration")
    {
        static const double s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION = 123.45;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_ingredient_concentration(s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION);
            CHECK_EQ(
                s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION,
                *series->get_contrast_bolus_ingredient_concentration()
            );
            CHECK_EQ(
                doctest::Approx(s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION).epsilon(0.00001),
                std::stod(
                    series->get_byte_value(
                        sight::data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration
                    )
                )
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(
                sight::data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration,
                std::to_string(s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION)
            );
            CHECK_EQ(
                s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION,
                *series->get_contrast_bolus_ingredient_concentration()
            );
            CHECK_EQ(
                doctest::Approx(s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION).epsilon(0.00001),
                std::stod(
                    series->get_byte_value(
                        sight::data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration
                    )
                )
            );
        }

        {
            // Test emtpy value
            auto series = std::make_shared<sight::data::image_series>();
            series->set_contrast_bolus_ingredient_concentration();

            CHECK(!series->get_contrast_bolus_ingredient_concentration());
            CHECK_EQ(
                std::string(),
                series->get_byte_value(sight::data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "rows")
    {
        static const std::uint16_t s_ROWS = 256;
        static const std::string s_ROWS_BYTES(reinterpret_cast<const char*>(&s_ROWS), sizeof(s_ROWS));

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_rows(s_ROWS);
            CHECK_EQ(s_ROWS, series->get_rows().value_or(0));
            CHECK_EQ(s_ROWS_BYTES, series->get_byte_value(sight::data::dicom::attribute::Keyword::Rows));
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::Rows, s_ROWS_BYTES);
            CHECK_EQ(s_ROWS, series->get_rows().value_or(0));
            CHECK_EQ(s_ROWS_BYTES, series->get_byte_value(sight::data::dicom::attribute::Keyword::Rows));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "columns")
    {
        static const std::uint16_t s_COLUMNS = 256;
        static const std::string s_COLUMNS_BYTES(reinterpret_cast<const char*>(&s_COLUMNS), sizeof(s_COLUMNS));

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_columns(s_COLUMNS);
            CHECK_EQ(s_COLUMNS, series->get_columns().value_or(0));
            CHECK_EQ(s_COLUMNS_BYTES, series->get_byte_value(sight::data::dicom::attribute::Keyword::Columns));
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::Columns, s_COLUMNS_BYTES);
            CHECK_EQ(s_COLUMNS, series->get_columns().value_or(0));
            CHECK_EQ(s_COLUMNS_BYTES, series->get_byte_value(sight::data::dicom::attribute::Keyword::Columns));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "window_center")
    {
        static const double s_CENTER = 333.33;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_window_center({s_CENTER});
            CHECK_EQ(s_CENTER, series->window_center().front());
            CHECK_EQ(
                s_CENTER,
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::WindowCenter))
            );
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::WindowCenter, std::to_string(s_CENTER));
            CHECK_EQ(s_CENTER, series->window_center().front());
            CHECK_EQ(
                s_CENTER,
                std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::WindowCenter))
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "window_width")
    {
        static const double s_WIDTH = 666.66;

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_window_width({s_WIDTH});
            CHECK_EQ(s_WIDTH, series->window_width().front());
            CHECK_EQ(s_WIDTH, std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::WindowWidth)));
        }

        {
            auto series = std::make_shared<sight::data::image_series>();
            series->set_byte_value(sight::data::dicom::attribute::Keyword::WindowWidth, std::to_string(s_WIDTH));
            CHECK_EQ(s_WIDTH, series->window_width().front());
            CHECK_EQ(s_WIDTH, std::stod(series->get_byte_value(sight::data::dicom::attribute::Keyword::WindowWidth)));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "image_position_patient")
    {
        static const std::vector<double> s_POSITION {111.111, 222.222, 333.333};

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to store the image position patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::CTImageStorage);

            series->set_image_position_patient(s_POSITION, std::nullopt);
            CHECK(s_POSITION == series->get_image_position_patient(std::nullopt));

            const auto values = series->get_byte_values(sight::data::dicom::attribute::Keyword::ImagePositionPatient);
            const std::vector<double> actual {
                std::stod(values[0]),
                std::stod(values[1]),
                std::stod(values[2])
            };

            CHECK(s_POSITION == actual);
        }

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to read the image position patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::CTImageStorage);

            series->set_byte_values(
                sight::data::dicom::attribute::Keyword::ImagePositionPatient,
            {
                std::to_string(s_POSITION[0]),
                std::to_string(s_POSITION[1]),
                std::to_string(s_POSITION[2])
            });

            CHECK(s_POSITION == series->get_image_position_patient(std::nullopt));

            const auto values = series->get_byte_values(sight::data::dicom::attribute::Keyword::ImagePositionPatient);
            const std::vector<double> actual {
                std::stod(values[0]),
                std::stod(values[1]),
                std::stod(values[2])
            };

            CHECK(s_POSITION == actual);
        }

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to read the image position patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

            // test Multi-Frame image
            series->set_image_position_patient(s_POSITION, 0);
            CHECK(s_POSITION == series->get_image_position_patient(0));

            // Also test shared group
            series->set_image_position_patient(s_POSITION, std::nullopt);
            CHECK(s_POSITION == series->get_image_position_patient(std::nullopt));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "image_orientation_patient")
    {
        static const std::vector<double> s_ORIENTATION {111.111, 222.222, 333.333, 444.444, 555.555, 666.666};

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to store the image orientation patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::CTImageStorage);

            series->set_image_orientation_patient(s_ORIENTATION, std::nullopt);
            CHECK(s_ORIENTATION == series->get_image_orientation_patient(std::nullopt));

            const auto values =
                series->get_byte_values(sight::data::dicom::attribute::Keyword::ImageOrientationPatient);
            const std::vector<double> actual {
                std::stod(values[0]),
                std::stod(values[1]),
                std::stod(values[2]),
                std::stod(values[3]),
                std::stod(values[4]),
                std::stod(values[5])
            };

            CHECK(s_ORIENTATION == actual);
        }

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to read the image orientation patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::CTImageStorage);

            series->set_byte_values(
                sight::data::dicom::attribute::Keyword::ImageOrientationPatient,
            {
                std::to_string(s_ORIENTATION[0]),
                std::to_string(s_ORIENTATION[1]),
                std::to_string(s_ORIENTATION[2]),
                std::to_string(s_ORIENTATION[3]),
                std::to_string(s_ORIENTATION[4]),
                std::to_string(s_ORIENTATION[5])
            });

            CHECK(s_ORIENTATION == series->get_image_orientation_patient(std::nullopt));

            const auto values =
                series->get_byte_values(sight::data::dicom::attribute::Keyword::ImageOrientationPatient);
            const std::vector<double> actual {
                std::stod(values[0]),
                std::stod(values[1]),
                std::stod(values[2]),
                std::stod(values[3]),
                std::stod(values[4]),
                std::stod(values[5])
            };

            CHECK(s_ORIENTATION == actual);
        }

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to read the image orientation patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

            // test Multi-Frame image
            series->set_image_orientation_patient(s_ORIENTATION, 0);
            CHECK(s_ORIENTATION == series->get_image_orientation_patient(0));

            // Also test shared group
            series->set_image_orientation_patient(s_ORIENTATION, std::nullopt);
            CHECK(s_ORIENTATION == series->get_image_orientation_patient(std::nullopt));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "image_transform_patient")
    {
        const sight::data::matrix4 matrix({
            0., 1., 0., 0.,
            1., 0., 0., 0.,
            0., 0., -1., 0.,
            0., 0., 0., 1.
        });

        {
            auto series = std::make_shared<sight::data::image_series>();

            // We need to set the SOP class UID to let Sight know how to read the image orientation/position patient
            series->set_sop_keyword(sight::data::dicom::sop::Keyword::CTImageStorage);

            series->set_image_transform_patient(matrix, std::nullopt);
            const auto value = series->get_image_transform_patient(std::nullopt);
            CHECK(value.has_value());
            CHECK(matrix == *value);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "frame_acquisition_date_time")
    {
        // DICOM DT format is  "YYYYMMDDHHMMSS.FFFFFF"
        static const std::string s_EXPECTED_FRAME_ACQUISITION_DATE_TIME0("20221026150703.000000");
        static const std::string s_EXPECTED_FRAME_ACQUISITION_DATE_TIME1("20221026150703.000001");
        static const std::string s_EXPECTED_FRAME_ACQUISITION_DATE_TIME2("20221026150703.000002");

        {
            auto series = std::make_shared<sight::data::image_series>();

            series->set_frame_acquisition_date_time(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME0, 0);
            series->set_frame_acquisition_date_time(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME1, 1);
            series->set_frame_acquisition_date_time(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME2, 2);

            const std::string actual_frame_acquisition_date_time0(*(series->get_frame_acquisition_date_time(0)));
            const std::string actual_frame_acquisition_date_time1(*(series->get_frame_acquisition_date_time(1)));
            const std::string actual_frame_acquisition_date_time2(*(series->get_frame_acquisition_date_time(2)));

            CHECK_EQ(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME0, actual_frame_acquisition_date_time0);
            CHECK_EQ(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME1, actual_frame_acquisition_date_time1);
            CHECK_EQ(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME2, actual_frame_acquisition_date_time2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "frame_comments")
    {
        static const std::string s_EXPECTED_FRAME_COMMENTS0("Comments0");
        static const std::string s_EXPECTED_FRAME_COMMENTS1("Comments1");
        static const std::string s_EXPECTED_FRAME_COMMENTS2("Comments2");

        {
            auto series = std::make_shared<sight::data::image_series>();

            series->set_frame_comments(s_EXPECTED_FRAME_COMMENTS0, 0);
            series->set_frame_comments(s_EXPECTED_FRAME_COMMENTS1, 1);
            series->set_frame_comments(s_EXPECTED_FRAME_COMMENTS2, 2);

            const std::string actual_frame_comments0(*(series->get_frame_comments(0)));
            const std::string actual_frame_comments1(*(series->get_frame_comments(1)));
            const std::string actual_frame_comments2(*(series->get_frame_comments(2)));

            CHECK_EQ(s_EXPECTED_FRAME_COMMENTS0, actual_frame_comments0);
            CHECK_EQ(s_EXPECTED_FRAME_COMMENTS1, actual_frame_comments1);
            CHECK_EQ(s_EXPECTED_FRAME_COMMENTS2, actual_frame_comments2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "frame_label")
    {
        static const std::string s_EXPECTED_FRAME_LABEL0("Label0");
        static const std::string s_EXPECTED_FRAME_LABEL1("Label1");
        static const std::string s_EXPECTED_FRAME_LABEL2("Label2");

        {
            auto series = std::make_shared<sight::data::image_series>();

            series->set_frame_label(s_EXPECTED_FRAME_LABEL0, 0);
            series->set_frame_label(s_EXPECTED_FRAME_LABEL1, 1);
            series->set_frame_label(s_EXPECTED_FRAME_LABEL2, 2);

            const std::string actual_frame_label0(*(series->get_frame_label(0)));
            const std::string actual_frame_label1(*(series->get_frame_label(1)));
            const std::string actual_frame_label2(*(series->get_frame_label(2)));

            CHECK_EQ(s_EXPECTED_FRAME_LABEL0, actual_frame_label0);
            CHECK_EQ(s_EXPECTED_FRAME_LABEL1, actual_frame_label1);
            CHECK_EQ(s_EXPECTED_FRAME_LABEL2, actual_frame_label2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "frame_acquisition_time_point")
    {
        // DICOM DT format is  "YYYYMMDDHHMMSS.FFFFFF"
        static const std::string s_EXPECTED_0("20221026150703.000000");
        static const std::string s_EXPECTED_1("20221026150703.000001");
        static const std::string s_EXPECTED_2("20221026150703.000002");
        static const std::string s_EXPECTED_3("2023");
        static const std::string s_EXPECTED_3B("20230101000000.000000");
        static const std::string s_EXPECTED_4("20221026150703.100000");
        static const std::string s_EXPECTED_5("20221026150703");
        static const std::string s_EXPECTED_5B("20221026150703.000000");

        auto series = std::make_shared<sight::data::image_series>();

        series->set_frame_acquisition_date_time(s_EXPECTED_0, 0);
        series->set_frame_acquisition_date_time(s_EXPECTED_1, 1);
        series->set_frame_acquisition_date_time(s_EXPECTED_2, 2);
        series->set_frame_acquisition_date_time(s_EXPECTED_3, 3);
        series->set_frame_acquisition_date_time(s_EXPECTED_4, 4);
        series->set_frame_acquisition_date_time(s_EXPECTED_5, 5);

        const auto time_point0 = *series->get_frame_acquisition_time_point(0);
        const auto time_point1 = *series->get_frame_acquisition_time_point(1);
        const auto time_point2 = *series->get_frame_acquisition_time_point(2);
        const auto time_point3 = *series->get_frame_acquisition_time_point(3);
        const auto time_point4 = *series->get_frame_acquisition_time_point(4);
        const auto time_point5 = *series->get_frame_acquisition_time_point(5);

        CHECK_EQ(s_EXPECTED_0, sight::data::series::time_point_to_date_time(time_point0));
        CHECK_EQ(s_EXPECTED_1, sight::data::series::time_point_to_date_time(time_point1));
        CHECK_EQ(s_EXPECTED_2, sight::data::series::time_point_to_date_time(time_point2));
        CHECK_EQ(s_EXPECTED_4, sight::data::series::time_point_to_date_time(time_point4));

        // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
        CHECK_EQ(s_EXPECTED_3B, sight::data::series::time_point_to_date_time(time_point3));
        CHECK_EQ(s_EXPECTED_5B, sight::data::series::time_point_to_date_time(time_point5));

        // Test setting time point directly, with overwrite
        series->set_frame_acquisition_time_point(time_point1, 0);
        series->set_frame_acquisition_time_point(time_point2, 1);
        series->set_frame_acquisition_time_point(time_point3, 2);
        series->set_frame_acquisition_time_point(time_point4, 3);
        series->set_frame_acquisition_time_point(time_point5, 4);
        series->set_frame_acquisition_time_point(time_point0, 5);

        const std::string actual_1(*(series->get_frame_acquisition_date_time(0)));
        const std::string actual_2(*(series->get_frame_acquisition_date_time(1)));
        const std::string actual_3(*(series->get_frame_acquisition_date_time(2)));
        const std::string actual_4(*(series->get_frame_acquisition_date_time(3)));
        const std::string actual_5(*(series->get_frame_acquisition_date_time(4)));
        const std::string actual_0(*(series->get_frame_acquisition_date_time(5)));

        CHECK_EQ(s_EXPECTED_0, actual_0);
        CHECK_EQ(s_EXPECTED_1, actual_1);
        CHECK_EQ(s_EXPECTED_2, actual_2);
        CHECK_EQ(s_EXPECTED_4, actual_4);

        // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
        CHECK_EQ(s_EXPECTED_3B, actual_3);
        CHECK_EQ(s_EXPECTED_5B, actual_5);

        // Test DeepCopy
        {
            auto destination      = std::make_shared<sight::data::image_series>();
            auto destination_lock = destination->dump_lock();

            {
                auto source      = std::make_shared<sight::data::image_series>();
                auto source_lock = source->dump_lock();

                source->set_frame_acquisition_date_time(s_EXPECTED_0, 0);
                source->set_frame_acquisition_date_time(s_EXPECTED_1, 1);
                source->set_frame_acquisition_date_time(s_EXPECTED_2, 2);
                source->set_frame_acquisition_date_time(s_EXPECTED_3, 3);
                source->set_frame_acquisition_date_time(s_EXPECTED_4, 4);
                source->set_frame_acquisition_date_time(s_EXPECTED_5, 5);

                destination->deep_copy(source);

                // This should not change destination
                source->set_frame_acquisition_date_time(s_EXPECTED_5, 0);
                source->set_frame_acquisition_date_time(s_EXPECTED_0, 1);
                source->set_frame_acquisition_date_time(s_EXPECTED_1, 2);
                source->set_frame_acquisition_date_time(s_EXPECTED_2, 3);
                source->set_frame_acquisition_date_time(s_EXPECTED_3, 4);
                source->set_frame_acquisition_date_time(s_EXPECTED_4, 5);
            }

            const std::string destination_0(*(destination->get_frame_acquisition_date_time(0)));
            const std::string destination_1(*(destination->get_frame_acquisition_date_time(1)));
            const std::string destination_2(*(destination->get_frame_acquisition_date_time(2)));
            const std::string destination_4(*(destination->get_frame_acquisition_date_time(4)));

            CHECK_EQ(s_EXPECTED_0, destination_0);
            CHECK_EQ(s_EXPECTED_1, destination_1);
            CHECK_EQ(s_EXPECTED_2, destination_2);
            CHECK_EQ(s_EXPECTED_4, destination_4);

            // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
            CHECK_EQ(s_EXPECTED_3B, actual_3);
            CHECK_EQ(s_EXPECTED_5B, actual_5);
        }

        // test shrinking
        {
            auto destination      = std::make_shared<sight::data::image_series>();
            auto destination_lock = destination->dump_lock();

            {
                auto source      = std::make_shared<sight::data::image_series>();
                auto source_lock = source->dump_lock();

                source->set_frame_acquisition_date_time(s_EXPECTED_0, 0);
                source->set_frame_acquisition_date_time(s_EXPECTED_1, 1);
                source->set_frame_acquisition_date_time(s_EXPECTED_2, 2);

                destination->deep_copy(source);
            }

            CHECK_EQ(s_EXPECTED_0, *destination->get_frame_acquisition_date_time(0));
            CHECK_EQ(s_EXPECTED_1, *destination->get_frame_acquisition_date_time(1));
            CHECK_EQ(s_EXPECTED_2, *destination->get_frame_acquisition_date_time(2));

            // Now resize the destination to 2 elements
            destination->shrink_frames(2);

            CHECK_EQ(s_EXPECTED_0, *destination->get_frame_acquisition_date_time(0));
            CHECK_EQ(s_EXPECTED_1, *destination->get_frame_acquisition_date_time(1));
            CHECK(!destination->get_frame_acquisition_date_time(2));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "string_conversion")
    {
        // Test binary to string and string to binary conversion for binary attributes

        // Unsigned Short (US)
        {
            static const uint16_t s_ROWS = 666;
            auto series                  = std::make_shared<sight::data::image_series>();

            // Rows - "666" string (4) is bigger than short size (2)
            series->set_string_value(0x0018, 0x1049, std::to_string(s_ROWS));
            CHECK_EQ(std::to_string(s_ROWS), series->get_string_value(0x0018, 0x1049));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "private_tag")
    {
        const std::string expected1 {uuid::generate()};
        const std::string expected2 {uuid::generate()};
        const std::vector<double> expected3 {1.1, 2.2, 3.3};

        {
            auto series = std::make_shared<sight::data::image_series>();

            series->set_private_value(expected1, 0x11);
            const auto& actual1 = series->get_private_value(0x11);
            CHECK(actual1);
            CHECK_EQ(expected1, *actual1);

            series->set_private_value(expected2, 0x12);
            const auto& actual2 = series->get_private_value(0x12);
            CHECK(actual2);
            CHECK_EQ(expected2, *actual2);

            // test removing the tag
            series->set_private_value(std::nullopt, 0x11);
            CHECK(!series->get_private_value(0x11).has_value());

            // test double vector
            series->set_private_values(expected3, 0x13);
            const auto actual3 = series->get_private_values(0x13);
            CHECK(sight::core::is_equal(expected3, actual3));
            series->set_private_values({}, 0x13);
            CHECK(!series->get_private_value(0x13).has_value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "multi_frame_private_tag")
    {
        const std::string expected1 {uuid::generate()};
        const std::string expected2 {uuid::generate()};
        const std::string expected3 {uuid::generate()};
        const std::vector<double> expected4 {1.1, 2.2, 3.3};

        {
            auto series = std::make_shared<sight::data::image_series>();

            // test first if nonexisting value is handled
            const auto& no_value = series->get_multi_frame_private_value(0x11, 0);
            CHECK(!no_value.has_value());

            series->set_multi_frame_private_value(expected1, 0x11, 0);
            const auto actual1 = series->get_multi_frame_private_value(0x11, 0);
            CHECK(actual1);
            CHECK_EQ(expected1, *actual1);

            series->set_multi_frame_private_value(expected2, 0x11, 1);
            const auto actual2 = series->get_multi_frame_private_value(0x11, 1);
            CHECK(actual2);
            CHECK_EQ(expected2, *actual2);

            series->set_multi_frame_private_value(expected3, 0x11, 2);
            const auto actual3 = series->get_multi_frame_private_value(0x11, 2);
            CHECK(actual3);
            CHECK_EQ(expected3, *actual3);

            // test removing the tag
            series->set_multi_frame_private_value(std::nullopt, 0x11, 0);
            CHECK(!series->get_multi_frame_private_value(0x11, 0).has_value());

            series->set_multi_frame_private_value(std::nullopt, 0x11, 1);
            CHECK(!series->get_multi_frame_private_value(0x11, 1).has_value());

            series->set_multi_frame_private_value(std::nullopt, 0x11, 2);
            CHECK(!series->get_multi_frame_private_value(0x11, 2).has_value());

            // test double vector
            series->set_multi_frame_private_values(expected4, 0x12, 0);
            const auto actual4 = series->get_multi_frame_private_values(0x12, 0);
            CHECK(sight::core::is_equal(expected4, actual4));
            series->set_multi_frame_private_values({}, 0x12, 0);
            CHECK(!series->get_multi_frame_private_value(0x12, 0).has_value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_patient_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_patient_name("1");
        series1->set_patient_id("2");
        series1->set_patient_birth_date("5");
        series1->set_patient_sex("6");

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_patient_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_general_study_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_study_instance_uid("1");
        series1->set_study_date("2");
        series1->set_study_time("3");
        series1->set_referring_physician_name("4");
        series1->set_study_id("6");
        series1->set_study_description("9");

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_general_study_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_patient_study_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_patient_age("3");
        series1->set_patient_size(4);
        series1->set_patient_weight(5);

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_patient_study_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_general_series_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_modality(sight::data::dicom::modality_t::ct);
        series1->set_series_instance_uid("2");
        series1->set_series_number(3);
        series1->set_laterality("4");
        series1->set_series_date("5");
        series1->set_series_time("6");
        series1->set_performing_physician_name("7");
        series1->set_protocol_name("9");
        series1->set_series_description("10");
        series1->set_body_part_examined("16");
        series1->set_patient_position("17");
        series1->set_performed_procedure_step_id("21");
        series1->set_performed_procedure_step_start_date("22");
        series1->set_performed_procedure_step_start_time("23");
        series1->set_performed_procedure_step_end_date("24");
        series1->set_performed_procedure_step_end_time("25");
        series1->set_performed_procedure_step_description("26");
        series1->set_comments_on_the_performed_procedure_step("28");
        series1->set_anatomical_orientation_type("29");

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_general_series_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_general_equipment_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_institution_name("2");

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_general_equipment_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_sop_common_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_sop_keyword(sight::data::dicom::sop::Keyword::ComputedRadiographyImageStorage);
        series1->set_sop_instance_uid("2");
        series1->set_specific_character_set("3");
        series1->set_instance_number(13);

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_sop_common_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "copy_general_image_module")
    {
        auto series1 = std::make_shared<sight::data::series>();
        series1->set_instance_number(1);
        series1->set_content_time("4");
        series1->set_acquisition_number(6);
        series1->set_acquisition_date("7");
        series1->set_acquisition_time("8");

        auto series2 = std::make_shared<sight::data::series>();
        series2->copy_general_image_module(series1);
        CHECK(*series1 == *series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "get_patient_position_string")
    {
        static const std::map<std::string, std::string> s_SHORT_TO_LONG {
            {"HFP", "Head First-Prone"},
            {"HFS", "Head First-Supine"},
            {"HFDR", "Head First-Decubitus Right"},
            {"HFDL", "Head First-Decubitus Left"},
            {"FFDR", "Feet First-Decubitus Right"},
            {"FFDL", "Feet First-Decubitus Left"},
            {"FFP", "Feet First-Prone"},
            {"FFS", "Feet First-Supine"},
            {"LFP", "Left First-Prone"},
            {"LFS", "Left First-Supine"},
            {"RFP", "Right First-Prone"},
            {"RFS", "Right First-Supine"},
            {"AFDR", "Anterior First-Decubitus Right"},
            {"AFDL", "Anterior First-Decubitus Left"},
            {"PFDR", "Posterior First-Decubitus Right"},
            {"PFDL", "Posterior First-Decubitus Left"},
            {"NOT_A_POSITION", "NOT_A_POSITION"}
        };
        auto series = std::make_shared<sight::data::series>();
        for(auto&& [key, value] : s_SHORT_TO_LONG)
        {
            series->set_patient_position(key);
            CHECK_MESSAGE(value == series->get_patient_position_string(), key);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "new_instances")
    {
        auto series = std::make_shared<sight::data::series>();

        CHECK(series->get_sop_instance_uid().empty());
        CHECK(series->get_study_instance_uid().empty());
        CHECK(series->get_series_instance_uid().empty());

        CHECK(series->get_instance_creation_date().empty());
        CHECK(series->get_instance_creation_time().empty());
        CHECK(series->get_study_date().empty());
        CHECK(series->get_study_time().empty());
        CHECK(series->get_series_date().empty());
        CHECK(series->get_series_time().empty());

        series->new_sop_instance();
        series->new_study_instance();
        series->new_series_instance();

        CHECK(!series->get_instance_creation_date().empty());
        CHECK(!series->get_instance_creation_time().empty());
        CHECK(!series->get_study_date().empty());
        CHECK(!series->get_study_time().empty());
        CHECK(!series->get_series_date().empty());
        CHECK(!series->get_series_time().empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "iso_date_time")
    {
        static const std::string s_DATE("20221026");
        static const std::string s_TIME("150703.123456");
        static const std::string s_EXPECTED_DATE("2022-10-26");
        static const std::string s_EXPECTED_TIME("15:07:03.123");

        CHECK_EQ(s_EXPECTED_DATE, sight::data::series::date_to_iso(s_DATE));
        CHECK_EQ(s_EXPECTED_TIME, sight::data::series::time_to_iso(s_TIME));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "path")
    {
        static const std::string s_DATE       = "20221026";
        static const std::string s_TIME       = "150703.123456";
        static const std::string s_UID        = "789";
        static const std::string s_PATIENT_ID = "666";
        static const auto s_MODALITY          = sight::data::dicom::modality_t::ct;

        static const std::string s_MODALITY_STRING(*sight::data::dicom::to_string(s_MODALITY));

        // FNV-1a(64) hash of "789" encoded as lowercase hexadecimal
        static const std::string s_HASH = "350a6a180f1c6dc9";

        auto series = std::make_shared<sight::data::series>();

        series->set_study_date(s_DATE);
        series->set_study_time(s_TIME);
        series->set_series_instance_uid(s_UID);
        series->set_series_date(s_DATE);
        series->set_series_time(s_TIME);
        series->set_patient_id(s_PATIENT_ID);
        series->set_modality(s_MODALITY);

        std::filesystem::path path;

        CHECK_NOTHROW((path = series->file_path(std::filesystem::path("/tmp"), ".ima")));

        const auto expected_path = std::filesystem::path("/tmp") / s_PATIENT_ID / (s_DATE + s_TIME)
                                   / (s_PATIENT_ID + "." + s_MODALITY_STRING + "." + s_DATE + s_TIME + "." + s_HASH
                                      + std::string(".ima"));

        CHECK_EQ(
            std::filesystem::weakly_canonical(expected_path).string(),
            std::filesystem::weakly_canonical(path).string()
        );
    }
} // TEST_SUITE("sight::data::series")
