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

#include "ui/qt/dialog/MessageDialog.hpp"

#include <ui/base/registry/macros.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>

fwGuiRegisterMacro( ::sight::ui::qt::dialog::MessageDialog, ::sight::ui::base::dialog::IMessageDialog::REGISTRY_KEY );

namespace sight::ui::qt
{
namespace dialog
{

//------------------------------------------------------------------------------

typedef const std::map< ui::base::dialog::IMessageDialog::Icons, QMessageBox::Icon> MessageDialogQtIconsType;
MessageDialogQtIconsType messageDialogQtIcons = { {ui::base::dialog::IMessageDialog::NONE, QMessageBox::NoIcon},
                                                  {ui::base::dialog::IMessageDialog::QUESTION, QMessageBox::Question },
                                                  {ui::base::dialog::IMessageDialog::INFO, QMessageBox::Information},
                                                  {ui::base::dialog::IMessageDialog::WARNING, QMessageBox::Warning},
                                                  {ui::base::dialog::IMessageDialog::CRITICAL, QMessageBox::Critical} };

typedef const std::map< ui::base::dialog::IMessageDialog::Buttons,
                        QMessageBox::StandardButtons> MessageDialogQtButtonType;
MessageDialogQtButtonType messageDialogQtButton = { {ui::base::dialog::IMessageDialog::OK, QMessageBox::Ok},
                                                    {ui::base::dialog::IMessageDialog::CANCEL, QMessageBox::Cancel},
                                                    {ui::base::dialog::IMessageDialog::YES, QMessageBox::Yes},
                                                    {ui::base::dialog::IMessageDialog::NO, QMessageBox::No} };

//------------------------------------------------------------------------------

MessageDialog::MessageDialog(ui::base::GuiBaseObject::Key ) :
    m_buttons(ui::base::dialog::IMessageDialog::NOBUTTON),
    m_defaultButton(ui::base::dialog::IMessageDialog::NOBUTTON),
    m_icon(ui::base::dialog::IMessageDialog::NONE)
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

void MessageDialog::setIcon( ui::base::dialog::IMessageDialog::Icons icon )
{
    m_icon = icon;
}

//------------------------------------------------------------------------------

void MessageDialog::addButton( ui::base::dialog::IMessageDialog::Buttons button )
{
    m_buttons = (ui::base::dialog::IMessageDialog::Buttons) ( m_buttons | button );
}

//------------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
    QPushButton* button = new QPushButton( QString::fromStdString(label) );
    m_customButtons.push_back( button );
    QObject::connect(button, &QPushButton::clicked, clickedFn);
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(ui::base::dialog::IMessageDialog::Buttons button )
{
    m_defaultButton = button;
}

//------------------------------------------------------------------------------

ui::base::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    MessageDialogQtIconsType::const_iterator iterIcon = messageDialogQtIcons.find(m_icon);
    SIGHT_ASSERT("Unknown Icon", iterIcon != messageDialogQtIcons.end());

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

    ui::base::dialog::IMessageDialog::Buttons result = ui::base::dialog::IMessageDialog::NOBUTTON;

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
} // namespace sight::ui::qt
