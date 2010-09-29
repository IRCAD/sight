/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwGui/dialog/SelectorDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog()
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::ISelectorDialog>( ::fwGui::dialog::ISelectorDialog::REGISTRY_KEY);
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

} //namespace dialog
} //namespace fwGui
