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
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QMessageBox>
#include <QObject>

MessageDialogs::MessageDialogs()
{
    m_icon    = 0;
    m_buttons = 0;
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // add context for root Context
    engine->getRootContext()->setContextProperty("pocDialogMessageDialogs", this);
}

//------------------------------------------------------------------------------

MessageDialogs::~MessageDialogs()
{
}

//------------------------------------------------------------------------------

void MessageDialogs::open()
{
    ::fwGui::dialog::MessageDialog::sptr dialog = ::fwGui::dialog::MessageDialog::New();
    dialog->setTitle(m_title.toStdString());
    if (QMessageBox::Question == m_icon)
    {
        dialog->setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    }
    else if (QMessageBox::Information == m_icon)
    {
        dialog->setIcon(::fwGui::dialog::IMessageDialog::INFO);
    }
    else if (QMessageBox::Warning == m_icon)
    {
        dialog->setIcon(::fwGui::dialog::IMessageDialog::WARNING);
    }
    else if (QMessageBox::Critical == m_icon)
    {
        dialog->setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    }
    else
    {
        dialog->setIcon(::fwGui::dialog::IMessageDialog::NONE);
    }
    if (QMessageBox::Ok& m_buttons)
    {
        dialog->addButton(::fwGui::dialog::IMessageDialog::OK);
    }
    if (QMessageBox::Yes& m_buttons)
    {
        dialog->addButton(::fwGui::dialog::IMessageDialog::YES);
    }
    if (QMessageBox::No& m_buttons)
    {
        dialog->addButton(::fwGui::dialog::IMessageDialog::NO);
    }
    if (QMessageBox::Cancel& m_buttons)
    {
        dialog->addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    }
    dialog->setMessage(m_message.toStdString());
    dialog->show();
//    std::string result = ::fwGui::dialog::InputDialog::showInputDialog(
//        m_title.toStdString(), m_message.toStdString(), m_input.toStdString());
//    m_result = QString::fromStdString(result);
//    Q_EMIT onResultChanged();
}
