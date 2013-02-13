/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_EQUIPMENT_HPP__
#define __FWMEDDATA_EQUIPMENT_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/macros.hpp"

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(Equipment), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @class Equipment
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API Equipment : public ::fwData::Object
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (Equipment)(::fwData::Object) );

    fwCampMakeFriendDataMacro((fwMedData)(Equipment));

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Equipment(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Equipment();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( Equipment::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( Equipment::csptr _source );

    fwDataSetCRefMacro(InstitutionName, DicomValueType);

protected:

    DicomValueType m_attrInstitutionName;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_EQUIPMENT_HPP__


