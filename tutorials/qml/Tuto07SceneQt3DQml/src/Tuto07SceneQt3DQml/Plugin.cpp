/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "Tuto07SceneQt3DQml/Plugin.hpp"

#include "Tuto07SceneQt3DQml/AppManager.hpp"

#include <core/runtime/operations.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <services/AppManager.hpp>

#include <QQuickItem>

#include <ui/qml/QmlEngine.hpp>

namespace Tuto07SceneQt3DQml
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar(
    "::Tuto07SceneQt3DQml::Plugin");

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
    // Registers custom AppManager class as QML type.
    qmlRegisterType< AppManager >("tutosceneqt3d", 1, 0, "AppManager");

    // Declares an app manager used only to allow fwRenderQt3D features as QML types.
    auto appManager = std::unique_ptr< services::AppManager >(
        new services::AppManager );
    appManager->create();

    // Adds services to the app manager. Those services are only used to register QML types.
    appManager->addService("::fwRenderQt3D::SRender", "genericScene", true, false);
    appManager->addService("::visuQt3DAdaptor::SMesh", "meshAdaptor", true, false);
    appManager->addService("::visuQt3DAdaptor::SMaterial", "materialAdaptor", true, false);
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    auto path = core::runtime::getBundleResourceFilePath("Tuto07SceneQt3DQml", "ui.qml");

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

} // namespace Tuto07SceneQt3DQml.
