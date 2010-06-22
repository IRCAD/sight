/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/LocationDialog.hpp"

namespace fwGui
{


LocationDialog::LocationDialog()
{
        m_implementation = fwTools::ClassFactoryRegistry::create< ILocationDialog>( ILocationDialog::factoryRegistryKey);

}



void LocationDialog::setTitle(const std::string &title)
{
    m_implementation->setTitle(title);
}



::fwData::location::ILocation::sptr LocationDialog::show()
{
    return m_implementation->show();
}



void LocationDialog::setDefaultLocation( ::fwData::location::ILocation::csptr loc)
{
    return m_implementation->setDefaultLocation( loc );
}



// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string &filterName, const std::string &wildcardList )
{
    return m_implementation->addFilter(filterName,wildcardList);
}



ILocationDialog& LocationDialog::setOption( ILocationDialog::Options option)
{
    return m_implementation->setOption(option);
}



} //namespace fwGui
