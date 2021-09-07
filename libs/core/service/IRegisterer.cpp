/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "service/IRegisterer.hpp"

#include <core/com/Slots.hxx>

namespace sight::service
{

// -----------------------------------------------------------------------------

const core::com::Slots::SlotKeyType IRegisterer::s_COMPUTE_REGISTRATION_SLOT = "computeRegistration";

// ----------------------------------------------------------------------------

IRegisterer::IRegisterer() noexcept
{
    newSlot(s_COMPUTE_REGISTRATION_SLOT, &IRegisterer::computeRegistration, this);
}

// ----------------------------------------------------------------------------

IRegisterer::~IRegisterer() noexcept
{
}

// ----------------------------------------------------------------------------

} // namespace sight::service
