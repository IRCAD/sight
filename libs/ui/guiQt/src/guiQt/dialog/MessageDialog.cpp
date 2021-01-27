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

#include "guiQt/dialog/MessageDialog.hpp"

#include <gui/registry/macros.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>

fwGuiRegisterMacro( ::sight::guiQt::dialog::MessageDialog, ::sight::gui::dialog::IMessageDialog::REGISTRY_KEY );

namespace sight::guiQt
{
namespace dialog
{

//------------------------------------------------------------------------------

typedef const std::map< gui::dialog::IMessageDialog::Icons, QMessageBox::Icon> MessageDialogQtIconsType;
MessageDialogQtIconsType messageDialogQtIcons = { {gui::dialog::IMessageDialog::NONE, QMessageBox::NoIcon},
                                                  {gui::dialog::IMessageDialog::QUESTION, QMessageBox::Question },
                                                  {gui::dialog::IMessageDialog::INFO, QMessageBox::Information},
                                                  {gui::dialog::IMessageDialog::WARNING, QMessageBox::Warning},
                                                  {gui::dialog::IMessageDialog::CRITICAL, QMessageBox::Critical} };

typedef const std::map< gui::dialog::IMessageDialog::Buttons,
                        QMessageBox::StandardButtons> MessageDialogQtButtonType;
MessageDialogQtButtonType messageDialogQtButton = { {gui::dialog::IMessageDialog::OK, QMessageBox::Ok},
                                                    {gui::dialog::IMessageDialog::CANCEL, QMessageBox::Cancel},
                                                    {gui::dialog::IMessageDialog::YES, QMessageBox::Yes},
                                                    {gui::dialog::IMessageDialog::NO, QMessageBox::No} };

//------------------------------------------------------------------------------

MessageDialog::MessageDialog(gui::GuiBaseObject::Key ) :
    m_buttons(gui::dialog::IMessageDialog::NOBUTTON),
    m_defaultButton(gui::dialog::IMessageDialog::NOBUTTON),
    m_icon(gui::dialog::IMessageDialog::NONE)
{
}

//------------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
}

//------------------------------------------------------------------------------

void MessageDialog::setTitle( const std::string& title )
{
    m_title = title;
}

//------------------------------------------------------------------------------

void MessageDialog::setMessage( const std::string& msg )
{
    m_message = msg;
}

//------------------------------------------------------------------------------

void MessageDialog::setIcon( gui::dialog::IMessageDialog::Icons icon )
{
    m_icon = icon;
}

//------------------------------------------------------------------------------

void MessageDialog::addButton( gui::dialog::IMessageDialog::Buttons button )
{
    m_buttons = (gui::dialog::IMessageDialog::Buttons) ( m_buttons | button );
}

//------------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
    QPushButton* button = new QPushButton( QString::fromStdString(label) );
    m_customButtons.push_back( button );
    QObject::connect(button, &QPushButton::clicked, clickedFn);
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(gui::dialog::IMessageDialog::Buttons button )
{
    m_defaultButton = button;
}

//------------------------------------------------------------------------------

gui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    MessageDialogQtIconsType::const_iterator iterIcon = messageDialogQtIcons.find(m_icon);
    SLM_ASSERT("Unknown Icon", iterIcon != messageDialogQtIcons.end());

    QMessageBox::Icon icon               = iterIcon->second;
    QString title                        = QString::fromStdString(m_title);
    QString text                         = QString::fromStdString(m_message);
    QMessageBox::StandardButtons buttons = QMessageBox::NoButton;

    for(MessageDialogQtButtonType::value_type button :  messageDialogQtButton)
    {
        if ( m_buttons & button.first)
        {
            buttons |= button.second;
        }
    }

    QMessageBox box(icon, title, text, buttons, qApp->activeWindow());

    for(auto customButton : m_customButtons)
    {
        box.addButton(customButton, QMessageBox::ActionRole);
    }

    MessageDialogQtButtonType::const_iterator iter = messageDialogQtButton.find(m_defaultButton);
    if(iter != messageDialogQtButton.end())
    {
        box.setDefaultButton(QMessageBox::StandardButton(static_cast<int>(iter->second)));
    }

    gui::dialog::IMessageDialog::Buttons result = gui::dialog::IMessageDialog::NOBUTTON;

    box.exec();

    for(MessageDialogQtButtonType::value_type button :  messageDialogQtButton)
    {
        if ( box.standardButton( box.clickedButton() ) == button.second)
        {
            result = button.first;
            break;
        }
    }
    return result;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace sight::guiQt
