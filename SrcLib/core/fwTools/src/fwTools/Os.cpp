/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/filesystem.hpp>

#include "fwCore/base.hpp"
#include "fwTools/Os.hpp"


namespace fwTools
{

namespace os
{

std::string getEnv(const std::string &name, bool *ok)
{
    char *value = std::getenv(name.c_str());
    bool exists = (value != NULL);
    if(ok != NULL)
    {
        *ok = exists;
    }
    return std::string(exists ? value : "");
}

//------------------------------------------------------------------------------

std::string getEnv(const std::string &name, const std::string &defaultValue)
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
    char *appData = std::getenv("APPDATA");
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

} // namespace os

} // namespace fwTools

