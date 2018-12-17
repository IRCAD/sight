/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiNetwork/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace uiNetwork
{

/**
 * @brief   This class is started when the bundles is loaded
 * @deprecated uiNetwork bundle is no longer supported and will be removed in 19.0
 */
struct UINETWORK_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    UINETWORK_API ~Plugin() noexcept;

    /// Overrides
    UINETWORK_API void start();

    /// Overrides
    UINETWORK_API void stop() noexcept;

};

} // namespace uiNetwork
