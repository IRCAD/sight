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

#include <core/runtime/path.hpp>

#include <ui/qml/QmlEngine.hpp>

namespace Tuto01BasicQml
{

SIGHT_REGISTER_PLUGIN("Tuto01BasicQml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept =
    default;

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

//------------------------------------------------------------------------------

void Plugin::start()
{
    // get the qml engine
    auto engine = sight::ui::qml::QmlEngine::getDefault();

    // get the path of the qml ui file
    auto path = sight::core::runtime::getModuleResourceFilePath("Tuto01BasicQml", "ui.qml");

    // load the qml ui
    engine->loadMainComponent(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace Tuto01BasicQml
