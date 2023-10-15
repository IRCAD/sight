/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "default_action.hpp"

#include <service/macros.hpp>

namespace sight::module::ui
{

default_action::default_action() noexcept
{
    SIGHT_WARN(
        "'default_action' is deprecated and will be removed in Sight 23.0,"
        " please use 'action' instead."
    );
}

//-----------------------------------------------------------------------------

default_action::~default_action() noexcept =
    default;

//-----------------------------------------------------------------------------

void default_action::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void default_action::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void default_action::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void default_action::updating()
{
}

//-----------------------------------------------------------------------------

void default_action::info(std::ostream& _sstream)
{
    _sstream << "Default button" << std::endl;
}

} // namespace sight::module::ui
