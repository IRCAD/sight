/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "guiQml/editor/SActivityView.hpp"

#include <fwCore/base.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include <fwGuiQml/App.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwQt/WorkerQt.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <QQuickStyle>
#include <QSharedPointer>
#include <QtQml>

#include <functional>

namespace guiQml
{
//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiQml::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType< ::guiQml::editor::SActivityView >("guiQml", 1, 0, "SActivityView");

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
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

    ::fwServices::registry::ActiveWorkers::setDefaultWorker(m_workerQt);
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // add custom controls and the singleton theme for all qml project
    auto path = ::fwRuntime::getBundleResourcePath("guiQml");
    engine->importModulePath(path);
    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
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
    ::fwRuntime::profile::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_workerQt->post( std::bind( &setup ) );
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    int result = ::boost::any_cast<int>(m_workerQt->getFuture().get());

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

} // namespace guiQt
