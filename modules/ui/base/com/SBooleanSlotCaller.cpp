/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SBooleanSlotCaller.hpp"

#include <core/com/Slots.hxx>

namespace sight::module::ui::base::com
{

//-----------------------------------------------------------------------------

SBooleanSlotCaller::SBooleanSlotCaller() noexcept
{
    SIGHT_WARN(
        "'SBooleanSlotCaller' is deprecated and will be removed in Sight 23.0,"
        " please use 'SAction' with regular signal/slots connections instead."
    );
}

//-----------------------------------------------------------------------------

SBooleanSlotCaller::~SBooleanSlotCaller() noexcept =
    default;

//-----------------------------------------------------------------------------

void SBooleanSlotCaller::updating()
{
    for(const SlotInfoType& info : m_slotInfos)
    {
        HasSlotIDType HasSlotId               = info.first;
        core::com::Slots::SlotKeyType slotKey = info.second;

        if(core::tools::fwID::exist(HasSlotId))
        {
            core::tools::Object::sptr obj      = core::tools::fwID::getObject(HasSlotId);
            core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast<core::com::HasSlots>(obj);
            SIGHT_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

            core::com::SlotBase::sptr slot = hasSlots->slot(slotKey);

            slot->asyncRun(this->checked());
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::base::com
