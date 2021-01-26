/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "guiQml/Plugin.hpp"

#include <core/base.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include <fwGuiQml/App.hpp>

#include <fwQml/IQmlAppManager.hpp>
#include <fwQml/QmlEngine.hpp>

#include <fwQt/WorkerQt.hpp>

#include <services/registry/ActiveWorkers.hpp>

#include <QQuickStyle>
#include <QSharedPointer>
#include <QtQml>

#include <functional>

namespace guiQml
{
//-----------------------------------------------------------------------------

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiQml::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType< ::fwQml::IQmlAppManager >("fwQml", 1, 0, "IQmlAppManager");

    core::runtime::profile::Profile::sptr profile = core::runtime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback
        = [](int& argc, char** argv)
          {
              return QSharedPointer< QGuiApplication > ( new ::fwGuiQml::App(argc, argv) );
          };

    m_workerQt = ::fwQt::getQtWorker(argc, argv, callback, profile->getName(), profile->getVersion());
    QQuickStyle::setStyle("Material");

    services::registry::ActiveWorkers::setDefaultWorker(m_workerQt);
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // add custom controls and the singleton theme for all qml project
    auto path = core::runtime::getModuleResourcePath("guiQml");
    engine->importModulePath(path);
    core::runtime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    if(m_workerQt)
    {
        m_workerQt->stop();
        m_workerQt.reset();
    }
}

//-----------------------------------------------------------------------------

void setup()
{
    core::runtime::profile::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_workerQt->post( std::bind( &setup ) );
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    core::runtime::profile::getCurrentProfile()->cleanup();
    int result = std::any_cast<int>(m_workerQt->getFuture().get());

    services::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

} // namespace guiQt
