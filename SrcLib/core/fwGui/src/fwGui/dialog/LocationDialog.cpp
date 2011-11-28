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

//------------------------------------------------------------------------------

LocationDialog::~LocationDialog()
{}

//-----------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    return m_implementation->show();
}

//-----------------------------------------------------------------------------

void LocationDialog::setType( ::fwGui::dialog::ILocationDialog::Types type)
{
    m_implementation->setType( type );
}

//-----------------------------------------------------------------------------

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

void LocationDialog::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

const std::string& LocationDialog::getTitle()
{
    return m_implementation->getTitle();
}

//-----------------------------------------------------------------------------

void LocationDialog::setDefaultLocation(::fwData::location::ILocation::sptr loc)
{
    m_implementation->setDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path LocationDialog::getDefaultLocation()
{
    return m_implementation->getDefaultLocation();
}

//-----------------------------------------------------------------------------

void LocationDialog::saveDefaultLocation(::fwData::location::ILocation::sptr loc)
{
    m_implementation->saveDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
