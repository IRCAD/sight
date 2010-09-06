/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include <QMessageBox>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/MessageDialog.hpp"


REGISTER_BINDING( ::fwGui::IMessageDialog, ::fwGuiQt::MessageDialog, ::fwGui::IMessageDialog::FactoryRegistryKeyType , ::fwGui::IMessageDialog::REGISTRY_KEY );

namespace fwGuiQt
{

typedef const std::map< ::fwGui::IMessageDialog::Icons, QMessageBox::Icon> MessageDialogQtIconsType;
MessageDialogQtIconsType messageDialogQtIcons =
                        ::boost::assign::map_list_of(::fwGui::IMessageDialog::NONE     , QMessageBox::NoIcon     )
                                                    (::fwGui::IMessageDialog::QUESTION , QMessageBox::Question   )
                                                    (::fwGui::IMessageDialog::INFO     , QMessageBox::Information)
                                                    (::fwGui::IMessageDialog::WARNING  , QMessageBox::Warning    )
                                                    (::fwGui::IMessageDialog::CRITICAL , QMessageBox::Critical   );

typedef const std::map< ::fwGui::IMessageDialog::Buttons, QMessageBox::StandardButtons> MessageDialogQtButtonType;
MessageDialogQtButtonType messageDialogQtButton =
                        ::boost::assign::map_list_of(::fwGui::IMessageDialog::OK     , QMessageBox::Ok   )
                                                    (::fwGui::IMessageDialog::CANCEL , QMessageBox::Cancel)
                                                    (::fwGui::IMessageDialog::YES    , QMessageBox::Yes    )
                                                    (::fwGui::IMessageDialog::NO     , QMessageBox::No   );




MessageDialog::MessageDialog() : m_buttons(::fwGui::IMessageDialog::NOBUTTON)
{}


MessageDialog::~MessageDialog()
{}

void MessageDialog::setTitle( const std::string &title )
{
    m_title = title;
}

void MessageDialog::setMessage( const std::string &msg )
{
    m_message = msg;
}

void MessageDialog::setIcon( ::fwGui::IMessageDialog::Icons icon )
{
    m_icon = icon;
}

void MessageDialog::addButton( ::fwGui::IMessageDialog::Buttons button )
{
    m_buttons = (::fwGui::IMessageDialog::Buttons) ( m_buttons | button );
}

::fwGui::IMessageDialog::Buttons MessageDialog::show()
{
    MessageDialogQtIconsType::const_iterator iterIcon = messageDialogQtIcons.find(m_icon);
    SLM_ASSERT("Unknwown Icon", iterIcon != messageDialogQtIcons.end())

    QMessageBox::Icon icon = iterIcon->second;
    QString title = QString::fromStdString(m_title);
    QString text = QString::fromStdString(m_message);
    QMessageBox::StandardButtons buttons = QMessageBox::NoButton;
    QWidget *parent = 0;

    BOOST_FOREACH(MessageDialogQtButtonType::value_type button, messageDialogQtButton)
    {
        if ( m_buttons & button.first)
        {
            buttons |= button.second;
        }
    }


    QMessageBox box(icon, title, text, buttons, parent);
    ::fwGui::IMessageDialog::Buttons result;

    box.exec();

    BOOST_FOREACH(MessageDialogQtButtonType::value_type button, messageDialogQtButton)
    {
        if ( box.standardButton( box.clickedButton() ) == button.second)
        {
            result = button.first;
            break;
        }
    }

    return result;
}

} // namespace fwGuiQt



