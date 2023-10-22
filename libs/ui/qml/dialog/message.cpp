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
using MessageDialogQmlIconsType = const std::map<message::Icons, std::string>;
MessageDialogQmlIconsType messageDialogQmlIcons =
{
    {ui::qml::dialog::message::NONE, ""},
    {ui::qml::dialog::message::QUESTION, "fwGuiQml/question.svg"},
    {ui::qml::dialog::message::INFO, "fwGuiQml/information.svg"},
    {ui::qml::dialog::message::WARNING, "fwGuiQml/warning.svg"},
    {ui::qml::dialog::message::CRITICAL, "fwGuiQml/critical.svg"}
};

// value of the enum in int value of Dialog
using MessageDialogQmlButtonType =
    const std::map<message::Buttons, StandardButton::ButtonList>;
MessageDialogQmlButtonType messageDialogQmlButton =
{
    {ui::qml::dialog::message::OK, StandardButton::ButtonList::Ok},
    {ui::qml::dialog::message::CANCEL, StandardButton::ButtonList::Cancel},
    {ui::qml::dialog::message::YES, StandardButton::ButtonList::Yes},
    {ui::qml::dialog::message::NO, StandardButton::ButtonList::No},
    {ui::qml::dialog::message::RETRY, StandardButton::ButtonList::Retry}
};

//------------------------------------------------------------------------------

message::message()
{
    qmlRegisterType<StandardButton>("Dialog", 1, 0, "StandardButton");
}

//------------------------------------------------------------------------------

message::~message()
= default;

//------------------------------------------------------------------------------

void message::setTitle(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

void message::setMessage(const std::string& _msg)
{
    m_message = QString::fromStdString(_msg);
}

//------------------------------------------------------------------------------

void message::setIcon(message::Icons _icon)
{
    m_icon = _icon;
}

//------------------------------------------------------------------------------

void message::addButton(message::Buttons _button)
{
    m_buttons = (message::Buttons) (m_buttons | _button);
}

//------------------------------------------------------------------------------

void message::addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/)
{
}

//-----------------------------------------------------------------------------

void message::setDefaultButton(message::Buttons /*button*/)
{
}

//------------------------------------------------------------------------------

message::Buttons message::show()
{
    auto iter_icon = messageDialogQmlIcons.find(m_icon);
    SIGHT_ASSERT("Unknown Icon", iter_icon != messageDialogQmlIcons.end());
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();
    std::string icon = iter_icon->second;

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialog_path = core::runtime::get_library_resource_file_path(
        "ui_qml/dialog/message.qml"
    );
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("message", this);
    // load the qml ui component
    QObject* window = engine->createComponent(dialog_path, context);
    SIGHT_ASSERT("The Qml File message is not found or not loaded", window);
    // keep window to destroy it

    window->setProperty("title", m_title);
    auto* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);

    auto* button_setting = qobject_cast<StandardButton*>(dialog->findChild<QObject*>("standardButton"));
    Q_EMIT messageChanged();
    //set icon
    auto path_icon = core::runtime::get_library_resource_file_path(icon);
    if(!std::filesystem::exists(path_icon))
    {
        path_icon = "";
    }

    emitIcon(QUrl::fromLocalFile(QString::fromStdString(path_icon.string())));
    emitButtons(button_setting);

    m_clicked = dialog::message::CANCEL;
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

void message::resultDialog(int _clicked)
{
    for(MessageDialogQmlButtonType::value_type button : messageDialogQmlButton)
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

void message::emitIcon(const QUrl& _icon_path)
{
    m_iconImage = _icon_path;
    Q_EMIT iconChanged();
}

//------------------------------------------------------------------------------

void message::emitButtons(StandardButton* _standard_button)
{
    int buttons = 0;
    // add the different type of button needed
    for(MessageDialogQmlButtonType::value_type button : messageDialogQmlButton)
    {
        if((m_buttons & button.first) != 0)
        {
            buttons |= button.second;
        }
    }

    _standard_button->setButton(static_cast<StandardButton::ButtonList>(buttons));
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
