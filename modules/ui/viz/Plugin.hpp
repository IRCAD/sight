/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/ui/viz/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace sight::module::ui::viz
{

/**
 * @brief   This class is started when the module is loaded.
 */
struct MODULE_UI_VIZ_CLASS_API Plugin : public core::runtime::Plugin
{
    /**
     * @brief   destructor
     */
    MODULE_UI_VIZ_API ~Plugin() noexcept;

    // Overrides
    MODULE_UI_VIZ_API void start();

    // Overrides
    MODULE_UI_VIZ_API void stop() noexcept;
};

} // namespace sight::module::ui::viz
