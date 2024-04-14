/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/dialog/pulse_progress.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <ui/__/macros.hpp>
#include <ui/qml/qml_engine.hpp>

#include <QFutureWatcher>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QString>
#include <QtConcurrent>
#include <QtCore>

SIGHT_REGISTER_GUI(
    sight::ui::qml::dialog::pulse_progress,
    sight::ui::dialog::pulse_progress_base::REGISTRY_KEY
);

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

void pulse_progress::set_title(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

void pulse_progress::set_message(const std::string& _msg)
{
    m_message = QString::fromStdString(_msg);
}

//------------------------------------------------------------------------------

void pulse_progress::show()
{
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialog_path =
        core::runtime::get_library_resource_file_path("ui_qml/dialog/pulse_progress.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->get_root_context()));
    context->setContextProperty("pulseProgressDialog", this);
    // load the qml ui component
    QObject* window = engine->create_component(dialog_path, context);
    SIGHT_ASSERT("The Qml File pulse_progress is not found or not loaded", window);
    // keep window to destroy it

    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> future_watcher;

    window->setProperty("title", m_title);
    auto* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);
    Q_EMIT message_changed();
    // Start the computation.

    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    QObject::connect(&future_watcher, SIGNAL(finished()), &loop, SLOT(quit()));

    [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(dialog, "open");
    SIGHT_ASSERT("The slot `open` was not found.", ok);

    future_watcher.setFuture(QtConcurrent::run(m_stuff));
    qGuiApp->installEventFilter(this);
    loop.exec();
    qGuiApp->removeEventFilter(this);
    delete window;
}

//------------------------------------------------------------------------------

bool pulse_progress::eventFilter(QObject* /*watched*/, QEvent* _event)
{
    return _event->type() == QEvent::Shortcut || _event->type() == QEvent::ShortcutOverride;
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
