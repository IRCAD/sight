/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "sight/modules/ui/console/config.hpp"

#include <core/runtime/plugin.hpp>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::module::ui::console
{

/**
 * @brief   This class is called when the console module is loaded.
 */
class SIGHT_MODULE_UI_CONSOLE_CLASS_API plugin : public core::runtime::plugin
{
public:

    /// Destructor
    SIGHT_MODULE_UI_CONSOLE_API ~plugin() noexcept override;

    /// @override
    SIGHT_MODULE_UI_CONSOLE_API void start() override;

    /// @override
    SIGHT_MODULE_UI_CONSOLE_API void stop() noexcept override;

    /// Run callback
    static SIGHT_MODULE_UI_CONSOLE_API int run() noexcept;
};

} // namespace sight::module::ui::console
