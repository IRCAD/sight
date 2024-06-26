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

namespace sight::core::runtime
{

class executable_factory;

/**
 * @brief   Defines an executable factory registry class.
 *
 * An instance of this class is responsible for the registration of an
 * executable factory instance in the runtime environment. The factory instace
 * gets registered as soon as a registry instance gets created.
 * That's why this class is design to be instantiated as static object.
 *
 *
 */
struct SIGHT_CORE_CLASS_API executable_factory_registry
{
    /**
     * @brief       Constructor
     *
     * @param[in]   _factory a shared pointer to an executable factory instance to manage
     * @pre         Loading module exists.
     */
    SIGHT_CORE_API executable_factory_registry(std::shared_ptr<executable_factory> _factory);
};

} // namespace sight::core::runtime
