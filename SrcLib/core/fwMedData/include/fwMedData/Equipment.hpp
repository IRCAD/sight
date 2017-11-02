/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_EQUIPMENT_HPP__
#define __FWMEDDATA_EQUIPMENT_HPP__

#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>
#include <fwData/Object.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(Equipment), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @brief Holds equipment information
 */
class FWMEDDATA_CLASS_API Equipment : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Equipment)(::fwData::Object), (()), ::fwData::factory::New< Equipment >);

    fwCampMakeFriendDataMacro((fwMedData)(Equipment));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Equipment(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Equipment();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Institution where the equipment that produced the composite instances is located (0008,0080)
     * @{ */
    const DicomValueType& getInstitutionName() const;
    void setInstitutionName(const DicomValueType& val);
    /**  @} */

    /**  @} */

protected:

    /// Institution name
    DicomValueType m_institutionName;
};

//-----------------------------------------------------------------------------

inline const DicomValueType& Equipment::getInstitutionName() const
{
    return m_institutionName;
}

//-----------------------------------------------------------------------------

inline void Equipment::setInstitutionName(const DicomValueType& val)
{
    m_institutionName = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData

#endif // __FWMEDDATA_EQUIPMENT_HPP__

