/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#define __FWCOM_SLOTS_HPP__

#include "core/config.hpp"
#include "core/macros.hpp"
#include "core/thread/Worker.hpp"

#include <map>
#include <string>
#include <vector>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::core::com
{

struct SlotBase;

template<typename F>
class Slot;

/**
 * @brief   This class proposes a storage for slots.
 */
class CORE_CLASS_API Slots
{
public:

    using key_t                = std::string;
    using SlotKeyType          = key_t;
    using SlotKeyContainerType = std::vector<key_t>;

    CORE_API Slots()          = default;
    CORE_API virtual ~Slots() = default;

    /// Copy constructor forbidden
    Slots& operator=(const Slots&) = delete;

    /// Registers SlotBase in m_slots
    CORE_API Slots& operator()(const key_t& key, const SPTR(SlotBase)& slot);

    /// Registers Slot  in m_slots (defined here to avoid compiler error C2244)
    template<typename R, typename ... A>
    Slots& operator()(const key_t& key, SPTR(Slot<R(A ...)>)slot)
    {
        SPTR(SlotBase) slotBase = std::dynamic_pointer_cast<SlotBase>(slot);
        return this->operator()(key, slotBase);
    }

    /// Returns the SlotBase associated to the key, if key does not exist, the ptr is null
    CORE_API SPTR(SlotBase) operator[](const key_t& key) const;

    /// Assigns the worker to all slots stored in m_slots
    CORE_API void setWorker(const SPTR(core::thread::Worker)& worker);

    /// Returns all key_t registered in m_slots
    [[nodiscard]] CORE_API SlotKeyContainerType getSlotKeys() const;

protected:

    /// Copy constructor forbidden
    Slots(const Slots& /*unused*/);

    /// Association < key , SPTR( SlotBase ) >
    using SlotMapType = std::map<key_t, SPTR(SlotBase)>;
    SlotMapType m_slots;
};

} // namespace sight::core::com
