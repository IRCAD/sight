/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/dialog/MessageDialog.hpp"

#include <fwGui/registry/macros.hpp>

#include <boost/assign/list_of.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>

fwGuiRegisterMacro( ::fwGuiQt::dialog::MessageDialog, ::fwGui::dialog::IMessageDialog::REGISTRY_KEY );

namespace fwGuiQt
{
namespace dialog
{

//------------------------------------------------------------------------------

typedef const std::map< ::fwGui::dialog::IMessageDialog::Icons, QMessageBox::Icon> MessageDialogQtIconsType;
MessageDialogQtIconsType messageDialogQtIcons =
    ::boost::assign::map_list_of(::fwGui::dialog::IMessageDialog::NONE, QMessageBox::NoIcon     )
        (::fwGui::dialog::IMessageDialog::QUESTION, QMessageBox::Question   )
        (::fwGui::dialog::IMessageDialog::INFO, QMessageBox::Information)
        (::fwGui::dialog::IMessageDialog::WARNING, QMessageBox::Warning    )
        (::fwGui::dialog::IMessageDialog::CRITICAL, QMessageBox::Critical   );

typedef const std::map< ::fwGui::dialog::IMessageDialog::Buttons,
                        QMessageBox::StandardButtons> MessageDialogQtButtonType;
MessageDialogQtButtonType messageDialogQtButton =
    ::boost::assign::map_list_of(::fwGui::dialog::IMessageDialog::OK, QMessageBox::Ok   )
        (::fwGui::dialog::IMessageDialog::CANCEL, QMessageBox::Cancel)
        (::fwGui::dialog::IMessageDialog::YES, QMessageBox::Yes    )
        (::fwGui::dialog::IMessageDialog::NO, QMessageBox::No   );

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
    QPushButton* button = new QPushButton( QString::fromStdString(label) );
    m_customButtons.push_back( button );
    QObject::connect(button, &QPushButton::clicked, clickedFn);
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(::fwGui::dialog::IMessageDialog::Buttons button )
{
    m_defaultButton = button;
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
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

    ::fwGui::dialog::IMessageDialog::Buttons result = ::fwGui::dialog::IMessageDialog::NOBUTTON;

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
} // namespace fwGuiQt
