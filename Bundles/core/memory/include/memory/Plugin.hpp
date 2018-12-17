/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __MEMORY_PLUGIN_HPP__
#define __MEMORY_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace memory
{
/**
 * @brief   Implements a plugin for the memory bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   Destructor
     */
    ~Plugin() noexcept;

    /**
     * @brief Start method. (does nothing)
     *
     * This method is used by runtime in order to initialise the bundle.
     *
     * @exception ::fwRuntime::RuntimeException.
     */
    void start();

    /**
     * @brief Stop method. (does nothing)
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() noexcept;

};

} // namespace memory

#endif //__MEMORY_PLUGIN_HPP__
