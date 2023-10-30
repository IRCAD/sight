/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/base.hpp"
#include "core/config.hpp"
#include "core/runtime/detail/extension.hpp"
#include "core/runtime/executable_factory.hpp"
#include "core/runtime/plugin.hpp"

#include <regex>
#include <set>

namespace sight::core::runtime::detail
{

class extension_point;
class module;

/**
 * @brief   Defines the runtime class.
 *
 */
class runtime
{
public:

    /**
     * @name    Type Definitions
     */
    //@{
    /// Defines the module container type.
    using module_container = std::set<std::shared_ptr<core::runtime::module> >;
    /// Defines the extension container type.
    using extension_container = std::set<std::shared_ptr<extension> >;
    /// Defines the extension container type.
    using extension_iterator = extension_container::iterator;
    //@}

    /// Regex used in different places to find the module part in a path
    static const std::regex MATCH_MODULE_PATH;

    /// Retrieves the singleton
    static runtime& get();

    /**
     * @brief   Constructor.
     */
    runtime();
    /**
     * @brief   Destructor : does nothing.
     */
    ~runtime();

    /**
     * @name    Public API implementation
     * @{
     */

    /**
     * @brief       Adds all module found in the given path.
     *
     * @param[in]   repository  a path that may containing modules
     */
    void add_modules(const std::filesystem::path& _repository);

    /**
     * @brief       Retrieves the module for the specified identifier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     */
    [[nodiscard]] SPTR(core::runtime::module) find_module(const std::string& _identifier) const;

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * An attempt is made to retrieve a registered executable factory. If none
     * is found, the creation will fail.
     *
     * @remark      This method will not try to load any module.
     *
     * @param[in]   type    a string containing an executable type
     *
     * @return      a pointer to the created executable instance
     */
    [[nodiscard]] executable* create_executable_instance(const std::string& _type) const;

    /**
     * @brief       Retrieves the extension instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension identifier
     *
     * @return      a shared pointer to the found extension instance or null if none
     */
    [[nodiscard]] std::shared_ptr<core::runtime::extension> find_extension(const std::string& _identifier) const;

    /// Return all modules known by the runtime
    module_container modules() const;

    /// Get the path where libraries, modules and share folder are located.
    [[nodiscard]] std::filesystem::path working_path() const;

    /// Get the path where libraries, modules and share folder are located, for each known location
    [[nodiscard]] std::vector<std::pair<std::filesystem::path,
                                        std::filesystem::path> > get_repositories_path() const;
    //@}

    /**
     * @name    Modules
     *
     * @{
     */

    /**
     * @brief       Adds a new module instance to the runtime system.
     *
     * @remark      The given module state will be altered according to the current configuration rules.
     * @param[in]   module  a shared pointer to the module instance to add
     */
    void add_module(std::shared_ptr<detail::module> _module);

    /**
     * @brief       Unregister a module instance to the runtime system.
     *
     * @param[in]   module  a shared pointer to the module instance to unregister
     */
    void unregister_module(std::shared_ptr<detail::module> _module);

    /**
     * @brief       Retrieves the enabled module for the specified identifier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     */
    [[nodiscard]] std::shared_ptr<core::runtime::module> find_enabled_module(const std::string& _identifier) const;

    //@}

    /**
     * @name    Executable, Executable Factories & Dynamic Executable Instantiation
     *
     * @{
     */

    /**
     * @brief       Adds a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    void add_executable_factory(std::shared_ptr<executable_factory> _factory);

    /**
     * @brief       Unregister a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    void unregister_executable_factory(std::shared_ptr<executable_factory> _factory);

    /**
     * @brief       Retrieves the executable factory for the given identifier.
     *
     * @param[in]   type    a string containing a type identifier
     *
     * @return      a sgared pointer to the found executable factory or null of none
     */
    [[nodiscard]] std::shared_ptr<executable_factory> find_executable_factory(const std::string& _type) const;

    /**
     * @brief       Retrieves the plugin instance for the specified module.
     *
     * @param[in]   module  a shared pointer to a module instance
     *
     * @return      a shared pointer to a plugin instance or null if none
     */
    [[nodiscard]] std::shared_ptr<plugin> get_plugin(std::shared_ptr<module> _module) const;

    //@}

    /**
     * @name    Extensions
     *
     * @{
     */

    /**
     * @brief       Registers a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    void add_extension(std::shared_ptr<detail::extension> _extension);

    /**
     * @brief       Unregister a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    void unregister_extension(std::shared_ptr<detail::extension> _extension);

    /// Retrieves the extension collection.
    extension_container extensions() const;
    extension_iterator extensions_begin() const;
    extension_iterator extensions_end() const;
    //@}

    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Registers a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    void add_extension_point(std::shared_ptr<extension_point> _point);

    /**
     * @brief       Unregister a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    void unregister_extension_point(std::shared_ptr<extension_point> _point);

    /**
     * @brief       Retrieves the extension point instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension point identifier
     *
     * @return      a shared pointer to the found extension point instance or null if none
     */
    [[nodiscard]] std::shared_ptr<extension_point> find_extension_point(const std::string& _identifier) const;
    //@}

private:

    ///< Defines the executable factory container type.
    using executable_factory_container = std::set<std::shared_ptr<executable_factory> >;

    ///< Defines the extension point container type.
    using extension_point_container = std::set<std::shared_ptr<extension_point> >;

    ///< Defines the plugin container type.
    using plugin_container = std::vector<std::shared_ptr<plugin> >;

    static std::shared_ptr<runtime> s_instance; ///< The runtime instance.

    executable_factory_container m_executable_factories; ///< Contains all executable factories.
    extension_container m_extensions;                    ///< Contains all registered extensions.
    extension_point_container m_extension_points;        ///< Contains all registered extension points.
    module_container m_modules;                          ///< Contains all modules.
    plugin_container m_plugins;                          ///< Contains all plugins.

    std::filesystem::path m_working_path; ///< Main path where Modules and share folder are located.

    /// All paths as a pair of <binary path/resource path> containing modules, added from the CLI
    std::vector<std::pair<std::filesystem::path, std::filesystem::path> > m_repositories;
};

} // namespace sight::core::runtime::detail
