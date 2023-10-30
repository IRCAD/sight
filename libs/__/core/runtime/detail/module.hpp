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

#include "core/runtime/detail/dl/library.hpp"
#include "core/runtime/detail/extension.hpp"
#include "core/runtime/detail/runtime.hpp"
#include "core/runtime/module.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <map>
#include <set>
#include <string>

namespace sight::core::runtime::detail
{

class extension_point;

/**
 * @brief   Defines the module class.
 *
 */
class module : public core::runtime::module,
               public std::enable_shared_from_this<module>
{
public:

    /**
     * @name    Type definitions
     */
    //@{
    using extension_impl_container = std::set<SPTR(detail::extension)>;

    using executable_factory_container      = std::set<SPTR(executable_factory)>;
    using executable_factory_const_iterator = executable_factory_container::const_iterator;

    using extension_point_container      = std::set<SPTR(extension_point)>;
    using extension_point_const_iterator = extension_point_container::const_iterator;

    using extension_const_iterator = extension_impl_container::const_iterator;
    //@}

    /**
     * @brief       Constructor.
     *
     * @param[in]   location        a path to the directory containing the module
     * @param[in]   id              a string containing the module identifier
     * @param[in]   version         a string containing the module version
     * @param[in]   pluginClass     a string containing the module's pugin class name
     *
     * @todo        test parameters validity
     *
     */
    module(
        const std::filesystem::path& _location,
        std::string _id,
        std::string _c = "",
        int _priority  = 0
    );

    /**
     * @brief   Assignement operator.
     *
     * @remark  Assignement is forbidden for this class.
     */
    void operator=(const module&) = delete;

    /**
     * @name    Public API implementation
     * @{
     */

    /**
     * @brief   Starts the module.
     * @remark  The module must be enabled to be able to start.
     */
    void start() final;
    void stop() final;

    /**
     * @brief   Retrieves the module identifier.
     *
     * @return  a string containing the module identifier
     */
    const std::string& identifier() const final;

    /**
     * @brief   Retrieves the library name if it exists.
     *
     * @return  a path representing the module location, can be empty if no library is set
     */
    std::string get_library_name() const final;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    const std::filesystem::path& get_library_location() const final;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    const std::filesystem::path& get_resources_location() const final;

    /**
     * @brief   Retrieves the class representing the module executable part.
     *
     * @return  a string containing the module's plugin class
     */
    std::string get_class() const final;

    /**
     * @brief   Retrieves the plugin instance for the specified module identifier.
     *
     * @return  a shared pointer to a plugin instance or null if the module has not been started.
     */
    SPTR(plugin) get_plugin() const final;

    /** @copydoc core::runtime::module */
    std::string get_parameter_value(const std::string& _identifier) const final;

    /**
     * @brief   Tells if a parameter exists.
     * @return  true or false
     */
    bool has_parameter(const std::string& _identifier) const final;

    /// @copydoc core::runtime::module::getExtensions
    extension_container extensions() const final;

    /// @copydoc core::runtime::module::isEnabled
    bool enabled() const final;
    //@}

    /**
     * @brief   Retrieves the pointer to the module that is currently loading its dynamic libraries
     */
    static SPTR(module) get_loading_module();

    /**
     * @name    Dynamic Libraries
     */
    //@{
    /**
     * @brief       Sets the specified library to the module.
     *
     * @param[in]   library a shared pointer to the library to add
     */
    void set_library(SPTR(dl::library) _library);
    //@}

    /**
     * @name    Executable Factories & Dynamic Objects Instantiation
     */
    //@{
    /**
     * @brief       Adds an executable factory instance to the module.
     *
     * @param[in]   factory a shared pointer to the executable factory instance to add
     */
    void add_executable_factory(SPTR(executable_factory) _factory);

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * @param   type    a string containing an executable type
     *
     * @return  a pointer to the created executable instance
     *
     * @todo    not implemented and not used !!!!!!!
     */
    executable* create_executable_instance(const std::string& _type);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed executable factory collection.
     *
     * @return  an iterator
     */
    executable_factory_const_iterator executable_factories_begin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed executable factory collection.
     *
     * @return  an iterator
     */
    executable_factory_const_iterator executable_factories_end() const;

    /**
     * @brief       Retrieves the executable factory instance for the specified type name.
     *
     * @param[in]   type    a string containing a type name
     *
     * @return      a shared pointer to the found executable factory instance or null if none
     */
    SPTR(executable_factory) find_executable_factory(const std::string& _type) const;
    //@}

    /**
     * @name    Extensions
     */
    //@{
    /**
     * @brief       Adds the specified extension to the module.
     *
     * @param[in]   extension   a shared pointer to the extension to add
     */
    void add_extension(SPTR(detail::extension) _extension);

    /**
     * @brief       Tells if an specific extension exists.
     *
     * @param[in]   identifier  the extension identifier
     *
     * @return      true or false
     */
    bool has_extension(const std::string& _identifier) const;

    /**
     * @brief       Search a specific extension in the module to enable or
     *              disable it
     *
     * @remark      if the extension is not found, the method do nothing.
     *
     * @param[in]   identifier  the extension identifier
     * @param[in]   enable      enable or disable this extension
     */
    void set_enable_extension(const std::string& _identifier, bool _enable);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed extension collection.
     *
     * @return  an iterator
     */
    extension_const_iterator extensions_begin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed extension collection.
     *
     * @return  an iterator
     */
    extension_const_iterator extensions_end() const;
    //@}

    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Adds the specified extension point to the module.
     *
     * @param[in]   extension   a shared pointer to the extension point to add
     */
    void add_extension_point(SPTR(extension_point) _extension);

    /**
     * @brief       Retrieves the extension point for the given identifier.
     *
     * @param[in]   identifier  a string containing the extension point identifier
     *
     * @return      a shared pointer to the found extension point, may be empty if none
     */
    SPTR(extension_point) find_extension_point(const std::string& _identifier) const;

    /**
     * @brief       Tells if a specific extension point exists.
     *
     * @param[in]   identifier  the extension point identifier
     *
     * @return      true or false
     */
    bool has_extension_point(const std::string& _identifier) const;

    /**
     * @brief       Search a specific extension point in the module to enable or
     *              disable it
     *
     * @remark      if the extension point is not found, the method do nothing.
     *
     * @param[in]   identifier  the extension point identifier
     * @param[in]   enable      enable or disable this extension point
     */
    void set_enable_extension_point(const std::string& _identifier, bool _enable);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed extension point collection.
     *
     * @return  an iterator
     */
    extension_point_const_iterator extension_points_begin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed extension point collection.
     *
     * @return  an iterator
     */
    extension_point_const_iterator extension_points_end() const;
    //@}

    /**
     * @name    Misc
     */
    //@{
    /**
     * @brief       Adds a requirement to the module.
     *
     * @param[in]   requirement a string containing a module identifier that is required
     */
    void add_requirement(const std::string& _requirement);
    //@}

    /**
     * @name        State Management
     */
    //@{
    /**
     * @brief   Changes the enable state of the module.
     *
     * @remark  It is possible to disable a started module but this
     *          will have no effect.
     */
    void set_enable(bool _state);
    //@}

    /**
     * @brief   Initialize the module.
     *
     * @remark  The module and it's own dependencies must be started to be able to be initialized.
     */
    void initialize();

    void uninitialize();
    //@}

    /**
     * @name    Parameters Management
     */
    //@{
    /**
     * @brief       Adds a parameter to the module.
     *
     * @param[in]   identifier  a string containing the parameter identifier
     * @param[in]   value       a string containing the parameter value
     */
    void add_parameter(const std::string& _identifier, const std::string& _value);
    //@}

    bool is_started() const override
    {
        return m_started;
    }

    //------------------------------------------------------------------------------

    int priority() const
    {
        return m_priority;
    }

    static std::string get_module_str(const std::string& _identifier);

private:

    using requirement_container = std::set<std::string>; ///< Defines the requirement container
    // type.
    using parameter_container = std::map<std::string, std::string>; ///< defines the parameter container type

    static SPTR(module) s_loading_module; ///< a pointer to the module that is currently loading its
    // dynamic libraries

    std::filesystem::path m_library_location;            ///< the path to the module libraries
    const std::filesystem::path m_resources_location;    ///< the path to the module resources
    const std::string m_identifier;                      ///< a string containing the module identifier
    const std::string m_class;                           ///< a string containing the module's plugin class name
    extension_impl_container m_extensions;               ///< all extensions
    extension_point_container m_extension_points;        ///< all extension points
    executable_factory_container m_executable_factories; ///< all executable factories
    requirement_container m_requirements;                ///< all requirements of the module
    SPTR(plugin)  m_plugin;                              ///< a shared pointer to the plugin instance
    SPTR(dl::library) m_library;                         ///< library that is part of the module
    parameter_container m_parameters;                    ///< all parameters

    bool m_enabled {true}; ///< a boolean telling if the module is enabled or not
    bool m_started {false};
    int m_priority; ///< start order, lower is more favorable

    /**
     * @brief   Load module's library in the current process.
     */
    void load_libraries();

    /**
     * @brief   loads all requirements needed by the module
     */
    void load_requirements();

    /**
     * @brief   Starts the plugin associated to the module.
     */
    void start_plugin();
};

} // namespace sight::core::runtime::detail
