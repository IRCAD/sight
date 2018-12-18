/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwCore/base.hpp"

#include <boost/filesystem.hpp>

#if defined(__APPLE__)
#   include <mach-o/dyld.h>
#elif defined(WIN32)
#   include <windows.h>
#else
#   include <link.h>
#endif

namespace fwTools
{

namespace os
{

//------------------------------------------------------------------------------

std::string getEnv(const std::string& name, bool* ok)
{
    char* value = std::getenv(name.c_str());
    bool exists = (value != NULL);
    if(ok != NULL)
    {
        *ok = exists;
    }
    return std::string(exists ? value : "");
}

//------------------------------------------------------------------------------

std::string getEnv(const std::string& name, const std::string& defaultValue)
{
    bool ok           = false;
    std::string value = getEnv(name, &ok);
    return ok ? value : defaultValue;
}

//------------------------------------------------------------------------------

std::string getUserDataDir( std::string company, std::string appName, bool createDirectory )
{
    std::string dataDir;
#ifdef WIN32
    char* appData = std::getenv("APPDATA");
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
        if (boost::filesystem::exists(dataDir))
        {
            if ( !boost::filesystem::is_directory(dataDir) )
            {
                OSLM_ERROR( dataDir << " already exists and is not a directory." );
                dataDir = "";
            }
        }
        else if (createDirectory)
        {
            OSLM_INFO("Creating application data directory: "<< dataDir);
            boost::filesystem::create_directories(dataDir);
        }
    }

    return dataDir;
}

//------------------------------------------------------------------------------

boost::filesystem::path getExecutablePath()
{
#ifdef _WIN32
    char progPath[MAX_PATH];
    if(  GetModuleFileName(NULL, progPath, MAX_PATH) == 0 )
#elif __APPLE__
    char progPath[PATH_MAX];
    uint32_t size = PATH_MAX;
    if( _NSGetExecutablePath(progPath, &size) != 0 )
#else
    char progPath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", progPath, PATH_MAX-1);
    if ( len != -1 )
    {
        progPath[len] = '\0';
    }
    if ( len == -1 )
#endif
    {
        FW_RAISE_EXCEPTION(::fwTools::Exception("Cannot determine executable path"));
    }

    return progPath;
}

//------------------------------------------------------------------------------

#if defined(WIN32)

static std::string _getWin32SharedLibraryPath(const std::string& _libName)
{
    char path[MAX_PATH];
    HMODULE hm = NULL;

    if ( ( hm = GetModuleHandle(_libName.c_str()) ) == 0)
    {
        int ret = GetLastError();
        std::stringstream err;
        err << "Cannot determine executable path, see error below." << std::endl;
        err << "GetModuleHandle failed, error = " << ret << std::endl;
        FW_RAISE_EXCEPTION(::fwTools::Exception(err.str()));
    }
    if (GetModuleFileName(hm, path, sizeof(path)) == 0)
    {
        int ret = GetLastError();
        std::stringstream err;
        err << "Cannot determine executable path, see error below." << std::endl;
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
    for (int32_t i = _dyld_image_count(); i >= 0; i--)
    {
        const char* image_name = _dyld_get_image_name(i);
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
                auto cut = libName.find(".framework");
                path = libName.substr(0, cut + 10); // cut after .framework
                break;
            }
        }
    }
    return path;
}
#else
struct FindModuleFunctor
{
    //------------------------------------------------------------------------------

    static int callback(struct dl_phdr_info* info, size_t, void*)
    {
        std::string libName(info->dlpi_name);
        const std::regex matchModule(s_libName);

        if(std::regex_search(libName, matchModule))
        {
            s_location = info->dlpi_name;
        }
        return 0;
    }

    static std::string s_location;
    private:
        static std::string s_libName;
};
std::string FindModuleFunctor::location;

#endif

//------------------------------------------------------------------------------

::boost::filesystem::path getSharedLibraryPath(const std::string& _libName)
{
#if defined(WIN32)
    return _getWin32SharedLibraryPath(_libName);
#elif defined(__APPLE__)
    return _getMacOsSharedLibraryPath();
#else
    FindModuleFunctor functor;
    FindModuleFunctor::s_libName = _libName;
    dl_iterate_phdr(&FindModuleFunctor::callback, nullptr);
    return functor.location;
#endif
}

} // namespace os

} // namespace fwTools
