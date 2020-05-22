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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Object.hpp"

fwCampAutoDeclareDataMacro((fwData)(Integer), FWDATA_API);

namespace fwData
{

/**
 * @brief   This class contains an integer value.
 * Integer object is essentially used as a field in other objects.
 */
class FWDATA_CLASS_API Integer : public GenericField< std::int64_t >
{
public:

    fwCoreClassMacro(Integer, ::fwData::Object);

    //------------------------------------------------------------------------------

    static sptr New(const std::int64_t val = 0)
    {
        return GenericFieldFactory< Integer >(val);
    }

    fwCampMakeFriendDataMacro((fwData)(Integer));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Integer( ::fwData::Object::Key key ) noexcept;

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Integer() noexcept;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;
};

} // namespace fwData
