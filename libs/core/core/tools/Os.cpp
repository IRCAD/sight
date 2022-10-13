/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/tools/Os.hpp"

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

std::string getEnv(const std::string& name, bool* ok)
{
#ifdef WIN32
    std::string value;
    std::size_t requiredSize = 0;
    // verify if env var exists and retrieves value size
    getenv_s(&requiredSize, nullptr, 0, name.c_str());
    const bool envVarExists = (requiredSize > 0);
    if(envVarExists)
    {
        std::vector<char> data(requiredSize + 1);
        // get the value of the env variable.
        getenv_s(&requiredSize, &data[0], requiredSize, name.c_str());
        value = std::string(&data[0], requiredSize - 1);
    }

    if(ok != nullptr)
    {
        *ok = envVarExists;
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

std::string getEnv(const std::string& name, const std::string& defaultValue)
{
    bool ok                 = false;
    const std::string value = getEnv(name, &ok);
    return ok ? value : defaultValue;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path getUserDir(
    const std::string& variable,
    [[maybe_unused]] const std::string& subdirectory_fallback,
    const std::string& company,
    const std::string& appName,
    bool createDirectory = false
)
{
    // get the environment variable for user directory
    std::filesystem::path dir = core::tools::os::getEnv(variable);

#ifndef WIN32
    // On Unix, fallback to $HOME / subdirectory_fallback
    if(dir.empty())
    {
        dir = core::tools::os::getEnv("HOME");

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

    if(!appName.empty())
    {
        dir /= appName;
    }

    if(std::filesystem::exists(dir))
    {
        SIGHT_THROW_IF(
            dir.string() << " already exists and is not a directory.",
            !std::filesystem::is_directory(dir)
        );
    }
    else if(createDirectory)
    {
        SIGHT_INFO("Creating user application directory: " << dir.string());
        std::filesystem::create_directories(dir);
    }

    return dir;
}

//------------------------------------------------------------------------------

std::filesystem::path getUserDataDir(const std::string& appName, bool createDirectory, const std::string& company)
{
#ifdef WIN32
    return getUserDir("APPDATA", "", company, appName, createDirectory);
#else
    return getUserDir("XDG_DATA_HOME", ".local/share", company, appName, createDirectory);
#endif
}

//------------------------------------------------------------------------------

std::filesystem::path getUserConfigDir(const std::string& appName, bool createDirectory, const std::string& company)
{
#ifdef WIN32
    return getUserDir("APPDATA", "", company, appName, createDirectory);
#else
    return getUserDir("XDG_CONFIG_HOME", ".config", company, appName, createDirectory);
#endif
}

//------------------------------------------------------------------------------

std::filesystem::path getUserCacheDir(const std::string& appName, bool createDirectory, const std::string& company)
{
#ifdef WIN32
    return getUserDir("APPDATA", "", company, appName, createDirectory);
#else
    return getUserDir("XDG_CACHE_HOME", ".cache", company, appName, createDirectory);
#endif
}

//------------------------------------------------------------------------------

#if defined(WIN32)
static std::string _getWin32SharedLibraryPath(const std::string& _libName)
{
    char path[MAX_PATH];
    HMODULE hm = nullptr;

#ifdef _DEBUG
    // On Windows Debug, we try first to look for a library with a 'd' suffix. If it does not work
    // we try with the raw name.
    const std::string libNameDbg = _libName + "d";

    if((hm = GetModuleHandle(libNameDbg.c_str())) == nullptr)
#endif // _DEBUG
    if((hm = GetModuleHandle(_libName.c_str())) == nullptr)
    {
        const DWORD ret = GetLastError();
        std::stringstream err;
        err << "Could not find shared library path, see error below." << std::endl;
        err << "GetModuleHandle failed, error = " << ret << std::endl;
        SIGHT_THROW_EXCEPTION(core::Exception(err.str()));
    }

    if(GetModuleFileName(hm, path, sizeof(path)) == NULL)
    {
        const DWORD ret = GetLastError();
        std::stringstream err;
        err << "Could not get shared library path, see error below." << std::endl;
        err << "GetModuleFileName failed, error = " << ret << std::endl;
        SIGHT_THROW_EXCEPTION(core::Exception(err.str()));
    }

    return path;
}
#else // if defined(WIN32)
struct FindModuleFunctor
{
    //------------------------------------------------------------------------------

    static int callback(struct dl_phdr_info* info, std::size_t /*unused*/, void* /*unused*/)
    {
        const std::string libName(info->dlpi_name);
        const std::regex matchModule(s_libName);

        if(std::regex_search(libName, matchModule))
        {
            s_location = info->dlpi_name;
        }

        return 0;
    }

    static std::string s_location;
    static std::string s_libName;
};

std::string FindModuleFunctor::s_location;
std::string FindModuleFunctor::s_libName;
#endif // if defined(WIN32)

//------------------------------------------------------------------------------

std::filesystem::path getSharedLibraryPath(const std::string& _libName)
{
#if defined(WIN32)
    return _getWin32SharedLibraryPath(_libName);
#else
    FindModuleFunctor::s_location.clear();
    FindModuleFunctor::s_libName = _libName;
    dl_iterate_phdr(&FindModuleFunctor::callback, nullptr);

    if(sight::core::tools::os::FindModuleFunctor::s_location.empty())
    {
        SIGHT_THROW_EXCEPTION(
            core::Exception(
                std::string("Could not find shared library path for ")
                + _libName
            )
        );
    }
    return sight::core::tools::os::FindModuleFunctor::s_location;
#endif
}

} // namespace sight::core::tools::os
