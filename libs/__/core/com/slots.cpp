/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/com/slots.hpp"

#include "core/com/slot.hpp"
#include "core/com/slot.hxx"

#include <core/thread/worker.hpp>

namespace sight::core::com
{

//-----------------------------------------------------------------------------

void slots::set_worker(const core::thread::worker::sptr& _worker)
{
    for(const auto& elem : *this)
    {
        elem.second->set_worker(_worker);
    }
}

//-----------------------------------------------------------------------------

slots& slots::operator()(const key_t& _key, const slot_base::sptr& _slot)
{
    insert({_key, _slot});
    return *this;
}

//-----------------------------------------------------------------------------

slot_base::sptr slots::operator[](const key_t& _key) const
{
    auto it = find(_key);

    if(it != end())
    {
        return it->second;
    }

    return {};
}

//-----------------------------------------------------------------------------

slots::slot_key_container_t slots::get_slot_keys() const
{
    slots::slot_key_container_t slot_keys;
    for(const auto& elem : *this)
    {
        slot_keys.push_back(elem.first);
    }

    return slot_keys;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
