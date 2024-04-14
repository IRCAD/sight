/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#pragma once

#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class series_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(series_test);

CPPUNIT_TEST(private_tag_test);
CPPUNIT_TEST(multi_frame_private_tag_test);
CPPUNIT_TEST(attr_patient_test);
CPPUNIT_TEST(attr_study_test);
CPPUNIT_TEST(attr_equipment_test);
CPPUNIT_TEST(attr_instance_uid_test);
CPPUNIT_TEST(attr_modality_test);
CPPUNIT_TEST(attr_date_test);
CPPUNIT_TEST(attr_time_test);
CPPUNIT_TEST(attr_performing_physician_name_test);
CPPUNIT_TEST(attr_description_test);

CPPUNIT_TEST(equality_test);

CPPUNIT_TEST(sop_class_uid_test);
CPPUNIT_TEST(sop_instance_uid_test);
CPPUNIT_TEST(specific_character_set_test);
CPPUNIT_TEST(encoding_test);
CPPUNIT_TEST(series_date_test);
CPPUNIT_TEST(series_time_test);
CPPUNIT_TEST(modality_test);
CPPUNIT_TEST(series_description_test);
CPPUNIT_TEST(performing_physician_name_test);
CPPUNIT_TEST(anatomical_orientation_type_test);
CPPUNIT_TEST(body_part_examined_test);
CPPUNIT_TEST(protocol_name_test);
CPPUNIT_TEST(patient_position_test);
CPPUNIT_TEST(series_instance_uid_test);
CPPUNIT_TEST(series_number_test);
CPPUNIT_TEST(laterality_test);
CPPUNIT_TEST(performed_procedure_step_start_date_test);
CPPUNIT_TEST(performed_procedure_step_start_time_test);
CPPUNIT_TEST(performed_procedure_step_end_date_test);
CPPUNIT_TEST(performed_procedure_step_end_time_test);
CPPUNIT_TEST(performed_procedure_step_id_test);
CPPUNIT_TEST(performed_procedure_step_description_test);
CPPUNIT_TEST(comments_on_the_performed_procedure_step_test);
CPPUNIT_TEST(institution_name_test);
CPPUNIT_TEST(patient_name_test);
CPPUNIT_TEST(patient_id_test);
CPPUNIT_TEST(patient_birth_date_test);
CPPUNIT_TEST(patient_sex_test);
CPPUNIT_TEST(study_date_test);
CPPUNIT_TEST(study_time_test);
CPPUNIT_TEST(referring_physician_name_test);
CPPUNIT_TEST(study_description_test);
CPPUNIT_TEST(study_instance_uid_test);
CPPUNIT_TEST(study_id_test);
CPPUNIT_TEST(patient_age_test);
CPPUNIT_TEST(patient_size_test);
CPPUNIT_TEST(patient_weight_test);
CPPUNIT_TEST(acquisition_date_test);
CPPUNIT_TEST(acquisition_time_test);
CPPUNIT_TEST(acquisition_number_test);
CPPUNIT_TEST(instance_number_test);
CPPUNIT_TEST(content_time_test);
CPPUNIT_TEST(contrast_bolus_agent_test);
CPPUNIT_TEST(contrast_bolus_route_test);
CPPUNIT_TEST(contrast_bolus_volume_test);
CPPUNIT_TEST(contrast_bolus_start_time_test);
CPPUNIT_TEST(contrast_bolus_stop_time_test);
CPPUNIT_TEST(contrast_bolus_total_dose_test);
CPPUNIT_TEST(contrast_flow_rate_test);
CPPUNIT_TEST(contrast_flow_duration_test);
CPPUNIT_TEST(contrast_bolus_ingredient_test);
CPPUNIT_TEST(contrast_bolus_ingredient_concentration_test);
CPPUNIT_TEST(rows_test);
CPPUNIT_TEST(columns_test);
CPPUNIT_TEST(window_center_test);
CPPUNIT_TEST(window_width_test);
CPPUNIT_TEST(image_position_patient_test);
CPPUNIT_TEST(image_orientation_patient_test);
CPPUNIT_TEST(image_transform_patient_test);
CPPUNIT_TEST(frame_acquisition_date_time_test);
CPPUNIT_TEST(frame_acquisition_time_point_test);
CPPUNIT_TEST(frame_comments_test);
CPPUNIT_TEST(frame_label_test);
CPPUNIT_TEST(string_conversion_test);

CPPUNIT_TEST(copy_patient_module_test);
CPPUNIT_TEST(copy_general_study_module_test);
CPPUNIT_TEST(copy_patient_study_module_test);
CPPUNIT_TEST(copy_general_series_module_test);
CPPUNIT_TEST(copy_general_equipment_module_test);
CPPUNIT_TEST(copy_sop_common_module_test);
CPPUNIT_TEST(copy_general_image_module_test);

CPPUNIT_TEST(get_patient_position_string_test);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void private_tag_test();
    static void multi_frame_private_tag_test();

    void attr_patient_test();
    void attr_study_test();
    void attr_equipment_test();
    void attr_instance_uid_test();
    void attr_modality_test();
    void attr_date_test();
    void attr_time_test();
    void attr_performing_physician_name_test();
    void attr_description_test();

    static void equality_test();

    static void sop_class_uid_test();
    static void sop_instance_uid_test();
    static void specific_character_set_test();
    static void encoding_test();
    static void series_date_test();
    static void series_time_test();
    static void modality_test();
    static void series_description_test();
    static void performing_physician_name_test();
    static void anatomical_orientation_type_test();
    static void body_part_examined_test();
    static void protocol_name_test();
    static void patient_position_test();
    static void series_instance_uid_test();
    static void series_number_test();
    static void laterality_test();
    static void performed_procedure_step_start_date_test();
    static void performed_procedure_step_start_time_test();
    static void performed_procedure_step_end_date_test();
    static void performed_procedure_step_end_time_test();
    static void performed_procedure_step_id_test();
    static void performed_procedure_step_description_test();
    static void comments_on_the_performed_procedure_step_test();
    static void institution_name_test();
    static void patient_name_test();
    static void patient_id_test();
    static void patient_birth_date_test();
    static void patient_sex_test();
    static void study_date_test();
    static void study_time_test();
    static void referring_physician_name_test();
    static void study_description_test();
    static void study_instance_uid_test();
    static void study_id_test();
    static void patient_age_test();
    static void patient_size_test();
    static void patient_weight_test();
    static void acquisition_date_test();
    static void acquisition_time_test();
    static void acquisition_number_test();
    static void instance_number_test();
    static void content_time_test();
    static void contrast_bolus_agent_test();
    static void contrast_bolus_route_test();
    static void contrast_bolus_volume_test();
    static void contrast_bolus_start_time_test();
    static void contrast_bolus_stop_time_test();
    static void contrast_bolus_total_dose_test();
    static void contrast_flow_rate_test();
    static void contrast_flow_duration_test();
    static void contrast_bolus_ingredient_test();
    static void contrast_bolus_ingredient_concentration_test();
    static void rows_test();
    static void columns_test();
    static void window_center_test();
    static void window_width_test();
    static void image_position_patient_test();
    static void image_orientation_patient_test();
    static void image_transform_patient_test();
    static void frame_acquisition_date_time_test();
    static void frame_acquisition_time_point_test();
    static void frame_comments_test();
    static void frame_label_test();

    static void string_conversion_test();

    static void copy_patient_module_test();
    static void copy_general_study_module_test();
    static void copy_patient_study_module_test();
    static void copy_general_series_module_test();
    static void copy_general_equipment_module_test();
    static void copy_sop_common_module_test();
    static void copy_general_image_module_test();

    static void get_patient_position_string_test();

protected:

    using series_vector_t = std::vector<std::shared_ptr<series> >;

    series_vector_t m_series;
};

} // namespace sight::data::ut
