/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/__/config.hpp"

#include <core/runtime/plugin.hpp>

namespace sight::module::ui
{

/**
 * @brief   This class is called when the gui module is loaded.
 */

class MODULE_UI_CLASS_API plugin : public core::runtime::plugin
{
public:

    ///@brief  Destructor. Do nothing.
    MODULE_UI_API ~plugin() noexcept override = default;

    /**
     * @brief This method is used by runtime to initialize the module.
     *
     * @exception core::runtime::RuntimeException
     *
     * Register first basic services of type action, editor and view.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
     */
    MODULE_UI_API void start() override;

    /**
     * @brief This method is used by runtime to close the module. Do nothing.
     */
    MODULE_UI_API void stop() noexcept override;
};

} // namespace sight::module::ui
