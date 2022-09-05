/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Object.hpp"
#include "data/types.hpp"

namespace sight::data
{

/**
 * @brief Holds an equipment information.
 */
class DATA_CLASS_API Equipment : public Object
{
public:

    SIGHT_DECLARE_CLASS(Equipment, Object, factory::New<Equipment>);

    /**
     * @brief Creates the equipment.
     * @param _key private construction key.
     */
    DATA_API Equipment(Object::Key _key);

    /// Destroys the equipment.
    DATA_API ~Equipment() override;

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets the institution where the equipment that produced the composite instances is located.
    const DicomValueType& getInstitutionName() const;

    /// Sets the institution where the equipment that produced the composite instances is located.
    void setInstitutionName(const DicomValueType& _val);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Equipment& other) const noexcept;
    DATA_API bool operator!=(const Equipment& other) const noexcept;

    /// @}

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

} // Namespace sight::data
