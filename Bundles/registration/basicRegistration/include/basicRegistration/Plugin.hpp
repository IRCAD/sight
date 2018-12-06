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


#ifndef __BASICREGISTRATION_PLUGIN_HPP__
#define __BASICREGISTRATION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "basicRegistration/config.hpp"

namespace basicRegistration
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
struct BASICREGISTRATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    BASICREGISTRATION_API ~Plugin() noexcept;

    /// Overrides
    BASICREGISTRATION_API void start();

    /// Overrides
    BASICREGISTRATION_API void stop() noexcept;

};


} // namespace basicRegistration


#endif //__BASICREGISTRATION_PLUGIN_HPP__
