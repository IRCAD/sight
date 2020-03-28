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
#include "fwRuntime/Plugin.hpp"

namespace fwRuntime
{

namespace impl
{

/**
 * @brief   Implements a default plugin for modules that don't provide a
 *          ::fwRuntime::IPlugin interface implementation. This plugin does nothing
 *          by default. It has not been design to be subclassed, but subclassing
 *          is neither forbidden.
 *
 * @remark  The factory for this executable structure is registered by the runtime
 *          it-self.
 */
class EmptyPlugin : public Plugin
{
public:
    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~EmptyPlugin() noexcept
    {
    }

    // Overrides
    virtual void start();

    // Overrides
    virtual void stop() noexcept;

};

} // namespace impl

} // namespace fwRuntime
