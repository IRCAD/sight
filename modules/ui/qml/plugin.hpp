/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/ui/qml/config.hpp"

#include <core/runtime/plugin.hpp>
namespace sight::module::ui::qml
{

/**
 * @brief   This class is called when the guiQt module is loaded.
 */
class MODULE_UI_QML_CLASS_API plugin : public core::runtime::plugin
{
public:

    /**
     * @brief   destructor
     */
    MODULE_UI_QML_API ~plugin() noexcept override;

    // Overrides
    MODULE_UI_QML_API void start() override;

    // Overrides
    MODULE_UI_QML_API void stop() noexcept override;

    static MODULE_UI_QML_API int run() noexcept;
};

} // namespace sight::module::ui::qml
