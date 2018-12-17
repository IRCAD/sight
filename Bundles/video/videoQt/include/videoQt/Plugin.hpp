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

#ifndef __VIDEOQT_PLUGIN_HPP__
#define __VIDEOQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "videoQt/config.hpp"

namespace videoQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct VIDEOQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOQT_API void start();

    // Overrides
    VIDEOQT_API void stop() noexcept;

};

} // namespace videoQt

#endif //__VIDEOQT_PLUGIN_HPP__
