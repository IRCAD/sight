/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __CTRLHISTORY_PLUGIN_HPP__
#define __CTRLHISTORY_PLUGIN_HPP__

#include "ctrlHistory/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ctrlHistory
{

/**
 * @brief This class starts when the bundle is loaded.
 */
struct CTRLHISTORY_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    CTRLHISTORY_API ~Plugin() noexcept;

    /// Overrides start method. Does nothing.
    CTRLHISTORY_API void start();

    /// Overrides stop method. Does nothing.
    CTRLHISTORY_API void stop() noexcept;

};

} // namespace ctrlHistory

#endif //__CTRLHISTORY_PLUGIN_HPP__
