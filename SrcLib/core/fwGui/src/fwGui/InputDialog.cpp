/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Factory.hpp>

#include "fwGui/InputDialog.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

InputDialog::InputDialog()
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::IInputDialog>( IInputDialog::REGISTRY_KEY);
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

FWGUI_API void InputDialog::setInput(const std::string &text)
{
    m_implementation->setInput(text);
}

//-----------------------------------------------------------------------------

FWGUI_API std::string InputDialog::getInput()
{
    return m_implementation->getInput();
}

//-----------------------------------------------------------------------------

} // namespace fwGui



