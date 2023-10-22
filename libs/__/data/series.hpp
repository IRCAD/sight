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
    DATA_API dicom::sop::Keyword getSOPKeyword() const noexcept;
    DATA_API void setSOPKeyword(dicom::sop::Keyword _keyword);
    DATA_API std::string_view getSOPClassName() const noexcept;

    DATA_API std::string getSOPInstanceUID() const noexcept;
    DATA_API void setSOPInstanceUID(const std::string& _sop_instance_uid);

    DATA_API std::string getSOPClassUID() const noexcept;
    DATA_API void setSOPClassUID(const std::string& _sop_class_uid);

    DATA_API std::string getSpecificCharacterSet() const noexcept;
    DATA_API void setSpecificCharacterSet(const std::string& _specific_character_set);

    /// Translate DICOM specific character set to something compatible with iconv/ICU/Qt
    DATA_API std::string getEncoding() const noexcept;
    /// @}

    /// Getter/Setter of DICOM General series Module related attributes
    /// @{
    DATA_API std::string getSeriesDate() const noexcept;
    DATA_API void setSeriesDate(const std::string& _series_date);

    DATA_API std::string getSeriesTime() const noexcept;
    DATA_API void setSeriesTime(const std::string& _series_time);

    DATA_API std::string getModality() const noexcept;
    DATA_API void setModality(const std::string& _modality);

    DATA_API std::string getSeriesDescription() const noexcept;
    DATA_API void setSeriesDescription(const std::string& _series_description);

    DATA_API std::vector<std::string> getPerformingPhysicianNames() const noexcept;
    DATA_API void setPerformingPhysicianNames(const std::vector<std::string>& _performing_physician_names);

    DATA_API std::string getPerformingPhysicianName() const noexcept;
    DATA_API void setPerformingPhysicianName(const std::string& _performing_physician_name);

    DATA_API std::string get_anatomical_orientation_type() const noexcept;
    DATA_API void set_anatomical_orientation_type(const std::string& _anatomical_orientation_type);

    DATA_API std::string getBodyPartExamined() const noexcept;
    DATA_API void setBodyPartExamined(const std::string& _body_part_examined);

    DATA_API std::string getProtocolName() const noexcept;
    DATA_API void setProtocolName(const std::string& _protocol_name);

    DATA_API std::string getPatientPosition() const noexcept;
    DATA_API void setPatientPosition(const std::string& _patient_position);

    /// Returns the patient position as human readable string
    DATA_API std::string getPatientPositionString() const noexcept;

    DATA_API std::string getSeriesInstanceUID() const noexcept;
    DATA_API void setSeriesInstanceUID(const std::string& _series_instance_uid);

    DATA_API std::optional<std::int32_t> getSeriesNumber() const noexcept;
    DATA_API void setSeriesNumber(const std::optional<std::int32_t>& _series_number = std::nullopt);

    DATA_API std::string getLaterality() const noexcept;
    DATA_API void setLaterality(const std::string& _laterality);

    DATA_API std::string getPerformedProcedureStepStartDate() const noexcept;
    DATA_API void setPerformedProcedureStepStartDate(const std::string& _performed_procedure_step_start_date);

    DATA_API std::string getPerformedProcedureStepStartTime() const noexcept;
    DATA_API void setPerformedProcedureStepStartTime(const std::string& _performed_procedure_step_start_time);

    DATA_API std::string getPerformedProcedureStepEndDate() const noexcept;
    DATA_API void setPerformedProcedureStepEndDate(const std::string& _performed_procedure_step_end_date);

    DATA_API std::string getPerformedProcedureStepEndTime() const noexcept;
    DATA_API void setPerformedProcedureStepEndTime(const std::string& _performed_procedure_step_end_time);

    DATA_API std::string getPerformedProcedureStepID() const noexcept;
    DATA_API void setPerformedProcedureStepID(const std::string& _performed_procedure_step_id);

    DATA_API std::string getPerformedProcedureStepDescription() const noexcept;
    DATA_API void setPerformedProcedureStepDescription(const std::string& _performed_procedure_step_description);

    DATA_API std::string getCommentsOnThePerformedProcedureStep() const noexcept;
    DATA_API void setCommentsOnThePerformedProcedureStep(const std::string& _comments_on_the_performed_procedure_step);
    /// @}

    /// Getter/Setter of DICOM General Equipment Module related attributes
    /// @{
    DATA_API std::string getInstitutionName() const noexcept;
    DATA_API void setInstitutionName(const std::string& _institution_name);
    /// @}

    /// Getter/Setter of DICOM Patient Module related attributes
    /// @{
    DATA_API std::string getPatientName() const noexcept;
    DATA_API void setPatientName(const std::string& _patient_name);

    DATA_API std::string getPatientID() const noexcept;
    DATA_API void setPatientID(const std::string& _patient_id);

    DATA_API std::string getPatientBirthDate() const noexcept;
    DATA_API void setPatientBirthDate(const std::string& _patient_birth_date);

    DATA_API std::string getPatientSex() const noexcept;
    DATA_API void setPatientSex(const std::string& _patient_sex);
    /// @}

    /// Getter/Setter of DICOM General Study Module related attributes
    /// @{
    DATA_API std::string getStudyDate() const noexcept;
    DATA_API void setStudyDate(const std::string& _study_date);

    DATA_API std::string getStudyTime() const noexcept;
    DATA_API void setStudyTime(const std::string& _study_time);

    DATA_API std::string getReferringPhysicianName() const noexcept;
    DATA_API void setReferringPhysicianName(const std::string& _referring_physician_name);

    DATA_API std::string getStudyDescription() const noexcept;
    DATA_API void setStudyDescription(const std::string& _study_description);

    DATA_API std::string getStudyInstanceUID() const noexcept;
    DATA_API void setStudyInstanceUID(const std::string& _study_instance_uid);

    DATA_API std::string getStudyID() const noexcept;
    DATA_API void setStudyID(const std::string& _study_id);
    /// @}

    /// Getter/Setter of DICOM Patient Study Module related attributes
    /// @{
    DATA_API std::string getPatientAge() const noexcept;
    DATA_API void setPatientAge(const std::string& _patient_age);

    DATA_API std::optional<double> getPatientSize() const noexcept;
    DATA_API void setPatientSize(const std::optional<double>& _patient_size = std::nullopt);

    DATA_API std::optional<double> getPatientWeight() const noexcept;
    DATA_API void setPatientWeight(const std::optional<double>& _patient_weight = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM General Image Module related attributes
    /// @{
    DATA_API std::string getAcquisitionDate(std::size_t _instance                                = 0) const;
    DATA_API void setAcquisitionDate(const std::string& _acquisition_date, std::size_t _instance = 0);

    DATA_API std::string getAcquisitionTime(std::size_t _instance                                = 0) const;
    DATA_API void setAcquisitionTime(const std::string& _acquisition_time, std::size_t _instance = 0);

    DATA_API std::string getContentTime(std::size_t _instance                            = 0) const;
    DATA_API void setContentTime(const std::string& _content_time, std::size_t _instance = 0);

    DATA_API std::optional<std::int32_t> getInstanceNumber(std::size_t _instance = 0) const;

    DATA_API void setInstanceNumber(
        const std::optional<std::int32_t>& _instance_number = std::nullopt,
        std::size_t _instance                               = 0
    );

    DATA_API std::optional<std::int32_t> getAcquisitionNumber(
        std::size_t _instance = 0
    ) const;

    DATA_API void setAcquisitionNumber(
        std::optional<std::int32_t> _acquisition_number = std::nullopt,
        std::size_t _instance                           = 0
    );
    /// @}

    /// Getter/Setter of DICOM Contrast/Bolus Module related attributes
    /// @{
    DATA_API std::string getContrastBolusAgent() const noexcept;
    DATA_API void setContrastBolusAgent(const std::string& _contrast_bolus_agent);

    DATA_API std::string getContrastBolusRoute() const noexcept;
    DATA_API void setContrastBolusRoute(const std::string& _contrast_bolus_route);

    DATA_API std::optional<double> getContrastBolusVolume() const noexcept;
    DATA_API void setContrastBolusVolume(const std::optional<double>& _contrast_bolus_volume = std::nullopt);

    DATA_API std::string getContrastBolusStartTime() const noexcept;
    DATA_API void setContrastBolusStartTime(const std::string& _contrast_bolus_start_time);

    DATA_API std::string getContrastBolusStopTime() const noexcept;
    DATA_API void setContrastBolusStopTime(const std::string& _contrast_bolus_stop_time);

    DATA_API std::optional<double> getContrastBolusTotalDose() const noexcept;
    DATA_API void setContrastBolusTotalDose(const std::optional<double>& _contrast_bolus_total_dose = std::nullopt);

    DATA_API std::vector<double> getContrastFlowRates() const noexcept;
    DATA_API void setContrastFlowRates(const std::vector<double>& _contrast_flow_rates);

    DATA_API std::string getContrastFlowRate() const noexcept;
    DATA_API void setContrastFlowRate(const std::string& _contrast_flow_rates);

    DATA_API std::vector<double> getContrastFlowDurations() const noexcept;
    DATA_API void setContrastFlowDurations(const std::vector<double>& _contrast_flow_durations);

    DATA_API std::string getContrastFlowDuration() const noexcept;
    DATA_API void setContrastFlowDuration(const std::string& _contrast_flow_durations);

    DATA_API std::string getContrastBolusIngredient() const noexcept;
    DATA_API void setContrastBolusIngredient(const std::string& _contrast_bolus_ingredient);

    DATA_API std::optional<double> getContrastBolusIngredientConcentration() const noexcept;
    DATA_API void setContrastBolusIngredientConcentration(
        const std::optional<double>& _contrast_bolus_ingredient_concentration = std::nullopt
    );

    /// Getter/Setter of DICOM Image Pixel Module
    /// @{
    DATA_API std::optional<std::uint16_t> getRows() const noexcept;
    DATA_API virtual void setRows(const std::optional<std::uint16_t>& _rows = std::nullopt);

    DATA_API std::optional<std::uint16_t> getColumns() const noexcept;
    DATA_API virtual void setColumns(const std::optional<std::uint16_t>& _columns = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    DATA_API virtual std::vector<double> getWindowCenter() const noexcept;
    DATA_API virtual void setWindowCenter(const std::vector<double>& _window_centers);

    DATA_API virtual std::vector<double> getWindowWidth() const noexcept;
    DATA_API virtual void setWindowWidth(const std::vector<double>& _window_widths);
    /// @}

    /// Getter/Setter of DICOM Modality LUT Module
    /// @{
    DATA_API std::optional<double> getRescaleIntercept(std::size_t _instance = 0) const noexcept;
    DATA_API void setRescaleIntercept(
        const std::optional<double>& _rescale_intercept = std::nullopt,
        std::size_t _instance                           = 0
    );

    DATA_API std::optional<double> getRescaleSlope(std::size_t _instance = 0) const noexcept;
    DATA_API void setRescaleSlope(
        const std::optional<double>& _rescale_slope = std::nullopt,
        std::size_t _instance                       = 0
    );
    /// @}

    /// Getter/Setter of DICOM Image Plane Module related attributes
    /// ...and Multi-frame Functional Groups Module
    /// @{
    DATA_API std::vector<double> getImagePositionPatient(std::size_t _instance = 0) const;

    DATA_API void setImagePositionPatient(
        const std::vector<double>& _image_position_patient,
        std::size_t _instance = 0
    );

    DATA_API std::vector<double> getImageOrientationPatient(std::size_t _instance = 0) const;

    DATA_API void setImageOrientationPatient(
        const std::vector<double>& _image_orientation_patient,
        std::size_t _instance = 0
    );

    DATA_API sight::data::matrix4 getImageTransformPatient(std::size_t _instance                         = 0) const;
    DATA_API void setImageTransformPatient(const sight::data::matrix4& _transform, std::size_t _instance = 0);

    DATA_API std::optional<double> getSliceThickness() const noexcept;
    DATA_API void setSliceThickness(const std::optional<double>& _slice_thickness = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM Multi-frame Functional Groups Module related attributes
    /// @{
    DATA_API std::vector<double> getImagePositionVolume(std::size_t _frame_index = 0) const;

    DATA_API void setImagePositionVolume(
        const std::vector<double>& _image_position_volume,
        std::size_t _frame_index = 0
    );

    DATA_API std::vector<double> getImageOrientationVolume(std::size_t _frame_index = 0) const;

    DATA_API void setImageOrientationVolume(
        const std::vector<double>& _image_orientation_volume,
        std::size_t _frame_index = 0
    );

    DATA_API std::optional<std::string> getFrameAcquisitionDateTime(std::size_t _frame_index = 0) const;

    DATA_API void setFrameAcquisitionDateTime(
        const std::optional<std::string>& _frame_acquisition_date_time = std::nullopt,
        std::size_t _frame_index                                       = 0
    );

    DATA_API std::optional<std::chrono::system_clock::time_point> getFrameAcquisitionTimePoint(
        std::size_t _frame_index = 0
    ) const noexcept;

    DATA_API void setFrameAcquisitionTimePoint(
        const std::optional<std::chrono::system_clock::time_point>& _time_point = std::nullopt,
        std::size_t _frame_index                                                = 0
    );

    DATA_API void setFrameAcquisitionTimePoint(
        sight::core::hires_clock::type _time_point,
        std::size_t _frame_index = 0
    );

    DATA_API std::optional<std::string> getFrameComments(std::size_t _frame_index = 0) const;

    DATA_API void setFrameComments(
        const std::optional<std::string>& _frame_comments = std::nullopt,
        std::size_t _frame_index                          = 0
    );

    DATA_API std::optional<std::string> getFrameLabel(std::size_t _frame_index = 0) const;

    DATA_API void setFrameLabel(
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
    DATA_API void copyPatientModule(const series::csptr& _source, std::size_t _instance              = 0);
    DATA_API void copyClinicalTrialSubjectModule(const series::csptr& _source, std::size_t _instance = 0);
    DATA_API void copyGeneralStudyModule(const series::csptr& _source, std::size_t _instance         = 0);
    DATA_API void copyPatientStudyModule(const series::csptr& _source, std::size_t _instance         = 0);
    DATA_API void copyClinicalTrialStudyModule(const series::csptr& _source, std::size_t _instance   = 0);
    DATA_API void copyGeneralSeriesModule(const series::csptr& _source, std::size_t _instance        = 0);
    DATA_API void copyClinicalTrialSeriesModule(const series::csptr& _source, std::size_t _instance  = 0);
    DATA_API void copyGeneralEquipmentModule(const series::csptr& _source, std::size_t _instance     = 0);
    DATA_API void copyFrameOfReferenceModule(const series::csptr& _source, std::size_t _instance     = 0);
    DATA_API void copySOPCommonModule(const series::csptr& _source, std::size_t _instance            = 0);
    DATA_API void copyGeneralImageModule(const series::csptr& _source, std::size_t _instance         = 0);
    /// @}

    /// Value getter using given tag. The returned string is used as a Byte buffer and is not necessarily a string.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    DATA_API std::string getByteValue(std::uint16_t _group, std::uint16_t _element, std::size_t _instance = 0) const;
    DATA_API std::string getByteValue(dicom::attribute::Keyword _tag, std::size_t _instance               = 0) const;
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
    DATA_API void setByteValue(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::string& _value,
        std::size_t _instance = 0
    );

    DATA_API void setByteValue(
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
    DATA_API std::vector<std::string> getByteValues(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;

    DATA_API std::vector<std::string> getByteValues(dicom::attribute::Keyword _tag, std::size_t _instance = 0) const;
    /// @}

    /// Values setter using given tag. Strings are joined using DICOM delimiter '\'.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the vector of value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @{
    DATA_API void setByteValues(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::vector<std::string>& _values,
        std::size_t _instance = 0
    );

    DATA_API void setByteValues(
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
    DATA_API std::string getStringValue(
        std::uint16_t _group,
        std::uint16_t _element,
        std::size_t _instance = 0
    ) const;

    DATA_API std::string getStringValue(dicom::attribute::Keyword _tag, std::size_t _instance = 0) const;
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
    DATA_API void setStringValue(
        std::uint16_t _group,
        std::uint16_t _element,
        const std::string& _value,
        std::size_t _instance = 0
    );

    DATA_API void setStringValue(dicom::attribute::Keyword _tag, const std::string& _value, std::size_t _instance = 0);
    /// @}

    /// Private value getter.
    /// @throws data::exception if tag doesn't exist
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the private value as a string
    DATA_API std::optional<std::string> getPrivateValue(
        std::uint8_t _element,
        std::size_t _instance = 0
    ) const;

    /// Private values setter.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    DATA_API void setPrivateValue(
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
    DATA_API std::optional<std::string> getMultiFramePrivateValue(
        std::uint8_t _element,
        std::size_t _frame_index = 0
    ) const;

    /// Private values setter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::exception if the data mismatch the tag type
    /// @param[in] value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] frameIndex the frame index where to store the private tag.
    DATA_API void setMultiFramePrivateValue(
        const std::optional<std::string>& _value,
        std::uint8_t _element,
        std::size_t _frame_index = 0
    );

    /// Enum that defines the kind of DICOM series we are
    enum class dicom_t : std::uint64_t
    {
        IMAGE     = 1,
        MODEL     = IMAGE << 1,
        REPORT    = MODEL << 1,
        FIDUCIALS = REPORT << 1,
        UNKNOWN   = 0
    };

    /// In case we want to filter series by type
    using DicomTypes = std::underlying_type_t<dicom_t>;

    /// Convenience function to convert from dicom_t enum value to string
    /// @param[in] type the dicom_t enum value to convert
    constexpr static std::string_view dicomTypeToString(dicom_t _type) noexcept
    {
        switch(_type)
        {
            case dicom_t::IMAGE:
                return "image";

            case dicom_t::MODEL:
                return "model";

            case dicom_t::REPORT:
                return "report";

            case dicom_t::FIDUCIALS:
                return "fiducials";

            default:
                return "unknown";
        }
    }

    /// Convenience function to convert from string to dicom_t enum value
    /// @param[in] type the string to convert
    constexpr static dicom_t stringToDicomType(std::string_view _type) noexcept
    {
        if(constexpr auto image = dicomTypeToString(dicom_t::IMAGE); _type == image)
        {
            return dicom_t::IMAGE;
        }
        else if(constexpr auto model = dicomTypeToString(dicom_t::MODEL); _type == model)
        {
            return dicom_t::MODEL;
        }
        else if(constexpr auto report = dicomTypeToString(dicom_t::REPORT); _type == report)
        {
            return dicom_t::REPORT;
        }
        else if(constexpr auto fiducials = dicomTypeToString(dicom_t::FIDUCIALS); _type == fiducials)
        {
            return dicom_t::FIDUCIALS;
        }
        else
        {
            return dicom_t::UNKNOWN;
        }
    }

    /// Convenience function to convert from / to DicomTypes values to string
    /// This may be used to configure reader/writer service
    /// @{
    DATA_API static std::string dicomTypesToString(DicomTypes _types) noexcept;
    DATA_API static DicomTypes stringToDicomTypes(const std::string& _types) noexcept;

    using SopKeywords = std::set<dicom::sop::Keyword>;
    DATA_API static SopKeywords dicomTypesToSops(DicomTypes _types) noexcept;
    DATA_API static DicomTypes sopsToDicomTypes(const SopKeywords& _sops) noexcept;
    DATA_API static SopKeywords stringToSops(const std::string& _sops) noexcept;
    DATA_API static std::string sopsToString(const SopKeywords& _sops) noexcept;
    /// @}

    /// Returns the type of the series. For now, only "Image", "Model" and "Fiducials" are supported.
    DATA_API dicom_t get_dicom_type() const noexcept;
    DATA_API static dicom_t get_dicom_type(const std::string& _sop_class_uid) noexcept;

    /// DataSet getter/setter, needed for serialization.
    /// @{
    DATA_API const gdcm::DataSet& getDataSet(std::size_t _instance                = 0) const;
    DATA_API gdcm::DataSet& getDataSet(std::size_t _instance                      = 0);
    DATA_API void setDataSet(const gdcm::DataSet& _dataset, std::size_t _instance = 0);
    /// @}

    /// This is used when sorting DICOM files
    DATA_API std::filesystem::path get_file(std::size_t _instance                    = 0) const;
    DATA_API void set_file(const std::filesystem::path& _file, std::size_t _instance = 0);
    DATA_API std::size_t numInstances() const noexcept;

    /// Utility function that sort instances according to a sorted map of instance index.
    /// @param[in] sorted the sorted vector of instance.
    DATA_API bool sort(const std::vector<std::size_t>& _sorted);

    /// Returns true if the series is multi-frame.
    DATA_API bool isMultiFrame() const noexcept;

    /// Returns the number of frames in the series.
    DATA_API std::size_t numFrames() const noexcept;

    /// Helper function to convert a DICOM date time string to/from a std::chrono::system_clock::time_point
    DATA_API static std::chrono::system_clock::time_point dateTimeToTimePoint(const std::string& _date_time);

    /// Helper function to convert a std::chrono::system_clock::time_point to /from DICOM date time string
    DATA_API static std::string timePointToDateTime(const std::chrono::system_clock::time_point& _time_point);

    /// Shrink the number of instances / frames to the given size.
    /// This is mainly an optimization and a bugfix when using GDCM to write a multi-frame DICOM file.
    /// @param size
    DATA_API void shrinkFrames(std::size_t _size);

    DATA_API void setFrameLandmarks(std::vector<data::landmarks::sptr> _landmarks, std::size_t _frame_index);
    DATA_API std::vector<data::landmarks::sptr> getFrameLandmarks(std::size_t _frame_index);

protected:

    std::unique_ptr<detail::SeriesImpl> m_pimpl;
};

} // Namespace fwMedData.
