/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ui/base/dialog/ILocationDialog.hpp"

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <service/IService.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/preferences/helper.hpp>

namespace sight::ui::base
{
namespace dialog
{

const ILocationDialog::FactoryRegistryKeyType ILocationDialog::REGISTRY_KEY = "::ui::base::dialog::LocationDialog";

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

void ILocationDialog::setDefaultLocation( data::location::ILocation::sptr loc)
{
    data::location::SingleFile::csptr singleFile = data::location::SingleFile::dynamicConstCast(loc);
    data::location::Folder::csptr folder         = data::location::Folder::dynamicConstCast(loc);
    SIGHT_FATAL_IF( "Unsupported location",  !singleFile && !folder );
    m_defaultLocaction = loc;
}

//------------------------------------------------------------------------------

const std::filesystem::path ILocationDialog::getDefaultLocation()
{
    std::filesystem::path defaultPath;
    data::Composite::sptr prefUI = this->getPreferenceUI();
    data::location::ILocation::sptr location;
    if(prefUI)
    {
        if ( prefUI->find( ILocationDialog::DLG_DEFAULT_LOCATION ) != prefUI->end() )
        {
            location = data::location::ILocation::dynamicCast( (*prefUI)[ ILocationDialog::DLG_DEFAULT_LOCATION ] );
            SIGHT_ASSERT("LOCATION not correct", location);
        }
    }

    if(!location)
    {
        location = m_defaultLocaction;
    }

    data::location::SingleFile::csptr singleFile = data::location::SingleFile::dynamicConstCast(location);
    data::location::Folder::csptr folder         = data::location::Folder::dynamicConstCast(location);
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

void ILocationDialog::saveDefaultLocation(data::location::ILocation::sptr loc)
{
    data::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI && loc)
    {
        (*prefUI)[ ILocationDialog::DLG_DEFAULT_LOCATION ] = loc;
    }
}

//-----------------------------------------------------------------------------

data::Composite::sptr ILocationDialog::getPreferenceUI()
{
    data::Composite::sptr prefUI;

    // Get preferences
    data::Composite::sptr prefs = ui::base::preferences::getPreferences();
    if(prefs)
    {
        data::Composite::sptr framesUI;
        // Retrieves software UI pref
        if ( prefs->find( ILocationDialog::SOFTWARE_UI ) != prefs->end() )
        {
            framesUI = data::Composite::dynamicCast( (*prefs)[ ILocationDialog::SOFTWARE_UI ]);
        }
        else
        {
            framesUI                                 = data::Composite::New();
            (*prefs)[ ILocationDialog::SOFTWARE_UI ] = framesUI;
        }
        // Retrieves associated dialog UI pref
        if ( framesUI->find( this->getTitle() ) != framesUI->end() )
        {
            prefUI = data::Composite::dynamicCast( (*framesUI)[ this->getTitle() ] );
        }
        else
        {
            prefUI                          = data::Composite::New();
            (*framesUI)[ this->getTitle() ] = prefUI;
        }
    }
    return prefUI;
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace sight::ui::base
