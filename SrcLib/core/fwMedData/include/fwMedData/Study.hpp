/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_STUDY_HPP__
#define __FWMEDDATA_STUDY_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/macros.hpp"

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(Study), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @class Study
 * Holds study information
 *
 * @author IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API Study : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Study)(::fwData::Object), (()), ::fwData::factory::New< Study >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Study));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Study(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Study();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /**
     * @name Getters/Setters
     * @{ */

    /**
     * @brief Unique identifier of the Study (0020,000D)
     * @{ */
    fwDataGetSetCRefMacro(InstanceUID, DicomValueType);
    /**  @} */

    /**
     * @brief Date the Study started (0008,0020)
     * @{ */
    fwDataGetSetCRefMacro(Date, DicomValueType);
    /**  @} */

    /**
     * @brief Time the Study started (0008,0030)
     * @{ */
    fwDataGetSetCRefMacro(Time, DicomValueType);
    /**  @} */

    /**
     * @brief Name of the patient's referring physician (0008,0090)
     * @{ */
    fwDataGetSetCRefMacro(ReferringPhysicianName, DicomValueType);
    /**  @} */

    /**
     * @brief Institution-generated description or classification of the Study (component) performed (0008,1030)
     * @{ */
    fwDataGetSetCRefMacro(Description, DicomValueType);
    /**  @} */

    /**
     * @brief Age of the Patient (0010,1010)
     * @{ */
    fwDataGetSetCRefMacro(PatientAge, DicomValueType);
    /**  @} */

    /**  @} */

protected:

    /// Study unique identifier
    DicomValueType m_attrInstanceUID;

    /// Date
    DicomValueType m_attrDate;

    /// Time
    DicomValueType m_attrTime;

    /// Referring physician name
    DicomValueType m_attrReferringPhysicianName;

    /// Description
    DicomValueType m_attrDescription;

    /// Patient age
    DicomValueType m_attrPatientAge;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_STUDY_HPP__


