/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/config.hpp"
#include "data/dicom/Attribute.hpp"
#include "data/dicom/Sop.hpp"
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

class SeriesImpl;

}

namespace sight::data
{

/**
 * @brief Holds series information.
 */
class DATA_CLASS_API series : public virtual object
{
public:

    SIGHT_DECLARE_CLASS(series, object);

    /// Constructor / Destructor
    /// @{
    DATA_API series();
    DATA_API ~series() noexcept override;
    /// @}

    /// Getter/Setter of DICOM SOP Common Module related attributes
    /// @{
    DATA_API dicom::sop::Keyword get_sop_keyword() const noexcept;
    DATA_API void set_sop_keyword(dicom::sop::Keyword _keyword);
    DATA_API std::string_view get_sop_class_name() const noexcept;

    DATA_API std::string get_sop_instance_uid() const noexcept;
    DATA_API void set_sop_instance_uid(const std::string& _sop_instance_uid);

    DATA_API std::string get_sop_class_uid() const noexcept;
    DATA_API void set_sop_class_uid(const std::string& _sop_class_uid);

    DATA_API std::string get_specific_character_set() const noexcept;
    DATA_API void set_specific_character_set(const std::string& _specific_character_set);

    /// Translate DICOM specific character set to something compatible with iconv/ICU/Qt
    DATA_API std::string get_encoding() const noexcept;
    /// @}

    /// Getter/Setter of DICOM General series Module related attributes
    /// @{
    DATA_API std::string get_series_date() const noexcept;
    DATA_API void set_series_date(const std::string& _series_date);

    DATA_API std::string get_series_time() const noexcept;
    DATA_API void set_series_time(const std::string& _series_time);

    DATA_API std::string get_modality() const noexcept;
    DATA_API void set_modality(const std::string& _modality);

    DATA_API std::string get_series_description() const noexcept;
    DATA_API void set_series_description(const std::string& _series_description);

    DATA_API std::vector<std::string> get_performing_physician_names() const noexcept;
    DATA_API void set_performing_physician_names(const std::vector<std::string>& _performing_physician_names);

    DATA_API std::string get_performing_physician_name() const noexcept;
    DATA_API void set_performing_physician_name(const std::string& _performing_physician_name);

    DATA_API std::string get_anatomical_orientation_type() const noexcept;
    DATA_API void set_anatomical_orientation_type(const std::string& _anatomical_orientation_type);

    DATA_API std::string get_body_part_examined() const noexcept;
    DATA_API void set_body_part_examined(const std::string& _body_part_examined);

    DATA_API std::string get_protocol_name() const noexcept;
    DATA_API void set_protocol_name(const std::string& _protocol_name);

    DATA_API std::string get_patient_position() const noexcept;
    DATA_API void set_patient_position(const std::string& _patient_position);

    /// Returns the patient position as human readable string
    DATA_API std::string get_patient_position_string() const noexcept;

    DATA_API std::string get_series_instance_uid() const noexcept;
    DATA_API void set_series_instance_uid(const std::string& _series_instance_uid);

    DATA_API std::optional<std::int32_t> get_series_number() const noexcept;
    DATA_API void set_series_number(const std::optional<std::int32_t>& _series_number = std::nullopt);

    DATA_API std::string get_laterality() const noexcept;
    DATA_API void set_laterality(const std::string& _laterality);

    DATA_API std::string get_performed_procedure_step_start_date() const noexcept;
    DATA_API void set_performed_procedure_step_start_date(const std::string& _performed_procedure_step_start_date);

    DATA_API std::string get_performed_procedure_step_start_time() const noexcept;
    DATA_API void set_performed_procedure_step_start_time(const std::string& _performed_procedure_step_start_time);

    DATA_API std::string get_performed_procedure_step_end_date() const noexcept;
    DATA_API void set_performed_procedure_step_end_date(const std::string& _performed_procedure_step_end_date);

    DATA_API std::string get_performed_procedure_step_end_time() const noexcept;
    DATA_API void set_performed_procedure_step_end_time(const std::string& _performed_procedure_step_end_time);

    DATA_API std::string get_performed_procedure_step_id() const noexcept;
    DATA_API void set_performed_procedure_step_id(const std::string& _performed_procedure_step_id);

    DATA_API std::string get_performed_procedure_step_description() const noexcept;
    DATA_API void set_performed_procedure_step_description(const std::string& _performed_procedure_step_description);

    DATA_API std::string get_comments_on_the_performed_procedure_step() const noexcept;
    DATA_API void set_comments_on_the_performed_procedure_step(
        const std::string& _comments_on_the_performed_procedure_step
    );
    /// @}

    /// Getter/Setter of DICOM General Equipment Module related attributes
    /// @{
    DATA_API std::string get_institution_name() const noexcept;
    DATA_API void set_institution_name(const std::string& _institution_name);
    /// @}

    /// Getter/Setter of DICOM Patient Module related attributes
    /// @{
    DATA_API std::string get_patient_name() const noexcept;
    DATA_API void set_patient_name(const std::string& _patient_name);

    DATA_API std::string get_patient_id() const noexcept;
    DATA_API void set_patient_id(const std::string& _patient_id);

    DATA_API std::string get_patient_birth_date() const noexcept;
    DATA_API void set_patient_birth_date(const std::string& _patient_birth_date);

    DATA_API std::string get_patient_sex() const noexcept;
    DATA_API void set_patient_sex(const std::string& _patient_sex);
    /// @}

    /// Getter/Setter of DICOM General Study Module related attributes
    /// @{
    DATA_API std::string get_study_date() const noexcept;
    DATA_API void set_study_date(const std::string& _study_date);

    DATA_API std::string get_study_time() const noexcept;
    DATA_API void set_study_time(const std::string& _study_time);

    DATA_API std::string get_referring_physician_name() const noexcept;
    DATA_API void set_referring_physician_name(const std::string& _referring_physician_name);

    DATA_API std::string get_study_description() const noexcept;
    DATA_API void set_study_description(const std::string& _study_description);

    DATA_API std::string get_study_instance_uid() const noexcept;
    DATA_API void set_study_instance_uid(const std::string& _study_instance_uid);

    DATA_API std::string get_study_id() const noexcept;
    DATA_API void set_study_id(const std::string& _study_id);
    /// @}

    /// Getter/Setter of DICOM Patient Study Module related attributes
    /// @{
    DATA_API std::string get_patient_age() const noexcept;
    DATA_API void set_patient_age(const std::string& _patient_age);

    DATA_API std::optional<double> get_patient_size() const noexcept;
    DATA_API void set_patient_size(const std::optional<double>& _patient_size = std::nullopt);

    DATA_API std::optional<double> get_patient_weight() const noexcept;
    DATA_API void set_patient_weight(const std::optional<double>& _patient_weight = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM General Image Module related attributes
    /// @{
    DATA_API std::string get_acquisition_date(std::size_t _instance                                = 0) const;
    DATA_API void set_acquisition_date(const std::string& _acquisition_date, std::size_t _instance = 0);

    DATA_API std::string get_acquisition_time(std::size_t _instance                                = 0) const;
    DATA_API void set_acquisition_time(const std::string& _acquisition_time, std::size_t _instance = 0);

    DATA_API std::string get_content_time(std::size_t _instance                            = 0) const;
    DATA_API void set_content_time(const std::string& _content_time, std::size_t _instance = 0);

    DATA_API std::optional<std::int32_t> get_instance_number(std::size_t _instance = 0) const;

    DATA_API void set_instance_number(
        const std::optional<std::int32_t>& _instance_number = std::nullopt,
        std::size_t _instance                               = 0
    );

    DATA_API std::optional<std::int32_t> get_acquisition_number(
        std::size_t _instance = 0
    ) const;

    DATA_API void set_acquisition_number(
        std::optional<std::int32_t> _acquisition_number = std::nullopt,
        std::size_t _instance                           = 0
    );
    /// @}

    /// Getter/Setter of DICOM Contrast/Bolus Module related attributes
    /// @{
    DATA_API std::string get_contrast_bolus_agent() const noexcept;
    DATA_API void set_contrast_bolus_agent(const std::string& _contrast_bolus_agent);

    DATA_API std::string get_contrast_bolus_route() const noexcept;
    DATA_API void set_contrast_bolus_route(const std::string& _contrast_bolus_route);

    DATA_API std::optional<double> get_contrast_bolus_volume() const noexcept;
    DATA_API void set_contrast_bolus_volume(const std::optional<double>& _contrast_bolus_volume = std::nullopt);

    DATA_API std::string get_contrast_bolus_start_time() const noexcept;
    DATA_API void set_contrast_bolus_start_time(const std::string& _contrast_bolus_start_time);

    DATA_API std::string get_contrast_bolus_stop_time() const noexcept;
    DATA_API void set_contrast_bolus_stop_time(const std::string& _contrast_bolus_stop_time);

    DATA_API std::optional<double> get_contrast_bolus_total_dose() const noexcept;
    DATA_API void set_contrast_bolus_total_dose(const std::optional<double>& _contrast_bolus_total_dose = std::nullopt);

    DATA_API std::vector<double> get_contrast_flow_rates() const noexcept;
    DATA_API void set_contrast_flow_rates(const std::vector<double>& _contrast_flow_rates);

    DATA_API std::string get_contrast_flow_rate() const noexcept;
    DATA_API void set_contrast_flow_rate(const std::string& _contrast_flow_rates);

    DATA_API std::vector<double> get_contrast_flow_durations() const noexcept;
    DATA_API void set_contrast_flow_durations(const std::vector<double>& _contrast_flow_durations);

    DATA_API std::string get_contrast_flow_duration() const noexcept;
    DATA_API void set_contrast_flow_duration(const std::string& _contrast_flow_durations);

    DATA_API std::string get_contrast_bolus_ingredient() const noexcept;
    DATA_API void set_contrast_bolus_ingredient(const std::string& _contrast_bolus_ingredient);

    DATA_API std::optional<double> get_contrast_bolus_ingredient_concentration() const noexcept;
    DATA_API void set_contrast_bolus_ingredient_concentration(
        const std::optional<double>& _contrast_bolus_ingredient_concentration = std::nullopt
    );

    /// Getter/Setter of DICOM Image Pixel Module
    /// @{
    DATA_API std::optional<std::uint16_t> get_rows() const noexcept;
    DATA_API virtual void set_rows(const std::optional<std::uint16_t>& _rows = std::nullopt);

    DATA_API std::optional<std::uint16_t> get_columns() const noexcept;
    DATA_API virtual void set_columns(const std::optional<std::uint16_t>& _columns = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    DATA_API virtual std::vector<double> window_center() const noexcept;
    DATA_API virtual void set_window_center(const std::vector<double>& _window_centers);

    DATA_API virtual std::vector<double> window_width() const noexcept;
    DATA_API virtual void set_window_width(const std::vector<double>& _window_widths);
    /// @}

    /// Getter/Setter of DICOM Modality LUT Module
    /// @{
    DATA_API std::optional<double> get_rescale_intercept(std::size_t _instance = 0) const noexcept;
    DATA_API void set_rescale_intercept(
        const std::optional<double>& _rescale_intercept = std::nullopt,
        std::size_t _instance                           = 0
    );

    DATA_API std::optional<double> get_rescale_slope(std::size_t _instance = 0) const noexcept;
    DATA_API void set_rescale_slope(
        const std::optional<double>& _rescale_slope = std::nullopt,
        std::size_t _instance                       = 0
    );
    /// @}

    /// Getter/Setter of DICOM Image Plane Module related attributes
    /// ...and Multi-frame Functional Groups Module
    /// @{
    DATA_API std::vector<double> get_image_position_patient(std::size_t _instance = 0) const;

    DATA_API void set_image_position_patient(
        const std::vector<double>& _image_position_patient,
        std::size_t _instance = 0
    );

    DATA_API std::vector<double> get_image_orientation_patient(std::size_t _instance = 0) const;

    DATA_API void set_image_orientation_patient(
        const std::vector<double>& _image_orientation_patient,
        std::size_t _instance = 0
    );

    DATA_API sight::data::matrix4 get_image_transform_patient(std::size_t _instance                         = 0) const;
    DATA_API void set_image_transform_patient(const sight::data::matrix4& _transform, std::size_t _instance = 0);

    DATA_API std::optional<double> get_slice_thickness() const noexcept;
    DATA_API void set_slice_thickness(const std::optional<double>& _slice_thickness = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM Multi-frame Functional Groups Module related attributes
    /// @{
    DATA_API std::vector<double> get_image_position_volume(std::size_t _frame_index = 0) const;

    DATA_API void set_image_position_volume(
        const std::vector<double>& _image_position_volume,
        std::size_t _frame_index = 0
    );

    DATA_API std::vector<double> get_image_orientation_volume(std::size_t _frame_index = 0) const;

    DATA_API void set_image_orientation_volume(
        const std::vector<double>& _image_orientation_volume,
        std::size_t _frame_index = 0
    );

    DATA_API std::optional<std::string> get_frame_acquisition_date_time(std::size_t _frame_index = 0) const;

    DATA_API void set_frame_acquisition_date_time(
        const std::optional<std::string>& _frame_acquisition_date_time = std::nullopt,
        std::size_t _frame_index                                       = 0
    );

    DATA_API std::optional<std::chrono::system_clock::time_point> get_frame_acquisition_time_point(
        std::size_t _frame_index = 0
    ) const noexcept;

    DATA_API void set_frame_acquisition_time_point(
        const std::optional<std::chrono::system_clock::time_point>& _time_point = std::nullopt,
        std::size_t _frame_index                                                = 0
    );

    DATA_API void set_frame_acquisition_time_point(
        sight::core::hires_clock::type _time_point,
        std::size_t _frame_index = 0
    );

    DATA_API std::optional<std::string> get_frame_comments(std::size_t _frame_index = 0) const;

    DATA_API void set_frame_comments(
        const std::optional<std::string>& _frame_comments = std::nullopt,
        std::size_t _frame_index                          = 0
    );

    DATA_API std::optional<std::string> get_frame_label(std::size_t _frame_index = 0) const;

    DATA_API void set_frame_label(
        const std::optional<std::string>& _frame_label = std::nullopt,
        std::size_t _frame_index                       = 0
    );

    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const series& _other) const noexcept;
    DATA_API bool operator!=(const series& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /// Defines partial copy. Useful to share data between different series
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy from
    /// @{
    DATA_API void copy_patient_module(const series::csptr& _source, std::size_t _instance                = 0);
    DATA_API void copy_clinical_trial_subject_module(const series::csptr& _source, std::size_t _instance = 0);
    DATA_API void copy_general_study_module(const series::csptr& _source, std::size_t _instance          = 0);
    DATA_API void copy_patient_study_module(const series::csptr& _source, std::size_t _instance          = 0);
    DATA_API void copy_clinical_trial_study_module(const series::csptr& _source, std::size_t _instance   = 0);
    DATA_API void copy_general_series_module(const series::csptr& _source, std::size_t _instance         = 0);
    DATA_API void copy_clinical_trial_series_module(const series::csptr& _source, std::size_t _instance  = 0);
    DATA_API void copy_general_equipment_module(const series::csptr& _source, std::size_t _instance      = 0);
    DATA_API void copy_frame_of_reference_module(const series::csptr& _source, std::size_t _instance     = 0);
    DATA_API void copy_sop_common_module(const series::csptr& _source, std::size_t _instance             = 0);
    DATA_API void copy_general_image_module(const series::csptr& _source, std::size_t _instance          = 0);
    /// @}

    /// Value getter using given tag. The returned string is used as a Byte buffer and is not necessarily a string.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    DATA_API std::string get_byte_value(std::uint16_t _group, std::uint16_t _element, std::size_t _instance = 0) const;
    DATA_API std::string get_byte_value(dicom::attribute::Keyword _tag, std::size_t _instance               = 0) const;
    /// @}

    /// Value setter using given tag. The string argument is used as a Byte buffer and is not necessarily a string.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @throw data::exception if tag doesn't exist
    /// @throw data::exception if value size is not correct
    /// @throw data::exception if instance index is out of bounds
    /// @{
    DATA_API void set_byte_value(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::string& _value,
        std::size_t _instance = 0
    );

    DATA_API void set_byte_value(
        dicom::attribute::Keyword _tag,
        const std::string& _value,
        std::size_t _instance = 0
    );

    /// @}

    /// Values getter using given tag. Initial string is split using DICOM delimiter '\'.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a vector of strings
    /// @{
    DATA_API std::vector<std::string> get_byte_values(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;

    DATA_API std::vector<std::string> get_byte_values(dicom::attribute::Keyword _tag, std::size_t _instance = 0) const;
    /// @}

    /// Values setter using given tag. Strings are joined using DICOM delimiter '\'.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the vector of value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @{
    DATA_API void set_byte_values(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::vector<std::string>& _values,
        std::size_t _instance = 0
    );

    DATA_API void set_byte_values(
        dicom::attribute::Keyword _tag,
        const std::vector<std::string>& _values,
        std::size_t _instance = 0
    );
    /// @}

    /// Value getter using given tag. The value is converted to string, depending of the VR.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    DATA_API std::string get_string_value(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;

    DATA_API std::string get_string_value(dicom::attribute::Keyword _tag, std::size_t _instance = 0) const;
    /// @}

    /// Value setter using given tag. The string argument is converted to the right, depending of the VR.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @throw data::exception if tag doesn't exist
    /// @throw data::exception if value size is not correct
    /// @throw data::exception if instance index is out of bounds
    /// @{
    DATA_API void set_string_value(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::string& _value,
        std::size_t _instance = 0
    );

    DATA_API void set_string_value(
        dicom::attribute::Keyword _tag,
        const std::string& _value,
        std::size_t _instance = 0
    );
    /// @}

    /// Private value getter.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the private value as a string
    DATA_API std::optional<std::string> get_private_value(
        std::uint8_t _element,
        std::size_t _instance = 0
    ) const;

    /// Private values setter.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    DATA_API void set_private_value(
        const std::optional<std::string>& _value,
        std::uint8_t _element,
        std::size_t _instance = 0
    );

    /// Private value getter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] frameIndex the frame index where to store the private tag.
    /// @return the private value as a string
    DATA_API std::optional<std::string> get_multi_frame_private_value(
        std::uint8_t _element,
        std::size_t _frame_index = 0
    ) const;

    /// Private values setter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] frameIndex the frame index where to store the private tag.
    DATA_API void set_multi_frame_private_value(
        const std::optional<std::string>& _value,
        std::uint8_t _element,
        std::size_t _frame_index = 0
    );

    /// Enum that defines the kind of DICOM series we are
    enum class dicom_t : std::uint64_t
    {
        image     = 1,
        model     = image << 1,
        report    = model << 1,
        fiducials = report << 1,
        unknown   = 0
    };

    /// In case we want to filter series by type
    using DicomTypes = std::underlying_type_t<dicom_t>;

    /// Convenience function to convert from dicom_t enum value to string
    /// @param[in] type the dicom_t enum value to convert
    constexpr static std::string_view dicom_type_to_string(dicom_t _type) noexcept
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

            default:
                return "unknown";
        }
    }

    /// Convenience function to convert from string to dicom_t enum value
    /// @param[in] type the string to convert
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

    /// Convenience function to convert from / to DicomTypes values to string
    /// This may be used to configure reader/writer service
    /// @{
    DATA_API static std::string dicom_types_to_string(DicomTypes _types) noexcept;
    DATA_API static DicomTypes string_to_dicom_types(const std::string& _types) noexcept;

    using SopKeywords = std::set<dicom::sop::Keyword>;
    DATA_API static SopKeywords dicom_types_to_sops(DicomTypes _types) noexcept;
    DATA_API static DicomTypes sops_to_dicom_types(const SopKeywords& _sops) noexcept;
    DATA_API static SopKeywords string_to_sops(const std::string& _sops) noexcept;
    DATA_API static std::string sops_to_string(const SopKeywords& _sops) noexcept;
    /// @}

    /// Returns the type of the series. For now, only "Image", "Model" and "Fiducials" are supported.
    DATA_API dicom_t get_dicom_type() const noexcept;
    DATA_API static dicom_t get_dicom_type(const std::string& _sop_class_uid) noexcept;

    /// DataSet getter/setter, needed for serialization.
    /// @{
    DATA_API const gdcm::DataSet& get_data_set(std::size_t _instance                = 0) const;
    DATA_API gdcm::DataSet& get_data_set(std::size_t _instance                      = 0);
    DATA_API void set_data_set(const gdcm::DataSet& _dataset, std::size_t _instance = 0);
    /// @}

    /// This is used when sorting DICOM files
    DATA_API std::filesystem::path get_file(std::size_t _instance                    = 0) const;
    DATA_API void set_file(const std::filesystem::path& _file, std::size_t _instance = 0);
    DATA_API std::size_t num_instances() const noexcept;

    /// Utility function that sort instances according to a sorted map of instance index.
    /// @param[in] sorted the sorted vector of instance.
    DATA_API bool sort(const std::vector<std::size_t>& _sorted);

    /// Returns true if the series is multi-frame.
    DATA_API bool is_multi_frame() const noexcept;

    /// Returns the number of frames in the series.
    DATA_API std::size_t num_frames() const noexcept;

    /// Helper function to convert a DICOM date time string to/from a std::chrono::system_clock::time_point
    DATA_API static std::chrono::system_clock::time_point date_time_to_time_point(const std::string& _date_time);

    /// Helper function to convert a std::chrono::system_clock::time_point to /from DICOM date time string
    DATA_API static std::string time_point_to_date_time(const std::chrono::system_clock::time_point& _time_point);

    /// Shrink the number of instances / frames to the given size.
    /// This is mainly an optimization and a bugfix when using GDCM to write a multi-frame DICOM file.
    /// @param size
    DATA_API void shrink_frames(std::size_t _size);

    DATA_API void set_frame_landmarks(std::vector<data::landmarks::sptr> _landmarks, std::size_t _frame_index);
    DATA_API std::vector<data::landmarks::sptr> get_frame_landmarks(std::size_t _frame_index);

protected:

    std::unique_ptr<detail::SeriesImpl> m_pimpl;
};

} // Namespace fwMedData.
