/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __SERVICESREG_PLUGIN_HPP__
#define __SERVICESREG_PLUGIN_HPP__

#include "servicesReg/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace servicesReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
struct SERVICESREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    SERVICESREG_API ~Plugin() noexcept;

    /// Overrides start method. Service declarations (services declared in fwServices libraries ).
    SERVICESREG_API void start();

    /// Overrides initialize method
    SERVICESREG_API void initialize();

    /// Overrides uninitialize method
    SERVICESREG_API void uninitialize();

    /// Overrides stop method. Do nothing
    SERVICESREG_API void stop() noexcept;

};

} // namespace servicesReg

#endif // __SERVICESREG_PLUGIN_HPP__
