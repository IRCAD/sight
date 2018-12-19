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

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "fwCore/base.hpp"
#include "fwTools/Os.hpp"

#ifdef __linux
#include <sys/stat.h>
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

} // namespace os

} // namespace fwTools
