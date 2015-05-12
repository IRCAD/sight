/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
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
class Equipment;

/**
 * @brief Holds series information.
 */
class FWMEDDATA_CLASS_API Series : public ::fwData::Object
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (Series)(::fwData::Object) );

    fwCampMakeFriendDataMacro((fwMedData)(Series));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Series(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Series();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Referring Patient
     * @{ */
    fwDataGetSetSptrMacro(Patient, SPTR(::fwMedData::Patient));
    /**  @} */

    /**
     * @brief Referring Study
     * @{ */
    fwDataGetSetSptrMacro(Study, SPTR(::fwMedData::Study));
    /**  @} */

    /**
     * @brief Related Equipment
     * @{ */
    fwDataGetSetSptrMacro(Equipment, SPTR(::fwMedData::Equipment));
    /**  @} */

    /**
     * @brief Unique identifier of the Series (0020,000E)
     * @{ */
    fwDataGetSetCRefMacro(InstanceUID, DicomValueType);
    /**  @} */

    /**
     * @brief Type of equipment that originally acquired the data used to create this Series (0008,0060)
     * @{ */
    fwDataGetSetCRefMacro(Modality, DicomValueType);
    /**  @} */

    /**
     * @brief Date the Series started (0008,0021)
     * @{ */
    fwDataGetSetCRefMacro(Date, DicomValueType);
    /**  @} */

    /**
     * @brief Time the Series started (0008,0031)
     * @{ */
    fwDataGetSetCRefMacro(Time, DicomValueType);
    /**  @} */

    /**
     * @brief Description of the Series (0008,103E)
     * @{ */
    fwDataGetSetCRefMacro(Description, DicomValueType);
    /**  @} */

    /**
     * @brief Name of the physician(s) administering the Series (0008,1050)
     * @{ */
    fwDataGetSetCRefMacro(PerformingPhysiciansName, DicomValuesType);
    /**  @} */

    /**  @} */

protected:

    /// Referring Patient
    SPTR(Patient) m_attrPatient;

    /// Referring Study
    SPTR(Study) m_attrStudy;

    /// Related Equipment
    SPTR(Equipment) m_attrEquipment;

    /// Series unique identifier
    DicomValueType m_attrInstanceUID;

    /// Modality
    DicomValueType m_attrModality;

    /// Date
    DicomValueType m_attrDate;

    /// Time
    DicomValueType m_attrTime;

    /// Description
    DicomValueType m_attrDescription;

    /// Performing physicians name
    DicomValuesType m_attrPerformingPhysiciansName;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_SERIES_HPP__


