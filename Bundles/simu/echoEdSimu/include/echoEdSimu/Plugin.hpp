/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#ifndef __ECHOEDSIMU_PLUGIN_HPP__
#define __ECHOEDSIMU_PLUGIN_HPP__

#include "echoEdSimu/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace echoEdSimu
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
struct ECHOEDSIMU_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    ECHOEDSIMU_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    ECHOEDSIMU_API void start();

    /// Overrides stop method. Do nothing
    ECHOEDSIMU_API void stop() noexcept;

};

} // namespace echoEdSimu

#endif //__ECHOEDSIMU_PLUGIN_HPP__
