/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>
#include <fwData/Object.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(Series))

namespace fwMedData
{

class Patient;
class Study;
class Equipment;

/**
 * @brief Holds series information.
 */
class FWMEDDATA_CLASS_API Series : public ::fwData::Object
{

public:
    fwCoreClassMacro(Series, ::fwData::Object)

    fwCampMakeFriendDataMacro((fwMedData)(Series))

    /**
     * @brief Creates the series.
     * @param _key private construction key.
     */
    FWMEDDATA_API Series(::fwData::Object::Key _key);

    /// Destroys the series.
    FWMEDDATA_API virtual ~Series();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets the referring patient.
    SPTR(::fwMedData::Patient) getPatient() const;

    /// Sets the referring patient.
    void setPatient(const SPTR(::fwMedData::Patient)& _val);

    /// Gets the referring study.
    SPTR(::fwMedData::Study) getStudy() const;

    /// Sets the referring study.
    void setStudy(const SPTR(::fwMedData::Study)& _val);

    /// Gets the related equipment.
    SPTR(::fwMedData::Equipment) getEquipment() const;

    /// Sets the related equipment.
    void setEquipment(const SPTR(::fwMedData::Equipment)& _val);

    /// Gets the type of equipment that originally acquired the data used to create this series.
    const DicomValueType& getModality () const;

    /// Sets the type of equipment that originally acquired the data used to create this series.
    void setModality (const DicomValueType& _val);

    /// Gets the unique identifier of the series.
    const DicomValueType& getInstanceUID () const;

    /// Sets the unique identifier of the series.
    void setInstanceUID (const DicomValueType& _val);

    // Gets the number that identify this series.
    DicomValueType getNumber() const;

    /// Sets the number that identify this series.
    void setNumber(const DicomValueType& _val);

    /// Gets the laterality of body part examined.
    DicomValueType getLaterality() const;

    /// Sets the laterality of body part examined.
    void setLaterality(const DicomValueType& _val);

    /// Gets the date the series started.
    const DicomValueType& getDate () const;

    /// Sets the date the series started.
    void setDate (const DicomValueType& _val);

    /// Gets the time the series started.
    const DicomValueType& getTime () const;

    /// Sets the time the series started.
    void setTime (const DicomValueType& _val);

    /// Gets the name of the physician(s) administering the series.
    const DicomValuesType& getPerformingPhysiciansName() const;

    /// Sets the name of the physician(s) administering the series.
    void setPerformingPhysiciansName(const DicomValuesType& _val);

    /// Gets the description of the conditions under which the Series was performed.
    const DicomValueType& getProtocolName() const;

    /// Sets description of the conditions under which the Series was performed.
    void setProtocolName(const DicomValueType& _val);

    /// Gets the description of the series.
    const DicomValueType& getDescription() const;

    /// Sets the description of the series.
    void setDescription(const DicomValueType& _val);

    /// Gets the body part examined.
    const DicomValueType& getBodyPartExamined() const;

    /// Sets the body part examined.
    void setBodyPartExamined(const DicomValueType& _val);

    /// Gets the patient position descriptor.
    const DicomValueType& getPatientPosition() const;

    /// Sets the patient position descriptor.
    void setPatientPosition(const DicomValueType& _val);

    /// Gets the anatomical orientation type.
    const DicomValueType& getAnatomicalOrientationType() const;

    /// Sets the anatomical orientation type.
    void setAnatomicalOrientationType(const DicomValueType& _val);

    /// Gets the user or equipment generated identifier.
    const DicomValueType& getPerformedProcedureStepID() const;

    /// Sets the user or equipment generated identifier.
    void setPerformedProcedureStepID(const DicomValueType& _val);

    /// Gets the date on which the performed procedure step started.
    const DicomValueType& getPerformedProcedureStepStartDate() const;

    /// Sets the date on which the performed procedure step started.
    void setPerformedProcedureStepStartDate(const DicomValueType& _val);

    /// Gets the time on which the performed procedure step started.
    const DicomValueType& getPerformedProcedureStepStartTime() const;

    /// Sets the time on which the performed procedure step started.
    void setPerformedProcedureStepStartTime(const DicomValueType& _val);

    /// Gets the date on which the performed procedure step end.
    const DicomValueType& getPerformedProcedureStepEndDate() const;

    /// Sets the date on which the performed procedure step end.
    void setPerformedProcedureStepEndDate(const DicomValueType& _val);

    /// Gets the time on which the performed procedure step end.
    const DicomValueType& getPerformedProcedureStepEndTime() const;

    /// Sets the time on which the performed procedure step end.
    void setPerformedProcedureStepEndTime(const DicomValueType& _val);

    /// Gets the institution-generated description or classification of the procedure step that was performed.
    const DicomValueType& getPerformedProcedureStepDescription() const;

    /// Sets the institution-generated description or classification of the procedure step that was performed.
    void setPerformedProcedureStepDescription(const DicomValueType& _val);

    /// Gets the user-defined comments on the performed procedure step.
    const DicomValueType& getPerformedProcedureComments() const;

    /// Sets the user-defined comments on the performed procedure step.
    void setPerformedProcedureComments(const DicomValueType& _va);

protected:

    /// Stores the referring patient.
    SPTR(Patient) m_patient;

    /// Stores the referring study.
    SPTR(Study) m_study;

    /// Stores the related equipment.
    SPTR(Equipment) m_equipment;

    /// Defines the modality.
    DicomValueType m_modality;

    /// Defines the series unique identifier.
    DicomValueType m_instanceUID;

    /// Defines the series number.
    DicomValueType m_number;

    /// Defines the lateriality of body part examined (L, R).
    DicomValueType m_laterality;

    /// Defines the date.
    DicomValueType m_date;

    /// Defines the time.
    DicomValueType m_time;

    /// Defines performing physicians name.
    DicomValuesType m_performingPhysiciansName;

    /// Defines the description of the conditions under which the Series was performed.
    DicomValueType m_protocolName;

    /// Defines the description.
    DicomValueType m_description;

    /// Defines the description of the part of the body examined.
    DicomValueType m_bodyPartExamined;

    /// Defines the patient position descriptor.
    DicomValueType m_patientPosition;

    /// Defines the anatomical orientation type (BIPED, QUADRUPED).
    DicomValueType m_anatomicalOrientationType;

    /// Defines the user or equipment generated identifier.
    DicomValueType m_performdedProcedureStepID;

    /// Defines the date on which the performed procedure step started.
    DicomValueType m_performedProcedureStepStartDate;

    /// Defines the time on which the performed procedure step started.
    DicomValueType m_performedProcedureStepStartTime;

    /// Defines the date on which the performed procedure step end.
    DicomValueType m_performedProcedureStepEndDate;

    /// Defines the time on which the performed procedure step end.
    DicomValueType m_performedProcedureStepEndTime;

    /// Defines the institution-generated description or classification of the procedure step that was performed.
    DicomValueType m_performedProcedureStepDescription;

    /// Defines the user-defined comments on the performed procedure step.
    DicomValueType m_performedProcedureComments;

};

//-----------------------------------------------------------------------------

inline SPTR(::fwMedData::Patient) Series::getPatient() const
{
    return m_patient;
}

//-----------------------------------------------------------------------------

inline void Series::setPatient(const SPTR(::fwMedData::Patient)& _val)
{
    m_patient = _val;
}

//-----------------------------------------------------------------------------

inline SPTR(::fwMedData::Study) Series::getStudy() const
{
    return m_study;
}

//-----------------------------------------------------------------------------

inline void Series::setStudy(const SPTR(::fwMedData::Study)& _val)
{
    m_study = _val;
}

//-----------------------------------------------------------------------------

inline SPTR(::fwMedData::Equipment) Series::getEquipment() const
{
    return m_equipment;
}

//-----------------------------------------------------------------------------

inline void Series::setEquipment(const SPTR(::fwMedData::Equipment)& _val)
{
    m_equipment = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getModality() const
{
    return m_modality;
}

//-----------------------------------------------------------------------------

inline void Series::setModality(const DicomValueType& _val)
{
    m_modality = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getInstanceUID () const
{
    return m_instanceUID;
}

//-----------------------------------------------------------------------------

inline void Series::setInstanceUID(const DicomValueType& _val)
{
    m_instanceUID = _val;
}

//-----------------------------------------------------------------------------

inline DicomValueType Series::getNumber() const
{
    return m_number;
}

//-----------------------------------------------------------------------------

inline void Series::setNumber(const DicomValueType& _val)
{
    m_number = _val;
}

//-----------------------------------------------------------------------------

inline DicomValueType Series::getLaterality() const
{
    return m_laterality;
}

//-----------------------------------------------------------------------------

inline void Series::setLaterality(const DicomValueType& _val)
{
    m_laterality = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getDate() const
{
    return m_date;
}

//-----------------------------------------------------------------------------

inline void Series::setDate(const DicomValueType& _val)
{
    m_date = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getTime() const
{
    return m_time;
}

//-----------------------------------------------------------------------------

inline void Series::setTime(const DicomValueType& _val)
{
    m_time = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValuesType& Series::getPerformingPhysiciansName() const
{
    return m_performingPhysiciansName;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformingPhysiciansName(const DicomValuesType& _val)
{
    m_performingPhysiciansName = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getProtocolName() const
{
    return m_protocolName;
}

//-----------------------------------------------------------------------------

inline void Series::setProtocolName(const DicomValueType& _val)
{
    m_protocolName = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getDescription() const
{
    return m_description;
}

//-----------------------------------------------------------------------------

inline void Series::setDescription(const DicomValueType& _val)
{
    m_description = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getBodyPartExamined() const
{
    return m_bodyPartExamined;
}

//-----------------------------------------------------------------------------

inline void Series::setBodyPartExamined(const DicomValueType& _val)
{
    m_bodyPartExamined = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPatientPosition() const
{
    return m_patientPosition;
}

//-----------------------------------------------------------------------------

inline void Series::setPatientPosition(const DicomValueType& _val)
{
    m_patientPosition = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getAnatomicalOrientationType() const
{
    return m_anatomicalOrientationType;
}

//-----------------------------------------------------------------------------

inline void Series::setAnatomicalOrientationType(const DicomValueType& _val)
{
    m_anatomicalOrientationType = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureStepID() const
{
    return m_performdedProcedureStepID;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureStepID(const DicomValueType& _val)
{
    m_performdedProcedureStepID = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureStepStartDate() const
{
    return m_performedProcedureStepStartDate;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureStepStartDate(const DicomValueType& _val)
{
    m_performedProcedureStepStartDate = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureStepStartTime() const
{
    return m_performedProcedureStepStartTime;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureStepStartTime(const DicomValueType& _val)
{
    m_performedProcedureStepStartTime = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureStepEndDate() const
{
    return m_performedProcedureStepEndDate;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureStepEndDate(const DicomValueType& _val)
{
    m_performedProcedureStepEndDate = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureStepEndTime() const
{
    return m_performedProcedureStepEndTime;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureStepEndTime(const DicomValueType& _val)
{
    m_performedProcedureStepEndTime = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureStepDescription() const
{
    return m_performedProcedureStepDescription;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureStepDescription(const DicomValueType& _val)
{
    m_performedProcedureStepDescription = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getPerformedProcedureComments() const
{
    return m_performedProcedureComments;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformedProcedureComments(const DicomValueType& _val)
{
    m_performedProcedureComments = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
