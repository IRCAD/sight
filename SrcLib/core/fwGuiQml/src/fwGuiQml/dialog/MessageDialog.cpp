/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/MessageDialog.hpp"

#include <fwGui/registry/macros.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QVector>

fwGuiRegisterMacro( ::fwGuiQml::dialog::MessageDialog, ::fwGui::dialog::IMessageDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

//value of the enum in integer
typedef const std::map< ::fwGui::dialog::IMessageDialog::Icons, int> MessageDialogQmlIconsType;
MessageDialogQmlIconsType messageDialogQmlIcons =
    ::boost::assign::map_list_of(::fwGui::dialog::IMessageDialog::NONE, 0)
        (::fwGui::dialog::IMessageDialog::QUESTION, 4)
        (::fwGui::dialog::IMessageDialog::INFO, 1)
        (::fwGui::dialog::IMessageDialog::WARNING, 2)
        (::fwGui::dialog::IMessageDialog::CRITICAL, 3);

typedef const std::map< ::fwGui::dialog::IMessageDialog::Buttons,
                        int> MessageDialogQmlButtonType;
MessageDialogQmlButtonType messageDialogQmlButton =
    ::boost::assign::map_list_of(::fwGui::dialog::IMessageDialog::OK, 0x00000400   )
        (::fwGui::dialog::IMessageDialog::CANCEL, 0x00400000)
        (::fwGui::dialog::IMessageDialog::YES, 0x00004000    )
        (::fwGui::dialog::IMessageDialog::NO, 0x00010000   );

//------------------------------------------------------------------------------

MessageDialog::MessageDialog(::fwGui::GuiBaseObject::Key key) :
    m_buttons(::fwGui::dialog::IMessageDialog::NOBUTTON),
    m_defaultButton(::fwGui::dialog::IMessageDialog::NOBUTTON),
    m_icon(::fwGui::dialog::IMessageDialog::NONE)
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

void MessageDialog::setIcon( ::fwGui::dialog::IMessageDialog::Icons icon )
{
    m_icon = icon;
}

//------------------------------------------------------------------------------

void MessageDialog::addButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    m_buttons = (::fwGui::dialog::IMessageDialog::Buttons) ( m_buttons | button );
}

//------------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
//    QPushButton* button = new QPushButton( QString::fromStdString(label) );
//    m_customButtons.push_back( button );
//    QObject::connect(button, &QPushButton::clicked, clickedFn);
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(::fwGui::dialog::IMessageDialog::Buttons button )
{
    m_defaultButton = button;
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    MessageDialogQmlIconsType::const_iterator iterIcon = messageDialogQmlIcons.find(m_icon);
    SLM_ASSERT("Unknown Icon", iterIcon != messageDialogQmlIcons.end());

    int icon      = iterIcon->second;
    QString title = QString::fromStdString(m_title);
    QString text  = QString::fromStdString(m_message);
    // 0 int value means no Button
    int buttons = 0x00000000;

    for(MessageDialogQmlButtonType::value_type button :  messageDialogQmlButton)
    {
        if ( m_buttons & button.first)
        {
            buttons |= button.second;
        }
    }

    //QMessageBox box(icon, title, text, buttons, qApp->focusWindow());

//    for(auto customButton : m_customButtons)
//    {
//        // box.addButton(customButton, QMessageBox::ActionRole);
//    }

    MessageDialogQmlButtonType::const_iterator iter = messageDialogQmlButton.find(m_defaultButton);
    if(iter != messageDialogQmlButton.end())
    {
        // box.setDefaultButton(QMessageBox::StandardButton(static_cast<int>(iter->second)));
    }

    ::fwGui::dialog::IMessageDialog::Buttons result = ::fwGui::dialog::IMessageDialog::NOBUTTON;

    //box.exec();

//    for(MessageDialogQmlButtonType::value_type button :  messageDialogQmlButton)
//    {
//        if ( box.standardButton( box.clickedButton() ) == button.second)
//        {
//            result = button.first;
//            break;
//        }
//    }
    return result;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
