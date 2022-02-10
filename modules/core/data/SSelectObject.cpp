/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
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

#include "SSelectObject.hpp"

#include <core/com/Slots.hxx>

namespace sight::module::data
{

const core::com::Slots::SlotKeyType SSelectObject::s_ADD_SLOT    = "add";
const core::com::Slots::SlotKeyType SSelectObject::s_REMOVE_SLOT = "remove";

//-----------------------------------------------------------------------------

SSelectObject::SSelectObject() noexcept
{
    newSlot(s_ADD_SLOT, &SSelectObject::add, this);
    newSlot(s_REMOVE_SLOT, &SSelectObject::remove, this);
}

//-----------------------------------------------------------------------------

SSelectObject::~SSelectObject() noexcept
{
}

//-----------------------------------------------------------------------------

void SSelectObject::configuring()
{
}

//-----------------------------------------------------------------------------

void SSelectObject::starting()
{
}

//-----------------------------------------------------------------------------

void SSelectObject::stopping()
{
}

//-----------------------------------------------------------------------------

void SSelectObject::updating()
{
}

//-----------------------------------------------------------------------------

void SSelectObject::info(std::ostream&)
{
}

//-----------------------------------------------------------------------------

void SSelectObject::add(sight::data::Object::sptr obj)
{
    m_object = obj;
}

//-----------------------------------------------------------------------------

void SSelectObject::remove()
{
    m_object = nullptr;
}

//-----------------------------------------------------------------------------

} // sight::module::data
