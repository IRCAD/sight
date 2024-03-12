/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <core/runtime/plugin.hpp>

namespace sight::core::thread
{

class Worker;

} // namespace sight::core::thread

namespace sight::module::ui::qt
{

/**
 * @brief   This class is called when the guiQt module is loaded.
 */
class MODULE_UI_QT_CLASS_API plugin final : public core::runtime::plugin
{
public:

    /**
     * @brief   destructor
     */
    MODULE_UI_QT_API ~plugin() noexcept final = default;

    // Overrides
    MODULE_UI_QT_API void start() final;

    // Overrides
    MODULE_UI_QT_API void stop() noexcept final;

    static MODULE_UI_QT_API int run() noexcept;

private:

    void load_style_sheet();
};

} // namespace sight::module::ui::qt
