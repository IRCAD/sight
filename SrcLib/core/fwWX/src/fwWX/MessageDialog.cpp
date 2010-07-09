/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwWX/convert.hpp"
#include "fwWX/MessageDialog.hpp"


REGISTER_BINDING( ::fwGui::IMessageDialog, ::fwWX::MessageDialog, ::fwGui::IMessageDialog::FactoryRegistryKeyType , ::fwGui::IMessageDialog::REGISTRY_KEY );

namespace fwWX
{

MessageDialog::MessageDialog() : m_buttons(0)
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
    if (icon == IMessageDialog::CRITICAL)
    {
        m_icon = wxICON_ERROR;
    }
    else if (icon == IMessageDialog::WARNING)
    {
        m_icon = wxICON_EXCLAMATION;
    }
    else if (icon == IMessageDialog::INFO)
    {
        m_icon = wxICON_INFORMATION;
    }
    else if (icon == IMessageDialog::QUESTION)
    {
        m_icon = wxICON_QUESTION;
    }
}

void MessageDialog::addButton( ::fwGui::IMessageDialog::Buttons button )
{
    if (button == IMessageDialog::OK)
    {
        m_buttons |= wxOK;
    }
    else if (button == IMessageDialog::CANCEL)
    {
        m_buttons |= wxCANCEL;
    }
    else if (button == IMessageDialog::YES_NO)
    {
        m_buttons |= wxYES_NO;
    }
}

::fwGui::IMessageDialog::Buttons MessageDialog::show()
{
    ::fwGui::IMessageDialog::Buttons result;
    int wxResult = wxMessageBox( ::fwWX::std2wx(m_message), ::fwWX::std2wx(m_title), m_buttons|m_icon );

    if (wxResult == wxOK)
    {
        result = ::fwGui::IMessageDialog::OK;
    }
    else if (wxResult == wxYES)
    {
        result = ::fwGui::IMessageDialog::YES;
    }
    else if (wxResult == wxNO)
    {
        result = ::fwGui::IMessageDialog::NO;
    }
    else if (wxResult == wxCANCEL)
    {
        result = ::fwGui::IMessageDialog::CANCEL;
    }
    return result;
}

} // namespace fwWX



