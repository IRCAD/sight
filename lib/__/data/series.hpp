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

#pragma once

#include <sight/data/config.hpp>

#include "data/dicom/attribute.hpp"
#include "data/dicom/coded_string.hpp"
#include "data/dicom/sop.hpp"
#include "data/factory/new.hpp"
#include "data/landmarks.hpp"
#include "data/matrix4.hpp"
#include "data/object.hpp"
#include "data/types.hpp"

#include <chrono>
#include <functional>
#include <optional>

namespace gdcm
{

class DataSet;

}

namespace sight::data::detail
{

class series_impl;

}

namespace sight::data
{

/**
 * @brief Holds series information.
 */
class SIGHT_DATA_CLASS_API series : public virtual object
{
public:

    SIGHT_DECLARE_CLASS(series, object);

    /// Constructor / Destructor
    /// @{
    SIGHT_DATA_API series();
    SIGHT_DATA_API ~series() noexcept override;
    /// @}

    /// Generate a new study instance uid / data / time
    SIGHT_DATA_API void new_sop_instance();

    /// Generate a new study instance uid / data / time
    SIGHT_DATA_API void new_study_instance();

    /// Generate a new study instance uid / data / time
    SIGHT_DATA_API void new_series_instance();

    /// Getter/Setter of DICOM SOP Common Module related attributes
    /// @{
    SIGHT_DATA_API dicom::sop::Keyword get_sop_keyword() const noexcept;
    SIGHT_DATA_API void set_sop_keyword(dicom::sop::Keyword _keyword);
    SIGHT_DATA_API std::string_view get_sop_class_name() const noexcept;

    SIGHT_DATA_API std::string get_sop_instance_uid() const noexcept;
    SIGHT_DATA_API void set_sop_instance_uid(const std::string& _sop_instance_uid);

    SIGHT_DATA_API std::string get_sop_class_uid() const noexcept;
    SIGHT_DATA_API void set_sop_class_uid(const std::string& _sop_class_uid);

    SIGHT_DATA_API std::string get_specific_character_set() const noexcept;
    SIGHT_DATA_API void set_specific_character_set(const std::string& _specific_character_set);

    /// Translate DICOM specific character set to something compatible with iconv/ICU/Qt
    SIGHT_DATA_API std::string get_encoding() const noexcept;

    SIGHT_DATA_API std::string get_instance_creation_date() const noexcept;
    SIGHT_DATA_API void set_instance_creation_date(const std::string& _instance_creation_date);

    SIGHT_DATA_API std::string get_instance_creation_time() const noexcept;
    SIGHT_DATA_API void set_instance_creation_time(const std::string& _instance_creation_time);
    /// @}

    /// Getter/Setter of DICOM General series Module related attributes
    /// @{
    SIGHT_DATA_API std::string get_series_date() const noexcept;
    SIGHT_DATA_API void set_series_date(const std::string& _series_date);

    SIGHT_DATA_API std::string get_series_time() const noexcept;
    SIGHT_DATA_API void set_series_time(const std::string& _series_time);

    SIGHT_DATA_API std::string get_modality_string() const noexcept;
    SIGHT_DATA_API dicom::modality_t get_modality() const noexcept;
    SIGHT_DATA_API void set_modality(dicom::modality_t _modality);
    SIGHT_DATA_API void set_modality(const std::string& _modality);

    SIGHT_DATA_API std::string get_series_description() const noexcept;
    SIGHT_DATA_API void set_series_description(const std::string& _series_description);

    SIGHT_DATA_API std::vector<std::string> get_performing_physician_names() const noexcept;
    SIGHT_DATA_API void set_performing_physician_names(const std::vector<std::string>& _performing_physician_names);

    SIGHT_DATA_API std::string get_performing_physician_name() const noexcept;
    SIGHT_DATA_API void set_performing_physician_name(const std::string& _performing_physician_name);

    SIGHT_DATA_API std::string get_anatomical_orientation_type() const noexcept;
    SIGHT_DATA_API void set_anatomical_orientation_type(const std::string& _anatomical_orientation_type);

    SIGHT_DATA_API std::string get_body_part_examined() const noexcept;
    SIGHT_DATA_API void set_body_part_examined(const std::string& _body_part_examined);

    SIGHT_DATA_API std::string get_protocol_name() const noexcept;
    SIGHT_DATA_API void set_protocol_name(const std::string& _protocol_name);

    SIGHT_DATA_API std::string get_patient_position() const noexcept;
    SIGHT_DATA_API void set_patient_position(const std::string& _patient_position);

    /// Returns the patient position as human readable string
    SIGHT_DATA_API std::string get_patient_position_string() const noexcept;

    SIGHT_DATA_API std::string get_series_instance_uid() const noexcept;
    SIGHT_DATA_API void set_series_instance_uid(const std::string& _series_instance_uid);

    SIGHT_DATA_API std::optional<std::int32_t> get_series_number() const noexcept;
    SIGHT_DATA_API void set_series_number(const std::optional<std::int32_t>& _series_number = std::nullopt);

    SIGHT_DATA_API std::string get_laterality() const noexcept;
    SIGHT_DATA_API void set_laterality(const std::string& _laterality);

    SIGHT_DATA_API std::string get_performed_procedure_step_start_date() const noexcept;
    SIGHT_DATA_API void set_performed_procedure_step_start_date(
        const std::string& _performed_procedure_step_start_date
    );

    SIGHT_DATA_API std::string get_performed_procedure_step_start_time() const noexcept;
    SIGHT_DATA_API void set_performed_procedure_step_start_time(
        const std::string& _performed_procedure_step_start_time
    );

    SIGHT_DATA_API std::string get_performed_procedure_step_end_date() const noexcept;
    SIGHT_DATA_API void set_performed_procedure_step_end_date(const std::string& _performed_procedure_step_end_date);

    SIGHT_DATA_API std::string get_performed_procedure_step_end_time() const noexcept;
    SIGHT_DATA_API void set_performed_procedure_step_end_time(const std::string& _performed_procedure_step_end_time);

    SIGHT_DATA_API std::string get_performed_procedure_step_id() const noexcept;
    SIGHT_DATA_API void set_performed_procedure_step_id(const std::string& _performed_procedure_step_id);

    SIGHT_DATA_API std::string get_performed_procedure_step_description() const noexcept;
    SIGHT_DATA_API void set_performed_procedure_step_description(
        const std::string& _performed_procedure_step_description
    );

    SIGHT_DATA_API std::string get_comments_on_the_performed_procedure_step() const noexcept;
    SIGHT_DATA_API void set_comments_on_the_performed_procedure_step(
        const std::string& _comments_on_the_performed_procedure_step
    );
    /// @}

    /// Getter/Setter of DICOM General Equipment Module related attributes
    /// @{
    SIGHT_DATA_API std::string get_institution_name() const noexcept;
    SIGHT_DATA_API void set_institution_name(const std::string& _institution_name);

    SIGHT_DATA_API std::string get_equipment_manufacturer() const noexcept;
    SIGHT_DATA_API void set_equipment_manufacturer(const std::string& _manufacturer);

    SIGHT_DATA_API std::string get_equipment_manufacturer_model_name() const noexcept;
    SIGHT_DATA_API void set_equipment_manufacturer_model_name(const std::string& _model_name);

    SIGHT_DATA_API std::string get_equipment_device_serial_number() const noexcept;
    SIGHT_DATA_API void set_equipment_device_serial_number(const std::string& _serial_number);

    SIGHT_DATA_API std::vector<std::string> get_software_versions() const noexcept;
    SIGHT_DATA_API void set_software_versions(const std::vector<std::string>& _software_versions);

    SIGHT_DATA_API std::optional<int> get_depth_of_scan_field_mm() const noexcept;
    SIGHT_DATA_API void set_depth_of_scan_field_mm(const int _depth_of_scan_field);

    SIGHT_DATA_API std::vector<double> get_depths_of_focus_mm() const noexcept;
    SIGHT_DATA_API void set_depths_of_focus_mm(std::vector<double>& _depth_of_focus_mm);

    SIGHT_DATA_API std::optional<std::string> get_processing_function() const noexcept;
    SIGHT_DATA_API void set_processing_function(const std::string& _processing_function);

    SIGHT_DATA_API std::optional<dicom::position_measuring_device_used_t> get_position_measuring_device_used() const
    noexcept;
    SIGHT_DATA_API void set_position_measuring_device_used(
        const dicom::position_measuring_device_used_t _position_measuring_device_used
    );
    /// @}

    /// Getter/Setter of DICOM Patient Module related attributes
    /// @{
    SIGHT_DATA_API std::string get_patient_name() const noexcept;
    SIGHT_DATA_API virtual void set_patient_name(const std::string& _patient_name);

    SIGHT_DATA_API std::string get_patient_id() const noexcept;
    SIGHT_DATA_API virtual void set_patient_id(const std::string& _patient_id);

    SIGHT_DATA_API std::string get_patient_birth_date() const noexcept;
    SIGHT_DATA_API void set_patient_birth_date(const std::string& _patient_birth_date);

    SIGHT_DATA_API std::string get_patient_sex() const noexcept;
    SIGHT_DATA_API void set_patient_sex(const std::string& _patient_sex);
    /// @}

    /// Getter/Setter of DICOM General Study Module related attributes
    /// @{
    SIGHT_DATA_API std::string get_study_date() const noexcept;
    SIGHT_DATA_API void set_study_date(const std::string& _study_date);

    SIGHT_DATA_API std::string get_study_time() const noexcept;
    SIGHT_DATA_API void set_study_time(const std::string& _study_time);

    SIGHT_DATA_API std::string get_accession_number() const noexcept;
    SIGHT_DATA_API void set_accession_number(const std::string& _accession_number);

    SIGHT_DATA_API std::string get_referring_physician_name() const noexcept;
    SIGHT_DATA_API void set_referring_physician_name(const std::string& _referring_physician_name);

    SIGHT_DATA_API std::string get_study_description() const noexcept;
    SIGHT_DATA_API void set_study_description(const std::string& _study_description);

    SIGHT_DATA_API std::string get_study_instance_uid() const noexcept;
    SIGHT_DATA_API virtual void set_study_instance_uid(const std::string& _study_instance_uid);

    SIGHT_DATA_API std::string get_study_id() const noexcept;
    SIGHT_DATA_API void set_study_id(const std::string& _study_id);
    /// @}

    /// Getter/Setter of DICOM Patient Study Module related attributes
    /// @{
    SIGHT_DATA_API std::string get_patient_age() const noexcept;
    SIGHT_DATA_API void set_patient_age(const std::string& _patient_age);

    SIGHT_DATA_API std::optional<double> get_patient_size() const noexcept;
    SIGHT_DATA_API void set_patient_size(const std::optional<double>& _patient_size = std::nullopt);

    SIGHT_DATA_API std::optional<double> get_patient_weight() const noexcept;
    SIGHT_DATA_API void set_patient_weight(const std::optional<double>& _patient_weight = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM General Image Module related attributes
    /// @{

    struct image_type_t
    {
        dicom::pixel_data_characteristics_t pixel_data_characteristics {
            dicom::pixel_data_characteristics_t::unknown
        };

        dicom::patient_examination_characteristics_t patient_examination_characteristics {
            dicom::patient_examination_characteristics_t::unknown
        };

        std::vector<std::string> other_values {};
    };

    SIGHT_DATA_API image_type_t get_image_type() const;
    SIGHT_DATA_API void set_image_type(const image_type_t& _image_type);

    SIGHT_DATA_API std::string get_acquisition_date(std::size_t _instance                                = 0) const;
    SIGHT_DATA_API void set_acquisition_date(const std::string& _acquisition_date, std::size_t _instance = 0);

    SIGHT_DATA_API std::string get_acquisition_time(std::size_t _instance                                = 0) const;
    SIGHT_DATA_API void set_acquisition_time(const std::string& _acquisition_time, std::size_t _instance = 0);

    SIGHT_DATA_API std::string get_content_time(std::size_t _instance                            = 0) const;
    SIGHT_DATA_API void set_content_time(const std::string& _content_time, std::size_t _instance = 0);

    SIGHT_DATA_API std::optional<std::int32_t> get_instance_number(std::size_t _instance = 0) const;

    SIGHT_DATA_API void set_instance_number(
        const std::optional<std::int32_t>& _instance_number = std::nullopt,
        std::size_t _instance                               = 0
    );

    SIGHT_DATA_API std::optional<std::int32_t> get_acquisition_number(
        std::size_t _instance = 0
    ) const;

    SIGHT_DATA_API void set_acquisition_number(
        std::optional<std::int32_t> _acquisition_number = std::nullopt,
        std::size_t _instance                           = 0
    );

    SIGHT_DATA_API std::optional<double> get_acquisition_duration() const;
    SIGHT_DATA_API void set_acquisition_duration(double _duration);
    /// @}

    /// Getter/Setter of DICOM Contrast/Bolus Module related attributes
    /// @{
    SIGHT_DATA_API std::string get_contrast_bolus_agent() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_agent(const std::string& _contrast_bolus_agent);

    SIGHT_DATA_API std::string get_contrast_bolus_route() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_route(const std::string& _contrast_bolus_route);

    SIGHT_DATA_API std::optional<double> get_contrast_bolus_volume() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_volume(const std::optional<double>& _contrast_bolus_volume = std::nullopt);

    SIGHT_DATA_API std::string get_contrast_bolus_start_time() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_start_time(const std::string& _contrast_bolus_start_time);

    SIGHT_DATA_API std::string get_contrast_bolus_stop_time() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_stop_time(const std::string& _contrast_bolus_stop_time);

    SIGHT_DATA_API std::optional<double> get_contrast_bolus_total_dose() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_total_dose(
        const std::optional<double>& _contrast_bolus_total_dose = std::nullopt
    );

    SIGHT_DATA_API std::vector<double> get_contrast_flow_rates() const noexcept;
    SIGHT_DATA_API void set_contrast_flow_rates(const std::vector<double>& _contrast_flow_rates);

    SIGHT_DATA_API std::string get_contrast_flow_rate() const noexcept;
    SIGHT_DATA_API void set_contrast_flow_rate(const std::string& _contrast_flow_rates);

    SIGHT_DATA_API std::vector<double> get_contrast_flow_durations() const noexcept;
    SIGHT_DATA_API void set_contrast_flow_durations(const std::vector<double>& _contrast_flow_durations);

    SIGHT_DATA_API std::string get_contrast_flow_duration() const noexcept;
    SIGHT_DATA_API void set_contrast_flow_duration(const std::string& _contrast_flow_durations);

    SIGHT_DATA_API std::string get_contrast_bolus_ingredient() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_ingredient(const std::string& _contrast_bolus_ingredient);

    SIGHT_DATA_API std::optional<double> get_contrast_bolus_ingredient_concentration() const noexcept;
    SIGHT_DATA_API void set_contrast_bolus_ingredient_concentration(
        const std::optional<double>& _contrast_bolus_ingredient_concentration = std::nullopt
    );

    /// Getter/Setter of DICOM Image Pixel Module
    /// @{
    SIGHT_DATA_API std::optional<std::uint16_t> get_rows() const noexcept;
    SIGHT_DATA_API virtual void set_rows(const std::optional<std::uint16_t>& _rows = std::nullopt);

    SIGHT_DATA_API std::optional<std::uint16_t> get_columns() const noexcept;
    SIGHT_DATA_API virtual void set_columns(const std::optional<std::uint16_t>& _columns = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    SIGHT_DATA_API virtual std::vector<double> window_center() const noexcept;
    SIGHT_DATA_API virtual void set_window_center(const std::vector<double>& _window_centers);

    SIGHT_DATA_API virtual std::vector<double> window_width() const noexcept;
    SIGHT_DATA_API virtual void set_window_width(const std::vector<double>& _window_widths);
    /// @}

    /// Getter/Setter of DICOM Modality LUT Module
    /// @{
    SIGHT_DATA_API std::optional<double> get_rescale_intercept(std::size_t _instance = 0) const noexcept;
    SIGHT_DATA_API void set_rescale_intercept(
        const std::optional<double>& _rescale_intercept = std::nullopt,
        std::size_t _instance                           = 0
    );

    SIGHT_DATA_API std::optional<double> get_rescale_slope(std::size_t _instance = 0) const noexcept;
    SIGHT_DATA_API void set_rescale_slope(
        const std::optional<double>& _rescale_slope = std::nullopt,
        std::size_t _instance                       = 0
    );
    /// @}

    /// Getter/Setter of DICOM Image Plane Module related attributes
    /// ...and Multi-frame Functional Groups Module
    /// @{
    SIGHT_DATA_API std::vector<double> get_image_position_patient(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API virtual void set_image_position_patient(
        const std::vector<double>& _image_position_patient,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::vector<double> get_image_orientation_patient(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API virtual void set_image_orientation_patient(
        const std::vector<double>& _image_orientation_patient,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::optional<matrix4> get_image_transform_patient(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    // Dicom only stores x and y axis for the transformation, so we must make sure that the cross has the correct
    // orientation
    SIGHT_DATA_API void set_image_transform_patient(
        const std::optional<std::reference_wrapper<const matrix4> >& _transform,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::optional<double> get_slice_thickness() const noexcept;
    SIGHT_DATA_API virtual void set_slice_thickness(const std::optional<double>& _slice_thickness = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM Ultrasound Frame of Reference Module
    /// @{
    SIGHT_DATA_API dicom::ultrasound_acquisition_geometry_t get_ultrasound_acquisition_geometry() const noexcept;
    SIGHT_DATA_API void set_ultrasound_acquisition_geometry(
        dicom::ultrasound_acquisition_geometry_t _ultrasound_acquisition_geometry
    );

    SIGHT_DATA_API std::vector<double> get_apex_position() const noexcept;
    SIGHT_DATA_API void set_apex_position(const std::vector<double>& _apex_position);

    SIGHT_DATA_API std::optional<matrix4> get_volume_to_transducer_mapping_matrix() const noexcept;
    SIGHT_DATA_API void set_volume_to_transducer_mapping_matrix(
        const std::optional<std::reference_wrapper<const matrix4> >& _volume_to_transducer_mapping_matrix
    );

    SIGHT_DATA_API std::optional<matrix4> get_volume_to_table_mapping_matrix() const noexcept;
    SIGHT_DATA_API void set_volume_to_table_mapping_matrix(
        const std::optional<std::reference_wrapper<const matrix4> >& _volume_to_table_mapping_matrix
    );

    SIGHT_DATA_API dicom::patient_frame_of_reference_source_t get_patient_frame_of_reference_source() const noexcept;
    SIGHT_DATA_API void set_patient_frame_of_reference_source(
        dicom::patient_frame_of_reference_source_t _patient_frame_of_reference_source
    );
    /// @}

    /// Getter/Setter of DICOM Multi-frame Dimension Module
    /// @{
    SIGHT_DATA_API dicom::dimension_organization_t get_dimension_organization_type() const noexcept;
    SIGHT_DATA_API void set_dimension_organization_type(dicom::dimension_organization_t _dimension_organization_type);
    /// @}

    /// Getter/Setter of DICOM Multi-frame Functional Groups Module related attributes
    /// @{
    SIGHT_DATA_API std::vector<double> get_image_position_volume(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API void set_image_position_volume(
        const std::vector<double>& _image_position_volume,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::vector<double> get_image_orientation_volume(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API void set_image_orientation_volume(
        const std::vector<double>& _image_orientation_volume,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::optional<std::string> get_frame_acquisition_date_time(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API void set_frame_acquisition_date_time(
        const std::optional<std::string>& _frame_acquisition_date_time = std::nullopt,
        const std::optional<std::size_t>& _frame_index                 = std::nullopt
    );

    SIGHT_DATA_API std::optional<std::chrono::system_clock::time_point> get_frame_acquisition_time_point(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const noexcept;

    SIGHT_DATA_API void set_frame_acquisition_time_point(
        const std::optional<std::chrono::system_clock::time_point>& _time_point = std::nullopt,
        const std::optional<std::size_t>& _frame_index                          = std::nullopt
    );

    SIGHT_DATA_API void set_frame_acquisition_time_point(
        sight::core::clock::type _time_point,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::optional<std::string> get_frame_comments(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API void set_frame_comments(
        const std::optional<std::string>& _frame_comments = std::nullopt,
        const std::optional<std::size_t>& _frame_index    = std::nullopt
    );

    SIGHT_DATA_API std::optional<std::string> get_frame_label(
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    SIGHT_DATA_API void set_frame_label(
        const std::optional<std::string>& _frame_label = std::nullopt,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    SIGHT_DATA_API std::optional<double> get_spacing_between_slices(
        const std::optional<std::size_t>& _frame_index
    ) const;

    SIGHT_DATA_API void set_spacing_between_slices(
        std::optional<double> _spacing_between_slices,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    /// @}

    /// Getter/Setter of DICOM SourceImageSequence
    /// We assume to only have one element in the sequence, thus we can only reference one DICOM instance
    /// If we want more, then we need to handle a SourceImageSequence index as parameter of the getter/setter
    /// @{
    SIGHT_DATA_API std::optional<std::string> get_referenced_sop_class_uid() const;
    SIGHT_DATA_API void set_referenced_sop_class_uid(const std::optional<std::string>& _sop_class_uid = std::nullopt);

    SIGHT_DATA_API std::optional<std::string> get_referenced_sop_instance_uid() const;
    SIGHT_DATA_API void set_referenced_sop_instance_uid(
        const std::optional<std::string>& _sop_instance_uid = std::nullopt
    );
    /// @}

    /// Getter/Setter of DICOM Frame of Reference Module related attributes
    /// @{
    SIGHT_DATA_API std::optional<std::string> get_frame_of_reference_uid() const noexcept;
    SIGHT_DATA_API void set_frame_of_reference_uid(
        const std::optional<std::string>& _frame_of_reference_uid = std::nullopt
    );
    /// @}

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const series& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const series& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /// Defines partial copy. Useful to share data between different series
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy from
    /// @{
    SIGHT_DATA_API void copy_patient_module(const series::csptr& _source, std::size_t _instance                = 0);
    SIGHT_DATA_API void copy_clinical_trial_subject_module(const series::csptr& _source, std::size_t _instance = 0);
    SIGHT_DATA_API void copy_general_study_module(const series::csptr& _source, std::size_t _instance          = 0);
    SIGHT_DATA_API void copy_patient_study_module(const series::csptr& _source, std::size_t _instance          = 0);
    SIGHT_DATA_API void copy_clinical_trial_study_module(const series::csptr& _source, std::size_t _instance   = 0);
    SIGHT_DATA_API void copy_general_series_module(const series::csptr& _source, std::size_t _instance         = 0);
    SIGHT_DATA_API void copy_clinical_trial_series_module(const series::csptr& _source, std::size_t _instance  = 0);
    SIGHT_DATA_API void copy_general_equipment_module(const series::csptr& _source, std::size_t _instance      = 0);
    SIGHT_DATA_API void copy_frame_of_reference_module(const series::csptr& _source, std::size_t _instance     = 0);
    SIGHT_DATA_API void copy_sop_common_module(const series::csptr& _source, std::size_t _instance             = 0);
    SIGHT_DATA_API void copy_general_image_module(const series::csptr& _source, std::size_t _instance          = 0);
    /// @}

    /// Value getter using given tag. The returned string is used as a Byte buffer and is not necessarily a string.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] _group the group tag to get
    /// @param[in] _element the element tag to get
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    SIGHT_DATA_API std::string get_byte_value(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;
    SIGHT_DATA_API std::string get_byte_value(
        dicom::attribute::Keyword _tag,
        std::size_t _instance = 0
    ) const;
    /// @}

    /// Value setter using given tag. The string argument is used as a Byte buffer and is not necessarily a string.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] _group the group tag to set
    /// @param[in] _element the element tag to set
    /// @param[in] _value the value to insert
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @throw data::exception if tag doesn't exist
    /// @throw data::exception if value size is not correct
    /// @throw data::exception if instance index is out of bounds
    /// @{
    SIGHT_DATA_API void set_byte_value(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::string& _value,
        std::size_t _instance = 0
    );

    SIGHT_DATA_API void set_byte_value(
        dicom::attribute::Keyword _tag,
        const std::string& _value,
        std::size_t _instance = 0
    );

    /// @}

    /// Values getter using given tag. Initial string is split using DICOM delimiter '\'.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] _group the group tag to get
    /// @param[in] _element the element tag to get
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a vector of strings
    /// @{
    SIGHT_DATA_API std::vector<std::string> get_byte_values(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;

    SIGHT_DATA_API std::vector<std::string> get_byte_values(
        dicom::attribute::Keyword _tag,
        std::size_t _instance = 0
    ) const;
    /// @}

    /// Values setter using given tag. Strings are joined using DICOM delimiter '\'.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] _group the group tag to set
    /// @param[in] _element the element tag to set
    /// @param[in] _values the vector of value to insert
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @{
    SIGHT_DATA_API void set_byte_values(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::vector<std::string>& _values,
        std::size_t _instance = 0
    );

    SIGHT_DATA_API void set_byte_values(
        dicom::attribute::Keyword _tag,
        const std::vector<std::string>& _values,
        std::size_t _instance = 0
    );
    /// @}

    /// Value getter using given tag. The value is converted to string, depending of the VR.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] _group the group tag to get
    /// @param[in] _element the element tag to get
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    SIGHT_DATA_API std::string get_string_value(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;

    SIGHT_DATA_API std::string get_string_value(dicom::attribute::Keyword _tag, std::size_t _instance = 0) const;
    /// @}

    /// Value setter using given tag. The string argument is converted to the right, depending of the VR.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] _group the group tag to set
    /// @param[in] _element the element tag to set
    /// @param[in] _value the value to insert
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @throw data::exception if tag doesn't exist
    /// @throw data::exception if value size is not correct
    /// @throw data::exception if instance index is out of bounds
    /// @{
    SIGHT_DATA_API void set_string_value(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::string& _value,
        std::size_t _instance = 0
    );

    SIGHT_DATA_API void set_string_value(
        dicom::attribute::Keyword _tag,
        const std::string& _value,
        std::size_t _instance = 0
    );
    /// @}

    /// Private value getter.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] _element private element number in the range of 0x10 to 0xFF
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the private value as a string
    SIGHT_DATA_API std::optional<std::string> get_private_value(
        std::uint8_t _element,
        std::size_t _instance = 0
    ) const;

    /// Private values setter.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] _value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] _element private element number in the range of 0x10 to 0xFF
    /// @param[in] _instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    SIGHT_DATA_API void set_private_value(
        const std::optional<std::string>& _value,
        std::uint8_t _element,
        std::size_t _instance = 0
    );

    /// Private value getter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] _element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] _frame_index the frame index where to store the private tag.
    /// @return the private value as a string
    SIGHT_DATA_API std::optional<std::string> get_multi_frame_private_value(
        std::uint8_t _element,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) const;

    /// Private values setter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] _value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] _element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] _frame_index the frame index where to store the private tag.
    SIGHT_DATA_API void set_multi_frame_private_value(
        const std::optional<std::string>& _value,
        std::uint8_t _element,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    );

    /// Enum that defines the kind of DICOM series we are
    enum class dicom_t : std::uint64_t
    {
        unknown = 0,
        image,
        model,
        report,
        fiducials
    };

    /// In case we want to filter series by type
    using dicom_types = std::underlying_type_t<dicom_t>;

    /// Convenience function to convert from dicom_t enum value to string
    /// @param[in] _type the dicom_t enum value to convert
    static constexpr std::optional<std::string_view> dicom_type_to_string(dicom_t _type)
    {
        switch(_type)
        {
            case dicom_t::image:
                return "image";

            case dicom_t::model:
                return "model";

            case dicom_t::report:
                return "report";

            case dicom_t::fiducials:
                return "fiducials";

            case dicom_t::unknown:
                return std::nullopt;

            default:
                throw std::invalid_argument("Invalid dicom_t enum value");
        }
    }

    /// Convenience function to convert from string to dicom_t enum value
    /// @param[in] _type the string to convert
    constexpr static dicom_t string_to_dicom_type(std::string_view _type) noexcept
    {
        if(constexpr auto image = dicom_type_to_string(dicom_t::image); _type == image)
        {
            return dicom_t::image;
        }
        else if(constexpr auto model = dicom_type_to_string(dicom_t::model); _type == model)
        {
            return dicom_t::model;
        }
        else if(constexpr auto report = dicom_type_to_string(dicom_t::report); _type == report)
        {
            return dicom_t::report;
        }
        else if(constexpr auto fiducials = dicom_type_to_string(dicom_t::fiducials); _type == fiducials)
        {
            return dicom_t::fiducials;
        }
        else
        {
            return dicom_t::unknown;
        }
    }

    /// Convenience function to convert from / to dicom_types values to string
    /// This may be used to configure reader/writer service
    /// @{
    SIGHT_DATA_API static std::string dicom_types_to_string(dicom_types _types) noexcept;
    SIGHT_DATA_API static dicom_types string_to_dicom_types(const std::string& _types) noexcept;

    using SopKeywords = std::set<dicom::sop::Keyword>;
    SIGHT_DATA_API static SopKeywords dicom_types_to_sops(dicom_types _types) noexcept;
    SIGHT_DATA_API static dicom_types sops_to_dicom_types(const SopKeywords& _sops) noexcept;
    SIGHT_DATA_API static SopKeywords string_to_sops(const std::string& _sops) noexcept;
    SIGHT_DATA_API static std::string sops_to_string(const SopKeywords& _sops) noexcept;
    /// @}

    /// Returns the type of the series. For now, only "Image", "Model" and "Fiducials" are supported.
    SIGHT_DATA_API dicom_t get_dicom_type() const noexcept;
    SIGHT_DATA_API static dicom_t get_dicom_type(const std::string& _sop_class_uid) noexcept;

    /// DataSet getter/setter, needed for serialization.
    /// @{
    SIGHT_DATA_API const gdcm::DataSet& get_data_set(std::size_t _instance                = 0) const;
    SIGHT_DATA_API gdcm::DataSet& get_data_set(std::size_t _instance                      = 0);
    SIGHT_DATA_API void set_data_set(const gdcm::DataSet& _dataset, std::size_t _instance = 0);
    /// @}

    /// This is used when sorting DICOM files
    SIGHT_DATA_API std::filesystem::path get_file(std::size_t _instance                    = 0) const;
    SIGHT_DATA_API void set_file(const std::filesystem::path& _file, std::size_t _instance = 0);
    SIGHT_DATA_API std::size_t num_instances() const noexcept;

    /// Utility function that sort instances according to a sorted map of instance index.
    /// @param[in] _sorted the sorted vector of instance.
    SIGHT_DATA_API bool sort(const std::vector<std::size_t>& _sorted);

    /// Returns true if the series is multi-frame.
    SIGHT_DATA_API bool is_multi_frame() const noexcept;

    /// Returns the number of frames in the series.
    SIGHT_DATA_API std::size_t num_frames() const noexcept;

    /// Shrink the number of instances / frames to the given size.
    /// This is mainly an optimization and a bugfix when using GDCM to write a multi-frame DICOM file.
    /// @param _size
    SIGHT_DATA_API void shrink_frames(std::size_t _size);

    /// Helper function to convert a DICOM date time string to/from a std::chrono::system_clock::time_point
    /// @param _date_time in DICOM format
    SIGHT_DATA_API static std::chrono::system_clock::time_point date_time_to_time_point(const std::string& _date_time);

    /// Helper function to convert a std::chrono::system_clock::time_point to /from DICOM date time string
    /// @param _time_point to convert to string in DICOM format
    SIGHT_DATA_API static std::string time_point_to_date_time(const std::chrono::system_clock::time_point& _time_point);

    /// Helper function to convert a DICOM date to ISO date string
    /// @param _date in DICOM format
    SIGHT_DATA_API static std::string date_to_iso(const std::string& _date);

    /// Helper function to convert a DICOM time to ISO time string
    /// @param _time in DICOM format
    SIGHT_DATA_API static std::string time_to_iso(const std::string& _time);

    /// Helper function to generate a file name / sub-folder for a given series.
    /// @{
    /// Returns "<PatientID>.<Modality>.hash(SeriesInstanceUID).<SeriesDate><SeriesTime>"
    SIGHT_DATA_API std::filesystem::path file_name(
        const std::optional<std::string>& _suffix = std::string(".dcm")
    ) const;

    /// Returns "<root> / <PatientID> / <StudyDate><StudyTime>"
    /// @param _root: the base root folder where to store the file
    SIGHT_DATA_API std::filesystem::path folder(
        const std::optional<std::filesystem::path>& _root = std::nullopt
    ) const;

    /// Returns "folder(root) / file_name(suffix)"
    /// @param _root: the base root folder where to store the file
    /// @param _suffix: the suffix to append to the file name
    SIGHT_DATA_API std::filesystem::path file_path(
        const std::optional<std::filesystem::path>& _root = std::nullopt,
        const std::optional<std::string>& _suffix         = std::string(".dcm")
    ) const;
    /// @}

    /// Helper function to generate a DICOM UID.
    SIGHT_DATA_API static std::string generate_uid();

protected:

    std::unique_ptr<detail::series_impl> m_pimpl;
};

} // Namespace fwMedData.
