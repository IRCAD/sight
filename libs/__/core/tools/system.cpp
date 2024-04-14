/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/tools/system.hpp"
#include "core/os/temp_path.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>

// for PID
#ifdef WIN32
#include <wtypes.h>
#include <windows.h>
#include <process.h>
#else
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <utility>

namespace sight::core::tools
{

//------------------------------------------------------------------------------

int system::get_pid() noexcept
{
    int pid = 0;
#ifdef WIN32
    pid = _getpid();
#else
    pid = getpid();
#endif

    return pid;
}

//------------------------------------------------------------------------------

bool system::is_process_running(int _pid) noexcept
{
#ifdef WIN32
    HANDLE h_process = OpenProcess(SYNCHRONIZE, FALSE, _pid);
    if(!h_process)
    {
        return false;
    }
    else
    {
        CloseHandle(h_process);
        return true;
    }
#else
    return kill(_pid, 0) == 0;
#endif
}

//------------------------------------------------------------------------------

void system::kill_process(int _pid) noexcept
{
#ifdef WIN32
    HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, _pid);
    if(h_process)
    {
        TerminateProcess(h_process, 0);
        CloseHandle(h_process);
    }
#else
    kill(_pid, SIGKILL);
#endif
}

//------------------------------------------------------------------------------

void system::robust_rename(
    const std::filesystem::path& _p1,
    const std::filesystem::path& _p2,
    bool _force
)
{
    // If both paths are indeed the same, do nothing
    if(_p1.lexically_normal().compare(_p2.lexically_normal()) == 0)
    {
        return;
    }

    std::error_code error;

    // Try a basic rename.
    std::filesystem::rename(_p1, _p2, error);

    if(error)
    {
        // Try to remove target if force is enabled
        if(_force)
        {
            std::filesystem::remove(_p2);
        }

        if(error == std::make_error_code(std::errc::cross_device_link))
        {
            std::filesystem::copy(_p1, _p2, error);
            if(error == std::make_error_code(std::errc::operation_not_permitted))
            {
                // This happens on copying on different filesystem types, i.e. EXT4 -> NTFS
                // In this case we use an alternative but less performant copy function
                std::filesystem::remove(_p2);
                std::ifstream src(_p1.string(), std::ios::binary);
                std::ofstream dst(_p2.string(), std::ios::binary);

                dst << src.rdbuf();
            }

            std::filesystem::remove(_p1);
        }
        else
        {
            // This will throw the expected exception
            std::filesystem::rename(_p1, _p2);
        }
    }
}

} //end namespace sight::core::tools
