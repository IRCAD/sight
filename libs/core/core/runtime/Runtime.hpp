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
#include "core/runtime/RuntimeException.hpp"
#include <core/macros.hpp>

#include <filesystem>
#include <set>
#include <vector>

namespace sight::core::runtime
{

class Module;
struct ConfigurationElement;
struct Extension;
class IExecutable;
class ExecutableFactory;
struct IPlugin;

}

namespace sight::core::runtime
{

/**
 * @brief   Defines the runtime class.
 */
class CORE_CLASS_API Runtime
{
public:

    /**
     * @name    Type Definitions
     */
    //@{
    /// Defines the module container type.
    typedef std::set<std::shared_ptr<Module> > ModuleContainer;
    /// Defines the extension container type.
    typedef std::set<std::shared_ptr<Extension> > ExtensionContainer;
    /// Defines the extension container type.
    typedef ExtensionContainer::iterator ExtensionIterator;
    //@}

    /**
     * @brief   Retrieves the default runtime instance.
     */
    CORE_API static Runtime* getDefault();
    CORE_API static Runtime& get();

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Runtime();

    /**
     * @brief       Set the working path where Modules and share folder are located.
     *
     * @param[in]   workingPath a std path.
     */
    [[deprecated("To be removed in Sight 22.0, Sight location is now detected automatically")]]
    CORE_API void setWorkingPath(const std::filesystem::path& workingPath);

    /**
     * @brief       Get the path where Modules and share folder are located.
     * @return      a std path.
     */
    CORE_API virtual std::filesystem::path getWorkingPath() const = 0;

    /**
     * @brief       Get the path where Modules and share folder are located.
     * @return      a vector of std path.
     */
    CORE_API virtual std::vector<std::pair<std::filesystem::path,
                                           std::filesystem::path> > getRepositoriesPath() const = 0;

    /**
     * @name    Modules
     *
     * @{
     */

    /**
     * @brief       Adds all modules found in the given path.
     *
     * @param[in]   repository  a path that may containing modules
     */
    CORE_API virtual void addModules(const std::filesystem::path& repository) = 0;

    /**
     * @brief       Retrieves the module for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     */
    CORE_API virtual std::shared_ptr<Module> findModule(const std::string& identifier) const = 0;

    /**
     * @name    Extensions
     *
     * @{
     */
    /**
     * @brief   Retrieves the extension collection.
     *
     * @return  an iterator
     */
    CORE_API virtual ExtensionContainer getExtensions() = 0;

    /**
     * @brief       Retrieves the extension instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension identifier
     *
     * @return      a shared pointer to the found extension instance or null if none
     */
    CORE_API virtual std::shared_ptr<Extension> findExtension(const std::string& identifier) const = 0;

    //@}

    /// Return all modules known by the runtime
    CORE_API virtual ModuleContainer getModules() = 0;

protected:

    /**
     * @brief   Constructor.
     */
    Runtime();

private:

    /// Defines the executable factory container type.
    typedef std::set<std::shared_ptr<ExecutableFactory> > ExecutableFactoryContainer;

    /// Defines the plugin container type.
    typedef std::vector<std::shared_ptr<IPlugin> > PluginContainer;

    /// The runtime instance.
    static std::shared_ptr<Runtime> m_instance;

    /// Contains all registered extensions.
    ExtensionContainer m_extensions;
};

/// Utility function used to trim leading '::' in identifiers of modules, services, objects, extensions, etc...
CORE_API std::string filterID(const std::string& identifier);

} // namespace sight::core::runtime
