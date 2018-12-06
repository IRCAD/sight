/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __CONSOLE_PLUGIN_HPP__
#define __CONSOLE_PLUGIN_HPP__

#include "console/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace fwThread
{
class Worker;
} //namespace fwThread

namespace console
{
/**
 * @brief   This class is called when the console bundle is loaded.
 */
class CONSOLE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// Destructor
    CONSOLE_API ~Plugin() noexcept;

    /// @override
    CONSOLE_API void start();

    /// @override
    CONSOLE_API void stop() noexcept;

    /// Run callback
    CONSOLE_API int run() noexcept;

private:

    SPTR(::fwThread::Worker) m_worker;

};

} // namespace console

#endif //__CONSOLE_PLUGIN_HPP__
