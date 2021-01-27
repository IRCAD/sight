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

#include "data/Composite.hpp"
#include "data/config.hpp"

namespace sight::data::tools
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an data::Composite and create in parallel the message to announce this
 * modification.
 */
class DATA_CLASS_API Composite
{

public:

    /// Constructor. Initialize parameters.
    DATA_API Composite( data::Composite::wptr _composite );

    /// Destrucotr. Do nothing.
    DATA_API ~Composite();

    /**
     * @brief Add an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     * @param[in] _newObject    object to add in the composite
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void add( std::string _compositeKey, data::Object::sptr _newObject );

    /**
     * @brief Remove an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void remove( std::string _compositeKey );

    /**
     * @brief Clear all objects in the composite.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void clear();

    /**
     * @brief Replace an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     * @param[in] _newObject    the new object to add in the composite.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void swap( std::string _compositeKey, data::Object::sptr _newObject );

    /**
     * @brief Send the message of modification
     * @param _serviceSource service used to send the message
     * @param _allowLoops if true, the _serviceSource will receive the message
     */
    DATA_API void notify();

private:

    /// Map of added objects, send on notify
    data::Composite::ContainerType m_addedObjects;
    /// Map of new changed objects, send on notify
    data::Composite::ContainerType m_newChangedObjects;
    /// Map of old changed objects, send on notify
    data::Composite::ContainerType m_oldChangedObjects;
    /// Map of removed objects, send on notify
    data::Composite::ContainerType m_removedObjects;
    /// Composite to add/remove/change objects
    data::Composite::wptr m_composite;
};

} // namespace helper
} // namespace sight::data::tools
