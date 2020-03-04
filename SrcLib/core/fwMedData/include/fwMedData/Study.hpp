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

fwCampAutoDeclareDataMacro((fwMedData)(Study), FWMEDDATA_API)

namespace fwMedData
{

/**
 * @brief Holds a study information.
 */
class FWMEDDATA_CLASS_API Study : public ::fwData::Object
{

public:

    fwCoreClassMacro(Study, ::fwData::Object, ::fwData::factory::New< Study >)

    fwCampMakeFriendDataMacro((fwMedData)(Study))

    /**
     * @brief Creates the study.
     * @param _key private construction key.
     */
    FWMEDDATA_API Study(::fwData::Object::Key _key);

    /// Destroys the study.
    FWMEDDATA_API virtual ~Study();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object where find data.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object where find data.
     * @param _cache
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @brief Gets the unique identifier of the study.
     * @return The unique identifier of the study.
     */
    const DicomValueType& getInstanceUID() const;

    /**
     * @brief Sets the unique identifier of the study.
     * @param _val the unique identifier of the study.
     */
    void setInstanceUID(const DicomValueType& _val);

    /**
     * @brief Gets the user or equipment generated study identifier.
     * @return The identifier.
     */
    const DicomValueType& getStudyID() const;

    /**
     * @brief Sets the user or equipment generated study identifier.
     * @param _val the identifier.
     */
    void setStudyID(const DicomValueType& _val);

    /**
     * @brief Gets the date the study started.
     * @return The date the study started.
     */
    const DicomValueType& getDate() const;

    /**
     * @brief Sets the date the study started.
     * @param _val the date the study started.
     */
    void setDate(const DicomValueType& _val);

    /**
     * @brief Gets the time the study started.
     * @return The time the study started.
     */
    const DicomValueType& getTime() const;

    /**
     * @brief Sets the time the study started.
     * @param _val the time the study started.
     */
    void setTime(const DicomValueType& _val);

    /**
     * @brief Gets the name of the patient's referring physician.
     * @return The name of the patient's referring physician.
     */
    const DicomValueType& getReferringPhysicianName() const;

    /**
     * @brief Sets the name of the patient's referring physician.
     * @param _val the name of the patient's referring physician.
     */
    void setReferringPhysicianName(const DicomValueType& _val);

    /**
     * @brief Gets the name of the patient's consulting physician.
     * @return The name of the patient's consulting physician.
     */
    const DicomValueType& getConsultingPhysicianName() const;

    /**
     * @brief Sets the name of the patient's consulting physician.
     * @param _val the name of the patient's consulting physician.
     */
    void setConsultingPhysicianName(const DicomValueType& _val);

    /**
     * @brief Gets the institution-generated description or classification of the study performed.
     * @return The institution-generated description or classification of the study performed.
     */
    const DicomValueType& getDescription() const;

    /**
     * @brief Sets the institution-generated description or classification of the study performed.
     * @param _val the institution-generated description or classification of the study performed.
     */
    void setDescription(const DicomValueType& _val);

    /**
     * @brief Gets the age of the Patient.
     * @return The age of the Patient.
     */
    const DicomValueType& getPatientAge() const;

    /**
     * @brief Sets the age of the Patient.
     * @param _val the age of the Patient.
     */
    void setPatientAge(const DicomValueType& _val);

    /**
     * @brief Gets the size of the Patient.
     * @return The size of the Patient.
     */
    const DicomValueType& getPatientSize() const;

    /**
     * @brief Sets the size of the Patient.
     * @param _val the size of the Patient.
     */
    void setPatientSize(const DicomValueType& _val);

    /**
     * @brief Gets the weight of the Patient.
     * @return The weight of the Patient.
     */
    const DicomValueType& getPatientWeight() const;

    /**
     * @brief Sets the weight of the Patient.
     * @param _val the weight of the Patient.
     */
    void setPatientWeight(const DicomValueType& _val);

    /**
     * @brief Gets the body mass index of the Patient.
     * @return The body mass index of the Patient.
     */
    const DicomValueType& getPatientBodyMassIndex() const;

    /**
     * @brief Sets the body mass index of the Patient.
     * @param _val the body mass index of the Patient.
     */
    void setPatientBodyMassIndex(const DicomValueType& _val);

private:

    /// Defines the study unique identifier.
    DicomValueType m_instanceUID;

    /// Defines the user or equipment generated study identifier.
    DicomValueType m_studyID;

    /// Defines the date the study started.
    DicomValueType m_date;

    /// Defines the date the time started.
    DicomValueType m_time;

    /// Defines the referring physician name.
    DicomValueType m_referringPhysicianName;

    /// Defines the referring physician name.
    DicomValueType m_consultingPhysicianName;

    /// Defines the description of this study.
    DicomValueType m_description;

    /// Defines the patient's age during this study.
    DicomValueType m_patientAge;

    /// Defines the patient's size during this study.
    DicomValueType m_patientSize;

    /// Defines the patient's weight during this study.
    DicomValueType m_patientWeight;

    /// Defines the patient's body mass during this study.
    DicomValueType m_patientBodyMassIndex;

};

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getInstanceUID() const
{
    return m_instanceUID;
}

//-----------------------------------------------------------------------------

inline void Study::setInstanceUID(const DicomValueType& _val)
{
    m_instanceUID = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getStudyID() const
{
    return m_studyID;
}

//-----------------------------------------------------------------------------

inline void Study::setStudyID(const DicomValueType& _val)
{
    m_studyID = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getDate() const
{
    return m_date;
}

//-----------------------------------------------------------------------------

inline void Study::setDate(const DicomValueType& _val)
{
    m_date = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getTime() const
{
    return m_time;
}

//-----------------------------------------------------------------------------

inline void Study::setTime(const DicomValueType& _val)
{
    m_time = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getReferringPhysicianName() const
{
    return m_referringPhysicianName;
}

//-----------------------------------------------------------------------------

inline void Study::setReferringPhysicianName(const DicomValueType& _val)
{
    m_referringPhysicianName = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getConsultingPhysicianName() const
{
    return m_consultingPhysicianName;
}

//-----------------------------------------------------------------------------

inline void Study::setConsultingPhysicianName(const DicomValueType& _val)
{
    m_consultingPhysicianName = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getDescription() const
{
    return m_description;
}

//-----------------------------------------------------------------------------

inline void Study::setDescription(const DicomValueType& _val)
{
    m_description = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getPatientAge() const
{
    return m_patientAge;
}

//-----------------------------------------------------------------------------

inline void Study::setPatientAge(const DicomValueType& _val)
{
    m_patientAge = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getPatientSize() const
{
    return m_patientSize;
}

//-----------------------------------------------------------------------------

inline void Study::setPatientSize(const DicomValueType& _val)
{
    m_patientSize = _val;
}
//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getPatientWeight() const
{
    return m_patientWeight;
}

//-----------------------------------------------------------------------------

inline void Study::setPatientWeight(const DicomValueType& _val)
{
    m_patientWeight = _val;
}
//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getPatientBodyMassIndex() const
{
    return m_patientBodyMassIndex;
}

//-----------------------------------------------------------------------------

inline void Study::setPatientBodyMassIndex(const DicomValueType& _val)
{
    m_patientBodyMassIndex = _val;
}
//-----------------------------------------------------------------------------

} // Namespace fwMedData.
