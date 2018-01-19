/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>
#include <fwData/Object.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(Study), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @brief Holds study information
 */
class FWMEDDATA_CLASS_API Study : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Study)(::fwData::Object), (()), ::fwData::factory::New< Study >);

    fwCampMakeFriendDataMacro((fwMedData)(Study));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Study(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Study();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters/Setters
     * @{ */

    /**
     * @brief Unique identifier of the Study (0020,000D)
     * @{ */
    const DicomValueType& getInstanceUID () const;
    void setInstanceUID (const DicomValueType& val);

    /**  @} */

    /**
     * @brief Date the Study started (0008,0020)
     * @{ */
    const DicomValueType& getDate () const;
    void setDate (const DicomValueType& val);
    /**  @} */

    /**
     * @brief Time the Study started (0008,0030)
     * @{ */

    const DicomValueType& getTime () const;
    void setTime (const DicomValueType& val);

    /**  @} */

    /**
     * @brief Name of the patient's referring physician (0008,0090)
     * @{ */
    const DicomValueType& getReferringPhysicianName () const;
    void setReferringPhysicianName (const DicomValueType& val);
    /**  @} */

    /**
     * @brief Institution-generated description or classification of the Study (component) performed (0008,1030)
     * @{ */
    const DicomValueType& getDescription () const;
    void setDescription (const DicomValueType& val);
    /**  @} */

    /**
     * @brief Age of the Patient (0010,1010)
     * @{ */
    const DicomValueType& getPatientAge () const;
    void setPatientAge (const DicomValueType& val);
    /**  @} */

    /**  @} */

protected:

    /// Study unique identifier
    DicomValueType m_instanceUID;

    /// Date
    DicomValueType m_date;

    /// Time
    DicomValueType m_time;

    /// Referring physician name
    DicomValueType m_referringPhysicianName;

    /// Description
    DicomValueType m_description;

    /// Patient age
    DicomValueType m_patientAge;
};

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getInstanceUID () const
{
    return m_instanceUID;
}

//-----------------------------------------------------------------------------

inline void Study::setInstanceUID (const DicomValueType& val)
{
    m_instanceUID = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getDate () const
{
    return m_date;
}

//-----------------------------------------------------------------------------

inline void Study::setDate (const DicomValueType& val)
{
    m_date = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getTime () const
{
    return m_time;
}

//-----------------------------------------------------------------------------

inline void Study::setTime (const DicomValueType& val)
{
    m_time = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getReferringPhysicianName () const
{
    return m_referringPhysicianName;
}

//-----------------------------------------------------------------------------

inline void Study::setReferringPhysicianName (const DicomValueType& val)
{
    m_referringPhysicianName = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getDescription () const
{
    return m_description;
}

//-----------------------------------------------------------------------------

inline void Study::setDescription (const DicomValueType& val)
{
    m_description = val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& Study::getPatientAge () const
{
    return m_patientAge;
}

//-----------------------------------------------------------------------------

inline void Study::setPatientAge (const DicomValueType& val)
{
    m_patientAge = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData
