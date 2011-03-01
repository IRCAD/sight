/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Factory.hpp>

#include "fwGui/dialog/InputDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

std::string InputDialog::showInputDialog(const std::string& title, const std::string& message, const std::string &text)
{
    ::fwGui::dialog::InputDialog  inputBox(title, message, text);
    return inputBox.getInput();
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog()
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::IInputDialog>( IInputDialog::REGISTRY_KEY);
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog(const std::string& title, const std::string& message, const std::string &text)
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::IInputDialog>( IInputDialog::REGISTRY_KEY);
    m_implementation->setTitle(title);
    m_implementation->setMessage(message);
    m_implementation->setInput(text);
}

//-----------------------------------------------------------------------------

InputDialog::~InputDialog()
{}

//-----------------------------------------------------------------------------

void InputDialog::setTitle( const std::string &title )
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void InputDialog::setMessage( const std::string &msg )
{
    m_implementation->setMessage(msg);
}

//-----------------------------------------------------------------------------

void InputDialog::setInput(const std::string &text)
{
    m_implementation->setInput(text);
}

//-----------------------------------------------------------------------------

std::string InputDialog::getInput()
{
    return m_implementation->getInput();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui



