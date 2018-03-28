/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/dialog/ILocationDialog.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace fwGui
{
namespace dialog
{

const ILocationDialog::FactoryRegistryKeyType ILocationDialog::REGISTRY_KEY = "::fwGui::dialog::LocationDialog";

const std::string ILocationDialog::SOFTWARE_UI          = "SOFTWARE_UI";
const std::string ILocationDialog::DLG_DEFAULT_LOCATION = "DLG_DEFAULT_LOCATION";

//-----------------------------------------------------------------------------

ILocationDialog::ILocationDialog()
{
}

//-----------------------------------------------------------------------------

ILocationDialog::~ILocationDialog()
{
}

//------------------------------------------------------------------------------

void ILocationDialog::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

const std::string& ILocationDialog::getTitle()
{
    return m_title;
}

//------------------------------------------------------------------------------

void ILocationDialog::setDefaultLocation( ::fwData::location::ILocation::sptr loc)
{
    ::fwData::location::SingleFile::csptr singleFile = ::fwData::location::SingleFile::dynamicConstCast(loc);
    ::fwData::location::Folder::csptr folder         = ::fwData::location::Folder::dynamicConstCast(loc);
    SLM_FATAL_IF( "Unsupported location",  !singleFile && !folder );
    m_defaultLocaction = loc;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path ILocationDialog::getDefaultLocation()
{
    ::boost::filesystem::path defaultPath;
    ::fwData::Composite::sptr prefUI = this->getPreferenceUI();
    ::fwData::location::ILocation::sptr location;
    if(prefUI)
    {
        if ( prefUI->find( ILocationDialog::DLG_DEFAULT_LOCATION ) != prefUI->end() )
        {
            location = ::fwData::location::ILocation::dynamicCast( (*prefUI)[ ILocationDialog::DLG_DEFAULT_LOCATION ] );
            SLM_ASSERT("LOCATION not correct", location);
        }
    }

    if(!location)
    {
        location = m_defaultLocaction;
    }

    ::fwData::location::SingleFile::csptr singleFile = ::fwData::location::SingleFile::dynamicConstCast(location);
    ::fwData::location::Folder::csptr folder         = ::fwData::location::Folder::dynamicConstCast(location);
    if (singleFile)
    {
        defaultPath = singleFile->getPath();
    }
    else if (folder)
    {
        defaultPath = folder->getFolder();
    }

    return defaultPath;
}

//-----------------------------------------------------------------------------

void ILocationDialog::saveDefaultLocation(::fwData::location::ILocation::sptr loc)
{
    ::fwData::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI && loc)
    {
        (*prefUI)[ ILocationDialog::DLG_DEFAULT_LOCATION ] = loc;
    }
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr ILocationDialog::getPreferenceUI()
{
    ::fwData::Composite::sptr prefUI;

    // Get preferences
    ::fwData::Composite::sptr prefs = ::fwPreferences::getPreferences();
    if(prefs)
    {
        ::fwData::Composite::sptr framesUI;
        // Retrieves software UI pref
        if ( prefs->find( ILocationDialog::SOFTWARE_UI ) != prefs->end() )
        {
            framesUI = ::fwData::Composite::dynamicCast( (*prefs)[ ILocationDialog::SOFTWARE_UI ]);
        }
        else
        {
            framesUI                                 = ::fwData::Composite::New();
            (*prefs)[ ILocationDialog::SOFTWARE_UI ] = framesUI;
        }
        // Retrieves associated dialog UI pref
        if ( framesUI->find( this->getTitle() ) != framesUI->end() )
        {
            prefUI = ::fwData::Composite::dynamicCast( (*framesUI)[ this->getTitle() ] );
        }
        else
        {
            prefUI                          = ::fwData::Composite::New();
            (*framesUI)[ this->getTitle() ] = prefUI;
        }
    }
    return prefUI;
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui
