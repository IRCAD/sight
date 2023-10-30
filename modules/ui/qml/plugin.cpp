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
#include <ui/qml/app.hpp>
#include <ui/qml/QmlEngine.hpp>
#include <ui/qt/worker_qt.hpp>

#include <modules/ui/qml/activity/sequencer.hpp>
#include <modules/ui/qml/activity/view.hpp>
#include <modules/ui/qml/image/slice_index_position_editor.hpp>
#include <modules/ui/qml/model/model_series_list.hpp>
#include <modules/ui/qml/model/organ_list_model.hpp>
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
        [](int& _argc, char** _argv)
        {
            return QSharedPointer<QGuiApplication>(new sight::ui::qml::app(_argc, _argv));
        };

    auto worker_qt = sight::ui::qt::get_qt_worker(argc, argv, callback, profile->name(), profile->get_version());
    core::thread::set_default_worker(worker_qt);

    auto engine = sight::ui::qml::qml_engine::get_default();

    // add custom controls and the singleton theme for all qml project
    const auto path = core::runtime::get_module_resource_path("sight::module::ui::qml");
    engine->import_module_path(path);

    qmlRegisterType<model::model_series_list>("model", 1, 0, "ModelSeriesList");
    qmlRegisterType<model::organ_list_model>("model", 1, 0, "OrganListModel");
    qmlRegisterType<image::slice_index_position_editor>("image", 1, 0, "SliceIndexPositionEditor");
    qmlRegisterType<reconstruction::organ_material_editor>("reconstruction", 1, 0, "OrganMaterialEditor");
    qmlRegisterType<reconstruction::representation_editor>("reconstruction", 1, 0, "RepresentationEditor");
    qmlRegisterType<activity::view>("activitiesQml", 1, 0, "View");
    qmlRegisterType<activity::sequencer>("activitiesQml", 1, 0, "Sequencer");

    QQuickStyle::setStyle("Material");

    QStringList icon_path = QIcon::themeSearchPaths();
    icon_path.append(QString::fromStdString(path.string()));
    QIcon::setThemeSearchPaths(icon_path);

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
    auto worker_qt = core::thread::get_default_worker();
    worker_qt->get_future().wait(); // This is required to start worker_qt loop

    int result = std::any_cast<int>(worker_qt->get_future().get());
    return result;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qml
