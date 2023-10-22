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

#include "core/config.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <set>
#include <string>

namespace sight::core::runtime
{

class extension;
struct extension_registry;
class executable;
class executable_factory;
class plugin;
class runtime;

/**
 * @brief   Defines the module class.
 *
 */
class CORE_CLASS_API module
{
public:

    /// Defines the extension container type.
    typedef std::set<SPTR(extension)> extension_container;

    virtual ~module() = default;

    /**
     * @brief   Retrieves the module identifier.
     *
     * @return  a string containing the module identifier
     */
    [[nodiscard]] CORE_API virtual const std::string& identifier() const = 0;

    /**
     * @brief   Retrieves the library name if it exists.
     *
     * @return  a path representing the module location, can be empty if no library is set
     */
    [[nodiscard]] CORE_API virtual std::string get_library_name() const = 0;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    [[nodiscard]] CORE_API virtual const std::filesystem::path& get_library_location() const = 0;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    [[nodiscard]] CORE_API virtual const std::filesystem::path& get_resources_location() const = 0;

    /**
     * @brief   Retrieves the class representing the module executable part.
     *
     * @return  a string containing the module's plugin class
     */
    [[nodiscard]] CORE_API virtual std::string get_class() const = 0;

    /**
     * @brief   Retrieves the plugin instance for the specified module identifier.
     *
     * @return  a shared pointer to a plugin instance or null if the module has not been started.
     */
    [[nodiscard]] CORE_API virtual SPTR(plugin) get_plugin() const = 0;

    /**
     * @brief       Retrieves the value of the given parameter
     * @remark      When no such parameter has been found, an empty string is returned.
     * @param[in]   identifier  a string containing a parameter identifier
     * @return      a string containing the parameter value
     */
    [[nodiscard]] CORE_API virtual std::string get_parameter_value(const std::string& _identifier) const = 0;

    /**
     * @brief   Tells if a parameter exists.
     * @return  true or false
     */
    [[nodiscard]] CORE_API virtual bool has_parameter(const std::string& _name) const = 0;
    /**
     * @brief   Returns the list of extensions contained in this module.
     */
    [[nodiscard]] CORE_API virtual extension_container extensions() const = 0;

    /**
     * @name        State Management
     */
    //@{
    /**
     * @brief   Starts the module.
     *
     * @remark  The module must be enabled to be able to start.
     */
    CORE_API virtual void start()                          = 0;
    CORE_API virtual void stop()                           = 0;
    [[nodiscard]] CORE_API virtual bool is_started() const = 0;
    /**
     * @brief   Tells if the module is enabled.
     */
    [[nodiscard]] CORE_API virtual bool enabled() const = 0;
    //@}
};

} // namespace sight::core::runtime
