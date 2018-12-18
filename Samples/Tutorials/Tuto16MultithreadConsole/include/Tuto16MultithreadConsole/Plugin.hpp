/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __TUTO16MULTITHREADCONSOLE_PLUGIN_HPP__
#define __TUTO16MULTITHREADCONSOLE_PLUGIN_HPP__

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto16MultithreadConsole
{

/**
 * @brief Class call when bundle is started/stopped
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    TUTO16MULTITHREADCONSOLE_API ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle.
    TUTO16MULTITHREADCONSOLE_API void start();

    /// This method is used by runtime to stop the bundle.
    TUTO16MULTITHREADCONSOLE_API void stop() noexcept;
};

} // namespace Tuto16MultithreadConsole

#endif //__TUTO16MULTITHREADCONSOLE_PLUGIN_HPP__
