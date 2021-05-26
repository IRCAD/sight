/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/config.hpp"
#include "core/tools/fwID.hpp"
#include <core/base.hpp>
namespace sight::core::tools
{

/**
 * @brief   Define Base class for Sight objects and services
 */
class CORE_CLASS_API Object : public core::BaseObject,
                              protected core::tools::fwID
{
public:

    SIGHT_DECLARE_CLASS(Object, core::BaseObject)
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default copy constructors and assignment operators
    Object(const Object&)            = delete;
    Object(Object&&)                 = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&)      = delete;

    // expose API for ID management
    using core::tools::fwID::hasID;
    using core::tools::fwID::getID;
    using core::tools::fwID::setID;
    using core::tools::fwID::resetID;

    CORE_API Object() = default;
    CORE_API virtual ~Object();

    /// Returns the uuid of the current instance
    CORE_API std::string getUUID() const;

    /// Sets the uuid of the current instance
    /// @param force force use of new uuid even if already used.
    CORE_API void setUUID(const std::string& uuid, const bool force = false);

    /// Retrieves the object associated with an uuid
    /// @param uuid the uuid of the object to retrieve
    CORE_API static Object::sptr fromUUID(const std::string& uuid);
};

}
