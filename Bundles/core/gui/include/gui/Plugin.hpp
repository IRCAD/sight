/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __GUI_PLUGIN_HPP__
#define __GUI_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "gui/config.hpp"

namespace gui
{

/**
 * @brief   This class is called when the gui bundle is loaded.
 * @class   Plugin
 */

class GUI_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    ///@brief  Destructor. Do nothing.
    GUI_API virtual ~Plugin() noexcept;

    /**
     * @brief This method is used by runtime to initialize the bundle.
     *
     * @exception ::fwRuntime::RuntimeException
     *
     * Register first basic services of type IAction, IEditor and IView.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
     */
    GUI_API void start();

    /**
     * @brief This method is used by runtime to close the bundle. Do nothing.
     */
    GUI_API void stop() noexcept;

};

} // namespace gui

#endif //__GUI_PLUGIN_HPP__
