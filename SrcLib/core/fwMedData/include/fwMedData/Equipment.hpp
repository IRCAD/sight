/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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
    fwCoreClassDefinitionsWithFactoryMacro( (Equipment)(::fwData::Object), ::fwData::factory::New< Equipment >);

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
