/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/factory/new.hpp"
#include "data/GenericField.hpp"
#include "data/Object.hpp"

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (String));

namespace sight::data
{

/**
 * @brief This class contains an std::string value.
 *
 * String object is essentially used as a field in other objects.
 */
class DATA_CLASS_API String : public GenericField<std::string>
{
public:

    SIGHT_DECLARE_CLASS(String, data::Object);

    //------------------------------------------------------------------------------

    static sptr New(const std::string val = "")
    {
        return GenericFieldFactory<String>(val);
    }

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (String));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API String(data::Object::Key key) noexcept;

    /**
     * @brief Destructor.
     */
    DATA_API virtual ~String() noexcept;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;
};

} // namespace sight::data
