/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/dialog/location_base.hpp"

#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/String.hpp>

#include <service/base.hpp>
#include <service/registry.hpp>

#include <ui/__/Preferences.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::ui::dialog
{

const location_base::FactoryRegistryKeyType location_base::REGISTRY_KEY = "::ui::dialog::location";

const std::string location_base::SOFTWARE_UI           = "SOFTWARE_UI";
const std::string location_base::DLG_DEFAULT_FILE      = "DLG_DEFAULT_FILE";
const std::string location_base::DLG_DEFAULT_DIRECTORY = "DLG_DEFAULT_DIRECTORY";

//------------------------------------------------------------------------------

inline static std::string getFrameKey(const std::string& title)
{
    return std::string(location_base::SOFTWARE_UI) + "." + title;
}

//------------------------------------------------------------------------------

inline static std::string getFileKey(const std::string& title)
{
    return getFrameKey(title) + "." + location_base::DLG_DEFAULT_FILE;
}

//------------------------------------------------------------------------------

inline static std::string getDirectoryKey(const std::string& title)
{
    return getFrameKey(title) + "." + location_base::DLG_DEFAULT_DIRECTORY;
}

//-----------------------------------------------------------------------------

location_base::location_base()
= default;

//-----------------------------------------------------------------------------

location_base::~location_base()
= default;

//------------------------------------------------------------------------------

void location_base::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

const std::string& location_base::getTitle()
{
    return m_title;
}

//------------------------------------------------------------------------------

void location_base::setDefaultLocation(core::location::base::sptr loc)
{
    m_defaultLocaction = loc;
}

//------------------------------------------------------------------------------

core::location::base::sptr location_base::getDefaultLocation()
{
    core::location::base::sptr location;

    try
    {
        ui::Preferences preferences;

        if(const auto& default_file = preferences.get_optional<std::filesystem::path>(getFileKey(getTitle()));
           default_file)
        {
            auto single_file = std::make_shared<core::location::single_file>();
            single_file->set_file(*default_file);
            location = single_file;
        }
        else if(const auto& default_directory =
                    preferences.get_optional<std::filesystem::path>(getDirectoryKey(getTitle())); default_directory)
        {
            auto single_directory = std::make_shared<core::location::single_folder>();
            single_directory->set_folder(*default_directory);
            location = single_directory;
        }
    }
    catch(const ui::PreferencesDisabled&)
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

void location_base::saveDefaultLocation(core::location::base::sptr loc)
{
    if(loc)
    {
        try
        {
            ui::Preferences preferences;

            if(auto singleFile = std::dynamic_pointer_cast<core::location::single_file>(loc))
            {
                preferences.put(getFileKey(getTitle()), singleFile->get_file());
            }
            else if(auto singleDirectory = std::dynamic_pointer_cast<core::location::single_folder>(loc))
            {
                preferences.put(getDirectoryKey(getTitle()), singleDirectory->get_folder());
            }
        }
        catch(const ui::PreferencesDisabled&)
        {
            // Nothing to do..
        }
    }
}

//------------------------------------------------------------------------------

std::vector<std::string> location_base::getSelectedExtensions() const
{
    // Get the current selection, remove all "*" characters
    const std::string& selection = boost::replace_all_copy(getCurrentSelection(), "*", "");

    // Split the selection into a vector of extensions
    std::vector<std::string> extensions;

    boost::split(
        extensions,
        selection,
        boost::is_any_of(" "),
        boost::token_compress_on
    );

    return extensions;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
