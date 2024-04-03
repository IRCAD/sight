/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/core/config.hpp>

#include "core/runtime/extension.hpp"
#include "core/runtime/module.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <iterator>
#include <memory>
#include <string>

namespace sight::core::runtime
{

class module;

/**
 * @brief   Initializes Sight runtime and discovers default modules.
 */
SIGHT_CORE_API void init();

/**
 * @brief   Shutdown Sight runtime, which means unload all loaded modules.
 */
SIGHT_CORE_API void shutdown();

/**
 * @brief   Loads all modules that can be found in the specified directory.
 *
 * @param   _directory   a path to the directory to explore for modules
 */
SIGHT_CORE_API void add_modules(const std::filesystem::path& _directory);

/**
 * @brief   Retrieves the module with the given identifier and version
 *
 * @param   _identifier  a string containing a module identifier
 *
 * @return  a shared pointer to the found module, or empty when none
 */
SIGHT_CORE_API std::shared_ptr<module> find_module(const std::string& _identifier);

/**
 * @brief   Retrieves all modules
 *
 * @return  a set of all modules
 */
SIGHT_CORE_API std::set<std::shared_ptr<module> > modules();

/**
 * @brief   Load a module.
 *
 * @param   _identifier  a string containing a module identifier
 *
 * @return  a shared pointer to the found module, or empty when it is not found
 */
SIGHT_CORE_API std::shared_ptr<module> load_module(const std::string& _identifier);

/**
 * @brief   Starts the module specified by the given identifier.
 *
 * @param   _identifier  a string containing a module identifier
 */
SIGHT_CORE_API void start_module(const std::string& _identifier);

/**
 * @brief   Unload a module.
 *
 * @param   _identifier  a string containing a module identifier
 *
 * @return  a shared pointer to the found module, or empty when it is not found
 */
SIGHT_CORE_API void unload_module(const std::string& _identifier);

/**
 * @brief   Load a library.
 *
 * @param   _identifier  a string containing a module identifier
 *
 * @return  success
 */
SIGHT_CORE_API bool load_library(const std::string& _identifier);

/**
 * @brief   Retrieve the extension having the specified identifier.
 *
 * @param   _identifier  a string containing an extension identifier
 *
 * @return  a shared pointer to the found extension or null if none
 */
SIGHT_CORE_API std::shared_ptr<extension> find_extension(const std::string& _identifier);

/// Returns extensions extending the _extension_pt extension point
SIGHT_CORE_API std::vector<std::shared_ptr<core::runtime::extension> > get_all_extensions_for_point(
    std::string _extension_pt
);

/// Utility function used to trim leading '::' in identifiers of modules, services, objects, extensions, etc...
SIGHT_CORE_API std::string filter_id(const std::string& _identifier);

} // namespace sight::core::runtime
