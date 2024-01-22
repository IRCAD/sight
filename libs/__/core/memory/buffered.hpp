/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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
#include "core/memory/buffer_object.hpp"

namespace sight::core::memory
{

/**
 * @brief Defines the interface to implement to allow locking of data that use a BufferObject
 */
class CORE_CLASS_API buffered
{
public:

    /// Convenience method to avoid declaring a vector of locks
    [[nodiscard]] inline std::vector<core::memory::buffer_object::lock_t> dump_lock() const
    {
        std::vector<core::memory::buffer_object::lock_t> locks;
        dump_lock_impl(locks);
        return locks;
    }

protected:

    /// Implementation of the interface must allocate a core::memory::buffer_object::lock_t and store it into the
    /// parameter
    /// @param _locks The vector of locks to store the new lock
    CORE_API virtual void dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const = 0;
};

} // namespace sight::core::memory
