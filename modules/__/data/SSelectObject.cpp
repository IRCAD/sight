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

#include "SSelectObject.hpp"

#include <core/com/slots.hxx>

namespace sight::module::data
{

const core::com::slots::key_t SSelectObject::ADD_SLOT    = "add";
const core::com::slots::key_t SSelectObject::REMOVE_SLOT = "remove";

//-----------------------------------------------------------------------------

SSelectObject::SSelectObject() noexcept
{
    new_slot(ADD_SLOT, &SSelectObject::add, this);
    new_slot(REMOVE_SLOT, &SSelectObject::remove, this);
}

//-----------------------------------------------------------------------------

SSelectObject::~SSelectObject() noexcept =
    default;

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

void SSelectObject::info(std::ostream& /*_sstream*/)
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

} // namespace sight::module::data
