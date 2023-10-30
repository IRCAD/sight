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

#include "ui/qml/dialog/message.hpp"

#include <core/runtime/path.hpp>

#include <ui/__/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QGuiApplication>
#include <QVector>

#include <filesystem>

SIGHT_REGISTER_GUI(sight::ui::qml::dialog::message, sight::ui::dialog::message_base::REGISTRY_KEY);

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

//value of the enum in QMessageBox
using message_dialog_qml_icons_type = const std::map<message::icons, std::string>;
message_dialog_qml_icons_type MESSAGE_DIALOG_QML_ICONS =
{
    {ui::qml::dialog::message::none, ""},
    {ui::qml::dialog::message::question, "fwGuiQml/question.svg"},
    {ui::qml::dialog::message::info, "fwGuiQml/information.svg"},
    {ui::qml::dialog::message::warning, "fwGuiQml/warning.svg"},
    {ui::qml::dialog::message::critical, "fwGuiQml/critical.svg"}
};

// value of the enum in int value of Dialog
using message_dialog_qml_button_type =
    const std::map<message::buttons, standard_button::button_list>;
message_dialog_qml_button_type MESSAGE_DIALOG_QML_BUTTON =
{
    {ui::qml::dialog::message::ok, standard_button::button_list::ok},
    {ui::qml::dialog::message::cancel, standard_button::button_list::cancel},
    {ui::qml::dialog::message::yes, standard_button::button_list::yes},
    {ui::qml::dialog::message::no, standard_button::button_list::no},
    {ui::qml::dialog::message::retry, standard_button::button_list::retry}
};

//------------------------------------------------------------------------------

message::message()
{
    qmlRegisterType<standard_button>("Dialog", 1, 0, "StandardButton");
}

//------------------------------------------------------------------------------

message::~message()
= default;

//------------------------------------------------------------------------------

void message::set_title(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

void message::set_message(const std::string& _msg)
{
    m_message = QString::fromStdString(_msg);
}

//------------------------------------------------------------------------------

void message::set_icon(message::icons _icon)
{
    m_icon = _icon;
}

//------------------------------------------------------------------------------

void message::add_button(message::buttons _button)
{
    m_buttons = (message::buttons) (m_buttons | _button);
}

//------------------------------------------------------------------------------

void message::add_custom_button(const std::string& /*label*/, std::function<void()> /*clickedFn*/)
{
}

//-----------------------------------------------------------------------------

void message::set_default_button(message::buttons /*button*/)
{
}

//------------------------------------------------------------------------------

message::buttons message::show()
{
    auto iter_icon = MESSAGE_DIALOG_QML_ICONS.find(m_icon);
    SIGHT_ASSERT("Unknown Icon", iter_icon != MESSAGE_DIALOG_QML_ICONS.end());
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();
    std::string icon = iter_icon->second;

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialog_path = core::runtime::get_library_resource_file_path(
        "ui_qml/dialog/message.qml"
    );
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->get_root_context()));
    context->setContextProperty("message", this);
    // load the qml ui component
    QObject* window = engine->create_component(dialog_path, context);
    SIGHT_ASSERT("The Qml File message is not found or not loaded", window);
    // keep window to destroy it

    window->setProperty("title", m_title);
    auto* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);

    auto* button_setting = qobject_cast<standard_button*>(dialog->findChild<QObject*>("standardButton"));
    Q_EMIT message_changed();
    //set icon
    auto path_icon = core::runtime::get_library_resource_file_path(icon);
    if(!std::filesystem::exists(path_icon))
    {
        path_icon = "";
    }

    emit_icon(QUrl::fromLocalFile(QString::fromStdString(path_icon.string())));
    emit_buttons(button_setting);

    m_clicked = dialog::message::cancel;
    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");
    loop.exec();
    delete window;
    return m_clicked;
}

//------------------------------------------------------------------------------

void message::result_dialog(int _clicked)
{
    for(auto button : MESSAGE_DIALOG_QML_BUTTON)
    {
        // get the button that has been click
        if(_clicked == button.second)
        {
            m_clicked = button.first;
            break;
        }
    }
}

//------------------------------------------------------------------------------

void message::emit_icon(const QUrl& _icon_path)
{
    m_icon_image = _icon_path;
    Q_EMIT icon_changed();
}

//------------------------------------------------------------------------------

void message::emit_buttons(standard_button* _standard_button)
{
    int buttons = 0;
    // add the different type of button needed
    for(auto button : MESSAGE_DIALOG_QML_BUTTON)
    {
        if((m_buttons & button.first) != 0)
        {
            buttons |= button.second;
        }
    }

    _standard_button->set_button(static_cast<standard_button::button_list>(buttons));
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
