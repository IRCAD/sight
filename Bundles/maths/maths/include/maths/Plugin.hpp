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

#ifndef __MATHS_PLUGIN_HPP__
#define __MATHS_PLUGIN_HPP__

#include "maths/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace maths
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct MATHS_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    MATHS_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    MATHS_API void start();

    /// Overrides stop method. Do nothing
    MATHS_API void stop() noexcept;

};

} // namespace maths

#endif //__MATHS_PLUGIN_HPP__
