/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

#include <fwCore/base.hpp>

#include <filesystem>
#include <iterator>
#include <memory>
#include <string>

namespace fwRuntime
{
class Module;
class Profile;

/**
 * @brief   Initializes Sight runtime and discovers default modules. To be used when building an external application
 *          with Sight.
 *
 * @param   directory   a path to the directory where Sight is installed
 */
FWRUNTIME_API void init( const std::filesystem::path& directory );

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
 * @brief   Retrieves all executable objects for the point having the specified identifier
 *
 * @param   identifier  a string containing the extension point identifier
 * @param   attribute   a string containing the name of the element attribute containing
 *                      the executable identifier (default is "class")
 *
 * This method use the container type specified by the template parameter. The
 * type of the elements of the container must be std::shared_ptr< T >
 * or the compilation will fail (where T is the type of the executable you want to create).
 *
 * @return  a container containing shared pointers to all created executable instances
 */
template< typename Container, typename T >
const Container getAllExecutableForPoint( const std::string& identifier,
                                          const std::string& attribute = "class" )
{
    // Retrieves all configuration elements.
    auto elements = getAllConfigurationElementsForPoint(identifier);

    // Walks through collected configuration elements and create desired executable instances
    Container result;

    // Defines an insert iterator type for the executable container.
    typedef std::back_insert_iterator< Container > Inserter;

    Inserter iInserter( result );
    for( auto iElement : elements)
    {
        std::shared_ptr< ConfigurationElement >    element( iElement );
        std::shared_ptr< T >                       executable( createExecutableInstance< T >(element, attribute) );

        iInserter = executable;
    }
    return result;
}

/**
 * @brief   Retrieve the configuation element with the given identifier for the
 *          given extension point
 *
 * @param   identifier          a string containing the configuration element identifier
 * @param   pointIdentifier     a string containing the extension point identifier
 *
 * @return  a shared pointer to the found configuration element or null if none
 */
FWRUNTIME_API std::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string& identifier,
                                                                                const std::string& pointIdentifier );

/**
 * @brief   Retrieve the extension having the specified identifier.
 *
 * @param   identifier  a string containing an extension identifier
 *
 * @return  a shared pointer to the found extension or null if none
 */
FWRUNTIME_API std::shared_ptr<Extension> findExtension( const std::string& identifier );

/**
 * @brief   Retrieve the filesystem valid path of resources of a module.
 *
 * @param   moduleIdentifier    a string containing a module identifier
 *
 * @return  a system valid path
 * @deprecated  Bundle has been renamed to Module, please use getModuleResourcePath() instead
 */
[[deprecated]] FWRUNTIME_API std::filesystem::path getBundleResourcePath(const std::string& moduleIdentifier) noexcept;

/**
 * @brief   Retrieve the filesystem valid path of resources of a module.
 *
 * @param   moduleIdentifier    a string containing a module identifier
 *
 * @return  a system valid path
 */
FWRUNTIME_API std::filesystem::path getModuleResourcePath(const std::string& moduleIdentifier) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module having the specified identifier.
 *
 * @param   moduleIdentifier    a string containing a module identifier
 * @param   path                a module relative path
 *
 * @return  a system valid path
 * @deprecated  Bundle has been renamed to Module, please use getModuleResourceFilePath() instead
 */
[[deprecated]] FWRUNTIME_API std::filesystem::path getBundleResourceFilePath(const std::string& moduleIdentifier,
                                                                             const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module having the specified identifier.
 *
 * @param   moduleIdentifier    a string containing a module identifier
 * @param   path                a module relative path
 *
 * @return  a system valid path
 */
FWRUNTIME_API std::filesystem::path getModuleResourceFilePath(const std::string& moduleIdentifier,
                                                              const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a module identifier.
 * For instance for "module-0.2/dir/file.txt", the function returns
 * "/home/login/sight/build/share/module-0.2/dir/file.txt"
 *
 * @param   path                relative path whose first element is a module identifier
 *
 * @return  a system valid path
 * @deprecated  module has been renamed to Module, please use getModuleResourceFilePath() instead
 */
[[deprecated]] FWRUNTIME_API std::filesystem::path getBundleResourceFilePath(
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
FWRUNTIME_API std::filesystem::path getModuleResourceFilePath(const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a library identifier.
 * For instance for "fwLib-0.2/dir/file.txt", the function returns
 * "/home/login/sight/build/share/fwLib-0.2/dir/file.txt"
 *
 * @param   path                relative path whose first element is a library identifier
 *
 * @return  a system valid path
 */
FWRUNTIME_API std::filesystem::path getLibraryResourceFilePath(const std::filesystem::path& path) noexcept;

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
FWRUNTIME_API std::filesystem::path getResourceFilePath(const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified module.
 *
 * @param   module  a pointer to a module instance
 * @param   path    a path relative to the module
 *
 * @return  a system valid path
 * @deprecated  Bundle has been renamed to Module, please use getModuleResourcePath() instead
 */
[[deprecated]] FWRUNTIME_API std::filesystem::path getBundleResourcePath( std::shared_ptr<Module> module,
                                                                          const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified module.
 *
 * @param   module  a pointer to a module instance
 * @param   path    a path relative to the module
 *
 * @return  a system valid path
 */
FWRUNTIME_API std::filesystem::path getModuleResourcePath( std::shared_ptr<Module> module,
                                                           const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module of the specified configuration element.
 *
 * @param   element a shared pointer to a configuration element instance
 * @param   path    a path relative to the module
 *
 * @return  a system valid path
 * @deprecated  Bundle has been renamed to Module, please use getModuleResourcePath() instead
 */
[[deprecated]] FWRUNTIME_API std::filesystem::path getBundleResourcePath( std::shared_ptr<ConfigurationElement> element,
                                                                          const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the module of the specified configuration element.
 *
 * @param   element a shared pointer to a configuration element instance
 * @param   path    a path relative to the module
 *
 * @return  a system valid path
 */
FWRUNTIME_API std::filesystem::path getModuleResourcePath( std::shared_ptr<ConfigurationElement> element,
                                                           const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified executable instance.
 *
 * @param   executable  a pointer to an executable instance
 * @param   path        a path relative to the module
 *
 * @return  a system valid path
 * @deprecated  Bundle has been renamed to Module, please use getModuleResourcePath() instead
 */
[[deprecated]] FWRUNTIME_API std::filesystem::path getBundleResourcePath(const IExecutable* executable,
                                                                         const std::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified executable instance.
 *
 * @param   executable  a pointer to an executable instance
 * @param   path        a path relative to the bundle
 *
 * @return  a system valid path
 */
FWRUNTIME_API std::filesystem::path getModuleResourcePath(const IExecutable* executable,
                                                          const std::filesystem::path& path) noexcept;

/**
 * @brief   Loads all bundles that can be found in the specified directory.
 *
 * @param   directory   a path to the directory to explore for bundles
 * @deprecated  Bundle has been renamed to Module, please use addModules() instead
 */
[[deprecated]] FWRUNTIME_API void addBundles( const std::filesystem::path& directory );

/**
 * @brief   Loads all bundles that can be found in the specified directory.
 *
 * @param   directory   a path to the directory to explore for bundles
 */
FWRUNTIME_API void addModules( const std::filesystem::path& directory );

/**
 * @brief   Load a bundle.
 *
 * @param   identifier  a string containing a bundle identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found bundle, or empty when it is not found
 * @deprecated  Bundle has been renamed to Module, please use loadModule() instead
 */
[[deprecated]] FWRUNTIME_API std::shared_ptr<Module> loadBundle( const std::string& identifier,
                                                                 const Version& version = Version() );

/**
 * @brief   Load a bundle.
 *
 * @param   identifier  a string containing a bundle identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found bundle, or empty when it is not found
 */
FWRUNTIME_API std::shared_ptr<Module> loadModule( const std::string& identifier, const Version& version = Version() );

/**
 * @brief   Starts the given bundle set profile.
 *
 * @param   path    a path to an xml profile file
 *
 * @return  a shared pointer to the started profile
 */
FWRUNTIME_API std::shared_ptr< ::fwRuntime::Profile > startProfile( const std::filesystem::path& path );

/**
 * @brief   Retrieves the bundle with the given identifier and version
 *
 * @param   identifier  a string containing a bundle identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found bundle, or empty when none
 * @deprecated  Bundle has been renamed to Module, please use findModule() instead
 */
[[deprecated]] FWRUNTIME_API std::shared_ptr<Module> findBundle( const std::string& identifier,
                                                                 const Version& version = Version() );

/**
 * @brief   Retrieves the bundle with the given identifier and version
 *
 * @param   identifier  a string containing a bundle identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found bundle, or empty when none
 */
FWRUNTIME_API std::shared_ptr<Module> findModule( const std::string& identifier, const Version& version = Version() );

/**
 * @brief   Starts the bundle specified by the given identifier.
 *
 * @param   identifier  a string containing a bundle identifier
 * @deprecated  Bundle has been renamed to Module, please use startBundle() instead
 */
[[deprecated]] FWRUNTIME_API void startBundle(const std::string& identifier);

/**
 * @brief   Starts the bundle specified by the given identifier.
 *
 * @param   identifier  a string containing a bundle identifier
 */
FWRUNTIME_API void startModule(const std::string& identifier);

} // namespace fwRuntime
