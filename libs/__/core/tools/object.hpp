/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/tools/id.hpp"

#include <core/base.hpp>
namespace sight::core::tools
{

/**
 * @brief   Define Base class for Sight objects and services
 */
class SIGHT_CORE_CLASS_API object : public core::base_object,
                                    protected core::tools::id
{
public:

    SIGHT_DECLARE_CLASS(object, core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default copy constructors and assignment operators
    object(const object&)            = delete;
    object(object&&)                 = delete;
    object& operator=(const object&) = delete;
    object& operator=(object&&)      = delete;

    // expose API for ID management
    using core::tools::id::has_id;
    using core::tools::id::get_id;
    using core::tools::id::set_id;
    using core::tools::id::reset_id;

    SIGHT_CORE_API object() = default;
    SIGHT_CORE_API ~object() override;

    /// Returns the uuid of the current instance
    SIGHT_CORE_API std::string get_uuid() const;

    /// Sets the uuid of the current instance
    /// @param _force force use of new uuid even if already used.
    SIGHT_CORE_API void set_uuid(const std::string& _uuid, bool _force = false);

    /// Retrieves the object associated with an uuid
    /// @param _uuid the uuid of the object to retrieve
    SIGHT_CORE_API static object::sptr from_uuid(const std::string& _uuid);
};

} // namespace sight::core::tools
