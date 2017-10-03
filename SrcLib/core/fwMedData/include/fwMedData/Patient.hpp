/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_PATIENT_HPP__
#define __FWMEDDATA_PATIENT_HPP__

#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>
#include <fwData/Object.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(Patient), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @brief Holds patient information
 */
class FWMEDDATA_CLASS_API Patient : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Patient)(::fwData::Object), (()), ::fwData::factory::New< Patient >);

    fwCampMakeFriendDataMacro((fwMedData)(Patient));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Patient(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Patient();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters/Setters
     * @{ */

    /**
     * @brief Patient's full name (0010,0010)
     * @{ */
    const DicomValueType& getName () const;
    void setName(const DicomValueType& val);
    /**  @} */

    /**
     * @brief Primary hospital identification number or code for the patient (0010,0020)
     * @{ */
    const DicomValueType& getPatientId () const;
    void setPatientId (const DicomValueType& val);
    /**  @} */

    /**
     * @brief Birthdate of the patient (0010,0030)
     * @{ */
    const DicomValueType& getBirthdate () const;
    void setBirthdate (const DicomValueType& val);
    /**  @} */

    /**
     * @brief Sex of the named patient (0010,0040)
     * Enumerated Values: M = male, F = female, O = other
     * @{ */
    const DicomValueType& getSex () const;
    void setSex (const DicomValueType& val);
    /**  @} */

    /**  @} */

protected:

    /// Full name
    DicomValueType m_name;

    /// Primary hospital identification
    DicomValueType m_patientId;

    /// Birthdate
    DicomValueType m_birthdate;

    /// Patient's sex
    DicomValueType m_sex;
};

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getName () const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void Patient::setName(const DicomValueType& val)
{
    m_name = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getPatientId () const
{
    return m_patientId;
}

//-----------------------------------------------------------------------------

inline void Patient::setPatientId (const DicomValueType& val)
{
    m_patientId = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getBirthdate () const
{
    return m_birthdate;
}

//-----------------------------------------------------------------------------

inline void Patient::setBirthdate (const DicomValueType& val)
{
    m_birthdate = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Patient::getSex () const
{
    return m_sex;
}

//-----------------------------------------------------------------------------

inline void Patient::setSex (const DicomValueType& val)
{
    m_sex = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData

#endif // __FWMEDDATA_PATIENT_HPP__

