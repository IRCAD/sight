/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace gui
{

/**
 * @brief   This class is called when the gui module is loaded.
 */

class GUI_CLASS_API Plugin : public core::runtime::Plugin
{

public:

    ///@brief  Destructor. Do nothing.
    GUI_API virtual ~Plugin() noexcept;

    /**
     * @brief This method is used by runtime to initialize the module.
     *
     * @exception core::runtime::RuntimeException
     *
     * Register first basic services of type IAction, IEditor and IView.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
     */
    GUI_API void start();

    /**
     * @brief This method is used by runtime to close the module. Do nothing.
     */
    GUI_API void stop() noexcept;

};

} // namespace gui
