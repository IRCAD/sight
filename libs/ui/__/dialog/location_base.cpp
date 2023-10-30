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

#include <data/string.hpp>

#include <service/base.hpp>
#include <service/registry.hpp>

#include <ui/__/preferences.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::ui::dialog
{

const location_base::factory_registry_key_t location_base::REGISTRY_KEY = "::ui::dialog::location";

const std::string location_base::SOFTWARE_UI           = "SOFTWARE_UI";
const std::string location_base::DLG_DEFAULT_FILE      = "DLG_DEFAULT_FILE";
const std::string location_base::DLG_DEFAULT_DIRECTORY = "DLG_DEFAULT_DIRECTORY";

//------------------------------------------------------------------------------

inline static std::string get_frame_key(const std::string& _title)
{
    return std::string(location_base::SOFTWARE_UI) + "." + _title;
}

//------------------------------------------------------------------------------

inline static std::string get_file_key(const std::string& _title)
{
    return get_frame_key(_title) + "." + location_base::DLG_DEFAULT_FILE;
}

//------------------------------------------------------------------------------

inline static std::string get_directory_key(const std::string& _title)
{
    return get_frame_key(_title) + "." + location_base::DLG_DEFAULT_DIRECTORY;
}

//-----------------------------------------------------------------------------

location_base::location_base()
= default;

//-----------------------------------------------------------------------------

location_base::~location_base()
= default;

//------------------------------------------------------------------------------

void location_base::set_title(const std::string& _title)
{
    m_title = _title;
}

//------------------------------------------------------------------------------

const std::string& location_base::get_title()
{
    return m_title;
}

//------------------------------------------------------------------------------

void location_base::set_default_location(core::location::base::sptr _loc)
{
    m_default_locaction = _loc;
}

//------------------------------------------------------------------------------

core::location::base::sptr location_base::get_default_location()
{
    core::location::base::sptr location;

    try
    {
        ui::preferences preferences;

        if(const auto& default_file = preferences.get_optional<std::filesystem::path>(get_file_key(get_title()));
           default_file)
        {
            auto single_file = std::make_shared<core::location::single_file>();
            single_file->set_file(*default_file);
            location = single_file;
        }
        else if(const auto& default_directory =
                    preferences.get_optional<std::filesystem::path>(get_directory_key(get_title())); default_directory)
        {
            auto single_directory = std::make_shared<core::location::single_folder>();
            single_directory->set_folder(*default_directory);
            location = single_directory;
        }
    }
    catch(const ui::preferences_disabled&)
    {
        // Nothing to do..
    }

    if(!location)
    {
        location = m_default_locaction;
    }

    return location;
}

//-----------------------------------------------------------------------------

void location_base::save_default_location(core::location::base::sptr _loc)
{
    if(_loc)
    {
        try
        {
            ui::preferences preferences;

            if(auto single_file = std::dynamic_pointer_cast<core::location::single_file>(_loc))
            {
                preferences.put(get_file_key(get_title()), single_file->get_file());
            }
            else if(auto single_directory = std::dynamic_pointer_cast<core::location::single_folder>(_loc))
            {
                preferences.put(get_directory_key(get_title()), single_directory->get_folder());
            }
        }
        catch(const ui::preferences_disabled&)
        {
            // Nothing to do..
        }
    }
}

//------------------------------------------------------------------------------

std::vector<std::string> location_base::get_selected_extensions() const
{
    // Get the current selection, remove all "*" characters
    const std::string& selection = boost::replace_all_copy(get_current_selection(), "*", "");

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
