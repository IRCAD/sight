/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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
#include "data/GenericField.hpp"
#include "data/Object.hpp"

namespace sight::data
{

/**
 * @brief   This class contains an Double value.
 *
 * Double object is essentially used as a field in other objects.
 */
class DATA_CLASS_API Double : public GenericField<double>
{
public:

    SIGHT_DECLARE_CLASS(Double, data::Object);

    //------------------------------------------------------------------------------

    static sptr New(const double val = 0.0)
    {
        return GenericFieldFactory<Double>(val);
    }

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Double(data::Object::Key key) noexcept;

    /**
     * @brief Destructor.
     */
    DATA_API virtual ~Double() noexcept = default;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

protected:

    DATA_API Double() noexcept = default;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;
};

} // namespace sight::data
