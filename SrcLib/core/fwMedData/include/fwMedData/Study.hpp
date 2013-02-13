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
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API Study : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Study)(::fwData::Object), (()), ::fwData::factory::New< Study >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Study));

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Study(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Study();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( Study::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( Study::csptr _source );

    fwDataSetCRefMacro(InstanceUID, DicomValueType);
    fwDataSetCRefMacro(Date, DicomValueType);
    fwDataSetCRefMacro(Time, DicomValueType);
    fwDataSetCRefMacro(ReferringPhysicianName, DicomValueType);
    fwDataSetCRefMacro(Description, DicomValueType);
    fwDataSetCRefMacro(PatientAge, DicomValueType);

protected:

    DicomValueType m_attrInstanceUID;
    DicomValueType m_attrDate;
    DicomValueType m_attrTime;
    DicomValueType m_attrReferringPhysicianName;
    DicomValueType m_attrDescription;
    DicomValueType m_attrPatientAge;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_STUDY_HPP__


