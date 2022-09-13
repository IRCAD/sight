/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/factory/new.hpp"
#include "data/Object.hpp"
#include "data/types.hpp"

#include <functional>
#include <optional>

namespace gdcm
{

class DataSet;

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
    DATA_API Series(Object::Key _key);
    DATA_API ~Series() noexcept override;
    /// @}

    /// Getter/Setter of DICOM SOP Common Module related attributes
    /// @{
    DATA_API std::string getSOPClassUID() const noexcept;
    DATA_API void setSOPClassUID(const std::string& sopClassUID);

    DATA_API std::string getSOPInstanceUID() const noexcept;
    DATA_API void setSOPInstanceUID(const std::string& sopInstanceUID);
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
    DATA_API void copyPatientModule(const Series::csptr& source);
    DATA_API void copyClinicalTrialSubjectModule(const Series::csptr& source);
    DATA_API void copyGeneralStudyModule(const Series::csptr& source);
    DATA_API void copyPatientStudyModule(const Series::csptr& source);
    DATA_API void copyClinicalTrialStudyModule(const Series::csptr& source);
    DATA_API void copyGeneralSeriesModule(const Series::csptr& source);
    DATA_API void copyClinicalTrialSeriesModule(const Series::csptr& source);
    DATA_API void copyGeneralEquipmentModule(const Series::csptr& source);
    DATA_API void copyFrameOfReferenceModule(const Series::csptr& source);
    DATA_API void copySOPCommonModule(const Series::csptr& source);
    DATA_API void copyGeneralImageModule(const Series::csptr& source);
    /// @}

    /// Value getter using given tag
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @return the values as a string
    DATA_API std::string getByteValue(std::uint16_t group, std::uint16_t element) const noexcept;

    /// Value setter using given tag
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the value to insert
    DATA_API void setByteValue(std::uint16_t group, std::uint16_t element, const std::string& value);

    /// Values getter using given tag. Initial string is split using DICOM delimiter '\'.
    /// @throws data::Exception if tag doesn't exist
    /// @param[in] group the group tag to get
    /// @param[in] element the element tag to get
    /// @return the values as a vector of strings
    DATA_API std::vector<std::string> getByteValues(std::uint16_t group, std::uint16_t element) const noexcept;

    /// Values setter using given tag. Strings are joined using DICOM delimiter '\'.
    /// @throws data::Exception if the data mismatch the tag type
    /// @param[in] group the group tag to set
    /// @param[in] element the element tag to set
    /// @param[in] value the vector of value to insert
    DATA_API void setByteValues(std::uint16_t group, std::uint16_t element, const std::vector<std::string>& values);

    /// DataSet getter/setter
    /// @{
    DATA_API const gdcm::DataSet& getDataSet() const noexcept;
    DATA_API gdcm::DataSet& getDataSet() noexcept;
    DATA_API void setDataSet(const gdcm::DataSet& dataset) noexcept;
    /// @}

protected:

    class SeriesImpl;
    std::unique_ptr<SeriesImpl> m_pimpl;
};

} // Namespace fwMedData.
