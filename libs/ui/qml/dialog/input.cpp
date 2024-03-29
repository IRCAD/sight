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

#include "ui/qml/dialog/input.hpp"

#include <core/runtime/path.hpp>

#include <ui/__/macros.hpp>
#include <ui/qml/qml_engine.hpp>

#include <QGuiApplication>
#include <QObject>

SIGHT_REGISTER_GUI(sight::ui::qml::dialog::input, sight::ui::dialog::input_base::REGISTRY_KEY);

namespace sight::ui::qml::dialog
{

static auto enum_registered =
    []
    {
        qmlRegisterUncreatableType<input>("sight.input", 1, 0, "EchoMode", "Enum EchoMode is not a type");
        return true;
    }();

//------------------------------------------------------------------------------

void input::set_title(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

void input::set_message(const std::string& _msg)
{
    m_message = QString::fromStdString(_msg);
}

//------------------------------------------------------------------------------

void input::set_echo_mode(input::echo_mode _echo_mode)
{
    m_echo_mode = _echo_mode;
}

//------------------------------------------------------------------------------

void input::set_input(const std::string& _text)
{
    m_input = QString::fromStdString(_text);
}

//------------------------------------------------------------------------------

std::pair<std::string, bool> input::get_input()
{
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();
    // get the path of the qml ui file in the 'rc' directory
    const auto& dialog_path =
        core::runtime::get_library_resource_file_path("ui_qml/dialog/input.qml");

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->get_root_context()));
    context->setContextProperty("input", this);
    // load the qml ui component
    QObject* window = engine->create_component(dialog_path, context);
    SIGHT_ASSERT("The Qml File input is not found or not loaded", window);
    // keep window to destroy it

    window->setProperty("title", m_title);
    Q_EMIT title_changed();
    Q_EMIT message_changed();
    Q_EMIT input_changed();
    auto* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);
    m_input = "";

    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));

    [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(dialog, "open");
    SIGHT_ASSERT("The slot `open` was not found.", ok);

    loop.exec();

    delete window;
    return std::make_pair(m_input.toStdString(), true);
}

//------------------------------------------------------------------------------

void input::result_dialog(const QVariant& _msg, bool _is_ok)
{
    if(_is_ok)
    {
        m_input = _msg.toString();
    }
    else
    {
        m_input = "";
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
