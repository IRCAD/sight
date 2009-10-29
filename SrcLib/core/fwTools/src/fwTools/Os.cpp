/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/filesystem.hpp>

#include "fwCore/base.hpp"
#include "fwTools/Os.hpp"


namespace fwTools {

namespace os {


std::string getUserDataDir( std::string company, std::string appName, bool createDirectory )
{
    std::string dataDir;
#ifdef WIN32
        char *appData = std::getenv("APPDATA");
        dataDir = appData ? appData : "";
#else
        char *xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
        char *home          = std::getenv("HOME");
        dataDir             = xdgConfigHome ? xdgConfigHome :
                                ( home ? std::string(home) + "/.config" : "" );
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
                if ( ! boost::filesystem::is_directory(dataDir) )
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

