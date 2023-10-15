/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "plugin.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <ui/__/service.hpp>
#include <ui/qml/App.hpp>
#include <ui/qml/QmlEngine.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <modules/ui/qml/activity/sequencer.hpp>
#include <modules/ui/qml/activity/view.hpp>
#include <modules/ui/qml/image/slice_index_position_editor.hpp>
#include <modules/ui/qml/model/model_series_list.hpp>
#include <modules/ui/qml/model/OrganListModel.hpp>
#include <modules/ui/qml/reconstruction/organ_material_editor.hpp>
#include <modules/ui/qml/reconstruction/representation_editor.hpp>

#include <QIcon>
#include <QQuickStyle>
#include <QSharedPointer>
#include <QtQml>

#include <functional>

namespace sight::module::ui::qml
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::ui::qml::plugin");

//-----------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void plugin::start()
{
    auto profile = core::runtime::get_current_profile();
    SIGHT_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->get_raw_arg_count();
    char** argv = profile->get_raw_params();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
            return QSharedPointer<QGuiApplication>(new sight::ui::qml::App(argc, argv));
        };

    auto workerQt = sight::ui::qt::getQtWorker(argc, argv, callback, profile->name(), profile->get_version());
    core::thread::set_default_worker(workerQt);

    auto engine = sight::ui::qml::QmlEngine::getDefault();

    // add custom controls and the singleton theme for all qml project
    const auto path = core::runtime::get_module_resource_path("sight::module::ui::qml");
    engine->importModulePath(path);

    qmlRegisterType<model::model_series_list>("model", 1, 0, "model_series_list");
    qmlRegisterType<model::OrganListModel>("model", 1, 0, "OrganListModel");
    qmlRegisterType<image::slice_index_position_editor>("image", 1, 0, "slice_index_position_editor");
    qmlRegisterType<reconstruction::organ_material_editor>("reconstruction", 1, 0, "organ_material_editor");
    qmlRegisterType<reconstruction::representation_editor>("reconstruction", 1, 0, "representation_editor");
    qmlRegisterType<activity::view>("activitiesQml", 1, 0, "view");
    qmlRegisterType<activity::sequencer>("activitiesQml", 1, 0, "sequencer");

    QQuickStyle::setStyle("Material");

    QStringList iconPath = QIcon::themeSearchPaths();
    iconPath.append(QString::fromStdString(path.string()));
    QIcon::setThemeSearchPaths(iconPath);

    profile->set_run_callback(run);
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
    core::thread::reset_default_worker();
}

//-----------------------------------------------------------------------------

int plugin::run() noexcept
{
    auto workerQt = core::thread::get_default_worker();
    workerQt->get_future().wait(); // This is required to start WorkerQt loop

    int result = std::any_cast<int>(workerQt->get_future().get());
    return result;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qml
