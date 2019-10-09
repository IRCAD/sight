/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto16MultithreadConsole
{

/**
 * @brief Class call when a module is started/stopped
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    TUTO16MULTITHREADCONSOLE_API ~Plugin() noexcept;

    /// This method is used by runtime to initialize the module.
    TUTO16MULTITHREADCONSOLE_API void start();

    /// This method is used by runtime to stop the module.
    TUTO16MULTITHREADCONSOLE_API void stop() noexcept;
};

} // namespace Tuto16MultithreadConsole
