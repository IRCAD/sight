/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "TutoSceneQt3DQml/Plugin.hpp"

#include "TutoSceneQt3DQml/AppManager.hpp"

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/AppManager.hpp>

#include <QQuickItem>

namespace TutoSceneQt3DQml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar(
    "::TutoSceneQt3DQml::Plugin");

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
    std::unique_ptr< ::fwServices::AppManager > appManager = std::unique_ptr< ::fwServices::AppManager >(
        new ::fwServices::AppManager );
    appManager->create();

    // Adds services to the app manager. Those services are only used to register QML types.
    appManager->addService("::fwRenderQt3D::SRender", "genericScene", true, false);
    appManager->addService("::visuQt3DAdaptor::SMesh", "meshAdaptor", true, false);
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    auto path = ::fwRuntime::getBundleResourceFilePath("TutoSceneQt3DQml", "ui.qml");

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

} // namespace TutoSceneQt3DQml.
