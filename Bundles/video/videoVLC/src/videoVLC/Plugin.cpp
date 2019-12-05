/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "videoVLC/Plugin.hpp"

#include <fwCore/spyLog.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/Runtime.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

namespace videoVLC
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::videoVLC::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    if(::boost::filesystem::exists(VLC_PLUGINS_DIR))
    {
        Plugin::exportVLCPluginsEnv(VLC_PLUGINS_DIR);
    }
    else
    {
        const ::boost::filesystem::path workingPath  = ::fwRuntime::Runtime::getDefault()->getWorkingPath();
        const ::boost::filesystem::path vlcPluginDir = workingPath / "vlc/plugins";
        SLM_ASSERT("VLC plugins directory is not found.", ::boost::filesystem::exists(vlcPluginDir));
        Plugin::exportVLCPluginsEnv(vlcPluginDir.string());
    }
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::exportVLCPluginsEnv(const std::string& path)
{
#ifdef WIN32
    {
        typedef int (_cdecl* PUTENVPROC)(const char*);

        HMODULE hmodule = GetModuleHandle("msvcrt");
        SLM_ASSERT("GetModuleHandle msvcrt failed", hmodule);

        PUTENVPROC putenvFunc = (PUTENVPROC) GetProcAddress(hmodule, "_putenv");
        SLM_ASSERT("GetProcAddress _putenv failed", hmodule);

        const std::string env = "VLC_PLUGIN_PATH=" + path;
        putenvFunc(env.c_str());
    }
#else
    setenv("VLC_PLUGIN_PATH", path.c_str(), 1);
#endif
}

//-----------------------------------------------------------------------------

} // namespace videoVLC
