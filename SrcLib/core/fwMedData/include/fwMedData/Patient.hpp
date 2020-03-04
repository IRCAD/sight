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

fwCampAutoDeclareDataMacro((fwMedData)(Patient), FWMEDDATA_API)

namespace fwMedData
{

/**
 * @brief Holds patient information
 */
class FWMEDDATA_CLASS_API Patient : public ::fwData::Object
{

public:
    fwCoreClassMacro(Patient, ::fwData::Object, ::fwData::factory::New< Patient >)

    fwCampMakeFriendDataMacro((fwMedData)(Patient))

    /**
     * @brief Creates the patient.
     * @param _key private construction key.
     */
    FWMEDDATA_API Patient(::fwData::Object::Key _key);

    /// Destroys the patient.
    FWMEDDATA_API virtual ~Patient();

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

    /// Gets the patient's full name.
    const DicomValueType& getName () const;

    /// Sets the patient's full name.
    void setName(const DicomValueType& _val);

    /// Gets the primary hospital identification number or code for the patient.
    const DicomValueType& getPatientId () const;

    /// Sets the primary hospital identification number or code for the patient.
    void setPatientId (const DicomValueType& _val);

    /// Gets the birthdate of the patient.
    const DicomValueType& getBirthdate () const;

    /// Sets the birthdate of the patient.
    void setBirthdate (const DicomValueType& _val);

    /**
     * @brief Gets the sex of the named patient.
     *
     * Enumerated _values: M = male, F = female, O = other
     */
    const DicomValueType& getSex () const;

    /**
     * @brief Sets the sex of the named patient.
     *
     * Enumerated _values: M = male, F = female, O = other
     */
    void setSex (const DicomValueType& _val);

protected:

    /// Defines the full name.
    DicomValueType m_name;

    /// Defines the primary hospital identification.
    DicomValueType m_patientId;

    /// Defines the patient's birthdate.
    DicomValueType m_birthdate;

    /// Defines the patient's sex.
    DicomValueType m_sex;

};

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getName () const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void Patient::setName(const DicomValueType& _val)
{
    m_name = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getPatientId () const
{
    return m_patientId;
}

//-----------------------------------------------------------------------------

inline void Patient::setPatientId (const DicomValueType& _val)
{
    m_patientId = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getBirthdate () const
{
    return m_birthdate;
}

//-----------------------------------------------------------------------------

inline void Patient::setBirthdate (const DicomValueType& _val)
{
    m_birthdate = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getSex () const
{
    return m_sex;
}

//-----------------------------------------------------------------------------

inline void Patient::setSex (const DicomValueType& _val)
{
    m_sex = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
