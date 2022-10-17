/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "Plugin.hpp"

#include "AppManager.hpp"
#include "SStringEditor.hpp"

#include <core/runtime/path.hpp>

#include <ui/qml/QmlEngine.hpp>

namespace core = sight::core;
namespace ui   = sight::ui;

namespace Tuto05EditorQml
{

SIGHT_REGISTER_PLUGIN("Tuto05EditorQml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept =
    default;

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<AppManager>("Tuto05EditorQml", 1, 0, "AppManager");
    qmlRegisterType<SStringEditor>("Tuto05EditorQml", 1, 0, "SStringEditor");

    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    auto path = core::runtime::getModuleResourceFilePath("Tuto05EditorQml", "ui.qml");

    engine->loadMainComponent(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace Tuto05EditorQml
