/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "plugin.hpp"

#include <core/runtime/utils/generic_executable_factory_registry.hpp>

#include <io/joystick/joystick.hpp>

namespace sight::module::io::joystick
{

SIGHT_REGISTER_PLUGIN("sight::module::io::joystick::plugin");

//------------------------------------------------------------------------------

void plugin::start() noexcept
{
    sight::io::joystick::init();
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
    sight::io::joystick::shutdown();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::joystick
