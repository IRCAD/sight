/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_SERIES_HPP__
#define __FWMEDDATA_SERIES_HPP__

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(Series), FWMEDDATA_API);

namespace fwMedData
{

class Patient;
class Study;
class Equipement;

/**
 * @class Series
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API Series : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Series)(::fwData::Object), (()), ::fwData::factory::New< Series >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Series));

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Series(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Series();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( Series::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( Series::csptr _source );

    fwDataGetSetSptrMacro(Patient, SPTR(::fwMedData::Patient));
    fwDataGetSetSptrMacro(Study, SPTR(::fwMedData::Study));
    fwDataGetSetSptrMacro(Equipement, SPTR(::fwMedData::Equipement));

    fwDataGetSetCRefMacro(InstanceUID, DicomValueType);
    fwDataGetSetCRefMacro(Modality, DicomValueType);
    fwDataGetSetCRefMacro(Date, DicomValueType);
    fwDataGetSetCRefMacro(Time, DicomValueType);
    fwDataGetSetCRefMacro(PerformingPhysiciansName, DicomValueType);
    fwDataGetSetCRefMacro(Description, DicomValueType);

protected:

    SPTR(Patient) m_attrPatient;
    SPTR(Study) m_attrStudy;
    SPTR(Equipement) m_attrEquipement;

    DicomValueType m_attrInstanceUID;
    DicomValueType m_attrModality;
    DicomValueType m_attrDate;
    DicomValueType m_attrTime;
    DicomValueType m_attrPerformingPhysiciansName;
    DicomValueType m_attrDescription;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_SERIES_HPP__


