/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "STimestampSlotCaller.hpp"

#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/hires_clock.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <regex>

namespace sight::module::ui::com
{

//-----------------------------------------------------------------------------

void STimestampSlotCaller::configuring()
{
    this->initialize();

    const auto& config = this->getConfiguration();

    const std::regex re("(.*)/(.*)");

    const auto& slots = config.get_child("slots");
    for(const auto& elem : boost::make_iterator_range(slots.equal_range("slot")))
    {
        const auto src = elem.second.get_value<std::string>();
        if(std::smatch match; std::regex_match(src, match, re))
        {
            SIGHT_ASSERT("Wrong value for attribute src: " << src, match.size() >= 3);
            const std::string uid(match[1].first, match[1].second);
            const std::string key(match[2].first, match[2].second);

            SIGHT_ASSERT("Missing hasSlotsId attribute", !uid.empty());
            SIGHT_ASSERT("Missing slotKey attribute", !key.empty());

            m_slotInfos.emplace_back(uid, key);
        }
    }
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::updating()
{
    core::hires_clock::type timestamp = core::hires_clock::get_time_in_milli_sec();

    for(const SlotInfoType& info : m_slotInfos)
    {
        HasSlotIDType HasSlotId         = info.first;
        core::com::slots::key_t slotKey = info.second;

        if(core::tools::id::exist(HasSlotId))
        {
            core::tools::object::sptr obj       = core::tools::id::get_object(HasSlotId);
            core::com::has_slots::sptr hasSlots = std::dynamic_pointer_cast<core::com::has_slots>(obj);
            SIGHT_ASSERT("Object with id " << HasSlotId << " is not a has_slots", hasSlots);

            core::com::slot_base::sptr slot = hasSlots->slot(slotKey);

            slot->async_run(timestamp);
        }
    }
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::com
