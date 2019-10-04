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

#include "fwTools/Os.hpp"

#include <fwCore/base.hpp>

#include <filesystem>
#include <boost/regex.hpp>

#if defined(__APPLE__)
#   include <mach-o/dyld.h>
#elif defined(WIN32)
#   include <windows.h>
#else
#   include <link.h>
#endif
#include <regex>

namespace fwTools
{

namespace os
{

//------------------------------------------------------------------------------

std::string getEnv(const std::string& name, bool* ok)
{
#ifdef WIN32
    std::string value;
    size_t requiredSize = 0;
    // verify if env var exists and retrieves value size
    getenv_s( &requiredSize, nullptr, 0, name.c_str() );
    const bool envVarExists = (requiredSize > 0);
    if (envVarExists)
    {
        std::vector<char> data(requiredSize + 1);
        // get the value of the env variable.
        getenv_s( &requiredSize, &data[0], requiredSize, name.c_str() );
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
    return std::string(exists ? value : "");
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

std::string getUserDataDir( std::string company, std::string appName, bool createDirectory )
{
    std::string dataDir;
#ifdef WIN32
    dataDir = ::fwTools::os::getEnv("APPDATA");
#else
    bool hasXdgConfigHome     = false;
    bool hasHome              = false;
    std::string xdgConfigHome = ::fwTools::os::getEnv("XDG_CONFIG_HOME", &hasXdgConfigHome);
    std::string home          = ::fwTools::os::getEnv("HOME", &hasHome);
    dataDir = hasXdgConfigHome ? xdgConfigHome : (hasHome ? std::string(home) + "/.config" : "");
#endif

    if ( !company.empty() )
    {
        dataDir += "/" + company;
    }

    if ( !appName.empty() )
    {
        dataDir += "/" + appName;
    }

    if ( !dataDir.empty() )
    {
        if (std::filesystem::exists(dataDir))
        {
            if ( !std::filesystem::is_directory(dataDir) )
            {
                OSLM_ERROR( dataDir << " already exists and is not a directory." );
                dataDir = "";
            }
        }
        else if (createDirectory)
        {
            OSLM_INFO("Creating application data directory: "<< dataDir);
            std::filesystem::create_directories(dataDir);
        }
    }

    return dataDir;
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

    if ( ( hm = GetModuleHandle(libNameDbg.c_str()) ) == nullptr)
#endif // _DEBUG
    if ( ( hm = GetModuleHandle(_libName.c_str()) ) == nullptr)
    {
        const DWORD ret = GetLastError();
        std::stringstream err;
        err << "Could not find shared library path, see error below." << std::endl;
        err << "GetModuleHandle failed, error = " << ret << std::endl;
        FW_RAISE_EXCEPTION(::fwTools::Exception(err.str()));
    }

    if (GetModuleFileName(hm, path, sizeof(path)) == NULL)
    {
        const DWORD ret = GetLastError();
        std::stringstream err;
        err << "Could not get shared library path, see error below." << std::endl;
        err << "GetModuleFileName failed, error = " << ret << std::endl;
        FW_RAISE_EXCEPTION(::fwTools::Exception(err.str()));
    }
    return path;
}

#elif defined(__APPLE__)
//------------------------------------------------------------------------------

static std::string _getMacOsSharedLibraryPath(const std::string& _libName)
{
    const std::regex matchLib(std::string("lib") + _libName);
    const std::regex matchFramework(_libName);
    std::string path;
    for (std::uint32_t i = 0; i < _dyld_image_count(); ++i)
    {
        const char* const image_name = _dyld_get_image_name(i);
        if (image_name)
        {
            const std::string libName(image_name);
            if(std::regex_search(libName, matchLib))
            {
                path = libName;
                break;
            }
            else if(std::regex_search(libName, matchFramework))
            {
                // get the path of the .framework folder
                const auto cut = libName.find(".framework");
                path = libName.substr(0, cut + 10); // cut after .framework
                break;
            }
        }
    }
    if(path.empty())
    {
        FW_RAISE_EXCEPTION(::fwTools::Exception(std::string("Could not find shared library path for ") + _libName));
    }
    return path;
}
#else
struct FindModuleFunctor
{
    //------------------------------------------------------------------------------

    static int callback(struct dl_phdr_info* info, size_t, void*)
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

#endif

//------------------------------------------------------------------------------

std::filesystem::path getSharedLibraryPath(const std::string& _libName)
{
#if defined(WIN32)
    return _getWin32SharedLibraryPath(_libName);
#elif defined(__APPLE__)
    return _getMacOsSharedLibraryPath(_libName);
#else
    FindModuleFunctor functor;
    FindModuleFunctor::s_location.clear();
    FindModuleFunctor::s_libName = _libName;
    dl_iterate_phdr(&FindModuleFunctor::callback, nullptr);

    if(functor.s_location.empty())
    {
        FW_RAISE_EXCEPTION(::fwTools::Exception(std::string("Could not find shared library path for ") + _libName));
    }
    return functor.s_location;
#endif
}

} // namespace os

} // namespace fwTools
