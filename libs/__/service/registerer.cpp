/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "service/registerer.hpp"

#include <core/com/slots.hxx>

namespace sight::service
{

// -----------------------------------------------------------------------------

const core::com::slots::key_t registerer::COMPUTE_REGISTRATION_SLOT = "computeRegistration";

// ----------------------------------------------------------------------------

registerer::registerer() noexcept
{
    new_slot(COMPUTE_REGISTRATION_SLOT, &registerer::compute_registration, this);
}

// ----------------------------------------------------------------------------

registerer::~registerer() noexcept =
    default;

// ----------------------------------------------------------------------------

} // namespace sight::service
