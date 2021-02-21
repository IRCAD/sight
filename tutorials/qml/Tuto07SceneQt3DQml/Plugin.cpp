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

#include "Plugin.hpp"

#include "AppManager.hpp"

#include <core/runtime/operations.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <service/AppManager.hpp>

#include <QQuickItem>

#include <ui/qml/QmlEngine.hpp>

using namespace sight;

namespace Tuto07SceneQt3DQml
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto07SceneQt3DQml::Plugin");

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
    auto appManager = std::unique_ptr< service::AppManager >(
        new service::AppManager );
    appManager->create();

    // Adds services to the app manager. Those services are only used to register QML types.
    appManager->addService("::sight::viz::qt3d::SRender", "genericScene", true, false);
    appManager->addService("::sight::module::viz::qt3d::adaptor::SMesh", "meshAdaptor", true, false);
    appManager->addService("::sight::module::viz::qt3d::adaptor::SMaterial", "materialAdaptor", true, false);
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    auto path = core::runtime::getModuleResourceFilePath("Tuto07SceneQt3DQml", "ui.qml");

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
