/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "plugin.hpp"

#include <core/runtime/path.hpp>

#include <ui/qml/qml_engine.hpp>

namespace tuto06_gui_qml
{

namespace ui   = sight::ui;
namespace core = sight::core;

SIGHT_REGISTER_PLUGIN("tuto06_gui_qml::plugin");

//------------------------------------------------------------------------------

void plugin::start()
{
    // get the qml engine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();

    // get the path of the qml ui file
    auto path = core::runtime::get_module_resource_file_path("tuto06_gui_qml", "app.qml");

    // load the qml ui
    engine->load_main_component(path);
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace tuto06_gui_qml
