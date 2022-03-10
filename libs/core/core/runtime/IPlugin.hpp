/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/config.hpp"
#include "core/runtime/IExecutable.hpp"
#include "core/runtime/RuntimeException.hpp"

#include <string>

namespace sight::core::runtime
{

/**
 * @brief   Defines the plugin interface.
 *
 */
struct CORE_CLASS_API IPlugin : public IExecutable
{
    /**
     * @brief   Destructor : does nothing.
     */
    CORE_API virtual ~IPlugin();

    /**
     * @brief   Notifies the plugin about its start.
     */
    virtual void start() = 0;

    /**
     * @brief   Notifies the plugin about its stop.
     */
    virtual void stop() noexcept = 0;
};

} // namespace sight::core::runtime
