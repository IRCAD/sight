/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Factory.hpp>

#include "fwGui/dialog/MessageDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

MessageDialog::MessageDialog()
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::IMessageDialog>( IMessageDialog::REGISTRY_KEY);
}

//-----------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{}

//-----------------------------------------------------------------------------

void MessageDialog::setTitle( const std::string &title )
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void MessageDialog::setMessage( const std::string &msg )
{
    m_implementation->setMessage(msg);
}

//-----------------------------------------------------------------------------

void MessageDialog::setIcon( ::fwGui::dialog::IMessageDialog::Icons icon )
{
    m_implementation->setIcon(icon);
}

//-----------------------------------------------------------------------------

void MessageDialog::addButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    m_implementation->addButton(button);
}

//-----------------------------------------------------------------------------

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    return m_implementation->show();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui



