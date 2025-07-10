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

#include "ui/qt/dialog/input.hpp"

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QInputDialog>
#include <QObject>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

void input::set_title(const std::string& _title)
{
    m_title = _title;
}

//------------------------------------------------------------------------------

void input::set_message(const std::string& _msg)
{
    m_message = _msg;
}

//------------------------------------------------------------------------------

void input::set_echo_mode(input::echo_mode _echo_mode)
{
    m_echo_mode = _echo_mode;
}

//------------------------------------------------------------------------------

/// Set the input text in the input field
void input::set_input(const std::string& _text)
{
    m_input = _text;
}

//------------------------------------------------------------------------------

/// Get the input text in the input field
std::pair<std::string, bool> input::get_input()
{
    QString title = QObject::tr(m_title.c_str());
    QString text  = QObject::tr(m_message.c_str());

    const QLineEdit::EchoMode echo_mode =
        [&]
        {
            switch(m_echo_mode)
            {
                case input::echo_mode::noecho:
                    return QLineEdit::EchoMode::NoEcho;

                case input::echo_mode::password:
                    return QLineEdit::EchoMode::Password;

                case input::echo_mode::echo_on_edit:
                    return QLineEdit::EchoMode::PasswordEchoOnEdit;

                default:
                    return QLineEdit::EchoMode::Normal;
            }
        }();

    bool is_ok_clicked = false;

    QString output_text = QInputDialog::getText(
        qApp->activeWindow(),
        title,
        text,
        echo_mode,
        QString::fromStdString(m_input),
        &is_ok_clicked
    );

    if(is_ok_clicked)
    {
        m_input = output_text.toStdString();
    }
    else
    {
        m_input.clear();
    }

    return std::make_pair(m_input, is_ok_clicked);
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
