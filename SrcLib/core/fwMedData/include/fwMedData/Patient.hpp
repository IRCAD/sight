/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_PATIENT_HPP__
#define __FWMEDDATA_PATIENT_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/macros.hpp"

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(Patient), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @brief Holds patient information
 */
class FWMEDDATA_CLASS_API Patient : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Patient)(::fwData::Object), (()), ::fwData::factory::New< Patient >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Patient));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Patient(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Patient();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /**
     * @name Getters/Setters
     * @{ */

    /**
     * @brief Patient's full name (0010,0010)
     * @{ */
    fwDataGetSetCRefMacro(Name, DicomValueType);
    /**  @} */

    /**
     * @brief Primary hospital identification number or code for the patient (0010,0020)
     * @{ */
    fwDataGetSetCRefMacro(PatientId, DicomValueType);
    /**  @} */

    /**
     * @brief Birthdate of the patient (0010,0030)
     * @{ */
    fwDataGetSetCRefMacro(Birthdate, DicomValueType);
    /**  @} */

    /**
     * @brief Sex of the named patient (0010,0040)
     * Enumerated Values: M = male, F = female, O = other
     * @{ */
    fwDataGetSetCRefMacro(Sex, DicomValueType);
    /**  @} */

    /**  @} */

protected:

    /// Full name
    DicomValueType m_attrName;

    /// Primary hospital identification
    DicomValueType m_attrPatientId;

    /// Birthdate
    DicomValueType m_attrBirthdate;

    /// Patient's sex
    DicomValueType m_attrSex;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_PATIENT_HPP__


