/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwCore/base.hpp"

#include "fwRuntime/config.hpp"
#include "fwRuntime/dl/Library.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

#include <filesystem>
#include <map>
#include <set>
#include <string>

namespace fwRuntime
{
struct Extension;
struct ExtensionRegistry;
class IExecutable;
struct ExecutableFactory;
struct IPlugin;
class Runtime;

/**
 * @brief   Defines the bundle class.
 *
 */
class FWRUNTIME_CLASS_API Bundle
{
public:
    virtual ~Bundle();
    /**
     * @name    Misc
     */
    //@{
    /**
     * @brief   Retrieves the bundle identifier.
     *
     * @return  a string containing the bundle identifier
     */
    FWRUNTIME_API virtual const std::string& getIdentifier() const = 0;

    /**
     * @brief   Retrieves the bundle location.
     *
     * @return  a path representing the bundle location
     */
    FWRUNTIME_API virtual const std::filesystem::path& getLibraryLocation() const = 0;

    /**
     * @brief   Retrieves the bundle location.
     *
     * @return  a path representing the bundle location
     */
    FWRUNTIME_API virtual const std::filesystem::path& getResourcesLocation() const = 0;

    /**
     * @brief   Retrieves the class representing the bundle executable part.
     *
     * @return  a string containing the bundle's plugin class
     */
    FWRUNTIME_API virtual const std::string getClass() const = 0;

    /**
     * @brief   Retrieves the version of the bundle.
     *
     * @return  the bundle version
     */
    FWRUNTIME_API virtual const Version& getVersion() const = 0;

    /**
     * @brief   Retrieves the plugin instance for the specified bundle identifier.
     *
     * @return  a shared pointer to a plugin instance or null if the bundle has not been started.
     */
    FWRUNTIME_API virtual SPTR( IPlugin ) getPlugin() const = 0;

    /**
     * @brief       Retrieves the value of the given parameter
     * @remark      When no such parameter has been found, an empty string is returned.
     * @param[in]   identifier  a string containing a parameter identifier
     * @return      a string containing the parameter value
     */
    FWRUNTIME_API virtual const std::string getParameterValue( const std::string& identifier ) const = 0;

    /**
     * @brief   Tells if a parameter exists.
     * @return  true or false
     */
    FWRUNTIME_API virtual bool hasParameter( const std::string& name ) const = 0;
    //@}

    /**
     * @name        State Management
     */
    //@{
    /**
     * @brief   Starts the bundle.
     *
     * @remark  The bundle must be enabled to be able to start.
     */
    FWRUNTIME_API virtual void start() = 0;
    FWRUNTIME_API virtual void stop()      = 0;
    FWRUNTIME_API virtual bool isStarted() = 0;
    //@}

};

//------------------------------------------------------------------------------

inline Bundle::~Bundle()
{
}

} // namespace fwRuntime
