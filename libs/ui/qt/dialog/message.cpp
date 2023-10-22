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

using MessageDialogQtIconsType = const std::map<message::Icons, QMessageBox::Icon>;
MessageDialogQtIconsType messageDialogQtIcons = {{ui::qt::dialog::message::NONE, QMessageBox::NoIcon},
    {ui::qt::dialog::message::QUESTION, QMessageBox::Question},
    {ui::qt::dialog::message::INFO, QMessageBox::Information},
    {ui::qt::dialog::message::WARNING, QMessageBox::Warning},
    {ui::qt::dialog::message::CRITICAL, QMessageBox::Critical
    }
};

using MessageDialogQtButtonType = const std::map<message::Buttons,
                                                 QMessageBox::StandardButtons>;
MessageDialogQtButtonType messageDialogQtButton = {
    {ui::qt::dialog::message::OK, QMessageBox::Ok},
    {ui::qt::dialog::message::CANCEL, QMessageBox::Cancel},
    {ui::qt::dialog::message::YES, QMessageBox::Yes},
    {ui::qt::dialog::message::NO, QMessageBox::No},
    {ui::qt::dialog::message::RETRY, QMessageBox::Retry}
};

//------------------------------------------------------------------------------

void message::setTitle(const std::string& _title)
{
    m_title = _title;
}

//------------------------------------------------------------------------------

void message::setMessage(const std::string& _msg)
{
    m_message = _msg;
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

void message::addCustomButton(const std::string& _label, std::function<void()> _clicked_fn)
{
    auto* button = new QPushButton(QString::fromStdString(_label));
    m_customButtons.push_back(button);
    QObject::connect(button, &QPushButton::clicked, _clicked_fn);
}

//-----------------------------------------------------------------------------

void message::setDefaultButton(message::Buttons _button)
{
    m_defaultButton = _button;
}

//------------------------------------------------------------------------------

message::Buttons message::show()
{
    auto iter_icon = messageDialogQtIcons.find(m_icon);
    SIGHT_ASSERT("Unknown Icon", iter_icon != messageDialogQtIcons.end());

    QMessageBox::Icon icon               = iter_icon->second;
    QString title                        = QString::fromStdString(m_title);
    QString text                         = QString::fromStdString(m_message);
    QMessageBox::StandardButtons buttons = QMessageBox::NoButton;

    for(MessageDialogQtButtonType::value_type button : messageDialogQtButton)
    {
        if((m_buttons & button.first) != 0)
        {
            buttons |= button.second;
        }
    }

    QMessageBox box(icon, title, text, buttons, qApp->activeWindow());

    for(auto* custom_button : m_customButtons)
    {
        box.addButton(custom_button, QMessageBox::ActionRole);
    }

    auto iter = messageDialogQtButton.find(m_defaultButton);
    if(iter != messageDialogQtButton.end())
    {
        box.setDefaultButton(QMessageBox::StandardButton(static_cast<int>(iter->second)));
    }

    message::Buttons result = message::NOBUTTON;

    box.exec();

    for(MessageDialogQtButtonType::value_type button : messageDialogQtButton)
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
