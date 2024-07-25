/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "series_test.hpp"

#include <core/compare.hpp>
#include <core/tools/uuid.hpp>

#include <data/dicom/attribute.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>

using uuid = sight::core::tools::uuid;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::series_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void series_test::setUp()
{
    // Set up context before running a test.
    m_series.push_back(std::make_shared<data::image_series>());
    m_series.push_back(std::make_shared<data::model_series>());
}

//------------------------------------------------------------------------------

void series_test::tearDown()
{
    // Clean up after the test run.
    m_series.clear();
}

//------------------------------------------------------------------------------

void series_test::attr_patient_test()
{
    const std::string patient_name       = "John DOE";
    const std::string patient_id         = "42";
    const std::string patient_birth_date = "02-14-2015 11:28";
    const std::string patient_sex        = "M";

    for(const auto& series : m_series)
    {
        series->set_patient_name(patient_name);
        CPPUNIT_ASSERT(series->get_patient_name() == patient_name);

        series->set_patient_id(patient_id);
        CPPUNIT_ASSERT(series->get_patient_id() == patient_id);

        series->set_patient_birth_date(patient_birth_date);
        CPPUNIT_ASSERT(series->get_patient_birth_date() == patient_birth_date);

        series->set_patient_sex(patient_sex);
        CPPUNIT_ASSERT(series->get_patient_sex() == patient_sex);
    }
}

//------------------------------------------------------------------------------

void series_test::attr_study_test()
{
    const std::string instance_uid = "1346357.1664.643101.421337.4123403";
    const std::string date         = "02-14-2015";
    const std::string time         = "11:59";
    const std::string rpn          = "Dr^Jekyl";
    const std::string desc         = "Say 33.";
    const std::string age          = "42";

    for(const auto& series : m_series)
    {
        series->set_study_instance_uid(instance_uid);
        CPPUNIT_ASSERT(series->get_study_instance_uid() == instance_uid);

        series->set_study_date(date);
        CPPUNIT_ASSERT(series->get_study_date() == date);

        series->set_study_time(time);
        CPPUNIT_ASSERT(series->get_study_time() == time);

        series->set_referring_physician_name(rpn);
        CPPUNIT_ASSERT(series->get_referring_physician_name() == rpn);

        series->set_study_description(desc);
        CPPUNIT_ASSERT(series->get_study_description() == desc);

        series->set_patient_age(age);
        CPPUNIT_ASSERT(series->get_patient_age() == age);
    }
}

//------------------------------------------------------------------------------

void series_test::attr_equipment_test()
{
    constexpr auto institution_name  = "IRCAD";
    constexpr auto manufacturer_name = "IRCAD";
    constexpr auto model_name        = "device-0";
    constexpr auto serial_number     = "S/N:N/A";
    std::vector<std::string> software_versions {"Sight-X.Y"};

    for(const auto& series : m_series)
    {
        series->set_institution_name(institution_name);
        CPPUNIT_ASSERT(series->get_institution_name() == institution_name);

        series->set_equipment_manufacturer(manufacturer_name);
        CPPUNIT_ASSERT(series->get_equipment_manufacturer() == manufacturer_name);

        series->set_equipment_manufacturer_model_name(model_name);
        CPPUNIT_ASSERT(series->get_equipment_manufacturer_model_name() == model_name);

        series->set_equipment_device_serial_number(serial_number);
        CPPUNIT_ASSERT(series->get_equipment_device_serial_number() == serial_number);

        series->set_software_versions(software_versions);
        CPPUNIT_ASSERT(series->get_software_versions() == software_versions);
    }
}

//------------------------------------------------------------------------------

void series_test::attr_instance_uid_test()
{
    const std::string instance_uid = "1337.1664.42";

    for(const auto& series : m_series)
    {
        series->set_series_instance_uid(instance_uid);
        CPPUNIT_ASSERT_EQUAL(instance_uid, series->get_series_instance_uid());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_modality_test()
{
    for(const auto& series : m_series)
    {
        series->set_modality(data::dicom::modality_t::mr);

        CPPUNIT_ASSERT_EQUAL(data::dicom::modality_t::mr, series->get_modality());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_date_test()
{
    const std::string date = "02-14-2015";

    for(const auto& series : m_series)
    {
        series->set_series_date(date);
        CPPUNIT_ASSERT_EQUAL(date, series->get_series_date());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_time_test()
{
    const std::string time = "14:02";

    for(const auto& series : m_series)
    {
        series->set_series_time(time);
        CPPUNIT_ASSERT_EQUAL(time, series->get_series_time());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_performing_physician_name_test()
{
    const std::string performing_physician_name =
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John";

    for(const auto& series : m_series)
    {
        series->set_performing_physician_name(performing_physician_name);
        CPPUNIT_ASSERT(performing_physician_name == series->get_performing_physician_name());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_description_test()
{
    const std::string description = "Series";

    for(const auto& series : m_series)
    {
        series->set_series_description(description);
        CPPUNIT_ASSERT_EQUAL(description, series->get_series_description());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_enhanced_us_image_test()
{
    int depth_mm = 150;
    std::vector<double> focus_depths_mm {8, 64};
    std::string processing_function                                              = "gain: 54;";
    data::dicom::position_measuring_device_used_t position_measuring_device_used =
        dicom::position_measuring_device_used_t::tracked;

    for(const auto& series : m_series)
    {
        series->set_depth_of_scan_field_mm(depth_mm);
        CPPUNIT_ASSERT(depth_mm == series->get_depth_of_scan_field_mm());

        series->set_depths_of_focus_mm(focus_depths_mm);
        CPPUNIT_ASSERT(focus_depths_mm.size() == series->get_depths_of_focus_mm().size());
        for(std::size_t i = 0 ; i < focus_depths_mm.size() ; i++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(focus_depths_mm.at(i), series->get_depths_of_focus_mm().at(i), 1.e-5);
        }

        series->set_processing_function(processing_function);
        CPPUNIT_ASSERT(processing_function == series->get_processing_function());

        position_measuring_device_used = data::dicom::position_measuring_device_used_t::tracked;
        CPPUNIT_ASSERT_NO_THROW(series->set_position_measuring_device_used(position_measuring_device_used));
        CPPUNIT_ASSERT(position_measuring_device_used == series->get_position_measuring_device_used());

        position_measuring_device_used = dicom::position_measuring_device_used_t::rigid;
        CPPUNIT_ASSERT_NO_THROW(series->set_position_measuring_device_used(position_measuring_device_used));
        CPPUNIT_ASSERT(position_measuring_device_used == series->get_position_measuring_device_used());

        position_measuring_device_used = dicom::position_measuring_device_used_t::freehand;
        CPPUNIT_ASSERT_NO_THROW(series->set_position_measuring_device_used(position_measuring_device_used));
        CPPUNIT_ASSERT(position_measuring_device_used == series->get_position_measuring_device_used());
    }
}

//------------------------------------------------------------------------------

void series_test::attr_general_acquisition_test()
{
    double duration = 123.456;

    for(const auto& series : m_series)
    {
        series->set_acquisition_duration(duration);
        auto dt = series->get_acquisition_duration();
        CPPUNIT_ASSERT(dt.has_value());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, *dt, 0.0001);
    }
}

//------------------------------------------------------------------------------

void series_test::attr_ultrasound_frame_of_reference_test()
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
        CPPUNIT_ASSERT(matrix);
        for(std::size_t i = 0 ; i < volume_to_transducer_mapping_matrix.size() ; i++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(volume_to_transducer_mapping_matrix.at(i), (*matrix).at(i), 0.0001);
        }
    }
}

//------------------------------------------------------------------------------

void series_test::equality_test()
{
    auto series1 = std::make_shared<data::series>();
    auto series2 = std::make_shared<data::series>();

    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_modality(data::dicom::modality_t::us);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_modality(series1->get_modality());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_series_instance_uid("2");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_series_instance_uid(series1->get_series_instance_uid());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_series_number(3);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_series_number(series1->get_series_number());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_laterality("4");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_laterality(series1->get_laterality());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_series_date("5");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_series_date(series1->get_series_date());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_series_time("6");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_series_time(series1->get_series_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performing_physician_name("7\\8\\9");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performing_physician_name(series1->get_performing_physician_name());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_protocol_name("10");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_protocol_name(series1->get_protocol_name());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_series_description("11");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_series_description(series1->get_series_description());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_body_part_examined("12");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_body_part_examined(series1->get_body_part_examined());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_position("13");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_position(series1->get_patient_position());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_anatomical_orientation_type("14");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_anatomical_orientation_type(series1->get_anatomical_orientation_type());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performed_procedure_step_id("15");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performed_procedure_step_id(series1->get_performed_procedure_step_id());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performed_procedure_step_start_date("16");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performed_procedure_step_start_date(series1->get_performed_procedure_step_start_date());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performed_procedure_step_start_time("17");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performed_procedure_step_start_time(series1->get_performed_procedure_step_start_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performed_procedure_step_end_date("18");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performed_procedure_step_end_date(series1->get_performed_procedure_step_end_date());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performed_procedure_step_end_time("19");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performed_procedure_step_end_time(series1->get_performed_procedure_step_end_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_performed_procedure_step_description("20");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_performed_procedure_step_description(series1->get_performed_procedure_step_description());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_comments_on_the_performed_procedure_step("21");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_comments_on_the_performed_procedure_step(series1->get_comments_on_the_performed_procedure_step());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_birth_date("22");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_birth_date(series1->get_patient_birth_date());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_name("23");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_name(series1->get_patient_name());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_sex("24");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_sex(series1->get_patient_sex());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_id("25");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_id(series1->get_patient_id());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_study_instance_uid("26");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_study_instance_uid(series1->get_study_instance_uid());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_study_date("28");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_study_date(series1->get_study_date());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_study_description("29");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_study_description(series1->get_study_description());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_age("30");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_age(series1->get_patient_age());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_size(32.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_size(series1->get_patient_size());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_patient_weight(33.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_weight(series1->get_patient_weight());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_referring_physician_name("34");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_referring_physician_name(series1->get_referring_physician_name());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_study_id("36");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_study_id(series1->get_study_id());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_study_time("37");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_study_time(series1->get_study_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_institution_name("38");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_institution_name(series1->get_institution_name());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_sop_keyword(dicom::sop::Keyword::EnhancedUSVolumeStorage);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_sop_keyword(series1->get_sop_keyword());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_sop_instance_uid("40");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_sop_instance_uid(series1->get_sop_instance_uid());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_specific_character_set("41");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_specific_character_set(series1->get_specific_character_set());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_content_time("42");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_content_time(series1->get_content_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_instance_number(43);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_instance_number(series1->get_instance_number());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_acquisition_number(44);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_acquisition_number(series1->get_acquisition_number());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_rescale_intercept(45);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_rescale_intercept(series1->get_rescale_intercept());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_rescale_slope(46);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_rescale_slope(series1->get_rescale_slope());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_image_position_patient({47, 48, 49}, 0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_image_position_patient(series1->get_image_position_patient(0), 0);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_image_position_patient({50, 51, 52}, std::nullopt);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_image_position_patient(series1->get_image_position_patient(std::nullopt), std::nullopt);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_image_orientation_patient({50, 51, 52, 53, 54, 55}, 0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_image_orientation_patient(series1->get_image_orientation_patient(0), 0);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_image_orientation_patient({56, 57, 58, 59, 60, 61}, std::nullopt);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_image_orientation_patient(series1->get_image_orientation_patient(std::nullopt), std::nullopt);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_slice_thickness(56);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_slice_thickness(series1->get_slice_thickness());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    CPPUNIT_ASSERT_NO_THROW(
        series1->set_ultrasound_acquisition_geometry(
            data::dicom::ultrasound_acquisition_geometry_t::patient
        )
    );

    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_ultrasound_acquisition_geometry(series1->get_ultrasound_acquisition_geometry());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    CPPUNIT_ASSERT_NO_THROW(
        series1->set_patient_frame_of_reference_source(
            data::dicom::patient_frame_of_reference_source_t::table
        )
    );

    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_frame_of_reference_source(series1->get_patient_frame_of_reference_source());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    CPPUNIT_ASSERT_NO_THROW(
        series1->set_dimension_organization_type(
            data::dicom::dimension_organization_t::volume
        )
    );

    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_dimension_organization_type(series1->get_dimension_organization_type());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    CPPUNIT_ASSERT_NO_THROW(series1->set_referenced_sop_class_uid("54b"));
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_referenced_sop_class_uid(series1->get_referenced_sop_class_uid());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    CPPUNIT_ASSERT_NO_THROW(series1->set_referenced_sop_instance_uid("54c"));
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_referenced_sop_instance_uid(series1->get_referenced_sop_instance_uid());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_slice_thickness(0.55);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_slice_thickness(series1->get_slice_thickness());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_frame_acquisition_date_time("57", 0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_frame_acquisition_date_time(series1->get_frame_acquisition_date_time(0), 0);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_frame_acquisition_date_time("57b", std::nullopt);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_frame_acquisition_date_time(series1->get_frame_acquisition_date_time(std::nullopt), std::nullopt);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_frame_comments("58", 0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_frame_comments(series1->get_frame_comments(0), 0);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_frame_comments("58b", std::nullopt);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_frame_comments(series1->get_frame_comments(std::nullopt), std::nullopt);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_frame_label("59", 0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_frame_label(series1->get_frame_label(0), 0);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_frame_label("59b", std::nullopt);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_frame_label(series1->get_frame_label(std::nullopt), std::nullopt);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_apex_position({60.0, 61.0, 62.0});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_apex_position(series1->get_apex_position());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    data::matrix4 matrix;
    series1->set_volume_to_transducer_mapping_matrix(matrix);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_volume_to_transducer_mapping_matrix(series1->get_volume_to_transducer_mapping_matrix());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_volume_to_table_mapping_matrix(matrix);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_volume_to_table_mapping_matrix(series1->get_volume_to_table_mapping_matrix());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_image_type(
        {
            dicom::pixel_data_characteristics_t::derived,
            dicom::patient_examination_characteristics_t::secondary,
            {"AXIAL"}
        });
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_image_type(series1->get_image_type());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Test also deepcopy, just for fun
    auto series3 = std::make_shared<data::series>();
    series3->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series3 && !(*series1 != *series3));
}

//------------------------------------------------------------------------------

void series_test::sop_class_uid_test()
{
    static const dicom::sop::Keyword s_KEYWORD = dicom::sop::Keyword::CTImageStorage;

    {
        auto series = std::make_shared<data::series>();
        series->set_sop_keyword(s_KEYWORD);
        CPPUNIT_ASSERT_EQUAL(s_KEYWORD, series->get_sop_keyword());
        CPPUNIT_ASSERT_EQUAL(
            s_KEYWORD,
            dicom::sop::get(series->get_byte_value(dicom::attribute::Keyword::sop_classUID)).m_keyword
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(dicom::attribute::Keyword::sop_classUID, std::string(dicom::sop::get(s_KEYWORD).m_uid));
        CPPUNIT_ASSERT_EQUAL(s_KEYWORD, series->get_sop_keyword());
        CPPUNIT_ASSERT_EQUAL(
            s_KEYWORD,
            dicom::sop::get(series->get_byte_value(dicom::attribute::Keyword::sop_classUID)).m_keyword
        );
    }
}

//------------------------------------------------------------------------------

void series_test::sop_instance_uid_test()
{
    static const std::string s_SOP_INSTANCE_UID(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_sop_instance_uid(s_SOP_INSTANCE_UID);
        CPPUNIT_ASSERT_EQUAL(s_SOP_INSTANCE_UID, series->get_sop_instance_uid());
        CPPUNIT_ASSERT_EQUAL(
            s_SOP_INSTANCE_UID,
            series->get_byte_value(data::dicom::attribute::Keyword::SOPInstanceUID)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SOPInstanceUID, s_SOP_INSTANCE_UID);
        CPPUNIT_ASSERT_EQUAL(s_SOP_INSTANCE_UID, series->get_sop_instance_uid());
        CPPUNIT_ASSERT_EQUAL(
            s_SOP_INSTANCE_UID,
            series->get_byte_value(data::dicom::attribute::Keyword::SOPInstanceUID)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::specific_character_set_test()
{
    static const std::string s_SPECIFIC_CHARACTER_SET("ISO 2022\\IR 100");

    {
        auto series = std::make_shared<data::series>();
        series->set_specific_character_set(s_SPECIFIC_CHARACTER_SET);
        CPPUNIT_ASSERT_EQUAL(s_SPECIFIC_CHARACTER_SET, series->get_specific_character_set());
        CPPUNIT_ASSERT_EQUAL(
            s_SPECIFIC_CHARACTER_SET,
            series->get_byte_value(data::dicom::attribute::Keyword::SpecificCharacterSet)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SpecificCharacterSet, s_SPECIFIC_CHARACTER_SET);
        CPPUNIT_ASSERT_EQUAL(s_SPECIFIC_CHARACTER_SET, series->get_specific_character_set());
        CPPUNIT_ASSERT_EQUAL(
            s_SPECIFIC_CHARACTER_SET,
            series->get_byte_value(data::dicom::attribute::Keyword::SpecificCharacterSet)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::encoding_test()
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
        auto series = std::make_shared<data::series>();

        for(const auto& character_set : s_CHARACTER_SET)
        {
            series->set_specific_character_set(character_set.first);
            CPPUNIT_ASSERT_EQUAL(character_set.second, series->get_encoding());
        }
    }

    {
        // By default, UTF-8 should be used...
        auto series = std::make_shared<data::series>();
        CPPUNIT_ASSERT_EQUAL(std::string("UTF-8"), series->get_encoding());
    }
}

//------------------------------------------------------------------------------

void series_test::series_date_test()
{
    static const std::string s_SERIES_DATE("20180201");

    {
        auto series = std::make_shared<data::series>();
        series->set_series_date(s_SERIES_DATE);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_DATE, series->get_series_date());
        CPPUNIT_ASSERT_EQUAL(s_SERIES_DATE, series->get_byte_value(data::dicom::attribute::Keyword::SeriesDate));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SeriesDate, s_SERIES_DATE);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_DATE, series->get_series_date());
        CPPUNIT_ASSERT_EQUAL(s_SERIES_DATE, series->get_byte_value(data::dicom::attribute::Keyword::SeriesDate));
    }
}

//------------------------------------------------------------------------------

void series_test::series_time_test()
{
    static const std::string s_SERIES_TIME("165649.123456");

    {
        auto series = std::make_shared<data::series>();
        series->set_series_time(s_SERIES_TIME);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_TIME, series->get_series_time());
        CPPUNIT_ASSERT_EQUAL(s_SERIES_TIME, series->get_byte_value(data::dicom::attribute::Keyword::SeriesTime));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SeriesTime, s_SERIES_TIME);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_TIME, series->get_series_time());
        CPPUNIT_ASSERT_EQUAL(s_SERIES_TIME, series->get_byte_value(data::dicom::attribute::Keyword::SeriesTime));
    }
}

//------------------------------------------------------------------------------

void series_test::modality_test()
{
    static const data::dicom::modality_t s_MODALITY = data::dicom::modality_t::us;
    static const std::string s_MODALITY_STRING(*data::dicom::to_string(s_MODALITY));

    {
        auto series = std::make_shared<data::series>();
        series->set_modality(s_MODALITY);
        CPPUNIT_ASSERT_EQUAL(s_MODALITY, series->get_modality());
        CPPUNIT_ASSERT_EQUAL(s_MODALITY_STRING, series->get_byte_value(data::dicom::attribute::Keyword::Modality));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::Modality, s_MODALITY_STRING);
        CPPUNIT_ASSERT_EQUAL(s_MODALITY, series->get_modality());
        CPPUNIT_ASSERT_EQUAL(s_MODALITY_STRING, series->get_byte_value(data::dicom::attribute::Keyword::Modality));
    }
}

//------------------------------------------------------------------------------

void series_test::series_description_test()
{
    static const std::string s_SERIES_DESCRIPTION(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_series_description(s_SERIES_DESCRIPTION);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_DESCRIPTION, series->get_series_description());
        CPPUNIT_ASSERT_EQUAL(
            s_SERIES_DESCRIPTION,
            series->get_byte_value(data::dicom::attribute::Keyword::SeriesDescription)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SeriesDescription, s_SERIES_DESCRIPTION);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_DESCRIPTION, series->get_series_description());
        CPPUNIT_ASSERT_EQUAL(
            s_SERIES_DESCRIPTION,
            series->get_byte_value(data::dicom::attribute::Keyword::SeriesDescription)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::performing_physician_name_test()
{
    static const std::vector<std::string> s_PERFORMING_PHYSICIAN_NAMES = {
        uuid::generate(),
        uuid::generate(),
        uuid::generate()
    };

    static const std::string s_PERFORMING_PHYSICIAN_NAME = boost::join(s_PERFORMING_PHYSICIAN_NAMES, "\\");

    {
        auto series = std::make_shared<data::series>();
        series->set_performing_physician_names(s_PERFORMING_PHYSICIAN_NAMES);
        CPPUNIT_ASSERT(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
        CPPUNIT_ASSERT(
            s_PERFORMING_PHYSICIAN_NAMES
            == series->get_byte_values(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMING_PHYSICIAN_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_values(data::dicom::attribute::Keyword::PerformingPhysicianName, s_PERFORMING_PHYSICIAN_NAMES);
        CPPUNIT_ASSERT(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
        CPPUNIT_ASSERT(
            s_PERFORMING_PHYSICIAN_NAMES
            == series->get_byte_values(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMING_PHYSICIAN_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_performing_physician_name(s_PERFORMING_PHYSICIAN_NAME);
        CPPUNIT_ASSERT(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
        CPPUNIT_ASSERT(
            s_PERFORMING_PHYSICIAN_NAMES
            == series->get_byte_values(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMING_PHYSICIAN_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PerformingPhysicianName, s_PERFORMING_PHYSICIAN_NAME);
        CPPUNIT_ASSERT(s_PERFORMING_PHYSICIAN_NAMES == series->get_performing_physician_names());
        CPPUNIT_ASSERT(
            s_PERFORMING_PHYSICIAN_NAMES
            == series->get_byte_values(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(s_PERFORMING_PHYSICIAN_NAME, series->get_performing_physician_name());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMING_PHYSICIAN_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::anatomical_orientation_type_test()
{
    static const std::string s_ANATOMICAL_ORIENTATION_TYPE("QUADRUPED");

    {
        auto series = std::make_shared<data::series>();
        series->set_anatomical_orientation_type(s_ANATOMICAL_ORIENTATION_TYPE);
        CPPUNIT_ASSERT_EQUAL(s_ANATOMICAL_ORIENTATION_TYPE, series->get_anatomical_orientation_type());
        CPPUNIT_ASSERT_EQUAL(
            s_ANATOMICAL_ORIENTATION_TYPE,
            series->get_byte_value(data::dicom::attribute::Keyword::AnatomicalOrientationType)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::AnatomicalOrientationType,
            s_ANATOMICAL_ORIENTATION_TYPE
        );
        CPPUNIT_ASSERT_EQUAL(s_ANATOMICAL_ORIENTATION_TYPE, series->get_anatomical_orientation_type());
        CPPUNIT_ASSERT_EQUAL(
            s_ANATOMICAL_ORIENTATION_TYPE,
            series->get_byte_value(data::dicom::attribute::Keyword::AnatomicalOrientationType)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::body_part_examined_test()
{
    static const std::string s_BODY_PART_EXAMINED("ABDOMEN");

    {
        auto series = std::make_shared<data::series>();
        series->set_body_part_examined(s_BODY_PART_EXAMINED);
        CPPUNIT_ASSERT_EQUAL(s_BODY_PART_EXAMINED, series->get_body_part_examined());
        CPPUNIT_ASSERT_EQUAL(
            s_BODY_PART_EXAMINED,
            series->get_byte_value(data::dicom::attribute::Keyword::BodyPartExamined)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::BodyPartExamined, s_BODY_PART_EXAMINED);
        CPPUNIT_ASSERT_EQUAL(s_BODY_PART_EXAMINED, series->get_body_part_examined());
        CPPUNIT_ASSERT_EQUAL(
            s_BODY_PART_EXAMINED,
            series->get_byte_value(data::dicom::attribute::Keyword::BodyPartExamined)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::protocol_name_test()
{
    static const std::string s_PROTOCOL_NAME(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_protocol_name(s_PROTOCOL_NAME);
        CPPUNIT_ASSERT_EQUAL(s_PROTOCOL_NAME, series->get_protocol_name());
        CPPUNIT_ASSERT_EQUAL(s_PROTOCOL_NAME, series->get_byte_value(data::dicom::attribute::Keyword::ProtocolName));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ProtocolName, s_PROTOCOL_NAME);
        CPPUNIT_ASSERT_EQUAL(s_PROTOCOL_NAME, series->get_protocol_name());
        CPPUNIT_ASSERT_EQUAL(s_PROTOCOL_NAME, series->get_byte_value(data::dicom::attribute::Keyword::ProtocolName));
    }
}

//------------------------------------------------------------------------------

void series_test::patient_position_test()
{
    static const std::string s_PATIENT_POSITION("FFP");

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_position(s_PATIENT_POSITION);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_POSITION, series->get_patient_position());
        CPPUNIT_ASSERT_EQUAL(
            s_PATIENT_POSITION,
            series->get_byte_value(data::dicom::attribute::Keyword::PatientPosition)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientPosition, s_PATIENT_POSITION);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_POSITION, series->get_patient_position());
        CPPUNIT_ASSERT_EQUAL(
            s_PATIENT_POSITION,
            series->get_byte_value(data::dicom::attribute::Keyword::PatientPosition)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::series_instance_uid_test()
{
    static const std::string s_SERIES_INSTANCE_UID(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_series_instance_uid(s_SERIES_INSTANCE_UID);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_INSTANCE_UID, series->get_series_instance_uid());
        CPPUNIT_ASSERT_EQUAL(
            s_SERIES_INSTANCE_UID,
            series->get_byte_value(data::dicom::attribute::Keyword::SeriesInstanceUID)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SeriesInstanceUID, s_SERIES_INSTANCE_UID);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_INSTANCE_UID, series->get_series_instance_uid());
        CPPUNIT_ASSERT_EQUAL(
            s_SERIES_INSTANCE_UID,
            series->get_byte_value(data::dicom::attribute::Keyword::SeriesInstanceUID)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::series_number_test()
{
    static const std::int32_t s_SERIES_NUMBER = 1;

    {
        auto series = std::make_shared<data::series>();
        series->set_series_number(s_SERIES_NUMBER);
        CPPUNIT_ASSERT_EQUAL(s_SERIES_NUMBER, *series->get_series_number());
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(s_SERIES_NUMBER),
            series->get_byte_value(data::dicom::attribute::Keyword::SeriesNumber)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::SeriesNumber, std::to_string(s_SERIES_NUMBER));
        CPPUNIT_ASSERT_EQUAL(s_SERIES_NUMBER, *series->get_series_number());
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(s_SERIES_NUMBER),
            series->get_byte_value(data::dicom::attribute::Keyword::SeriesNumber)
        );
    }

    {
        // Test emtpy value
        auto series = std::make_shared<data::series>();
        series->set_series_number();

        CPPUNIT_ASSERT(!series->get_series_number());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->get_byte_value(data::dicom::attribute::Keyword::SeriesNumber));
    }
}

//------------------------------------------------------------------------------

void series_test::laterality_test()
{
    static const std::string s_LATERALITY("R");

    {
        auto series = std::make_shared<data::series>();
        series->set_laterality(s_LATERALITY);
        CPPUNIT_ASSERT_EQUAL(s_LATERALITY, series->get_laterality());
        CPPUNIT_ASSERT_EQUAL(s_LATERALITY, series->get_byte_value(data::dicom::attribute::Keyword::Laterality));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::Laterality, s_LATERALITY);
        CPPUNIT_ASSERT_EQUAL(s_LATERALITY, series->get_laterality());
        CPPUNIT_ASSERT_EQUAL(s_LATERALITY, series->get_byte_value(data::dicom::attribute::Keyword::Laterality));
    }
}

//------------------------------------------------------------------------------

void series_test::performed_procedure_step_start_date_test()
{
    static const std::string s_PERFORMED_PROCEDURE_STEP_START_DATE("20000101");

    {
        auto series = std::make_shared<data::series>();
        series->set_performed_procedure_step_start_date(s_PERFORMED_PROCEDURE_STEP_START_DATE);
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_START_DATE, series->get_performed_procedure_step_start_date());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_START_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepStartDate)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::PerformedProcedureStepStartDate,
            s_PERFORMED_PROCEDURE_STEP_START_DATE
        );
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_START_DATE, series->get_performed_procedure_step_start_date());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_START_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepStartDate)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::performed_procedure_step_start_time_test()
{
    static const std::string s_PERFORMED_PROCEDURE_STEP_START_TIME("165649.123456");

    {
        auto series = std::make_shared<data::series>();
        series->set_performed_procedure_step_start_time(s_PERFORMED_PROCEDURE_STEP_START_TIME);
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_START_TIME, series->get_performed_procedure_step_start_time());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_START_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepStartTime)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::PerformedProcedureStepStartTime,
            s_PERFORMED_PROCEDURE_STEP_START_TIME
        );
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_START_TIME, series->get_performed_procedure_step_start_time());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_START_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepStartTime)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::performed_procedure_step_end_date_test()
{
    static const std::string s_PERFORMED_PROCEDURE_STEP_END_DATE("20000101");

    {
        auto series = std::make_shared<data::series>();
        series->set_performed_procedure_step_end_date(s_PERFORMED_PROCEDURE_STEP_END_DATE);
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_END_DATE, series->get_performed_procedure_step_end_date());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_END_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepEndDate)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::PerformedProcedureStepEndDate,
            s_PERFORMED_PROCEDURE_STEP_END_DATE
        );
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_END_DATE, series->get_performed_procedure_step_end_date());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_END_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepEndDate)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::performed_procedure_step_end_time_test()
{
    static const std::string s_PERFORMED_PROCEDURE_STEP_END_TIME("165649.123456");

    {
        auto series = std::make_shared<data::series>();
        series->set_performed_procedure_step_end_time(s_PERFORMED_PROCEDURE_STEP_END_TIME);
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_END_TIME, series->get_performed_procedure_step_end_time());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_END_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepEndTime)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::PerformedProcedureStepEndTime,
            s_PERFORMED_PROCEDURE_STEP_END_TIME
        );
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_END_TIME, series->get_performed_procedure_step_end_time());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_END_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepEndTime)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::performed_procedure_step_id_test()
{
    static const std::string s_PERFORMED_PROCEDURE_STEP_ID("B");

    {
        auto series = std::make_shared<data::series>();
        series->set_performed_procedure_step_id(s_PERFORMED_PROCEDURE_STEP_ID);
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_ID, series->get_performed_procedure_step_id());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_ID,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepID)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::PerformedProcedureStepID,
            s_PERFORMED_PROCEDURE_STEP_ID
        );
        CPPUNIT_ASSERT_EQUAL(s_PERFORMED_PROCEDURE_STEP_ID, series->get_performed_procedure_step_id());
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_ID,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepID)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::performed_procedure_step_description_test()
{
    static const std::string s_PERFORMED_PROCEDURE_STEP_DESCRIPTION(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_performed_procedure_step_description(s_PERFORMED_PROCEDURE_STEP_DESCRIPTION);
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
            series->get_performed_procedure_step_description()
        );
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepDescription)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::PerformedProcedureStepDescription,
            s_PERFORMED_PROCEDURE_STEP_DESCRIPTION
        );
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
            series->get_performed_procedure_step_description()
        );
        CPPUNIT_ASSERT_EQUAL(
            s_PERFORMED_PROCEDURE_STEP_DESCRIPTION,
            series->get_byte_value(data::dicom::attribute::Keyword::PerformedProcedureStepDescription)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::comments_on_the_performed_procedure_step_test()
{
    static const std::string s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_comments_on_the_performed_procedure_step(s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP);
        CPPUNIT_ASSERT_EQUAL(
            s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
            series->get_comments_on_the_performed_procedure_step()
        );
        CPPUNIT_ASSERT_EQUAL(
            s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
            series->get_byte_value(data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep,
            s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP
        );
        CPPUNIT_ASSERT_EQUAL(
            s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
            series->get_comments_on_the_performed_procedure_step()
        );
        CPPUNIT_ASSERT_EQUAL(
            s_COMMENTS_ON_THE_PERFORMED_PROCEDURE_STEP,
            series->get_byte_value(data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::institution_name_test()
{
    static const std::string s_INSTITUTION_NAME(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_institution_name(s_INSTITUTION_NAME);
        CPPUNIT_ASSERT_EQUAL(s_INSTITUTION_NAME, series->get_institution_name());
        CPPUNIT_ASSERT_EQUAL(
            s_INSTITUTION_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::InstitutionName)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::InstitutionName, s_INSTITUTION_NAME);
        CPPUNIT_ASSERT_EQUAL(s_INSTITUTION_NAME, series->get_institution_name());
        CPPUNIT_ASSERT_EQUAL(
            s_INSTITUTION_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::InstitutionName)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::patient_name_test()
{
    static const std::string s_PATIENT_NAME(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_name(s_PATIENT_NAME);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_NAME, series->get_patient_name());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_NAME, series->get_byte_value(data::dicom::attribute::Keyword::PatientName));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientName, s_PATIENT_NAME);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_NAME, series->get_patient_name());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_NAME, series->get_byte_value(data::dicom::attribute::Keyword::PatientName));
    }
}

//------------------------------------------------------------------------------

void series_test::patient_id_test()
{
    static const std::string s_PATIENT_ID(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_id(s_PATIENT_ID);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_ID, series->get_patient_id());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_ID, series->get_byte_value(data::dicom::attribute::Keyword::PatientID));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientID, s_PATIENT_ID);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_ID, series->get_patient_id());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_ID, series->get_byte_value(data::dicom::attribute::Keyword::PatientID));
    }
}

//------------------------------------------------------------------------------

void series_test::patient_birth_date_test()
{
    static const std::string s_PATIENT_BIRTH_DATE("20000101");

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_birth_date(s_PATIENT_BIRTH_DATE);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_BIRTH_DATE, series->get_patient_birth_date());
        CPPUNIT_ASSERT_EQUAL(
            s_PATIENT_BIRTH_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::PatientBirthDate)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientBirthDate, s_PATIENT_BIRTH_DATE);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_BIRTH_DATE, series->get_patient_birth_date());
        CPPUNIT_ASSERT_EQUAL(
            s_PATIENT_BIRTH_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::PatientBirthDate)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::patient_sex_test()
{
    static const std::string s_PATIENT_SEX("M");

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_sex(s_PATIENT_SEX);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_SEX, series->get_patient_sex());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_SEX, series->get_byte_value(data::dicom::attribute::Keyword::PatientSex));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientSex, s_PATIENT_SEX);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_SEX, series->get_patient_sex());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_SEX, series->get_byte_value(data::dicom::attribute::Keyword::PatientSex));
    }
}

//------------------------------------------------------------------------------

void series_test::study_date_test()
{
    static const std::string s_STUDY_DATE("20011012");

    {
        auto series = std::make_shared<data::series>();
        series->set_study_date(s_STUDY_DATE);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_DATE, series->get_study_date());
        CPPUNIT_ASSERT_EQUAL(s_STUDY_DATE, series->get_byte_value(data::dicom::attribute::Keyword::StudyDate));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::StudyDate, s_STUDY_DATE);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_DATE, series->get_study_date());
        CPPUNIT_ASSERT_EQUAL(s_STUDY_DATE, series->get_byte_value(data::dicom::attribute::Keyword::StudyDate));
    }
}

//------------------------------------------------------------------------------

void series_test::study_time_test()
{
    static const std::string s_STUDY_TIME("224513.123456");

    {
        auto series = std::make_shared<data::series>();
        series->set_study_time(s_STUDY_TIME);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_TIME, series->get_study_time());
        CPPUNIT_ASSERT_EQUAL(s_STUDY_TIME, series->get_byte_value(data::dicom::attribute::Keyword::StudyTime));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::StudyTime, s_STUDY_TIME);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_TIME, series->get_study_time());
        CPPUNIT_ASSERT_EQUAL(s_STUDY_TIME, series->get_byte_value(data::dicom::attribute::Keyword::StudyTime));
    }
}

//------------------------------------------------------------------------------

void series_test::referring_physician_name_test()
{
    static const std::string s_REFERRING_PHYSICIAN_NAME(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_referring_physician_name(s_REFERRING_PHYSICIAN_NAME);
        CPPUNIT_ASSERT_EQUAL(s_REFERRING_PHYSICIAN_NAME, series->get_referring_physician_name());
        CPPUNIT_ASSERT_EQUAL(
            s_REFERRING_PHYSICIAN_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::ReferringPhysicianName)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ReferringPhysicianName, s_REFERRING_PHYSICIAN_NAME);
        CPPUNIT_ASSERT_EQUAL(s_REFERRING_PHYSICIAN_NAME, series->get_referring_physician_name());
        CPPUNIT_ASSERT_EQUAL(
            s_REFERRING_PHYSICIAN_NAME,
            series->get_byte_value(data::dicom::attribute::Keyword::ReferringPhysicianName)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::study_description_test()
{
    static const std::string s_STUDY_DESCRIPTION(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_study_description(s_STUDY_DESCRIPTION);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_DESCRIPTION, series->get_study_description());
        CPPUNIT_ASSERT_EQUAL(
            s_STUDY_DESCRIPTION,
            series->get_byte_value(data::dicom::attribute::Keyword::StudyDescription)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::StudyDescription, s_STUDY_DESCRIPTION);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_DESCRIPTION, series->get_study_description());
        CPPUNIT_ASSERT_EQUAL(
            s_STUDY_DESCRIPTION,
            series->get_byte_value(data::dicom::attribute::Keyword::StudyDescription)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::study_instance_uid_test()
{
    static const std::string s_STUDY_INSTANCE_UID(uuid::generate());

    {
        auto series = std::make_shared<data::series>();
        series->set_study_instance_uid(s_STUDY_INSTANCE_UID);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_INSTANCE_UID, series->get_study_instance_uid());
        CPPUNIT_ASSERT_EQUAL(
            s_STUDY_INSTANCE_UID,
            series->get_byte_value(data::dicom::attribute::Keyword::StudyInstanceUID)
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::StudyInstanceUID, s_STUDY_INSTANCE_UID);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_INSTANCE_UID, series->get_study_instance_uid());
        CPPUNIT_ASSERT_EQUAL(
            s_STUDY_INSTANCE_UID,
            series->get_byte_value(data::dicom::attribute::Keyword::StudyInstanceUID)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::study_id_test()
{
    static const std::string s_STUDY_ID("A");

    {
        auto series = std::make_shared<data::series>();
        series->set_study_id(s_STUDY_ID);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_ID, series->get_study_id());
        CPPUNIT_ASSERT_EQUAL(s_STUDY_ID, series->get_byte_value(data::dicom::attribute::Keyword::StudyID));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::StudyID, s_STUDY_ID);
        CPPUNIT_ASSERT_EQUAL(s_STUDY_ID, series->get_study_id());
        CPPUNIT_ASSERT_EQUAL(s_STUDY_ID, series->get_byte_value(data::dicom::attribute::Keyword::StudyID));
    }
}

//------------------------------------------------------------------------------

void series_test::patient_age_test()
{
    static const std::string s_PATIENT_AGE("015Y");

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_age(s_PATIENT_AGE);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_AGE, series->get_patient_age());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_AGE, series->get_byte_value(data::dicom::attribute::Keyword::PatientAge));
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientAge, s_PATIENT_AGE);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_AGE, series->get_patient_age());
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_AGE, series->get_byte_value(data::dicom::attribute::Keyword::PatientAge));
    }
}

//------------------------------------------------------------------------------

void series_test::patient_size_test()
{
    static const double s_PATIENT_SIZE = 188.53;

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_size(s_PATIENT_SIZE);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_SIZE, *series->get_patient_size());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_PATIENT_SIZE,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::PatientSize)),
            0.00001
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientSize, std::to_string(s_PATIENT_SIZE));
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_SIZE, *series->get_patient_size());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_PATIENT_SIZE,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::PatientSize)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = std::make_shared<data::series>();
        series->set_patient_size();

        CPPUNIT_ASSERT(!series->get_patient_size());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->get_byte_value(data::dicom::attribute::Keyword::PatientSize));
    }
}

//------------------------------------------------------------------------------

void series_test::patient_weight_test()
{
    static const double s_PATIENT_WEIGHT = 115.07;

    {
        auto series = std::make_shared<data::series>();
        series->set_patient_weight(s_PATIENT_WEIGHT);
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_WEIGHT, *series->get_patient_weight());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_PATIENT_WEIGHT,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::PatientWeight)),
            0.00001
        );
    }

    {
        auto series = std::make_shared<data::series>();
        series->set_byte_value(data::dicom::attribute::Keyword::PatientWeight, std::to_string(s_PATIENT_WEIGHT));
        CPPUNIT_ASSERT_EQUAL(s_PATIENT_WEIGHT, *series->get_patient_weight());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_PATIENT_WEIGHT,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::PatientWeight)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = std::make_shared<data::series>();
        series->set_patient_weight();

        CPPUNIT_ASSERT(!series->get_patient_weight());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->get_byte_value(data::dicom::attribute::Keyword::PatientWeight));
    }
}

//------------------------------------------------------------------------------

void series_test::acquisition_date_test()
{
    static const std::string s_ACQUISITION_DATE("20011012");

    {
        auto series = std::make_shared<data::image_series>();
        series->set_acquisition_date(s_ACQUISITION_DATE);
        CPPUNIT_ASSERT_EQUAL(s_ACQUISITION_DATE, series->get_acquisition_date());
        CPPUNIT_ASSERT_EQUAL(
            s_ACQUISITION_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::AcquisitionDate)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::AcquisitionDate, s_ACQUISITION_DATE);
        CPPUNIT_ASSERT_EQUAL(s_ACQUISITION_DATE, series->get_acquisition_date());
        CPPUNIT_ASSERT_EQUAL(
            s_ACQUISITION_DATE,
            series->get_byte_value(data::dicom::attribute::Keyword::AcquisitionDate)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::acquisition_time_test()
{
    static const std::string s_ACQUISITION_TIME("224513.123456");

    {
        auto series = std::make_shared<data::image_series>();
        series->set_acquisition_time(s_ACQUISITION_TIME);
        CPPUNIT_ASSERT_EQUAL(s_ACQUISITION_TIME, series->get_acquisition_time());
        CPPUNIT_ASSERT_EQUAL(
            s_ACQUISITION_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::AcquisitionTime)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::AcquisitionTime, s_ACQUISITION_TIME);
        CPPUNIT_ASSERT_EQUAL(s_ACQUISITION_TIME, series->get_acquisition_time());
        CPPUNIT_ASSERT_EQUAL(
            s_ACQUISITION_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::AcquisitionTime)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::acquisition_number_test()
{
    static const std::int32_t s_ACQUISITION_NUMBER = 0x00200012;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_acquisition_number(s_ACQUISITION_NUMBER);
        CPPUNIT_ASSERT_EQUAL(s_ACQUISITION_NUMBER, series->get_acquisition_number().value_or(0));
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(s_ACQUISITION_NUMBER),
            series->get_byte_value(data::dicom::attribute::Keyword::AcquisitionNumber)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::AcquisitionNumber,
            std::to_string(s_ACQUISITION_NUMBER)
        );
        CPPUNIT_ASSERT_EQUAL(s_ACQUISITION_NUMBER, series->get_acquisition_number().value_or(0));
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(s_ACQUISITION_NUMBER),
            series->get_byte_value(data::dicom::attribute::Keyword::AcquisitionNumber)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::instance_number_test()
{
    static const std::int32_t s_INSTANCE_NUMBER = 0x00200013;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_instance_number(s_INSTANCE_NUMBER);
        CPPUNIT_ASSERT_EQUAL(s_INSTANCE_NUMBER, series->get_instance_number().value_or(0));
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(s_INSTANCE_NUMBER),
            series->get_byte_value(data::dicom::attribute::Keyword::InstanceNumber)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::InstanceNumber, std::to_string(s_INSTANCE_NUMBER));
        CPPUNIT_ASSERT_EQUAL(s_INSTANCE_NUMBER, series->get_instance_number().value_or(0));
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(s_INSTANCE_NUMBER),
            series->get_byte_value(data::dicom::attribute::Keyword::InstanceNumber)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::content_time_test()
{
    static const std::string s_CONTENT_TIME("224513.123456");

    {
        auto series = std::make_shared<data::image_series>();
        series->set_content_time(s_CONTENT_TIME);
        CPPUNIT_ASSERT_EQUAL(s_CONTENT_TIME, series->get_content_time());
        CPPUNIT_ASSERT_EQUAL(s_CONTENT_TIME, series->get_byte_value(data::dicom::attribute::Keyword::ContentTime));
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContentTime, s_CONTENT_TIME);
        CPPUNIT_ASSERT_EQUAL(s_CONTENT_TIME, series->get_content_time());
        CPPUNIT_ASSERT_EQUAL(s_CONTENT_TIME, series->get_byte_value(data::dicom::attribute::Keyword::ContentTime));
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_agent_test()
{
    static const std::string s_CONTRAST_BOLUS_AGENT(uuid::generate());

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_agent(s_CONTRAST_BOLUS_AGENT);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_AGENT, series->get_contrast_bolus_agent());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_AGENT,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusAgent)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastBolusAgent, s_CONTRAST_BOLUS_AGENT);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_AGENT, series->get_contrast_bolus_agent());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_AGENT,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusAgent)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_route_test()
{
    static const std::string s_CONTRAST_BOLUS_ROUTE(uuid::generate());

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_route(s_CONTRAST_BOLUS_ROUTE);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_ROUTE, series->get_contrast_bolus_route());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_ROUTE,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusRoute)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastBolusRoute, s_CONTRAST_BOLUS_ROUTE);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_ROUTE, series->get_contrast_bolus_route());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_ROUTE,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusRoute)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_volume_test()
{
    static const double s_CONTRAST_BOLUS_VOLUME = 188.53;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_volume(s_CONTRAST_BOLUS_VOLUME);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_VOLUME, *series->get_contrast_bolus_volume());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_CONTRAST_BOLUS_VOLUME,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusVolume)),
            0.00001
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::ContrastBolusVolume,
            std::to_string(s_CONTRAST_BOLUS_VOLUME)
        );
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_VOLUME, *series->get_contrast_bolus_volume());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_CONTRAST_BOLUS_VOLUME,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusVolume)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_volume();

        CPPUNIT_ASSERT(!series->get_contrast_bolus_volume());
        CPPUNIT_ASSERT_EQUAL(
            std::string(),
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusVolume)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_start_time_test()
{
    static const std::string s_CONTRAST_BOLUS_START_TIME("224513.123456");

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_start_time(s_CONTRAST_BOLUS_START_TIME);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_START_TIME, series->get_contrast_bolus_start_time());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_START_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusStartTime)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastBolusStartTime, s_CONTRAST_BOLUS_START_TIME);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_START_TIME, series->get_contrast_bolus_start_time());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_START_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusStartTime)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_stop_time_test()
{
    static const std::string s_CONTRAST_BOLUS_STOP_TIME("224513.123456");

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_stop_time(s_CONTRAST_BOLUS_STOP_TIME);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_STOP_TIME, series->get_contrast_bolus_stop_time());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_STOP_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusStopTime)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastBolusStopTime, s_CONTRAST_BOLUS_STOP_TIME);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_STOP_TIME, series->get_contrast_bolus_stop_time());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_STOP_TIME,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusStopTime)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_total_dose_test()
{
    static const double s_CONTRAST_BOLUS_TOTAL_DOSE = 123.45;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_total_dose(s_CONTRAST_BOLUS_TOTAL_DOSE);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_TOTAL_DOSE, *series->get_contrast_bolus_total_dose());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_CONTRAST_BOLUS_TOTAL_DOSE,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusTotalDose)),
            0.00001
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::ContrastBolusTotalDose,
            std::to_string(s_CONTRAST_BOLUS_TOTAL_DOSE)
        );
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_TOTAL_DOSE, *series->get_contrast_bolus_total_dose());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_CONTRAST_BOLUS_TOTAL_DOSE,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusTotalDose)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_total_dose();

        CPPUNIT_ASSERT(!series->get_contrast_bolus_total_dose());
        CPPUNIT_ASSERT_EQUAL(
            std::string(),
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusTotalDose)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_flow_rate_test()
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
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_flow_rates(s_CONTRAST_FLOW_RATES);
        CPPUNIT_ASSERT(core::is_equal(s_CONTRAST_FLOW_RATES, series->get_contrast_flow_rates()));
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_RATE_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_RATE,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowRate)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_values(data::dicom::attribute::Keyword::ContrastFlowRate, s_CONTRAST_FLOW_RATE_STRINGS);
        CPPUNIT_ASSERT(s_CONTRAST_FLOW_RATES == series->get_contrast_flow_rates());
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_RATE_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_RATE,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowRate)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_flow_rate(s_CONTRAST_FLOW_RATE);
        CPPUNIT_ASSERT(s_CONTRAST_FLOW_RATES == series->get_contrast_flow_rates());
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_RATE_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_RATE,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowRate)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastFlowRate, s_CONTRAST_FLOW_RATE);
        CPPUNIT_ASSERT(s_CONTRAST_FLOW_RATES == series->get_contrast_flow_rates());
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_RATE_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_RATE, series->get_contrast_flow_rate());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_RATE,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowRate)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_flow_duration_test()
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
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_flow_durations(s_CONTRAST_FLOW_DURATIONS);
        CPPUNIT_ASSERT(core::is_equal(s_CONTRAST_FLOW_DURATIONS, series->get_contrast_flow_durations()));
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_DURATION_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_DURATION,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_values(
            data::dicom::attribute::Keyword::ContrastFlowDuration,
            s_CONTRAST_FLOW_DURATION_STRINGS
        );
        CPPUNIT_ASSERT(s_CONTRAST_FLOW_DURATIONS == series->get_contrast_flow_durations());
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_DURATION_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_DURATION,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_flow_duration(s_CONTRAST_FLOW_DURATION);
        CPPUNIT_ASSERT(s_CONTRAST_FLOW_DURATIONS == series->get_contrast_flow_durations());
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_DURATION_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_DURATION,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastFlowDuration, s_CONTRAST_FLOW_DURATION);
        CPPUNIT_ASSERT(s_CONTRAST_FLOW_DURATIONS == series->get_contrast_flow_durations());
        CPPUNIT_ASSERT(
            s_CONTRAST_FLOW_DURATION_STRINGS
            == series->get_byte_values(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_FLOW_DURATION, series->get_contrast_flow_duration());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_FLOW_DURATION,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_ingredient_test()
{
    static const std::string s_CONTRAST_BOLUS_INGREDIENT("GADOLINIUM");

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_ingredient(s_CONTRAST_BOLUS_INGREDIENT);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_INGREDIENT, series->get_contrast_bolus_ingredient());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_INGREDIENT,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusIngredient)
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::ContrastBolusIngredient, s_CONTRAST_BOLUS_INGREDIENT);
        CPPUNIT_ASSERT_EQUAL(s_CONTRAST_BOLUS_INGREDIENT, series->get_contrast_bolus_ingredient());
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_INGREDIENT,
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusIngredient)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::contrast_bolus_ingredient_concentration_test()
{
    static const double s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION = 123.45;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_ingredient_concentration(s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION);
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION,
            *series->get_contrast_bolus_ingredient_concentration()
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)),
            0.00001
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(
            data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration,
            std::to_string(s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION)
        );
        CPPUNIT_ASSERT_EQUAL(
            s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION,
            *series->get_contrast_bolus_ingredient_concentration()
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            s_CONTRAST_BOLUS_INGREDIENT_CONCENTRATION,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = std::make_shared<data::image_series>();
        series->set_contrast_bolus_ingredient_concentration();

        CPPUNIT_ASSERT(!series->get_contrast_bolus_ingredient_concentration());
        CPPUNIT_ASSERT_EQUAL(
            std::string(),
            series->get_byte_value(data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)
        );
    }
}

//------------------------------------------------------------------------------

void series_test::rows_test()
{
    static const std::uint16_t s_ROWS = 256;
    static const std::string s_ROWS_BYTES(reinterpret_cast<const char*>(&s_ROWS), sizeof(s_ROWS));

    {
        auto series = std::make_shared<data::image_series>();
        series->set_rows(s_ROWS);
        CPPUNIT_ASSERT_EQUAL(s_ROWS, series->get_rows().value_or(0));
        CPPUNIT_ASSERT_EQUAL(s_ROWS_BYTES, series->get_byte_value(data::dicom::attribute::Keyword::Rows));
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::Rows, s_ROWS_BYTES);
        CPPUNIT_ASSERT_EQUAL(s_ROWS, series->get_rows().value_or(0));
        CPPUNIT_ASSERT_EQUAL(s_ROWS_BYTES, series->get_byte_value(data::dicom::attribute::Keyword::Rows));
    }
}

//------------------------------------------------------------------------------

void series_test::columns_test()
{
    static const std::uint16_t s_COLUMNS = 256;
    static const std::string s_COLUMNS_BYTES(reinterpret_cast<const char*>(&s_COLUMNS), sizeof(s_COLUMNS));

    {
        auto series = std::make_shared<data::image_series>();
        series->set_columns(s_COLUMNS);
        CPPUNIT_ASSERT_EQUAL(s_COLUMNS, series->get_columns().value_or(0));
        CPPUNIT_ASSERT_EQUAL(s_COLUMNS_BYTES, series->get_byte_value(data::dicom::attribute::Keyword::Columns));
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::Columns, s_COLUMNS_BYTES);
        CPPUNIT_ASSERT_EQUAL(s_COLUMNS, series->get_columns().value_or(0));
        CPPUNIT_ASSERT_EQUAL(s_COLUMNS_BYTES, series->get_byte_value(data::dicom::attribute::Keyword::Columns));
    }
}

//------------------------------------------------------------------------------

void series_test::window_center_test()
{
    static const double s_CENTER = 333.33;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_window_center({s_CENTER});
        CPPUNIT_ASSERT_EQUAL(s_CENTER, series->window_center().front());
        CPPUNIT_ASSERT_EQUAL(
            s_CENTER,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::WindowCenter))
        );
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::WindowCenter, std::to_string(s_CENTER));
        CPPUNIT_ASSERT_EQUAL(s_CENTER, series->window_center().front());
        CPPUNIT_ASSERT_EQUAL(
            s_CENTER,
            std::stod(series->get_byte_value(data::dicom::attribute::Keyword::WindowCenter))
        );
    }
}

//------------------------------------------------------------------------------

void series_test::window_width_test()
{
    static const double s_WIDTH = 666.66;

    {
        auto series = std::make_shared<data::image_series>();
        series->set_window_width({s_WIDTH});
        CPPUNIT_ASSERT_EQUAL(s_WIDTH, series->window_width().front());
        CPPUNIT_ASSERT_EQUAL(s_WIDTH, std::stod(series->get_byte_value(data::dicom::attribute::Keyword::WindowWidth)));
    }

    {
        auto series = std::make_shared<data::image_series>();
        series->set_byte_value(data::dicom::attribute::Keyword::WindowWidth, std::to_string(s_WIDTH));
        CPPUNIT_ASSERT_EQUAL(s_WIDTH, series->window_width().front());
        CPPUNIT_ASSERT_EQUAL(s_WIDTH, std::stod(series->get_byte_value(data::dicom::attribute::Keyword::WindowWidth)));
    }
}

//------------------------------------------------------------------------------

void series_test::image_position_patient_test()
{
    static const std::vector<double> s_POSITION {111.111, 222.222, 333.333};

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to store the image position patient
        series->set_sop_keyword(data::dicom::sop::Keyword::CTImageStorage);

        series->set_image_position_patient(s_POSITION, std::nullopt);
        CPPUNIT_ASSERT(s_POSITION == series->get_image_position_patient(std::nullopt));

        const auto values = series->get_byte_values(data::dicom::attribute::Keyword::ImagePositionPatient);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2])
        };

        CPPUNIT_ASSERT(s_POSITION == actual);
    }

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to read the image position patient
        series->set_sop_keyword(data::dicom::sop::Keyword::CTImageStorage);

        series->set_byte_values(
            data::dicom::attribute::Keyword::ImagePositionPatient,
            {
                std::to_string(s_POSITION[0]),
                std::to_string(s_POSITION[1]),
                std::to_string(s_POSITION[2])
            });

        CPPUNIT_ASSERT(s_POSITION == series->get_image_position_patient(std::nullopt));

        const auto values = series->get_byte_values(data::dicom::attribute::Keyword::ImagePositionPatient);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2])
        };

        CPPUNIT_ASSERT(s_POSITION == actual);
    }

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to read the image position patient
        series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // test Multi-Frame image
        series->set_image_position_patient(s_POSITION, 0);
        CPPUNIT_ASSERT(s_POSITION == series->get_image_position_patient(0));

        // Also test shared group
        series->set_image_position_patient(s_POSITION, std::nullopt);
        CPPUNIT_ASSERT(s_POSITION == series->get_image_position_patient(std::nullopt));
    }
}

//------------------------------------------------------------------------------

void series_test::image_orientation_patient_test()
{
    static const std::vector<double> s_ORIENTATION {111.111, 222.222, 333.333, 444.444, 555.555, 666.666};

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to store the image orientation patient
        series->set_sop_keyword(data::dicom::sop::Keyword::CTImageStorage);

        series->set_image_orientation_patient(s_ORIENTATION, std::nullopt);
        CPPUNIT_ASSERT(s_ORIENTATION == series->get_image_orientation_patient(std::nullopt));

        const auto values = series->get_byte_values(data::dicom::attribute::Keyword::ImageOrientationPatient);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2]),
            std::stod(values[3]),
            std::stod(values[4]),
            std::stod(values[5])
        };

        CPPUNIT_ASSERT(s_ORIENTATION == actual);
    }

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to read the image orientation patient
        series->set_sop_keyword(data::dicom::sop::Keyword::CTImageStorage);

        series->set_byte_values(
            data::dicom::attribute::Keyword::ImageOrientationPatient,
            {
                std::to_string(s_ORIENTATION[0]),
                std::to_string(s_ORIENTATION[1]),
                std::to_string(s_ORIENTATION[2]),
                std::to_string(s_ORIENTATION[3]),
                std::to_string(s_ORIENTATION[4]),
                std::to_string(s_ORIENTATION[5])
            });

        CPPUNIT_ASSERT(s_ORIENTATION == series->get_image_orientation_patient(std::nullopt));

        const auto values = series->get_byte_values(data::dicom::attribute::Keyword::ImageOrientationPatient);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2]),
            std::stod(values[3]),
            std::stod(values[4]),
            std::stod(values[5])
        };

        CPPUNIT_ASSERT(s_ORIENTATION == actual);
    }

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to read the image orientation patient
        series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // test Multi-Frame image
        series->set_image_orientation_patient(s_ORIENTATION, 0);
        CPPUNIT_ASSERT(s_ORIENTATION == series->get_image_orientation_patient(0));

        // Also test shared group
        series->set_image_orientation_patient(s_ORIENTATION, std::nullopt);
        CPPUNIT_ASSERT(s_ORIENTATION == series->get_image_orientation_patient(std::nullopt));
    }
}

//------------------------------------------------------------------------------

void series_test::image_transform_patient_test()
{
    const data::matrix4 matrix({
            0., 1., 0., 0.,
            1., 0., 0., 0.,
            0., 0., -1., 0.,
            0., 0., 0., 1.
        });

    {
        auto series = std::make_shared<data::image_series>();

        // We need to set the SOP class UID to let Sight know how to read the image orientation/position patient
        series->set_sop_keyword(data::dicom::sop::Keyword::CTImageStorage);

        series->set_image_transform_patient(matrix, std::nullopt);
        const auto& value = series->get_image_transform_patient(std::nullopt);
        CPPUNIT_ASSERT(
            value
            && matrix == *value
        );
    }
}

//------------------------------------------------------------------------------

void series_test::frame_acquisition_date_time_test()
{
    // DICOM DT format is  "YYYYMMDDHHMMSS.FFFFFF"
    static const std::string s_EXPECTED_FRAME_ACQUISITION_DATE_TIME0("20221026150703.000000");
    static const std::string s_EXPECTED_FRAME_ACQUISITION_DATE_TIME1("20221026150703.000001");
    static const std::string s_EXPECTED_FRAME_ACQUISITION_DATE_TIME2("20221026150703.000002");

    {
        auto series = std::make_shared<data::image_series>();

        series->set_frame_acquisition_date_time(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME0, 0);
        series->set_frame_acquisition_date_time(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME1, 1);
        series->set_frame_acquisition_date_time(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME2, 2);

        const std::string actual_frame_acquisition_date_time0(*(series->get_frame_acquisition_date_time(0)));
        const std::string actual_frame_acquisition_date_time1(*(series->get_frame_acquisition_date_time(1)));
        const std::string actual_frame_acquisition_date_time2(*(series->get_frame_acquisition_date_time(2)));

        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME0, actual_frame_acquisition_date_time0);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME1, actual_frame_acquisition_date_time1);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_ACQUISITION_DATE_TIME2, actual_frame_acquisition_date_time2);
    }
}

//------------------------------------------------------------------------------

void series_test::frame_comments_test()
{
    static const std::string s_EXPECTED_FRAME_COMMENTS0("Comments0");
    static const std::string s_EXPECTED_FRAME_COMMENTS1("Comments1");
    static const std::string s_EXPECTED_FRAME_COMMENTS2("Comments2");

    {
        auto series = std::make_shared<data::image_series>();

        series->set_frame_comments(s_EXPECTED_FRAME_COMMENTS0, 0);
        series->set_frame_comments(s_EXPECTED_FRAME_COMMENTS1, 1);
        series->set_frame_comments(s_EXPECTED_FRAME_COMMENTS2, 2);

        const std::string actual_frame_comments0(*(series->get_frame_comments(0)));
        const std::string actual_frame_comments1(*(series->get_frame_comments(1)));
        const std::string actual_frame_comments2(*(series->get_frame_comments(2)));

        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_COMMENTS0, actual_frame_comments0);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_COMMENTS1, actual_frame_comments1);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_COMMENTS2, actual_frame_comments2);
    }
}

//------------------------------------------------------------------------------

void series_test::frame_label_test()
{
    static const std::string s_EXPECTED_FRAME_LABEL0("Label0");
    static const std::string s_EXPECTED_FRAME_LABEL1("Label1");
    static const std::string s_EXPECTED_FRAME_LABEL2("Label2");

    {
        auto series = std::make_shared<data::image_series>();

        series->set_frame_label(s_EXPECTED_FRAME_LABEL0, 0);
        series->set_frame_label(s_EXPECTED_FRAME_LABEL1, 1);
        series->set_frame_label(s_EXPECTED_FRAME_LABEL2, 2);

        const std::string actual_frame_label0(*(series->get_frame_label(0)));
        const std::string actual_frame_label1(*(series->get_frame_label(1)));
        const std::string actual_frame_label2(*(series->get_frame_label(2)));

        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_LABEL0, actual_frame_label0);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_LABEL1, actual_frame_label1);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_FRAME_LABEL2, actual_frame_label2);
    }
}

//------------------------------------------------------------------------------

void series_test::frame_acquisition_time_point_test()
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

    auto series = std::make_shared<data::image_series>();

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

    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_0, data::series::time_point_to_date_time(time_point0));
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_1, data::series::time_point_to_date_time(time_point1));
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_2, data::series::time_point_to_date_time(time_point2));
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_4, data::series::time_point_to_date_time(time_point4));

    // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_3B, data::series::time_point_to_date_time(time_point3));
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_5B, data::series::time_point_to_date_time(time_point5));

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

    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_0, actual_0);
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_1, actual_1);
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_2, actual_2);
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_4, actual_4);

    // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_3B, actual_3);
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_5B, actual_5);

    // Test DeepCopy
    {
        auto destination      = std::make_shared<data::image_series>();
        auto destination_lock = destination->dump_lock();

        {
            auto source      = std::make_shared<data::image_series>();
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

        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_0, destination_0);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_1, destination_1);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_2, destination_2);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_4, destination_4);

        // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_3B, actual_3);
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_5B, actual_5);
    }

    // test shrinking
    {
        auto destination      = std::make_shared<data::image_series>();
        auto destination_lock = destination->dump_lock();

        {
            auto source      = std::make_shared<data::image_series>();
            auto source_lock = source->dump_lock();

            source->set_frame_acquisition_date_time(s_EXPECTED_0, 0);
            source->set_frame_acquisition_date_time(s_EXPECTED_1, 1);
            source->set_frame_acquisition_date_time(s_EXPECTED_2, 2);

            destination->deep_copy(source);
        }

        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_0, *destination->get_frame_acquisition_date_time(0));
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_1, *destination->get_frame_acquisition_date_time(1));
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_2, *destination->get_frame_acquisition_date_time(2));

        // Now resize the destination to 2 elements
        destination->shrink_frames(2);

        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_0, *destination->get_frame_acquisition_date_time(0));
        CPPUNIT_ASSERT_EQUAL(s_EXPECTED_1, *destination->get_frame_acquisition_date_time(1));
        CPPUNIT_ASSERT(!destination->get_frame_acquisition_date_time(2));
    }
}

//------------------------------------------------------------------------------

void series_test::string_conversion_test()
{
    // Test binary to string and string to binary conversion for binary attributes

    // Unsigned Short (US)
    {
        static const uint16_t s_ROWS = 666;
        auto series                  = std::make_shared<data::image_series>();

        // Rows - "666" string (4) is bigger than short size (2)
        series->set_string_value(0x0018, 0x1049, std::to_string(s_ROWS));
        CPPUNIT_ASSERT_EQUAL(std::to_string(s_ROWS), series->get_string_value(0x0018, 0x1049));
    }
}

//------------------------------------------------------------------------------

void series_test::private_tag_test()
{
    const std::string expected1 {uuid::generate()};
    const std::string expected2 {uuid::generate()};

    {
        auto series = std::make_shared<data::image_series>();
        series->set_private_value(expected1, 0x11);
        series->set_private_value(expected2, 0x12);

        const auto& actual1 = series->get_private_value(0x11);
        CPPUNIT_ASSERT(actual1);
        CPPUNIT_ASSERT_EQUAL(expected1, *actual1);

        const auto& actual2 = series->get_private_value(0x12);
        CPPUNIT_ASSERT(actual2);
        CPPUNIT_ASSERT_EQUAL(expected2, *actual2);

        // test removing the tag
        series->set_private_value(std::nullopt, 0x11);
        const auto& actual5 = series->get_private_value(0x11);
        CPPUNIT_ASSERT(!actual5.has_value());
    }
}

//------------------------------------------------------------------------------

void series_test::multi_frame_private_tag_test()
{
    const std::string expected1 {uuid::generate()};
    const std::string expected2 {uuid::generate()};
    const std::string expected3 {uuid::generate()};

    {
        auto series = std::make_shared<data::image_series>();

        // test first if nonexisting value is handled
        const auto& no_value = series->get_multi_frame_private_value(0x11, 0);
        CPPUNIT_ASSERT(!no_value.has_value());

        series->set_multi_frame_private_value(expected1, 0x11, 0);
        const auto& actual1 = series->get_multi_frame_private_value(0x11, 0);
        CPPUNIT_ASSERT(actual1);
        CPPUNIT_ASSERT_EQUAL(expected1, *actual1);

        series->set_multi_frame_private_value(expected2, 0x11, 1);
        const auto& actual2 = series->get_multi_frame_private_value(0x11, 1);
        CPPUNIT_ASSERT(actual2);
        CPPUNIT_ASSERT_EQUAL(expected2, *actual2);

        series->set_multi_frame_private_value(expected3, 0x11, 2);
        const auto& actual3 = series->get_multi_frame_private_value(0x11, 2);
        CPPUNIT_ASSERT(actual3);
        CPPUNIT_ASSERT_EQUAL(expected3, *actual3);

        // test removing the tag
        series->set_private_value(std::nullopt, 0x11, 0);
        const auto& actual4 = series->get_private_value(0x11, 0);
        CPPUNIT_ASSERT(!actual4.has_value());

        series->set_private_value(std::nullopt, 0x11, 1);
        const auto& actual5 = series->get_private_value(0x11, 1);
        CPPUNIT_ASSERT(!actual5.has_value());

        series->set_private_value(std::nullopt, 0x11, 2);
        const auto& actual6 = series->get_private_value(0x11, 2);
        CPPUNIT_ASSERT(!actual6.has_value());
    }
}

//------------------------------------------------------------------------------

void series_test::copy_patient_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_patient_name("1");
    series1->set_patient_id("2");
    series1->set_patient_birth_date("5");
    series1->set_patient_sex("6");

    auto series2 = std::make_shared<data::series>();
    series2->copy_patient_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::copy_general_study_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_study_instance_uid("1");
    series1->set_study_date("2");
    series1->set_study_time("3");
    series1->set_referring_physician_name("4");
    series1->set_study_id("6");
    series1->set_study_description("9");

    auto series2 = std::make_shared<data::series>();
    series2->copy_general_study_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::copy_patient_study_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_patient_age("3");
    series1->set_patient_size(4);
    series1->set_patient_weight(5);

    auto series2 = std::make_shared<data::series>();
    series2->copy_patient_study_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::copy_general_series_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_modality(data::dicom::modality_t::ct);
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

    auto series2 = std::make_shared<data::series>();
    series2->copy_general_series_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::copy_general_equipment_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_institution_name("2");

    auto series2 = std::make_shared<data::series>();
    series2->copy_general_equipment_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::copy_sop_common_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_sop_keyword(data::dicom::sop::Keyword::ComputedRadiographyImageStorage);
    series1->set_sop_instance_uid("2");
    series1->set_specific_character_set("3");
    series1->set_instance_number(13);

    auto series2 = std::make_shared<data::series>();
    series2->copy_sop_common_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::copy_general_image_module_test()
{
    auto series1 = std::make_shared<data::series>();
    series1->set_instance_number(1);
    series1->set_content_time("4");
    series1->set_acquisition_number(6);
    series1->set_acquisition_date("7");
    series1->set_acquisition_time("8");

    auto series2 = std::make_shared<data::series>();
    series2->copy_general_image_module(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void series_test::get_patient_position_string_test()
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
    auto series = std::make_shared<data::series>();
    for(auto&& [key, value] : s_SHORT_TO_LONG)
    {
        series->set_patient_position(key);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(key, value, series->get_patient_position_string());
    }
}

//------------------------------------------------------------------------------

void series_test::new_instances_test()
{
    auto series = std::make_shared<data::series>();

    CPPUNIT_ASSERT(series->get_sop_instance_uid().empty());
    CPPUNIT_ASSERT(series->get_study_instance_uid().empty());
    CPPUNIT_ASSERT(series->get_series_instance_uid().empty());

    CPPUNIT_ASSERT(series->get_instance_creation_date().empty());
    CPPUNIT_ASSERT(series->get_instance_creation_time().empty());
    CPPUNIT_ASSERT(series->get_study_date().empty());
    CPPUNIT_ASSERT(series->get_study_time().empty());
    CPPUNIT_ASSERT(series->get_series_date().empty());
    CPPUNIT_ASSERT(series->get_series_time().empty());

    series->new_sop_instance();
    series->new_study_instance();
    series->new_series_instance();

    CPPUNIT_ASSERT(!series->get_instance_creation_date().empty());
    CPPUNIT_ASSERT(!series->get_instance_creation_time().empty());
    CPPUNIT_ASSERT(!series->get_study_date().empty());
    CPPUNIT_ASSERT(!series->get_study_time().empty());
    CPPUNIT_ASSERT(!series->get_series_date().empty());
    CPPUNIT_ASSERT(!series->get_series_time().empty());
}

//------------------------------------------------------------------------------

void series_test::iso_date_time_test()
{
    static const std::string s_DATE("20221026");
    static const std::string s_TIME("150703.123456");
    static const std::string s_EXPECTED_DATE("2022-10-26");
    static const std::string s_EXPECTED_TIME("15:07:03.123");

    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_DATE, data::series::date_to_iso(s_DATE));
    CPPUNIT_ASSERT_EQUAL(s_EXPECTED_TIME, data::series::time_to_iso(s_TIME));
}

//------------------------------------------------------------------------------

void series_test::path_test()
{
    static const std::string s_DATE       = "20221026";
    static const std::string s_TIME       = "150703.123456";
    static const std::string s_UID        = "789";
    static const std::string s_PATIENT_ID = "666";
    static const auto s_MODALITY          = data::dicom::modality_t::ct;

    static const std::string s_MODALITY_STRING(*data::dicom::to_string(s_MODALITY));

    static const std::string s_HASH =
        []
        {
            std::ostringstream hex;
            hex << std::hex << std::hash<std::string> {}(s_UID);

            return hex.str();
        }();

    auto series = std::make_shared<data::series>();

    series->set_study_date(s_DATE);
    series->set_study_time(s_TIME);
    series->set_series_instance_uid(s_UID);
    series->set_series_date(s_DATE);
    series->set_series_time(s_TIME);
    series->set_patient_id(s_PATIENT_ID);
    series->set_modality(s_MODALITY);

    std::filesystem::path path;

    CPPUNIT_ASSERT_NO_THROW((path = series->file_path(std::filesystem::path("/tmp"), ".ima")));

    const auto expected_path = std::filesystem::path("/tmp") / s_PATIENT_ID / (s_DATE + s_TIME)
                               / (s_PATIENT_ID + "." + s_MODALITY_STRING + "." + s_DATE + s_TIME + "." + s_HASH
                                  + std::string(".ima"));

    CPPUNIT_ASSERT_EQUAL(
        std::filesystem::weakly_canonical(expected_path).string(),
        std::filesystem::weakly_canonical(path).string()
    );
}

} //namespace sight::data::ut
