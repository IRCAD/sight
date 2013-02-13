/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_EQUIPEMENT_HPP__
#define __FWMEDDATA_EQUIPEMENT_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/macros.hpp"

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(Equipement), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @class Equipement
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API Equipement : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Equipement)(::fwData::Object), (()), ::fwData::factory::New< Equipement >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Equipement));

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Equipement(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Equipement();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( Equipement::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( Equipement::csptr _source );

    fwDataSetCRefMacro(InstitutionName, DicomValueType);

protected:

    DicomValueType m_attrInstitutionName;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_EQUIPEMENT_HPP__


