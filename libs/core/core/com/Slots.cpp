/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "core/com/Slots.hpp"

#include "core/com/Slot.hpp"
#include "core/com/Slot.hxx"

#include <core/thread/Worker.hpp>

namespace sight::core::com
{

//-----------------------------------------------------------------------------

void Slots::setWorker(const core::thread::Worker::sptr& worker)
{
    for(const auto& elem : m_slots)
    {
        elem.second->setWorker(worker);
    }
}

//-----------------------------------------------------------------------------

Slots& Slots::operator()(const key_t& key, const SlotBase::sptr& slot)
{
    m_slots.insert({key, slot});
    return *this;
}

//-----------------------------------------------------------------------------

SlotBase::sptr Slots::operator[](const key_t& key) const
{
    auto it = m_slots.find(key);

    if(it != m_slots.end())
    {
        return it->second;
    }

    return {};
}

//-----------------------------------------------------------------------------

Slots::SlotKeyContainerType Slots::getSlotKeys() const
{
    Slots::SlotKeyContainerType slotKeys;
    for(const auto& elem : m_slots)
    {
        slotKeys.push_back(elem.first);
    }

    return slotKeys;
}

//-----------------------------------------------------------------------------

Slots::Slots(const Slots& /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
