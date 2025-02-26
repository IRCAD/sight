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

#define FWCOM_SLOTS_HPP

#include <sight/core/config.hpp>

#include "core/macros.hpp"
#include "core/thread/worker.hpp"

#include <map>
#include <string>
#include <vector>

namespace sight::core::thread
{

class worker;

} // namespace sight::core::thread

namespace sight::core::com
{

struct slot_base;

template<typename F>
class slot;

/**
 * @brief   This class proposes a storage for slots.
 */
class SIGHT_CORE_CLASS_API slots : public std::map<std::string,
                                                   SPTR(slot_base)>
{
public:

    using key_t                = std::string;
    using slot_key_type        = key_t;
    using slot_key_container_t = std::vector<key_t>;

    SIGHT_CORE_API slots()          = default;
    SIGHT_CORE_API virtual ~slots() = default;

    /// Copy constructor forbidden
    slots(const slots& /*unused*/) = delete;

    /// Registers SlotBase in m_slots
    SIGHT_CORE_API slots& operator()(const key_t& _key, const SPTR(slot_base)& _slot);

    /// Registers Slot  in m_slots (defined here to avoid compiler error C2244)
    template<typename R, typename ... A>
    slots& operator()(const key_t& _key, SPTR(slot<R(A ...)>)_slot)
    {
        SPTR(slot_base) base = std::dynamic_pointer_cast<slot_base>(_slot);
        return this->operator()(_key, base);
    }

    /// Returns the SlotBase associated to the key, if key does not exist, the ptr is null
    SIGHT_CORE_API SPTR(slot_base) operator[](const key_t& _key) const;

    /// Assigns the worker to all slots stored in m_slots
    SIGHT_CORE_API void set_worker(const SPTR(core::thread::worker)& _worker);

    /// Returns all key_t registered in m_slots
    [[nodiscard]] SIGHT_CORE_API slot_key_container_t get_slot_keys() const;
};

} // namespace sight::core::com
