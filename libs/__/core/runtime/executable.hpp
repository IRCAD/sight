/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include <memory>
#include <string>

namespace sight::core::runtime
{

namespace utils
{

template<typename E>
class generic_executable_factory;

} // namespace utils

class module;
class runtime;

/**
 * @brief   Defines the base executable interface.
 *
 * An executable object is an instance created by an extension
 * point of a plugin.
 */
class SIGHT_CORE_CLASS_API executable
{
public:

    template<typename E>
    friend class core::runtime::utils::generic_executable_factory;

    /**
     * @brief   Destructor : does nothing.
     */
    SIGHT_CORE_API virtual ~executable() = default;

    /**
     * @brief   Retrieves the module the executable originates from.
     *
     * @return  a pointer to the originating module.
     */
    [[nodiscard]] virtual std::shared_ptr<module> get_module() const = 0;

protected:

    /**
     * @brief       Updates the module the executable originates from.
     *
     * @param[in]   _module  a pointer to the module the executable originates from
     */
    virtual void set_module(std::shared_ptr<module> _module) = 0;
};

} // namespace sight::core::runtime
