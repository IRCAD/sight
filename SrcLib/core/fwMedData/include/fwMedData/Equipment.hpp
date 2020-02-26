/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

fwCampAutoDeclareDataMacro((fwMedData)(Equipment), FWMEDDATA_API)

namespace fwMedData
{

/**
 * @brief Holds an equipment information.
 */
class FWMEDDATA_CLASS_API Equipment : public ::fwData::Object
{

public:

    fwCoreClassMacro(Equipment, ::fwData::Object, ::fwData::factory::New< Equipment >)

    fwCampMakeFriendDataMacro((fwMedData)(Equipment))

    /**
     * @brief Creates the equipement.
     * @param _key private construction key.
     */
    FWMEDDATA_API Equipment(::fwData::Object::Key _key);

    /// Destroys the equipement.
    FWMEDDATA_API virtual ~Equipment();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object where find data.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object where find data.
     * @param _cache
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @brief Gets the institution where the equipment that produced the composite instances is located.
     * @return the institution name.
     */
    const DicomValueType& getInstitutionName() const;

    /**
     * @brief Sets the institution where the equipment that produced the composite instances is located.
     * @param _val the institution name.
     */
    void setInstitutionName(const DicomValueType& _val);

protected:

    /// Defines the institution name.
    DicomValueType m_institutionName;
};

//-----------------------------------------------------------------------------

inline const DicomValueType& Equipment::getInstitutionName() const
{
    return m_institutionName;
}

//-----------------------------------------------------------------------------

inline void Equipment::setInstitutionName(const DicomValueType& _val)
{
    m_institutionName = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
