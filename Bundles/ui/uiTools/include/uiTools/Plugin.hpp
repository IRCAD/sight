/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

#ifndef __UITOOLS_PLUGIN_HPP__
#define __UITOOLS_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiTools/config.hpp"

namespace uiTools
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct UITOOLS_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    UITOOLS_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    UITOOLS_API void start();

    /// Overrides stop method. Do nothing
    UITOOLS_API void stop() noexcept;

};

} // namespace uiTools

#endif //__UITOOLS_PLUGIN_HPP__
