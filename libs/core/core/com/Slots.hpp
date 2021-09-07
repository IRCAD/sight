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

}

namespace sight::core::com
{

struct SlotBase;

template<typename F>
class Slot;

/**
 * @brief   This class proposes a mapping between a SlotKeyType and a SlotBase.
 */
class CORE_CLASS_API Slots
{
public:

    typedef std::string SlotKeyType;
    typedef std::map<SlotKeyType, SPTR(SlotBase)> SlotMapType;
    typedef std::vector<SlotKeyType> SlotKeyContainerType;

    /// Constructor, does nothing
    CORE_API Slots();

    /// Constructor, check if all slots are disconnected
    CORE_API virtual ~Slots();

    /// Registers SlotBase in m_slots
    CORE_API Slots& operator()(const SlotKeyType& key, const SPTR(SlotBase)& slot);

    /// Registers Slot  in m_slots (defined here to avoid compiler error C2244)
    template<typename R, typename ... A>
    Slots& operator()(const SlotKeyType& key, SPTR(Slot<R(A ...)>)slot)
    {
        SPTR(SlotBase) slotBase = std::dynamic_pointer_cast<SlotBase>(slot);
        return this->operator()(key, slotBase);
    }

    /// Creates in intern a new slot from function and registers it in m_slots
    template<typename F, typename ... A>
    Slots& operator()(const SlotKeyType& key, F f, A ... a);

    /// Returns the SlotBase associated to the key, if key does not exist, the ptr is null
    CORE_API SPTR(SlotBase) operator[](const SlotKeyType& key) const;

    /// Assigns the worker to all slots stored in m_slots
    CORE_API void setWorker(const SPTR(core::thread::Worker)& worker);

    /// Returns all SlotKeyType registered in m_slots
    CORE_API SlotKeyContainerType getSlotKeys() const;

protected:

    /// Copy constructor forbidden
    Slots(const Slots&);

    /// Copy constructor forbidden
    Slots& operator=(const Slots&);

    /// Association < key , SPTR( SlotBase ) >
    SlotMapType m_slots;
};

} // namespace sight::core::com
