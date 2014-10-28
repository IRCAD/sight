/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/dialog/SelectorDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

std::string SelectorDialog::showSelectorDialog(const std::string& title, const std::string& message, std::vector< std::string > _selections)
{
    ::fwGui::dialog::SelectorDialog  selector(title, message, _selections);
    return selector.show();
}

//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog(const std::string& title, const std::string& message, std::vector< std::string > _selections)
{
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ISelectorDialog::REGISTRY_KEY);
    m_implementation = ::fwGui::dialog::ISelectorDialog::dynamicCast(guiObj);
    m_implementation->setTitle(title);
    m_implementation->setMessage( message );
    m_implementation->setSelections( _selections );
}

//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog()
{
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ISelectorDialog::REGISTRY_KEY);
    m_implementation = ::fwGui::dialog::ISelectorDialog::dynamicCast(guiObj);
}

//-----------------------------------------------------------------------------

void SelectorDialog::setTitle(std::string title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

std::string SelectorDialog::show()
{
    return m_implementation->show();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setSelections(std::vector< std::string > _selections)
{
    m_implementation->setSelections( _selections );
}

//-----------------------------------------------------------------------------

void SelectorDialog::setMessage(const std::string &msg)
{
    m_implementation->setMessage( msg );
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
