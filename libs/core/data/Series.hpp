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
#include "data/Landmarks.hpp"
#include "data/Matrix4.hpp"
#include "data/Object.hpp"
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
class DATA_CLASS_API Series : public virtual Object
{
public:

    SIGHT_DECLARE_CLASS(Series, Object, factory::New<Series>);

    /// Constructor / Destructor
    /// @{
    DATA_API Series(Object::Key key);
    DATA_API ~Series() noexcept override;
    /// @}

    /// Getter/Setter of DICOM SOP Common Module related attributes
    /// @{
    DATA_API dicom::sop::Keyword getSOPKeyword() const noexcept;
    DATA_API void setSOPKeyword(dicom::sop::Keyword keyword);
    DATA_API std::string_view getSOPClassName() const noexcept;

    DATA_API std::string getSOPInstanceUID() const noexcept;
    DATA_API void setSOPInstanceUID(const std::string& sopInstanceUID);

    DATA_API std::string getSOPClassUID() const noexcept;
    DATA_API void setSOPClassUID(const std::string& sopClassUID);

    DATA_API std::string getSpecificCharacterSet() const noexcept;
    DATA_API void setSpecificCharacterSet(const std::string& specificCharacterSet);

    /// Translate DICOM specific character set to something compatible with iconv/ICU/Qt
    DATA_API std::string getEncoding() const noexcept;
    /// @}

    /// Getter/Setter of DICOM General Series Module related attributes
    /// @{
    DATA_API std::string getSeriesDate() const noexcept;
    DATA_API void setSeriesDate(const std::string& seriesDate);

    DATA_API std::string getSeriesTime() const noexcept;
    DATA_API void setSeriesTime(const std::string& seriesTime);

    DATA_API std::string getModality() const noexcept;
    DATA_API void setModality(const std::string& modality);

    DATA_API std::string getSeriesDescription() const noexcept;
    DATA_API void setSeriesDescription(const std::string& seriesDescription);

    DATA_API std::vector<std::string> getPerformingPhysicianNames() const noexcept;
    DATA_API void setPerformingPhysicianNames(const std::vector<std::string>& performingPhysicianNames);

    DATA_API std::string getPerformingPhysicianName() const noexcept;
    DATA_API void setPerformingPhysicianName(const std::string& performingPhysicianName);

    DATA_API std::string getAnatomicalOrientationType() const noexcept;
    DATA_API void setAnatomicalOrientationType(const std::string& anatomicalOrientationType);

    DATA_API std::string getBodyPartExamined() const noexcept;
    DATA_API void setBodyPartExamined(const std::string& bodyPartExamined);

    DATA_API std::string getProtocolName() const noexcept;
    DATA_API void setProtocolName(const std::string& protocolName);

    DATA_API std::string getPatientPosition() const noexcept;
    DATA_API void setPatientPosition(const std::string& patientPosition);

    /// Returns the patient position as human readable string
    DATA_API std::string getPatientPositionString() const noexcept;

    DATA_API std::string getSeriesInstanceUID() const noexcept;
    DATA_API void setSeriesInstanceUID(const std::string& seriesInstanceUID);

    DATA_API std::optional<std::int32_t> getSeriesNumber() const noexcept;
    DATA_API void setSeriesNumber(const std::optional<std::int32_t>& seriesNumber = std::nullopt);

    DATA_API std::string getLaterality() const noexcept;
    DATA_API void setLaterality(const std::string& laterality);

    DATA_API std::string getPerformedProcedureStepStartDate() const noexcept;
    DATA_API void setPerformedProcedureStepStartDate(const std::string& performedProcedureStepStartDate);

    DATA_API std::string getPerformedProcedureStepStartTime() const noexcept;
    DATA_API void setPerformedProcedureStepStartTime(const std::string& performedProcedureStepStartTime);

    DATA_API std::string getPerformedProcedureStepEndDate() const noexcept;
    DATA_API void setPerformedProcedureStepEndDate(const std::string& performedProcedureStepEndDate);

    DATA_API std::string getPerformedProcedureStepEndTime() const noexcept;
    DATA_API void setPerformedProcedureStepEndTime(const std::string& performedProcedureStepEndTime);

    DATA_API std::string getPerformedProcedureStepID() const noexcept;
    DATA_API void setPerformedProcedureStepID(const std::string& performedProcedureStepID);

    DATA_API std::string getPerformedProcedureStepDescription() const noexcept;
    DATA_API void setPerformedProcedureStepDescription(const std::string& performedProcedureStepDescription);

    DATA_API std::string getCommentsOnThePerformedProcedureStep() const noexcept;
    DATA_API void setCommentsOnThePerformedProcedureStep(const std::string& commentsOnThePerformedProcedureStep);
    /// @}

    /// Getter/Setter of DICOM General Equipment Module related attributes
    /// @{
    DATA_API std::string getInstitutionName() const noexcept;
    DATA_API void setInstitutionName(const std::string& institutionName);
    /// @}

    /// Getter/Setter of DICOM Patient Module related attributes
    /// @{
    DATA_API std::string getPatientName() const noexcept;
    DATA_API void setPatientName(const std::string& patientName);

    DATA_API std::string getPatientID() const noexcept;
    DATA_API void setPatientID(const std::string& patientID);

    DATA_API std::string getPatientBirthDate() const noexcept;
    DATA_API void setPatientBirthDate(const std::string& patientBirthDate);

    DATA_API std::string getPatientSex() const noexcept;
    DATA_API void setPatientSex(const std::string& patientSex);
    /// @}

    /// Getter/Setter of DICOM General Study Module related attributes
    /// @{
    DATA_API std::string getStudyDate() const noexcept;
    DATA_API void setStudyDate(const std::string& studyDate);

    DATA_API std::string getStudyTime() const noexcept;
    DATA_API void setStudyTime(const std::string& studyTime);

    DATA_API std::string getReferringPhysicianName() const noexcept;
    DATA_API void setReferringPhysicianName(const std::string& referringPhysicianName);

    DATA_API std::string getStudyDescription() const noexcept;
    DATA_API void setStudyDescription(const std::string& studyDescription);

    DATA_API std::string getStudyInstanceUID() const noexcept;
    DATA_API void setStudyInstanceUID(const std::string& studyInstanceUID);

    DATA_API std::string getStudyID() const noexcept;
    DATA_API void setStudyID(const std::string& studyID);
    /// @}

    /// Getter/Setter of DICOM Patient Study Module related attributes
    /// @{
    DATA_API std::string getPatientAge() const noexcept;
    DATA_API void setPatientAge(const std::string& patientAge);

    DATA_API std::optional<double> getPatientSize() const noexcept;
    DATA_API void setPatientSize(const std::optional<double>& patientSize = std::nullopt);

    DATA_API std::optional<double> getPatientWeight() const noexcept;
    DATA_API void setPatientWeight(const std::optional<double>& patientWeight = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM General Image Module related attributes
    /// @{
    DATA_API std::string getAcquisitionDate(std::size_t instance                              = 0) const;
    DATA_API void setAcquisitionDate(const std::string& acquisitionDate, std::size_t instance = 0);

    DATA_API std::string getAcquisitionTime(std::size_t instance                              = 0) const;
    DATA_API void setAcquisitionTime(const std::string& acquisitionTime, std::size_t instance = 0);

    DATA_API std::string getContentTime(std::size_t instance                          = 0) const;
    DATA_API void setContentTime(const std::string& contentTime, std::size_t instance = 0);

    DATA_API std::optional<std::int32_t> getInstanceNumber(std::size_t instance = 0) const;

    DATA_API void setInstanceNumber(
        const std::optional<std::int32_t>& instanceNumber = std::nullopt,
        std::size_t instance                              = 0
    );

    DATA_API std::optional<std::int32_t> getAcquisitionNumber(
        std::size_t instance = 0
    ) const;

    DATA_API void setAcquisitionNumber(
        std::optional<std::int32_t> acquisitionNumber = std::nullopt,
        std::size_t instance                          = 0
    );
    /// @}

    /// Getter/Setter of DICOM Contrast/Bolus Module related attributes
    /// @{
    DATA_API std::string getContrastBolusAgent() const noexcept;
    DATA_API void setContrastBolusAgent(const std::string& contrastBolusAgent);

    DATA_API std::string getContrastBolusRoute() const noexcept;
    DATA_API void setContrastBolusRoute(const std::string& contrastBolusRoute);

    DATA_API std::optional<double> getContrastBolusVolume() const noexcept;
    DATA_API void setContrastBolusVolume(const std::optional<double>& contrastBolusVolume = std::nullopt);

    DATA_API std::string getContrastBolusStartTime() const noexcept;
    DATA_API void setContrastBolusStartTime(const std::string& contrastBolusStartTime);

    DATA_API std::string getContrastBolusStopTime() const noexcept;
    DATA_API void setContrastBolusStopTime(const std::string& contrastBolusStopTime);

    DATA_API std::optional<double> getContrastBolusTotalDose() const noexcept;
    DATA_API void setContrastBolusTotalDose(const std::optional<double>& contrastBolusTotalDose = std::nullopt);

    DATA_API std::vector<double> getContrastFlowRates() const noexcept;
    DATA_API void setContrastFlowRates(const std::vector<double>& contrastFlowRates);

    DATA_API std::string getContrastFlowRate() const noexcept;
    DATA_API void setContrastFlowRate(const std::string& contrastFlowRates);

    DATA_API std::vector<double> getContrastFlowDurations() const noexcept;
    DATA_API void setContrastFlowDurations(const std::vector<double>& contrastFlowDurations);

    DATA_API std::string getContrastFlowDuration() const noexcept;
    DATA_API void setContrastFlowDuration(const std::string& contrastFlowDurations);

    DATA_API std::string getContrastBolusIngredient() const noexcept;
    DATA_API void setContrastBolusIngredient(const std::string& contrastBolusIngredient);

    DATA_API std::optional<double> getContrastBolusIngredientConcentration() const noexcept;
    DATA_API void setContrastBolusIngredientConcentration(
        const std::optional<double>& contrastBolusIngredientConcentration = std::nullopt
    );

    /// Getter/Setter of DICOM Image Pixel Module
    /// @{
    DATA_API std::optional<std::uint16_t> getRows() const noexcept;
    DATA_API virtual void setRows(const std::optional<std::uint16_t>& rows = std::nullopt);

    DATA_API std::optional<std::uint16_t> getColumns() const noexcept;
    DATA_API virtual void setColumns(const std::optional<std::uint16_t>& columns = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    DATA_API virtual std::vector<double> getWindowCenter() const noexcept;
    DATA_API virtual void setWindowCenter(const std::vector<double>& windowCenters);

    DATA_API virtual std::vector<double> getWindowWidth() const noexcept;
    DATA_API virtual void setWindowWidth(const std::vector<double>& windowWidths);
    /// @}

    /// Getter/Setter of DICOM Modality LUT Module
    /// @{
    DATA_API std::optional<double> getRescaleIntercept(std::size_t instance = 0) const noexcept;
    DATA_API void setRescaleIntercept(
        const std::optional<double>& rescaleIntercept = std::nullopt,
        std::size_t instance                          = 0
    );

    DATA_API std::optional<double> getRescaleSlope(std::size_t instance = 0) const noexcept;
    DATA_API void setRescaleSlope(
        const std::optional<double>& rescaleSlope = std::nullopt,
        std::size_t instance                      = 0
    );
    /// @}

    /// Getter/Setter of DICOM Image Plane Module related attributes
    /// ...and Multi-frame Functional Groups Module
    /// @{
    DATA_API std::vector<double> getImagePositionPatient(std::size_t instance = 0) const;

    DATA_API void setImagePositionPatient(
        const std::vector<double>& imagePositionPatient,
        std::size_t instance = 0
    );

    DATA_API std::vector<double> getImageOrientationPatient(std::size_t instance = 0) const;

    DATA_API void setImageOrientationPatient(
        const std::vector<double>& imageOrientationPatient,
        std::size_t instance = 0
    );

    DATA_API sight::data::Matrix4 getImageTransformPatient(std::size_t instance                        = 0) const;
    DATA_API void setImageTransformPatient(const sight::data::Matrix4& transform, std::size_t instance = 0);

    DATA_API std::optional<double> getSliceThickness() const noexcept;
    DATA_API void setSliceThickness(const std::optional<double>& sliceThickness = std::nullopt);
    /// @}

    /// Getter/Setter of DICOM Multi-frame Functional Groups Module related attributes
    /// @{
    DATA_API std::vector<double> getImagePositionVolume(std::size_t frameIndex = 0) const;

    DATA_API void setImagePositionVolume(
        const std::vector<double>& imagePositionVolume,
        std::size_t frameIndex = 0
    );

    DATA_API std::vector<double> getImageOrientationVolume(std::size_t frameIndex = 0) const;

    DATA_API void setImageOrientationVolume(
        const std::vector<double>& imageOrientationVolume,
        std::size_t frameIndex = 0
    );

    DATA_API std::optional<std::string> getFrameAcquisitionDateTime(std::size_t frameIndex = 0) const;

    DATA_API void setFrameAcquisitionDateTime(
        const std::optional<std::string>& frameAcquisitionDateTime = std::nullopt,
        std::size_t frameIndex                                     = 0
    );

    DATA_API std::optional<std::chrono::system_clock::time_point> getFrameAcquisitionTimePoint(
        std::size_t frameIndex = 0
    ) const noexcept;

    DATA_API void setFrameAcquisitionTimePoint(
        const std::optional<std::chrono::system_clock::time_point>& timePoint = std::nullopt,
        std::size_t frameIndex                                                = 0
    );

    DATA_API void setFrameAcquisitionTimePoint(
        sight::core::HiResClock::HiResClockType timePoint,
        std::size_t frameIndex = 0
    );

    DATA_API std::optional<std::string> getFrameComments(std::size_t frameIndex = 0) const;

    DATA_API void setFrameComments(
        const std::optional<std::string>& frameComments = std::nullopt,
        std::size_t frameIndex                          = 0
    );

    DATA_API std::optional<std::string> getFrameLabel(std::size_t frameIndex = 0) const;

    DATA_API void setFrameLabel(
        const std::optional<std::string>& frameLabel = std::nullopt,
        std::size_t frameIndex                       = 0
    );

    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Series& other) const noexcept;
    DATA_API bool operator!=(const Series& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

    /// Defines partial copy. Useful to share data between different series
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy from
    /// @{
    DATA_API void copyPatientModule(const Series::csptr& source, std::size_t instance              = 0);
    DATA_API void copyClinicalTrialSubjectModule(const Series::csptr& source, std::size_t instance = 0);
    DATA_API void copyGeneralStudyModule(const Series::csptr& source, std::size_t instance         = 0);
    DATA_API void copyPatientStudyModule(const Series::csptr& source, std::size_t instance         = 0);
    DATA_API void copyClinicalTrialStudyModule(const Series::csptr& source, std::size_t instance   = 0);
    DATA_API void copyGeneralSeriesModule(const Series::csptr& source, std::size_t instance        = 0);
    DATA_API void copyClinicalTrialSeriesModule(const Series::csptr& source, std::size_t instance  = 0);
    DATA_API void copyGeneralEquipmentModule(const Series::csptr& source, std::size_t instance     = 0);
    DATA_API void copyFrameOfReferenceModule(const Series::csptr& source, std::size_t instance     = 0);
    DATA_API void copySOPCommonModule(const Series::csptr& source, std::size_t instance            = 0);
    DATA_API void copyGeneralImageModule(const Series::csptr& source, std::size_t instance         = 0);
    /// @}

    /// Value getter using given tag. The returned string is used as a Byte buffer and is not necessarily a string.
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    DATA_API std::string getByteValue(std::uint16_t group, std::uint16_t element, std::size_t instance = 0) const;
    DATA_API std::string getByteValue(dicom::attribute::Keyword tag, std::size_t instance              = 0) const;
    /// @}

    /// Value setter using given tag. The string argument is used as a Byte buffer and is not necessarily a string.
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @throw data::Exception if tag doesn't exist
    /// @throw data::Exception if value size is not correct
    /// @throw data::Exception if instance index is out of bounds
    /// @{
    DATA_API void setByteValue(
        std::uint16_t group,
        std::uint16_t element,
        const std::string& value,
        std::size_t instance = 0
    );

    DATA_API void setByteValue(
        dicom::attribute::Keyword tag,
        const std::string& value,
        std::size_t instance = 0
    );

    /// @}

    /// Values getter using given tag. Initial string is split using DICOM delimiter '\'.
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a vector of strings
    /// @{
    DATA_API std::vector<std::string> getByteValues(
        std::uint16_t group,
        std::uint16_t element,
        std::size_t instance = 0
    ) const;

    DATA_API std::vector<std::string> getByteValues(dicom::attribute::Keyword tag, std::size_t instance = 0) const;
    /// @}

    /// Values setter using given tag. Strings are joined using DICOM delimiter '\'.
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the vector of value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @{
    DATA_API void setByteValues(
        std::uint16_t group,
        std::uint16_t element,
        const std::vector<std::string>& values,
        std::size_t instance = 0
    );

    DATA_API void setByteValues(
        dicom::attribute::Keyword tag,
        const std::vector<std::string>& values,
        std::size_t instance = 0
    );
    /// @}

    /// Value getter using given tag. The value is converted to string, depending of the VR.
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the values as a string
    /// @{
    DATA_API std::string getStringValue(
        std::uint16_t group,
        std::uint16_t element,
        std::size_t instance = 0
    ) const;

    DATA_API std::string getStringValue(dicom::attribute::Keyword tag, std::size_t instance = 0) const;
    /// @}

    /// Value setter using given tag. The string argument is converted to the right, depending of the VR.
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the value to insert
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @throw data::Exception if tag doesn't exist
    /// @throw data::Exception if value size is not correct
    /// @throw data::Exception if instance index is out of bounds
    /// @{
    DATA_API void setStringValue(
        std::uint16_t group,
        std::uint16_t element,
        const std::string& value,
        std::size_t instance = 0
    );

    DATA_API void setStringValue(dicom::attribute::Keyword tag, const std::string& value, std::size_t instance = 0);
    /// @}

    /// Private value getter.
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    /// @return the private value as a string
    DATA_API std::optional<std::string> getPrivateValue(
        std::uint8_t element,
        std::size_t instance = 0
    ) const;

    /// Private values setter.
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] instance the instance index in case multi-frame is not supported by the current IOD.
    ///                     (nullopt means the global common instance, for attributes shared by all instance.)
    DATA_API void setPrivateValue(
        const std::optional<std::string>& value,
        std::uint8_t element,
        std::size_t instance = 0
    );

    /// Private value getter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] frameIndex the frame index where to store the private tag.
    /// @return the private value as a string
    DATA_API std::optional<std::string> getMultiFramePrivateValue(
        std::uint8_t element,
        std::size_t frameIndex = 0
    ) const;

    /// Private values setter for a DICOM Multi-frame Functional Groups Module.
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] value the string to insert. If empty (std::nullopt), the private tag is removed.
    /// @param[in] element private sequence element number in the range of 0x10 to 0xFF.
    ///                    The corresponding attribute will take element+0x01 as private element number.
    /// @param[in] frameIndex the frame index where to store the private tag.
    DATA_API void setMultiFramePrivateValue(
        const std::optional<std::string>& value,
        std::uint8_t element,
        std::size_t frameIndex = 0
    );

    /// Enum that defines the kind of DICOM Series we are
    enum class DicomType : std::uint64_t
    {
        IMAGE     = 1,
        MODEL     = IMAGE << 1,
        REPORT    = MODEL << 1,
        FIDUCIALS = REPORT << 1,
        UNKNOWN   = 0
    };

    /// In case we want to filter Series by type
    using DicomTypes = std::underlying_type_t<DicomType>;

    /// Convenience function to convert from DicomType enum value to string
    /// @param[in] type the DicomType enum value to convert
    constexpr static std::string_view dicomTypeToString(DicomType type) noexcept
    {
        switch(type)
        {
            case DicomType::IMAGE:
                return "image";

            case DicomType::MODEL:
                return "model";

            case DicomType::REPORT:
                return "report";

            case DicomType::FIDUCIALS:
                return "fiducials";

            default:
                return "unknown";
        }
    }

    /// Convenience function to convert from string to DicomType enum value
    /// @param[in] type the string to convert
    constexpr static DicomType stringToDicomType(std::string_view type) noexcept
    {
        if(constexpr auto IMAGE = dicomTypeToString(DicomType::IMAGE); type == IMAGE)
        {
            return DicomType::IMAGE;
        }
        else if(constexpr auto MODEL = dicomTypeToString(DicomType::MODEL); type == MODEL)
        {
            return DicomType::MODEL;
        }
        else if(constexpr auto REPORT = dicomTypeToString(DicomType::REPORT); type == REPORT)
        {
            return DicomType::REPORT;
        }
        else if(constexpr auto FIDUCIALS = dicomTypeToString(DicomType::FIDUCIALS); type == FIDUCIALS)
        {
            return DicomType::FIDUCIALS;
        }
        else
        {
            return DicomType::UNKNOWN;
        }
    }

    /// Convenience function to convert from / to DicomTypes values to string
    DATA_API static std::string dicomTypesToString(DicomTypes types) noexcept;
    DATA_API static DicomTypes stringToDicomTypes(const std::string& types) noexcept;

    /// Returns the type of the Series. For now, only "Image" and "Model" are supported.
    DATA_API DicomType getDicomType() const noexcept;
    DATA_API static DicomType getDicomType(const std::string& SOPClassUID) noexcept;

    /// DataSet getter/setter, needed for serialization.
    /// @{
    DATA_API const gdcm::DataSet& getDataSet(std::size_t instance               = 0) const;
    DATA_API gdcm::DataSet& getDataSet(std::size_t instance                     = 0);
    DATA_API void setDataSet(const gdcm::DataSet& dataset, std::size_t instance = 0);
    /// @}

    /// This is used when sorting DICOM files
    DATA_API std::filesystem::path getFile(std::size_t instance                   = 0) const;
    DATA_API void setFile(const std::filesystem::path& file, std::size_t instance = 0);
    DATA_API std::size_t numInstances() const noexcept;

    /// Utility function that sort instances according to a sorted map of instance index.
    /// @param[in] sorted the sorted vector of instance.
    DATA_API bool sort(const std::vector<std::size_t>& sorted);

    /// Returns true if the series is multi-frame.
    DATA_API bool isMultiFrame() const noexcept;

    /// Returns the number of frames in the series.
    DATA_API std::size_t numFrames() const noexcept;

    /// Helper function to convert a DICOM date time string to/from a std::chrono::system_clock::time_point
    DATA_API static std::chrono::system_clock::time_point dateTimeToTimePoint(const std::string& dateTime);

    /// Helper function to convert a std::chrono::system_clock::time_point to /from DICOM date time string
    DATA_API static std::string timePointToDateTime(const std::chrono::system_clock::time_point& timePoint);

    /// Shrink the number of instances / frames to the given size.
    /// This is mainly an optimization and a bugfix when using GDCM to write a multi-frame DICOM file.
    /// @param size
    DATA_API void shrinkFrames(std::size_t size);

    DATA_API void setFrameLandmarks(std::vector<data::Landmarks::sptr> landmarks, std::size_t frameIndex);
    DATA_API std::vector<data::Landmarks::sptr> getFrameLandmarks(std::size_t frameIndex);

protected:

    std::unique_ptr<detail::SeriesImpl> m_pimpl;
};

} // Namespace fwMedData.
