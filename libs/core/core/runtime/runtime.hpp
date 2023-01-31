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

#pragma once

#include "core/config.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/Module.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <iterator>
#include <memory>
#include <string>

namespace sight::core::runtime
{

class Module;

/**
 * @brief   Initializes Sight runtime and discovers default modules.
 */
CORE_API void init();

/**
 * @brief   Shutdown Sight runtime, which means unload all loaded modules.
 */
CORE_API void shutdown();

/**
 * @brief   Loads all modules that can be found in the specified directory.
 *
 * @param   directory   a path to the directory to explore for modules
 */
CORE_API void addModules(const std::filesystem::path& directory);

/**
 * @brief   Retrieves the module with the given identifier and version
 *
 * @param   identifier  a string containing a module identifier
 *
 * @return  a shared pointer to the found module, or empty when none
 */
CORE_API std::shared_ptr<Module> findModule(const std::string& identifier);

/**
 * @brief   Retrieves all modules
 *
 * @return  a set of all modules
 */
CORE_API std::set<std::shared_ptr<Module> > getModules();

/**
 * @brief   Load a module.
 *
 * @param   identifier  a string containing a module identifier
 *
 * @return  a shared pointer to the found module, or empty when it is not found
 */
CORE_API std::shared_ptr<Module> loadModule(const std::string& identifier);

/**
 * @brief   Starts the module specified by the given identifier.
 *
 * @param   identifier  a string containing a module identifier
 */
CORE_API void startModule(const std::string& identifier);

/**
 * @brief   Unload a module.
 *
 * @param   identifier  a string containing a module identifier
 *
 * @return  a shared pointer to the found module, or empty when it is not found
 */
CORE_API void unloadModule(const std::string& identifier);

/**
 * @brief   Load a library.
 *
 * @param   identifier  a string containing a module identifier
 *
 * @return  success
 */
CORE_API bool loadLibrary(const std::string& identifier);

/**
 * @brief   Retrieve the extension having the specified identifier.
 *
 * @param   identifier  a string containing an extension identifier
 *
 * @return  a shared pointer to the found extension or null if none
 */
CORE_API std::shared_ptr<Extension> findExtension(const std::string& identifier);

/// Returns extensions extending the _extension_pt extension point
CORE_API std::vector<std::shared_ptr<core::runtime::Extension> > getAllExtensionsForPoint(std::string extension_pt);

/// Utility function used to trim leading '::' in identifiers of modules, services, objects, extensions, etc...
CORE_API std::string filterID(const std::string& identifier);

} // namespace sight::core::runtime
