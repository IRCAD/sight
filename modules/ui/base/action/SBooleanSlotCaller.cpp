/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/base/action/SBooleanSlotCaller.hpp"

#include <core/com/Slots.hxx>

#include <services/macros.hpp>

#include <regex>

namespace sight::modules::ui::base
{
namespace action
{

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::sight::modules::ui::base::action::SBooleanSlotCaller )

//-----------------------------------------------------------------------------

SBooleanSlotCaller::SBooleanSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

SBooleanSlotCaller::~SBooleanSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

void SBooleanSlotCaller::updating()
{
    for(SlotInfoType info :  m_slotInfos)
    {
        HasSlotIDType HasSlotId               = info.first;
        core::com::Slots::SlotKeyType slotKey = info.second;

        if (core::tools::fwID::exist(HasSlotId))
        {
            core::tools::Object::sptr obj      = core::tools::fwID::getObject(HasSlotId);
            core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< core::com::HasSlots >(obj);
            SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

            core::com::SlotBase::sptr slot = hasSlots->slot(slotKey);

            slot->asyncRun(this->getIsActive());
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace sight::modules::ui::base
