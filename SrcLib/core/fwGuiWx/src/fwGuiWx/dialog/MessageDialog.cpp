/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/msgdlg.h>

#include <fwGui/registry/macros.hpp>

#include <fwGuiWx/convert.hpp>
#include "fwGuiWx/dialog/MessageDialog.hpp"


fwGuiRegisterMacro( ::fwGuiWx::dialog::MessageDialog, ::fwGui::dialog::IMessageDialog::REGISTRY_KEY );

namespace fwGuiWx
{
namespace dialog
{

MessageDialog::MessageDialog(::fwGui::GuiBaseObject::Key key) : m_title(""), m_message(""), m_buttons(0)
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

void MessageDialog::setIcon( ::fwGui::dialog::IMessageDialog::Icons icon )
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

void MessageDialog::addButton( ::fwGui::dialog::IMessageDialog::Buttons button )
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

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    ::fwGui::dialog::IMessageDialog::Buttons result;
    int wxResult = wxMessageBox( ::fwGuiWx::std2wx(m_message), ::fwGuiWx::std2wx(m_title), m_buttons|m_icon );

    if (wxResult == wxOK)
    {
        result = ::fwGui::dialog::IMessageDialog::OK;
    }
    else if (wxResult == wxYES)
    {
        result = ::fwGui::dialog::IMessageDialog::YES;
    }
    else if (wxResult == wxNO)
    {
        result = ::fwGui::dialog::IMessageDialog::NO;
    }
    else if (wxResult == wxCANCEL)
    {
        result = ::fwGui::dialog::IMessageDialog::CANCEL;
    }
    return result;
}
} // namespace dialog
} // namespace fwGuiWx



