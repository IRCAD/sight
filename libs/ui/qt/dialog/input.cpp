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

void input::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

void input::setMessage(const std::string& msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

void input::setEchoMode(input::EchoMode echoMode)
{
    m_echoMode = echoMode;
}

//------------------------------------------------------------------------------

/// Set the input text in the input field
void input::set_input(const std::string& text)
{
    m_input = text;
}

//------------------------------------------------------------------------------

/// Get the input text in the input field
std::pair<std::string, bool> input::getInput()
{
    QString title = QObject::tr(m_title.c_str());
    QString text  = QObject::tr(m_message.c_str());

    const QLineEdit::EchoMode echoMode =
        [&]
        {
            switch(m_echoMode)
            {
                case input::EchoMode::NOECHO:
                    return QLineEdit::EchoMode::NoEcho;

                case input::EchoMode::PASSWORD:
                    return QLineEdit::EchoMode::Password;

                case input::EchoMode::ECHO_ON_EDIT:
                    return QLineEdit::EchoMode::PasswordEchoOnEdit;

                default:
                    return QLineEdit::EchoMode::Normal;
            }
        }();

    bool IsOkClicked = false;

    QString outputText = QInputDialog::getText(
        qApp->activeWindow(),
        title,
        text,
        echoMode,
        QString::fromStdString(m_input),
        &IsOkClicked
    );

    if(IsOkClicked)
    {
        m_input = outputText.toStdString();
    }
    else
    {
        m_input.clear();
    }

    return std::make_pair(m_input, IsOkClicked);
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
