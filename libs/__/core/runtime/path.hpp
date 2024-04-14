/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/runtime/module.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace sight::core::runtime
{

/**
 * @brief   Get the path where libraries, modules and share folders are located.
 * Sight must have been initialized first through sight::runtime::init()
 *
 * @return  a system valid path
 */
SIGHT_CORE_API std::filesystem::path working_path() noexcept;

/**
 * @brief   Retrieve the filesystem valid path of resources of a module.
 *
 * @param   _module_identifier    a string containing a module identifier
 *
 * @return  a system valid path
 */
SIGHT_CORE_API std::filesystem::path get_module_resource_path(const std::string& _module_identifier) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module having the specified identifier.
 *
 * @param   _module_identifier    a string containing a module identifier
 * @param   _path                 a module relative path
 *
 * @return  a system valid path
 */
SIGHT_CORE_API std::filesystem::path get_module_resource_file_path(
    const std::string& _module_identifier,
    const std::filesystem::path& _path
) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a module identifier.
 * For instance for "module/dir/file.txt", the function returns
 * "/home/login/sight/build/share/module/dir/file.txt"
 *
 * @param   _path                relative path whose first element is a module identifier
 *
 * @return  a system valid path
 */
SIGHT_CORE_API std::filesystem::path get_module_resource_file_path(const std::filesystem::path& _path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a library identifier.
 * For instance for "fwLib/dir/file.txt", the function returns
 * "/home/login/sight/build/share/fwLib/dir/file.txt"
 *
 * @param   _path                relative path whose first element is a library identifier
 *
 * @return  a system valid path
 */
SIGHT_CORE_API std::filesystem::path get_library_resource_file_path(const std::filesystem::path& _path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path
 *  whose first element is a library or a module identifier.
 *
 * For instance for a library:
 *  - "fwLib/dir/file.txt"
 * the function returns:
 *  - "/home/login/sight/build/share/fwLib/dir/file.txt"
 *
 * For instance for a module:
 *  - "module/dir/file.txt"
 *  the function returns:
 * - "/home/login/sight/build/share/module/dir/file.txt"
 *
 * @param   _path   relative path whose first element is a module or library identifier
 *
 * @return  a system valid path or an empty path if nothing is found
 */
SIGHT_CORE_API std::filesystem::path get_resource_file_path(const std::filesystem::path& _path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified module.
 *
 * @param   _module  a pointer to a module instance
 * @param   _path    a path relative to the module
 *
 * @return  a system valid path
 */
SIGHT_CORE_API std::filesystem::path get_module_resource_path(
    std::shared_ptr<module> _module,
    const std::filesystem::path& _path
) noexcept;

/**
 * @brief   Retrieve all valid filesystem binaries paths. These paths contain binaries like sightrun or sightlog.
 *
 * @return  set of valid system paths
 */
SIGHT_CORE_API std::set<std::filesystem::path> get_binaries_paths() noexcept;

} // namespace sight::core::runtime
