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

#include "core/tools/os.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <regex>

#if defined(WIN32)
#   include <windows.h>
#else
#   include <link.h>
#endif

namespace sight::core::tools::os
{

//------------------------------------------------------------------------------

std::string get_env(const std::string& name, bool* ok)
{
#ifdef WIN32
    std::string value;
    std::size_t required_size = 0;
    // verify if env var exists and retrieves value size
    getenv_s(&required_size, nullptr, 0, name.c_str());
    const bool env_var_exists = (required_size > 0);
    if(env_var_exists)
    {
        std::vector<char> data(required_size + 1);
        // get the value of the env variable.
        getenv_s(&required_size, &data[0], required_size, name.c_str());
        value = std::string(&data[0], required_size - 1);
    }

    if(ok != nullptr)
    {
        *ok = env_var_exists;
    }

    return value;
#else
    const char* value = std::getenv(name.c_str());
    const bool exists = (value != nullptr);
    if(ok != nullptr)
    {
        *ok = exists;
    }
    return {exists ? value : ""};
#endif
}

//------------------------------------------------------------------------------

std::string get_env(const std::string& name, const std::string& default_value)
{
    bool ok                 = false;
    const std::string value = get_env(name, &ok);
    return ok ? value : default_value;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path get_user_dir(
    const std::string& variable,
    [[maybe_unused]] const std::string& subdirectory_fallback,
    const std::string& company,
    const std::string& app_name,
    bool create_directory = false
)
{
    // get the environment variable for user directory
    std::filesystem::path dir = core::tools::os::get_env(variable);

#ifndef WIN32
    // On Unix, fallback to $HOME / subdirectory_fallback
    if(dir.empty())
    {
        dir = core::tools::os::get_env("HOME");

        if(dir.empty())
        {
            SIGHT_THROW("No $HOME environment set.");
        }
        else
        {
            dir /= subdirectory_fallback;
        }
    }
#endif

    // Make canonical to be prettier
    if(!dir.empty())
    {
        dir = std::filesystem::weakly_canonical(dir);
    }

    if(!company.empty())
    {
        dir /= company;
    }

    if(!app_name.empty())
    {
        dir /= app_name;
    }

    if(std::filesystem::exists(dir))
    {
        SIGHT_THROW_IF(
            dir.string() << " already exists and is not a directory.",
            !std::filesystem::is_directory(dir)
        );
    }
    else if(create_directory)
    {
        SIGHT_INFO("Creating user application directory: " << dir.string());
        std::filesystem::create_directories(dir);
    }

    return dir;
}

//------------------------------------------------------------------------------

std::filesystem::path get_user_data_dir(const std::string& app_name, bool create_directory, const std::string& company)
{
#ifdef WIN32
    return get_user_dir("APPDATA", "", company, app_name, create_directory);
#else
    return get_user_dir("XDG_DATA_HOME", ".local/share", company, app_name, create_directory);
#endif
}

//------------------------------------------------------------------------------

std::filesystem::path get_user_config_dir(
    const std::string& app_name,
    bool create_directory,
    const std::string& company
)
{
#ifdef WIN32
    return get_user_dir("APPDATA", "", company, app_name, create_directory);
#else
    return get_user_dir("XDG_CONFIG_HOME", ".config", company, app_name, create_directory);
#endif
}

//------------------------------------------------------------------------------

std::filesystem::path get_user_cache_dir(const std::string& app_name, bool create_directory, const std::string& company)
{
#ifdef WIN32
    return get_user_dir("APPDATA", "", company, app_name, create_directory);
#else
    return get_user_dir("XDG_CACHE_HOME", ".cache", company, app_name, create_directory);
#endif
}

//------------------------------------------------------------------------------

#if defined(WIN32)
static std::string _get_win32shared_library_path(const std::string& _lib_name)
{
    char path[MAX_PATH];
    HMODULE hm = nullptr;

#ifdef _DEBUG
    // On Windows Debug, we try first to look for a library with a 'd' suffix. If it does not work
    // we try with the raw name.
    const std::string lib_name_dbg = _lib_name + "d";

    if((hm = GetModuleHandle(lib_name_dbg.c_str())) == nullptr)
#endif // _DEBUG
    if((hm = GetModuleHandle(_lib_name.c_str())) == nullptr)
    {
        const DWORD ret = GetLastError();
        std::stringstream err;
        err << "Could not find shared library path, see error below." << std::endl;
        err << "GetModuleHandle failed, error = " << ret << std::endl;
        SIGHT_THROW_EXCEPTION(core::exception(err.str()));
    }

    if(GetModuleFileName(hm, path, sizeof(path)) == NULL)
    {
        const DWORD ret = GetLastError();
        std::stringstream err;
        err << "Could not get shared library path, see error below." << std::endl;
        err << "GetModuleFileName failed, error = " << ret << std::endl;
        SIGHT_THROW_EXCEPTION(core::exception(err.str()));
    }

    return path;
}
#else // if defined(WIN32)
struct find_module_functor
{
    //------------------------------------------------------------------------------

    static int callback(struct dl_phdr_info* info, std::size_t /*unused*/, void* /*unused*/)
    {
        const std::string lib_name(info->dlpi_name);
        const std::regex match_module(s_lib_name);

        if(std::regex_search(lib_name, match_module))
        {
            s_location = info->dlpi_name;
        }

        return 0;
    }

    static std::string s_location;
    static std::string s_lib_name;
};

std::string find_module_functor::s_location;
std::string find_module_functor::s_lib_name;
#endif // if defined(WIN32)

//------------------------------------------------------------------------------

std::filesystem::path get_shared_library_path(const std::string& _lib_name)
{
#if defined(WIN32)
    return _get_win32shared_library_path(_lib_name);
#else
    find_module_functor::s_location.clear();
    find_module_functor::s_lib_name = _lib_name;
    dl_iterate_phdr(&find_module_functor::callback, nullptr);

    if(sight::core::tools::os::find_module_functor::s_location.empty())
    {
        SIGHT_THROW_EXCEPTION(
            core::exception(
                std::string("Could not find shared library path for ")
                + _lib_name
            )
        );
    }
    return sight::core::tools::os::find_module_functor::s_location;
#endif
}

} // namespace sight::core::tools::os
