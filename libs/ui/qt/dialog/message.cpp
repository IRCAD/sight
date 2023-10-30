/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/qt/dialog/message.hpp"

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

using message_dialog_qt_icons_type = const std::map<message::icons, QMessageBox::Icon>;
message_dialog_qt_icons_type MESSAGE_DIALOG_QT_ICONS = {{ui::qt::dialog::message::none, QMessageBox::NoIcon},
    {ui::qt::dialog::message::question, QMessageBox::Question},
    {ui::qt::dialog::message::info, QMessageBox::Information},
    {ui::qt::dialog::message::warning, QMessageBox::Warning},
    {ui::qt::dialog::message::critical, QMessageBox::Critical
    }
};

using message_dialog_qt_button_type = const std::map<message::buttons,
                                                     QMessageBox::StandardButtons>;
message_dialog_qt_button_type MESSAGE_DIALOG_QT_BUTTON = {
    {ui::qt::dialog::message::ok, QMessageBox::Ok},
    {ui::qt::dialog::message::cancel, QMessageBox::Cancel},
    {ui::qt::dialog::message::yes, QMessageBox::Yes},
    {ui::qt::dialog::message::no, QMessageBox::No},
    {ui::qt::dialog::message::retry, QMessageBox::Retry}
};

//------------------------------------------------------------------------------

void message::set_title(const std::string& _title)
{
    m_title = _title;
}

//------------------------------------------------------------------------------

void message::set_message(const std::string& _msg)
{
    m_message = _msg;
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

void message::add_custom_button(const std::string& _label, std::function<void()> _clicked_fn)
{
    auto* button = new QPushButton(QString::fromStdString(_label));
    m_custom_buttons.push_back(button);
    QObject::connect(button, &QPushButton::clicked, _clicked_fn);
}

//-----------------------------------------------------------------------------

void message::set_default_button(message::buttons _button)
{
    m_default_button = _button;
}

//------------------------------------------------------------------------------

message::buttons message::show()
{
    auto iter_icon = MESSAGE_DIALOG_QT_ICONS.find(m_icon);
    SIGHT_ASSERT("Unknown Icon", iter_icon != MESSAGE_DIALOG_QT_ICONS.end());

    QMessageBox::Icon icon               = iter_icon->second;
    QString title                        = QString::fromStdString(m_title);
    QString text                         = QString::fromStdString(m_message);
    QMessageBox::StandardButtons buttons = QMessageBox::NoButton;

    for(auto button : MESSAGE_DIALOG_QT_BUTTON)
    {
        if((m_buttons & button.first) != 0)
        {
            buttons |= button.second;
        }
    }

    QMessageBox box(icon, title, text, buttons, qApp->activeWindow());

    for(auto* custom_button : m_custom_buttons)
    {
        box.addButton(custom_button, QMessageBox::ActionRole);
    }

    auto iter = MESSAGE_DIALOG_QT_BUTTON.find(m_default_button);
    if(iter != MESSAGE_DIALOG_QT_BUTTON.end())
    {
        box.setDefaultButton(QMessageBox::StandardButton(static_cast<int>(iter->second)));
    }

    message::buttons result = message::nobutton;

    box.exec();

    for(auto button : MESSAGE_DIALOG_QT_BUTTON)
    {
        if(box.standardButton(box.clickedButton()) == button.second)
        {
            result = button.first;
            break;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
