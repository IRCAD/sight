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

#include <memory>
#include <string>

namespace fwRuntime
{

namespace utils
{
template<typename E> class GenericExecutableFactory;
}

class Module;
struct ConfigurationElement;
class Runtime;

/**
 * @brief   Defines the base executable interface.
 *
 * An executable object is an instance created by an extension
 * point of a plugin.
 */
class FWRUNTIME_CLASS_API IExecutable
{
public:
    template<typename E> friend class ::fwRuntime::utils::GenericExecutableFactory;

    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~IExecutable();

    /**
     * @brief   Retrieves the module the executable originates from.
     *
     * @return  a pointer to the originating module.
     */
    [[deprecated]] virtual std::shared_ptr<Module> getBundle() const = 0;

    /**
     * @brief   Retrieves the module the executable originates from.
     *
     * @return  a pointer to the originating module.
     */
    virtual std::shared_ptr<Module> getModule() const = 0;

    /**
     * @brief       Initializes the executable instance with the specified
     *              configuration element.
     *
     * @param[in]   configuration   a shared pointer to the configuration element used to
     *              trigger this execution
     */
    virtual void setInitializationData( const std::shared_ptr<ConfigurationElement> configuration ) = 0;

protected:

    /**
     * @brief       Updates the module the executable originates from.
     *
     * @param[in]   module  a pointer to the module the executable originates from
     */
    virtual void setModule( std::shared_ptr< Module > module ) = 0;

};

} // namespace fwRuntime
