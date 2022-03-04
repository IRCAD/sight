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

#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/String.hpp>

#include <service/IService.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/Preferences.hpp>

namespace sight::ui::base
{

namespace dialog
{

const ILocationDialog::FactoryRegistryKeyType ILocationDialog::REGISTRY_KEY = "::ui::base::dialog::LocationDialog";

const std::string ILocationDialog::SOFTWARE_UI           = "SOFTWARE_UI";
const std::string ILocationDialog::DLG_DEFAULT_FILE      = "DLG_DEFAULT_FILE";
const std::string ILocationDialog::DLG_DEFAULT_DIRECTORY = "DLG_DEFAULT_DIRECTORY";

//------------------------------------------------------------------------------

inline static std::string get_frame_key(const std::string& title)
{
    return ILocationDialog::SOFTWARE_UI + "." + title;
}

//------------------------------------------------------------------------------

inline static std::string get_file_key(const std::string& title)
{
    return get_frame_key(title) + "." + ILocationDialog::DLG_DEFAULT_FILE;
}

//------------------------------------------------------------------------------

inline static std::string get_directory_key(const std::string& title)
{
    return get_frame_key(title) + "." + ILocationDialog::DLG_DEFAULT_DIRECTORY;
}

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

void ILocationDialog::setDefaultLocation(core::location::ILocation::sptr loc)
{
    m_defaultLocaction = loc;
}

//------------------------------------------------------------------------------

const core::location::ILocation::sptr ILocationDialog::getDefaultLocation()
{
    core::location::ILocation::sptr location;

    try
    {
        ui::base::Preferences preferences;

        if(const auto& default_file = preferences.get_optional<std::filesystem::path>(get_file_key(getTitle()));
           default_file)
        {
            auto single_file = std::make_shared<core::location::SingleFile>();
            single_file->setFile(*default_file);
            location = single_file;
        }
        else if(const auto& default_directory =
                    preferences.get_optional<std::filesystem::path>(get_directory_key(getTitle())); default_directory)
        {
            auto single_directory = std::make_shared<core::location::SingleFolder>();
            single_directory->setFolder(*default_directory);
            location = single_directory;
        }
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }

    if(!location)
    {
        location = m_defaultLocaction;
    }

    return location;
}

//-----------------------------------------------------------------------------

void ILocationDialog::saveDefaultLocation(core::location::ILocation::sptr loc)
{
    if(loc)
    {
        try
        {
            ui::base::Preferences preferences;

            if(auto singleFile = core::location::SingleFile::dynamicCast(loc))
            {
                preferences.put(get_file_key(getTitle()), singleFile->getFile());
            }
            else if(auto singleDirectory = core::location::SingleFolder::dynamicCast(loc))
            {
                preferences.put(get_directory_key(getTitle()), singleDirectory->getFolder());
            }
        }
        catch(const ui::base::PreferencesDisabled&)
        {
            // Nothing to do..
        }
    }
}

//-----------------------------------------------------------------------------

} //namespace dialog

} // namespace sight::ui::base
