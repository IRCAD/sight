/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/dialog/LocationDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

LocationDialog::LocationDialog()
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::ILocationDialog>( ::fwGui::dialog::ILocationDialog::REGISTRY_KEY);
}

//-----------------------------------------------------------------------------

void LocationDialog::setTitle(const std::string &title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    return m_implementation->show();
}

//-----------------------------------------------------------------------------

void LocationDialog::setDefaultLocation( ::fwData::location::ILocation::csptr loc)
{
    m_implementation->setDefaultLocation( loc );
}

//-----------------------------------------------------------------------------

void LocationDialog::setType( ::fwGui::dialog::ILocationDialog::Types type)
{
    m_implementation->setType( type );
}

//-----------------------------------------------------------------------------

// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string &filterName, const std::string &wildcardList )
{
    m_implementation->addFilter(filterName,wildcardList);
}

//-----------------------------------------------------------------------------

ILocationDialog& LocationDialog::setOption( ::fwGui::dialog::ILocationDialog::Options option)
{
    return m_implementation->setOption(option);
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
