/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
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
 * @class Patient
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API Patient : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Patient)(::fwData::Object), (()), ::fwData::factory::New< Patient >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Patient));

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Patient(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Patient();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( Patient::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( Patient::csptr _source );

    fwDataGetSetCRefMacro(Name, DicomValueType);
    fwDataGetSetCRefMacro(Id, DicomValueType);
    fwDataGetSetCRefMacro(Birthdate, DicomValueType);
    fwDataGetSetCRefMacro(Sex, DicomValueType);

protected:

    DicomValueType m_attrName;
    DicomValueType m_attrId;
    DicomValueType m_attrBirthdate;
    DicomValueType m_attrSex;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_PATIENT_HPP__


