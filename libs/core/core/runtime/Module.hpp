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

#include <core/base.hpp>

#include <filesystem>
#include <set>
#include <string>

namespace sight::core::runtime
{

struct Extension;
struct ExtensionRegistry;
class IExecutable;
class ExecutableFactory;
struct IPlugin;
class Runtime;

/**
 * @brief   Defines the module class.
 *
 */
class CORE_CLASS_API Module
{
public:

    /// Defines the extension container type.
    typedef std::set<SPTR(Extension)> ExtensionContainer;

    virtual ~Module() = default;

    /**
     * @brief   Retrieves the module identifier.
     *
     * @return  a string containing the module identifier
     */
    CORE_API virtual const std::string& getIdentifier() const = 0;

    /**
     * @brief   Retrieves the library name if it exists.
     *
     * @return  a path representing the module location, can be empty if no library is set
     */
    CORE_API virtual const std::string getLibraryName() const = 0;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    CORE_API virtual const std::filesystem::path& getLibraryLocation() const = 0;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    CORE_API virtual const std::filesystem::path& getResourcesLocation() const = 0;

    /**
     * @brief   Retrieves the class representing the module executable part.
     *
     * @return  a string containing the module's plugin class
     */
    CORE_API virtual const std::string getClass() const = 0;

    /**
     * @brief   Retrieves the plugin instance for the specified module identifier.
     *
     * @return  a shared pointer to a plugin instance or null if the module has not been started.
     */
    CORE_API virtual SPTR(IPlugin) getPlugin() const = 0;

    /**
     * @brief       Retrieves the value of the given parameter
     * @remark      When no such parameter has been found, an empty string is returned.
     * @param[in]   identifier  a string containing a parameter identifier
     * @return      a string containing the parameter value
     */
    CORE_API virtual const std::string getParameterValue(const std::string& identifier) const = 0;

    /**
     * @brief   Tells if a parameter exists.
     * @return  true or false
     */
    CORE_API virtual bool hasParameter(const std::string& name) const = 0;
    /**
     * @brief   Returns the list of extensions contained in this module.
     */
    CORE_API virtual ExtensionContainer getExtensions() const = 0;

    /**
     * @name        State Management
     */
    //@{
    /**
     * @brief   Starts the module.
     *
     * @remark  The module must be enabled to be able to start.
     */
    CORE_API virtual void start()     = 0;
    CORE_API virtual void stop()      = 0;
    CORE_API virtual bool isStarted() = 0;
    /**
     * @brief   Tells if the module is enabled.
     */
    CORE_API virtual bool isEnabled() const = 0;
    //@}
};

} // namespace sight::core::runtime
