/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/qt/dialog/InputDialog.hpp"

#include <ui/base/registry/macros.hpp>

#include <QApplication>
#include <QInputDialog>
#include <QObject>

fwGuiRegisterMacro(::sight::ui::qt::dialog::InputDialog, ::sight::ui::base::dialog::IInputDialog::REGISTRY_KEY);

namespace sight::ui::qt
{

namespace dialog
{

//------------------------------------------------------------------------------

InputDialog::InputDialog(ui::base::GuiBaseObject::Key key) :
    m_input("")
{
}

//------------------------------------------------------------------------------

InputDialog::~InputDialog()
{
}

//------------------------------------------------------------------------------

void InputDialog::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

void InputDialog::setMessage(const std::string& msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

/// Set the input text in the input field
void InputDialog::setInput(const std::string& text)
{
    m_input = text;
}

//------------------------------------------------------------------------------

/// Get the input text in the input field
std::string InputDialog::getInput()
{
    QString title = QObject::tr(m_title.c_str());
    QString text  = QObject::tr(m_message.c_str());

    bool IsOkClicked;
    QString outputText = QInputDialog::getText(
        qApp->activeWindow(),
        title,
        text,
        QLineEdit::Normal,
        QString::fromStdString(m_input),
        &IsOkClicked
    );

    if(IsOkClicked)
    {
        m_input = outputText.toStdString();
    }
    else
    {
        m_input = "";
    }

    return m_input;
}

//------------------------------------------------------------------------------

} // namespace dialog

} // namespace sight::ui::qt
