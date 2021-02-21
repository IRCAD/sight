/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "core/runtime/ConfigurationElement.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/IExecutable.hpp"
#include "core/runtime/Runtime.hpp"
#include "core/runtime/RuntimeException.hpp"
#include "core/runtime/Version.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <iterator>
#include <memory>
#include <string>

namespace sight::core::runtime
{
class Module;
class Profile;

/**
 * @brief   Initializes Sight runtime and discovers default modules. To be used when building an external application
 *          with Sight.
 *
 * @param   directory   deprecated, path to the directory where Sight is installed. This is now automatically detected.
 */
CORE_API void init( [[deprecated("To be removed in Sight 22.0,this is now automatically detected")]]
                    const std::filesystem::path& directory = std::filesystem::path()
                    );

/**
 * @brief       Creates an executable instance for the specified configuration element.
 *
 * @param[in]   extension   a shared to an extension containing a configuration element
 *                          to use for the executable instance creation
 * @param[in]   element     a string containing the name of a configuration element
 * @param[in]   attribute   a string containing an element attribute name to use as
 *                          the executable type
 *
 * @return      a shared pointer to the built executable object
 */
template<typename T>
T* createExecutableInstance(
    const std::shared_ptr<Extension>    extension,
    const std::string& element   = "executable",
    const std::string& attribute = "class" )
{
    // Retrieves the right configuration element.
    std::shared_ptr<ConfigurationElement> elt( extension->findConfigurationElement(element) );
    if( elt == 0 )
    {
        throw RuntimeException(element + ": configuration element not found in extension.");
    }

    // Creates the executable instance from the found configuration element.
    return createExecutableInstance<T>( elt, attribute );
}

/**
 * @brief   Retrieves all configuration elements for the point having the
 *          specified identifier.
 * @return  a vector containing shared pointers to all found configuration elements
 */
std::vector< ConfigurationElement::sptr > getAllConfigurationElementsForPoint(const std::string& identifier);

/**
 * @brief   Retrieve the configuation element with the given identifier for the
 *          given extension point
 *
 * @param   identifier          a string containing the configuration element identifier
 * @param   pointIdentifier     a string containing the extension point identifier
 *
 * @return  a shared pointer to the found configuration element or null if none
 */
CORE_API std::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string& identifier,
                                                                           const std::string& pointIdentifier );

/**
 * @brief   Retrieve the extension having the specified identifier.
 *
 * @param   identifier  a string containing an extension identifier
 *
 * @return  a shared pointer to the found extension or null if none
 */
CORE_API std::shared_ptr<Extension> findExtension( const std::string& identifier );

/**
 * @brief   Retrieve the filesystem valid path of resources of a module.
 *
 * @param   moduleIdentifier    a string containing a module identifier
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getModuleResourcePath(const std::string& moduleIdentifier) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module having the specified identifier.
 *
 * @param   moduleIdentifier    a string containing a module identifier
 * @param   path                a module relative path
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getModuleResourceFilePath(const std::string& moduleIdentifier,
                                                         const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a module identifier.
 * For instance for "module-0.2/dir/file.txt", the function returns
 * "/home/login/sight/build/share/module-0.2/dir/file.txt"
 *
 * @param   path                relative path whose first element is a module identifier
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getModuleResourceFilePath(const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a library identifier.
 * For instance for "fwLib-0.2/dir/file.txt", the function returns
 * "/home/login/sight/build/share/fwLib-0.2/dir/file.txt"
 *
 * @param   path                relative path whose first element is a library identifier
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getLibraryResourceFilePath(const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path
 *  whose first element is a library or a module identifier.
 *
 * For instance for a library:
 *  - "fwLib-0.2/dir/file.txt"
 * the function returns:
 *  - "/home/login/sight/build/share/fwLib-0.2/dir/file.txt"
 *
 * For instance for a module:
 *  - "module-0.2/dir/file.txt"
 *  the function returns:
 * - "/home/login/sight/build/share/module-0.2/dir/file.txt"
 *
 * @param   path   relative path whose first element is a module or library identifier
 *
 * @return  a system valid path or an empty path if nothing is found
 */
CORE_API std::filesystem::path getResourceFilePath(const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified module.
 *
 * @param   module  a pointer to a module instance
 * @param   path    a path relative to the module
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getModuleResourcePath( std::shared_ptr<Module> module,
                                                      const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module of the specified configuration element.
 *
 * @param   element a shared pointer to a configuration element instance
 * @param   path    a path relative to the module
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getModuleResourcePath( std::shared_ptr<ConfigurationElement> element,
                                                      const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified executable instance.
 *
 * @param   executable  a pointer to an executable instance
 * @param   path        a path relative to the module
 *
 * @return  a system valid path
 */
CORE_API std::filesystem::path getModuleResourcePath(const IExecutable* executable,
                                                     const std::filesystem::path& path) noexcept;

/**
 * @brief   Loads all modules that can be found in the specified directory.
 *
 * @param   directory   a path to the directory to explore for modules
 */
CORE_API void addModules( const std::filesystem::path& directory );

/**
 * @brief   Load a module.
 *
 * @param   identifier  a string containing a module identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found module, or empty when it is not found
 */
CORE_API std::shared_ptr<Module> loadModule( const std::string& identifier, const Version& version = Version() );

/**
 * @brief   Load a library.
 *
 * @param   identifier  a string containing a module identifier
 * @param   dirName     optional folder where to look for the library
 *
 * @return  success
 */
CORE_API bool loadLibrary( const std::string& identifier, const std::string& dirName = std::string() );

/**
 * @brief   Starts the given module set profile.
 *
 * @param   path    a path to an xml profile file
 *
 * @return  a shared pointer to the started profile
 */
CORE_API std::shared_ptr< core::runtime::Profile > startProfile( const std::filesystem::path& path );

/**
 * @brief   Retrieves the module with the given identifier and version
 *
 * @param   identifier  a string containing a module identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found module, or empty when none
 */
CORE_API std::shared_ptr<Module> findModule( const std::string& identifier, const Version& version = Version() );

/**
 * @brief   Starts the module specified by the given identifier.
 *
 * @param   identifier  a string containing a module identifier
 */
CORE_API void startModule(const std::string& identifier);

} // namespace sight::core::runtime
