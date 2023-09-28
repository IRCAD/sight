/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/exception.hpp"

#include <filesystem>
#include <string>

namespace sight::core::tools::os
{

/**
 * @brief Returns a environment variable value
 *
 * @param name environment variable 'name'
 * @param[out] ok boolean set to true if variable exists
 *
 * @return The environment variable content if it exists, else an empty string
 */
CORE_API std::string get_env(const std::string& name, bool* ok = nullptr);

/**
 * @brief Returns a environment variable value
 *
 * @param name environment variable 'name'
 * @param defaultValue Value returned if variable 'name' doesn't exist
 */
CORE_API std::string get_env(const std::string& name, const std::string& default_value);

/**
 * @brief   Return the users's application config directory
 *
 * @param company The company name
 * @param appName The application name
 * @param createDirectory if true, create the returned directory if it don't exist
 *
 * Return the application config directory. If company or appName is not empty, append them to
 * the path. Under unix, XDG conventions are respected.
 * For example, the UserConfigDir under linux will be "~/.local/share/company/appName"
 */
CORE_API std::filesystem::path get_user_data_dir(
    const std::string& app_name = "",
    bool create_directory       = true,
    const std::string& company  = "sight"
);

/**
 * @brief   Return the users's application config directory
 *
 * @param company The company name
 * @param appName The application name
 * @param createDirectory if true, create the returned directory if it don't exist
 *
 * Return the application config directory. If company or appName is not empty, append them to
 * the path. Under unix, XDG conventions are respected.
 * For example, the UserConfigDir under linux will be "~/.config/company/appName"
 */
CORE_API std::filesystem::path get_user_config_dir(
    const std::string& app_name = "",
    bool create_directory       = true,
    const std::string& company  = "sight"
);

/**
 * @brief   Return the users's application cache directory
 *
 * @param company The company name
 * @param appName The application name
 * @param createDirectory if true, create the returned directory if it don't exist
 *
 * Return the application data directory. If company or appName is not empty, append them to
 * the path. Under unix, XDG conventions are respected.
 * For example, the UserCacheDir under linux will be "~/.cache/company/appName"
 */
CORE_API std::filesystem::path get_user_cache_dir(
    const std::string& app_name = "",
    bool create_directory       = true,
    const std::string& company  = "sight"
);

/**
 * @brief   Return the path to a shared library name
 * The library should have already been loaded before.
 * @param _libName The name of the shared library, without any 'lib' prefix, 'd' suffix or extension,
 * i.e. 'jpeg' or 'boost_filesystem'. The function will try to use the appropriate combination according to
 * the platform and the build type.
 * @return path to the library on the filesystem
 * @throw core::exception if the library could not be found (not loaded for instance)
 */
CORE_API std::filesystem::path get_shared_library_path(const std::string& _lib_name);

} // namespace sight::core::tools::os
