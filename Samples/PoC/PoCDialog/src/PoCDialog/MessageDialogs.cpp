/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "PoCDialog/MessageDialogs.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwQml/QmlEngine.hpp>

#include <boost/assign/list_of.hpp>

#include <QMessageBox>

MessageDialogs::MessageDialogs()
{
}

//------------------------------------------------------------------------------

MessageDialogs::~MessageDialogs()
{
}

//------------------------------------------------------------------------------
//value of the enum in integer
typedef const std::map< ::fwGui::dialog::IMessageDialog::Icons, int> MessageDialogQmlIconsType;
MessageDialogQmlIconsType messageDialogQmlIcons =
{
    {::fwGui::dialog::IMessageDialog::NONE, QMessageBox::NoIcon},
    {::fwGui::dialog::IMessageDialog::QUESTION, QMessageBox::Question},
    {::fwGui::dialog::IMessageDialog::INFO, QMessageBox::Information},
    {::fwGui::dialog::IMessageDialog::WARNING, QMessageBox::Warning},
    {::fwGui::dialog::IMessageDialog::CRITICAL, QMessageBox::Critical}
};
typedef const std::map< ::fwGui::dialog::IMessageDialog::Buttons,
                        QMessageBox::StandardButton> MessageDialogQmlButtonType;
MessageDialogQmlButtonType messageDialogQmlButton =
{
    {::fwGui::dialog::IMessageDialog::OK, QMessageBox::Ok},
    {::fwGui::dialog::IMessageDialog::CANCEL, QMessageBox::Cancel},
    {::fwGui::dialog::IMessageDialog::YES, QMessageBox::Yes},
    {::fwGui::dialog::IMessageDialog::NO, QMessageBox::No}
};

typedef const std::map< QString,
                        ::fwGui::dialog::IMessageDialog::Buttons> ResultDialogQmlButton;
ResultDialogQmlButton resultDialogQmlButton =
{
    {"::fwGui::dialog::IMessageDialog::OK", ::fwGui::dialog::IMessageDialog::OK},
    {"::fwGui::dialog::IMessageDialog::CANCEL", ::fwGui::dialog::IMessageDialog::CANCEL},
    {"::fwGui::dialog::IMessageDialog::YES", ::fwGui::dialog::IMessageDialog::YES},
    {"::fwGui::dialog::IMessageDialog::NO", ::fwGui::dialog::IMessageDialog::NO}
};

//------------------------------------------------------------------------------

void MessageDialogs::open()
{
    ::fwGui::dialog::MessageDialog::sptr dialog = ::fwGui::dialog::MessageDialog::New();
    dialog->setTitle(m_title.toStdString());

    for(const MessageDialogQmlIconsType::value_type& icon :  messageDialogQmlIcons)
    {
        if (icon.second == m_icon)
        {
            dialog->setIcon(icon.first);
            break;
        }
    }
    for(const MessageDialogQmlButtonType::value_type& button :  messageDialogQmlButton)
    {
        if (button.second & m_buttons)
        {
            dialog->addButton(button.first);
        }
    }
    dialog->setMessage(m_message.toStdString());
    const auto& result = dialog->show();
    for(const ResultDialogQmlButton::value_type& button :  resultDialogQmlButton)
    {
        if (button.second & result)
        {
            m_result = button.first;
        }
    }
    Q_EMIT onResultChanged();
}
