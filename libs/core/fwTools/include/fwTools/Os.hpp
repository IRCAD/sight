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

#pragma once

#include "fwTools/config.hpp"
#include "fwTools/Exception.hpp"

#include <filesystem>

#include <string>

namespace fwTools
{

/**
 * @brief   Namespace fwTools::os contains tools methods which depend on os like get user application data directory.
 */
namespace os
{

/**
 * @brief Returns a environment variable value
 *
 * @param name environment variable 'name'
 * @param[out] ok boolean set to true if variable exists
 *
 * @return The environment variable content if it exists, else an empty string
 */
FWTOOLS_API std::string getEnv(const std::string& name, bool* ok = nullptr);

/**
 * @brief Returns a environment variable value
 *
 * @param name environment variable 'name'
 * @param defaultValue Value returned if variable 'name' doesn't exist
 */
FWTOOLS_API std::string getEnv(const std::string& name, const std::string& defaultValue);

/**
 * @brief   Return the users's application data directory
 *
 * @param company The company name
 * @param appName The application name
 * @param createDirectory if true, create the returned directory if it don't exist
 *
 * Return the application data directory. If company or appName is not empty, append them to
 * the path. Under unix, XDG conventions are repected.
 * For example, the UserDataDir under linux will be "~/.config/company/appName"
 */
FWTOOLS_API std::string  getUserDataDir(
    std::string company  = "",
    std::string appName  = "",
    bool createDirectory = false
    );

/**
 * @brief   Return the path to a shared library name
 * The library should have already been loaded before.
 * @param _libName The name of the shared library, without any 'lib' prefix, 'd' suffix or extension,
 * i.e. 'jpeg' or 'boost_filesystem'. The function will try to use the appropriate combination according to
 * the platform and the build type.
 * @return path to the library on the filesystem
 * @throw ::fwTools::Exception if the library could not be found (not loaded for instance)
 */
FWTOOLS_API std::filesystem::path getSharedLibraryPath(const std::string& _libName);

} // namespace os

} // namespace fwTools
