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

#pragma once

#include <sight/module/io/joystick/config.hpp>

#include <core/runtime/plugin.hpp>

namespace sight::module::io::joystick
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class SIGHT_MODULE_IO_JOYSTICK_CLASS_API plugin final : public sight::core::runtime::plugin
{
public:

    // Overrides
    SIGHT_MODULE_IO_JOYSTICK_API void start() noexcept final;

    // Overrides
    SIGHT_MODULE_IO_JOYSTICK_API void stop() noexcept final;
};

} // namespace sight::module::io::joystick
