/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <data/Vector.hpp>

namespace sight::data
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an data::Vector and create in parallel the message to announce this
 * modification.
 */
class DATA_CLASS_API Vector
{

public:

    /// Constructor. Initialize parameters.
    DATA_API Vector( data::Vector::wptr vector );

    /// Destrucotr. Do nothing.
    DATA_API ~Vector();

    /**
     * @brief Add an object in the vector.
     * @param[in] _newObject    object to add in the vector
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void add( data::Object::sptr _newObject );

    /**
     * @brief Remove an object in the vector.
     * @param[in] _oldObject object to remove from the vector.
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void remove( data::Object::sptr _oldObject );

    /**
     * @brief Clear all objects in the vector.
     *
     * Prepare the signal to announce the modification.
     */
    DATA_API void clear();

    /// Send the signal of modification
    DATA_API void notify();

private:

    data::Vector::ContainerType m_addedObjects;
    data::Vector::ContainerType m_removedObjects;
    data::Vector::wptr m_vector;
};

} // namespace helper
} // namespace sight::data
