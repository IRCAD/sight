/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
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

#include "select_object.hpp"

#include <core/com/slots.hxx>

namespace sight::module::data
{

const core::com::slots::key_t select_object::ADD_SLOT    = "add";
const core::com::slots::key_t select_object::REMOVE_SLOT = "remove";

//-----------------------------------------------------------------------------

select_object::select_object() noexcept
{
    new_slot(ADD_SLOT, &select_object::add, this);
    new_slot(REMOVE_SLOT, &select_object::remove, this);
}

//-----------------------------------------------------------------------------

select_object::~select_object() noexcept =
    default;

//-----------------------------------------------------------------------------

void select_object::configuring()
{
}

//-----------------------------------------------------------------------------

void select_object::starting()
{
}

//-----------------------------------------------------------------------------

void select_object::stopping()
{
}

//-----------------------------------------------------------------------------

void select_object::updating()
{
}

//-----------------------------------------------------------------------------

void select_object::info(std::ostream& /*_sstream*/)
{
}

//-----------------------------------------------------------------------------

void select_object::add(sight::data::object::sptr obj)
{
    m_object = obj;
}

//-----------------------------------------------------------------------------

void select_object::remove()
{
    m_object = nullptr;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
