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

#include "Plugin.hpp"

#include <core/base.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/thread/ActiveWorkers.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

#include <ui/base/IGuiContainer.hpp>
#include <ui/qml/App.hpp>
#include <ui/qml/IQmlAppManager.hpp>
#include <ui/qml/QmlEngine.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <modules/ui/qml/activity/ActivityLauncherManager.hpp>
#include <modules/ui/qml/activity/SSequencer.hpp>
#include <modules/ui/qml/activity/SView.hpp>
#include <modules/ui/qml/image/SSliceIndexPositionEditor.hpp>
#include <modules/ui/qml/model/OrganListModel.hpp>
#include <modules/ui/qml/model/SModelSeriesList.hpp>
#include <modules/ui/qml/reconstruction/SOrganMaterialEditor.hpp>
#include <modules/ui/qml/reconstruction/SRepresentationEditor.hpp>

#include <QIcon>
#include <QQuickStyle>
#include <QSharedPointer>
#include <QtQml>

#include <functional>

namespace sight::module::ui::qml
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("::sight::module::ui::qml::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<sight::ui::qml::IQmlAppManager>("sight::ui::qml", 1, 0, "IQmlAppManager");

    core::runtime::Profile::sptr profile = core::runtime::getCurrentProfile();
    SIGHT_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
            return QSharedPointer<QGuiApplication>(new sight::ui::qml::App(argc, argv));
        };

    m_workerQt = sight::ui::qt::getQtWorker(argc, argv, callback, profile->getName(), profile->getVersion());
    QQuickStyle::setStyle("Material");

    core::thread::ActiveWorkers::setDefaultWorker(m_workerQt);
    auto engine = sight::ui::qml::QmlEngine::getDefault();

    // add custom controls and the singleton theme for all qml project
    auto path = core::runtime::getModuleResourcePath("qml");
    engine->importModulePath(path);
    core::runtime::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));

    qmlRegisterType<model::SModelSeriesList>("model", 1, 0, "SModelSeriesList");
    qmlRegisterType<model::OrganListModel>("model", 1, 0, "OrganListModel");
    qmlRegisterType<image::SSliceIndexPositionEditor>("image", 1, 0, "SSliceIndexPositionEditor");
    qmlRegisterType<reconstruction::SOrganMaterialEditor>("reconstruction", 1, 0, "SOrganMaterialEditor");
    qmlRegisterType<reconstruction::SRepresentationEditor>("reconstruction", 1, 0, "SRepresentationEditor");
    qmlRegisterType<activity::SView>("activitiesQml", 1, 0, "SView");
    qmlRegisterType<activity::SSequencer>("activitiesQml", 1, 0, "SSequencer");
    qmlRegisterType<activity::ActivityLauncherManager>("activitiesQml", 1, 0, "ActivityLauncherManager");

    QStringList iconPath = QIcon::themeSearchPaths();
    iconPath.append(QString::fromStdString(path.string()));
    QIcon::setThemeSearchPaths(iconPath);
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
    core::runtime::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_workerQt->post(std::bind(&setup));
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    core::runtime::getCurrentProfile()->cleanup();
    int result = std::any_cast<int>(m_workerQt->getFuture().get());

    core::thread::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
