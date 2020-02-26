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

fwCampAutoDeclareDataMacro((fwMedData)(Series), FWMEDDATA_API)

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
     * @brief Gets the referring patient.
     * return the referring patient.
     */
    SPTR(::fwMedData::Patient) getPatient() const;

    /**
     * @brief Sets the referring patient.
     * @param _val the referring patient.
     */
    void setPatient(const SPTR(::fwMedData::Patient)& _val);

    /**
     * @brief Gets the referring study.
     * return the referring study.
     */
    SPTR(::fwMedData::Study) getStudy() const;

    /**
     * @brief Sets the referring study.
     * @param _val the referring study.
     */
    void setStudy(const SPTR(::fwMedData::Study)& _val);

    /**
     * @brief Gets the related equipment.
     * return the related equipment.
     */
    SPTR(::fwMedData::Equipment) getEquipment() const;

    /**
     * @brief Sets the related equipment.
     * @param _val the related equipment.
     */
    void setEquipment(const SPTR(::fwMedData::Equipment)& _val);

    /**
     * @brief Gets the unique identifier of the series.
     * return the unique identifier of the series.
     */
    const DicomValueType& getInstanceUID () const;

    /**
     * @brief Sets the unique identifier of the series.
     * @param _val the unique identifier of the series.
     */
    void setInstanceUID (const DicomValueType& _val);

    /**
     * @brief Gets the type of equipment that originally acquired the data used to create this series.
     * return the type of equipment that originally acquired the data used to create this series.
     */
    const DicomValueType& getModality () const;

    /**
     * @brief Sets the type of equipment that originally acquired the data used to create this series.
     * @param _val the type of equipment that originally acquired the data used to create this series.
     */
    void setModality (const DicomValueType& _val);

    /**
     * @brief Gets the date the series started.
     * return the date the series started.
     */
    const DicomValueType& getDate () const;

    /**
     * @brief Sets the date the series started.
     * @param _val the date the series started.
     */
    void setDate (const DicomValueType& _val);

    /**
     * @brief Gets the time the series started.
     * return the time the series started.
     */
    const DicomValueType& getTime () const;

    /**
     * @brief Sets the time the series started.
     * @param _val the time the series started.
     */
    void setTime (const DicomValueType& _val);

    /**
     * @brief Gets the description of the series.
     * return the description of the series.
     */
    const DicomValueType& getDescription () const;

    /**
     * @brief Sets the description of the series.
     * @param _val the description of the series.
     */
    void setDescription (const DicomValueType& _val);

    /**
     * @brief Gets the name of the physician(s) administering the series.
     * return the name of the physician(s) administering the series.
     */
    const DicomValuesType& getPerformingPhysiciansName () const;

    /**
     * @brief Sets the name of the physician(s) administering the series.
     * @param _val the name of the physician(s) administering the series.
     */
    void setPerformingPhysiciansName (const DicomValuesType& _val);

protected:

    /// Stores the referring patient.
    SPTR(Patient) m_patient;

    /// Stores the referring study.
    SPTR(Study) m_study;

    /// Stores the related equipment.
    SPTR(Equipment) m_equipment;

    /// Defines the series unique identifier.
    DicomValueType m_instanceUID;

    /// Defines the modality.
    DicomValueType m_modality;

    /// Defines the date.
    DicomValueType m_date;

    /// Defines the time.
    DicomValueType m_time;

    /// Defines the description.
    DicomValueType m_description;

    /// Defines performing physicians name.
    DicomValuesType m_performingPhysiciansName;

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

inline const DicomValueType& Series::getInstanceUID () const
{
    return m_instanceUID;
}

//-----------------------------------------------------------------------------

inline void Series::setInstanceUID (const DicomValueType& _val)
{
    m_instanceUID = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getModality () const
{
    return m_modality;
}

//-----------------------------------------------------------------------------

inline void Series::setModality (const DicomValueType& _val)
{
    m_modality = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getDate () const
{
    return m_date;
}

//-----------------------------------------------------------------------------

inline void Series::setDate (const DicomValueType& _val)
{
    m_date = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getTime () const
{
    return m_time;
}

//-----------------------------------------------------------------------------

inline void Series::setTime (const DicomValueType& _val)
{
    m_time = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Series::getDescription () const
{
    return m_description;
}

//-----------------------------------------------------------------------------

inline void Series::setDescription (const DicomValueType& _val)
{
    m_description = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValuesType& Series::getPerformingPhysiciansName () const
{
    return m_performingPhysiciansName;
}

//-----------------------------------------------------------------------------

inline void Series::setPerformingPhysiciansName (const DicomValuesType& _val)
{
    m_performingPhysiciansName = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
