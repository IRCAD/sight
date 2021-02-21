/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2018 IHU Strasbourg
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

#include "SObjFromSlot.hpp"

#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

namespace sight::module::data
{

namespace updater
{

const core::com::Slots::SlotKeyType SObjFromSlot::s_ADD_SLOT    = "add";
const core::com::Slots::SlotKeyType SObjFromSlot::s_REMOVE_SLOT = "remove";

static const std::string s_OBJECT = "object";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::module::data::IUpdaterSrv, ::sight::module::data::updater::SObjFromSlot,
                         ::sight::data::Composite )

//-----------------------------------------------------------------------------

SObjFromSlot::SObjFromSlot() noexcept
{
    newSlot(s_ADD_SLOT, &SObjFromSlot::add, this);
    newSlot(s_REMOVE_SLOT, &SObjFromSlot::remove, this);
}

//-----------------------------------------------------------------------------

SObjFromSlot::~SObjFromSlot() noexcept
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::configuring()
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::starting()
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::stopping()
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::updating()
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::info( std::ostream& )
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::add(sight::data::Object::sptr obj)
{
    this->setOutput(s_OBJECT, obj);
}

//-----------------------------------------------------------------------------

void SObjFromSlot::remove()
{
    this->setOutput(s_OBJECT, nullptr);
}

//-----------------------------------------------------------------------------

} // updater
} // sight::module::data
