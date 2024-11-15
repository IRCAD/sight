/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "image_series_test.hpp"

#include <core/type.hpp>

#include <utest_data/generator/image.hpp>

#include <gdcmDataSet.h>
#include <gdcmExplicitDataElement.h>
#include <gdcmTagKeywords.h>
#include <gdcmTagToVR.h>
#include <gdcmUIDs.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::image_series_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void image_series_test::setUp()
{
    // Set up context before running a test.
    m_series = std::make_shared<data::image_series>();
}

//------------------------------------------------------------------------------

void image_series_test::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void image_series_test::image_test()
{
    CPPUNIT_ASSERT(m_series);

    utest_data::generator::image::generate_random_image(m_series, core::type::FLOAT);

    auto series2 = std::make_shared<data::image_series>();
    CPPUNIT_ASSERT(*series2 != *m_series);

    series2->shallow_copy(m_series);
    CPPUNIT_ASSERT(*series2 == *m_series);
}

//------------------------------------------------------------------------------

void image_series_test::equality_test()
{
    auto series1 = std::make_shared<data::image_series>();
    auto series2 = std::make_shared<data::image_series>();

    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_origin({1.0, 2.0, 3.0});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_origin(series1->origin());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_orientation({0.36, -0.8, 0.48, 0.48, 0.6, 0.64, -0.8, 0.0, 0.6});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_orientation(series1->orientation());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Also test origin and orientation with a different SOP.
    // A different SOP means that the data may be stored in a different location in DICOM data
    series1->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    series2->set_sop_keyword(series1->get_sop_keyword());
    series1->set_origin({4.0, 5.0, 6.0});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_origin(series1->origin());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));
    series1->set_orientation({0, 1, 0, -1, 0, 0, 0, 0, 1});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_orientation(series1->orientation());
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

    series1->set_patient_id("22");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_patient_id(series1->get_patient_id());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_study_instance_uid("23");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_study_instance_uid(series1->get_study_instance_uid());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_institution_name("24");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_institution_name(series1->get_institution_name());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    utest_data::generator::image::generate_random_image(series1, core::type::FLOAT);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->shallow_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    auto dicom_series = std::make_shared<data::dicom_series>();
    dicom_series->set_series_instance_uid("25");
    dicom_series->set_number_of_instances(1);
    series1->set_dicom_reference(dicom_series);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_dicom_reference(dicom_series);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_agent("26");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_agent(series1->get_contrast_bolus_agent());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_route("27");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_route(series1->get_contrast_bolus_route());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_volume(28.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_volume(series1->get_contrast_bolus_volume());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_start_time("29");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_start_time(series1->get_contrast_bolus_start_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_stop_time("30");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_stop_time(series1->get_contrast_bolus_stop_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_total_dose(31.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_total_dose(series1->get_contrast_bolus_total_dose());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_flow_rate("32.0\\32.1\\32.2");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_flow_rate(series1->get_contrast_flow_rate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_flow_duration("33.0\\33.1\\33.2");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_flow_duration(series1->get_contrast_flow_duration());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_ingredient("34");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_ingredient(series1->get_contrast_bolus_ingredient());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_contrast_bolus_ingredient_concentration(35.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_contrast_bolus_ingredient_concentration(series1->get_contrast_bolus_ingredient_concentration());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_acquisition_date("36");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_acquisition_date(series1->get_acquisition_date());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_acquisition_time("37");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_acquisition_time(series1->get_acquisition_time());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_window_center({38, 39});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_window_center(series1->window_center());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_window_width({40, 41});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_window_width(series1->window_width());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_rows(std::uint16_t(42));
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_rows(series1->get_rows());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->set_columns(std::uint16_t(43));
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->set_columns(series1->get_columns());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Test also deepcopy, just for fun
    auto series3 = std::make_shared<data::image_series>();
    series3->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series3 && !(*series1 != *series3));
}

//------------------------------------------------------------------------------

void image_series_test::resize_test()
{
    auto series1 = std::make_shared<data::image_series>();
    series1->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    do
    {
        utest_data::generator::image::generate_random_image(series1, core::type::INT8);
    }
    while(series1->size()[2] <= 2);

    auto size = series1->size();

    for(std::size_t i = 0, end = size[2] ; i < end ; ++i)
    {
        series1->set_image_position_patient({0.1 * double(i), 0.2 * double(i), 0.3 * double(i)}, i);
        series1->set_frame_comments("Comments" + std::to_string(i), i);
        series1->set_frame_label("Label" + std::to_string(i), i);
    }

    auto series2 = std::make_shared<data::image_series>();
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));

    series2->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Resize to 2 slices
    size[2] = 2;
    series1->resize(size, series1->type(), series1->pixel_format());

    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));

    CPPUNIT_ASSERT(series2->get_image_position_patient(0) == series1->get_image_position_patient(0));
    CPPUNIT_ASSERT(series2->get_image_position_patient(1) == series1->get_image_position_patient(1));
    CPPUNIT_ASSERT(
        series2->get_image_position_patient(2) != series1->get_image_position_patient(2)
        && series1->get_image_position_patient(2).empty()
    );

    CPPUNIT_ASSERT_EQUAL(*series2->get_frame_comments(0), *series1->get_frame_comments(0));
    CPPUNIT_ASSERT_EQUAL(*series2->get_frame_comments(1), *series1->get_frame_comments(1));
    CPPUNIT_ASSERT(series2->get_frame_comments(2) && !series1->get_frame_comments(2));

    CPPUNIT_ASSERT_EQUAL(*series2->get_frame_label(0), *series1->get_frame_label(0));
    CPPUNIT_ASSERT_EQUAL(*series2->get_frame_label(1), *series1->get_frame_label(1));
    CPPUNIT_ASSERT(series2->get_frame_label(2) && !series1->get_frame_label(2));

    series2->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));
}

//------------------------------------------------------------------------------

void image_series_test::origin_orientation_test()
{
    // In row major order
    static constexpr data::image::origin_t s_ORIGIN {1.0, 2.0, 3.0};
    static constexpr data::image::orientation_t s_ORIENTATION {
        0.36, -0.8, 0.48,
        0.48, 0.6, 0.64,
        -0.8, 0.0, 0.6
    };

    // Test no defined SOP
    {
        auto image_series = std::make_shared<data::image_series>();

        // Set values using standard setters and check standard getters
        image_series->set_origin(s_ORIGIN);
        CPPUNIT_ASSERT(core::is_equal(image_series->origin(), s_ORIGIN));

        image_series->set_orientation(s_ORIENTATION);
        CPPUNIT_ASSERT(core::is_equal(image_series->orientation(), s_ORIENTATION));

        // Check also DICOM values
        const auto& dataset = image_series->get_data_set();

        gdcm::Keywords::ImagePositionPatient position_attribute;
        position_attribute.SetFromDataSet(dataset);
        const auto position = std::vector(
            position_attribute.GetValues(),
            position_attribute.GetValues() + position_attribute.GetNumberOfValues()
        );

        CPPUNIT_ASSERT(core::is_equal(s_ORIGIN, position));

        gdcm::Keywords::ImageOrientationPatient orientation_attribute;
        orientation_attribute.SetFromDataSet(dataset);
        const auto orientation = std::vector(
            orientation_attribute.GetValues(),
            orientation_attribute.GetValues() + orientation_attribute.GetNumberOfValues()
        );

        // DICOM only uses 6 cosines in column-major order
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[0], orientation[0], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[1], orientation[3], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[3], orientation[1], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[4], orientation[4], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[6], orientation[2], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[7], orientation[5], 1e-6);
    }

    // Test EnhancedUSVolumeStorage with no acquisition geometry
    {
        auto image_series = std::make_shared<data::image_series>();
        image_series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // Set values using standard setters and check standard getters
        image_series->set_origin(s_ORIGIN);
        CPPUNIT_ASSERT(core::is_equal(image_series->origin(), s_ORIGIN));

        image_series->set_orientation(s_ORIENTATION);
        CPPUNIT_ASSERT(core::is_equal(image_series->orientation(), s_ORIENTATION));

        // Check also DICOM values
        const auto& dataset = image_series->get_data_set();

        gdcm::Keywords::ImagePositionPatient position_attribute;
        position_attribute.SetFromDataSet(dataset);
        const auto position = std::vector(
            position_attribute.GetValues(),
            position_attribute.GetValues() + position_attribute.GetNumberOfValues()
        );

        CPPUNIT_ASSERT(core::is_equal(s_ORIGIN, position));

        gdcm::Keywords::ImageOrientationPatient orientation_attribute;
        orientation_attribute.SetFromDataSet(dataset);
        const auto orientation = std::vector(
            orientation_attribute.GetValues(),
            orientation_attribute.GetValues() + orientation_attribute.GetNumberOfValues()
        );

        // DICOM only uses 6 cosines in column-major order
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[0], orientation[0], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[1], orientation[3], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[3], orientation[1], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[4], orientation[4], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[6], orientation[2], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(s_ORIENTATION[7], orientation[5], 1e-6);
    }

    // Test EnhancedUSVolumeStorage in APEX acquisition geometry
    {
        auto image_series = std::make_shared<data::image_series>();
        image_series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        image_series->set_ultrasound_acquisition_geometry(data::dicom::ultrasound_acquisition_geometry_t::apex);

        // Set values using standard setters and check standard getters
        image_series->set_origin(s_ORIGIN);
        CPPUNIT_ASSERT(core::is_equal(image_series->origin(), s_ORIGIN));

        image_series->set_orientation(s_ORIENTATION);
        CPPUNIT_ASSERT(core::is_equal(image_series->orientation(), s_ORIENTATION));

        // Check also DICOM values
        const auto& dataset = image_series->get_data_set();

        gdcm::Keywords::VolumeToTransducerMappingMatrix attribute;
        attribute.SetFromDataSet(dataset);

        const auto values = std::vector(
            attribute.GetValues(),
            attribute.GetValues() + attribute.GetNumberOfValues()
        );

        matrix4 mapping(values);
        CPPUNIT_ASSERT(core::is_equal(s_ORIGIN, mapping.position()));
        CPPUNIT_ASSERT(core::is_equal(s_ORIENTATION, mapping.orientation()));
    }

    // Test EnhancedUSVolumeStorage in patient acquisition geometry
    {
        auto image_series = std::make_shared<data::image_series>();
        image_series->set_sop_keyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        image_series->set_ultrasound_acquisition_geometry(data::dicom::ultrasound_acquisition_geometry_t::patient);

        // Set values using standard setters and check standard getters
        image_series->set_origin(s_ORIGIN);
        CPPUNIT_ASSERT(core::is_equal(image_series->origin(), s_ORIGIN));

        image_series->set_orientation(s_ORIENTATION);
        CPPUNIT_ASSERT(core::is_equal(image_series->orientation(), s_ORIENTATION));

        // Check also DICOM values
        const auto& dataset = image_series->get_data_set();

        gdcm::Keywords::VolumeToTableMappingMatrix attribute;
        attribute.SetFromDataSet(dataset);

        const auto values = std::vector(
            attribute.GetValues(),
            attribute.GetValues() + attribute.GetNumberOfValues()
        );

        matrix4 mapping(values);
        CPPUNIT_ASSERT(core::is_equal(s_ORIGIN, mapping.position()));
        CPPUNIT_ASSERT(core::is_equal(s_ORIENTATION, mapping.orientation()));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
