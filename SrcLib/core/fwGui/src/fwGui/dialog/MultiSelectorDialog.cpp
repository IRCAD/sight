/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwGui/dialog/MultiSelectorDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog()
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::IMultiSelectorDialog>( ::fwGui::dialog::IMultiSelectorDialog::REGISTRY_KEY);
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    return m_implementation->show();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    m_implementation->setSelections( _selections );
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string &msg)
{
    m_implementation->setMessage( msg );
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
