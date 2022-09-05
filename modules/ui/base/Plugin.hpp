/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/ui/base/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace sight::module::ui::base
{

/**
 * @brief   This class is called when the gui module is loaded.
 */

class MODULE_UI_BASE_CLASS_API Plugin : public core::runtime::Plugin
{
public:

    ///@brief  Destructor. Do nothing.
    MODULE_UI_BASE_API ~Plugin() noexcept override;

    /**
     * @brief This method is used by runtime to initialize the module.
     *
     * @exception core::runtime::RuntimeException
     *
     * Register first basic services of type IAction, IEditor and IView.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
     */
    MODULE_UI_BASE_API void start() override;

    /**
     * @brief This method is used by runtime to close the module. Do nothing.
     */
    MODULE_UI_BASE_API void stop() noexcept override;
};

} // namespace sight::module::ui::base
