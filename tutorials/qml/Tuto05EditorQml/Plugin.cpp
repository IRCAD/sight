/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include <core/runtime/operations.hpp>

#include <ui/qml/QmlEngine.hpp>

using namespace sight;

namespace Tuto05EditorQml
{

SIGHT_REGISTER_PLUGIN("::Tuto05EditorQml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<AppManager>("Tuto05EditorQml", 1, 0, "AppManager");
    qmlRegisterType<SStringEditor>("Tuto05EditorQml", 1, 0, "SStringEditor");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    auto path = core::runtime::getModuleResourceFilePath("Tuto05EditorQml", "ui.qml");

    engine->loadMainComponent(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace Tuto05EditorQml
