/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include "io/__/service/writer.hpp"

#include <ui/__/preferences.hpp>

namespace sight::io::service
{

//-----------------------------------------------------------------------------

writer::writer(const std::string& _default_window_title) noexcept :
    has_monitors(has_signals::signals()),
    m_window_title(this, WINDOW_TITLE_KEY, _default_window_title)
{
    new_signal<signals::void_t>(signals::FAILED);
    new_signal<signals::void_t>(signals::SUCCEEDED);
    new_signal<signals::void_t>(signals::PREFIX_SET);
    new_signal<signals::void_t>(signals::BASE_FOLDER_SET);

    new_slot(slots::OPEN_LOCATION_DIALOG, &writer::open_location_dialog, this);
    new_slot(slots::SET_PREFIX, &writer::set_prefix, this);
    new_slot(slots::SET_BASE_FOLDER, &writer::set_base_folder, this);

    new_slot(slots::UPDATE_DEFAULT_LOCATIONS, &writer::update_default_locations, this);
}

//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> > writer::get_supported_extensions()
{
    return {};
}

//-----------------------------------------------------------------------------

std::filesystem::path writer::get_file() const
{
    // WARNING: Pay attention here that no canonical paths are processed when concatenating paths
    // In this case, the behavior is to replace the previous path.

    SIGHT_THROW_IF("This writer doesn't manage files", !(this->get_path_type() & io::service::file));

    // Make sure the base folder is in sync (also re-reads file/folder entries to respect the developer choices)
    std::string base_folder;
    this->update_base_folder(base_folder);

    // Pre-pend the base folder by default, whether it is unset and empty
    // Or pre-filled by the user
    std::filesystem::path location = std::filesystem::path(base_folder);

    // Only use the prefix if we have a base folder,
    // We don't want to modify an user defined path (set through the GUI)
    if(!base_folder.empty() && !m_current_prefix.empty())
    {
        location /= std::filesystem::path(m_current_prefix);
    }

    SIGHT_THROW_IF("Exactly one file must be defined in location", m_locations.size() != 1);

    location /= m_locations.front();

    return location;
}

//-----------------------------------------------------------------------------

void writer::set_file(const std::filesystem::path& _file)
{
    SIGHT_THROW_IF("This writer doesn't manage files", !(this->get_path_type() & io::service::file));
    m_locations.clear();
    m_locations.push_back(_file);
}

//-----------------------------------------------------------------------------

const io::service::locations_t& writer::get_files() const
{
    SIGHT_THROW_IF("This writer doesn't manage files", !(this->get_path_type() & io::service::files));
    SIGHT_THROW_IF("At least one file must be defined in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void writer::set_files(const io::service::locations_t& _files)
{
    SIGHT_THROW_IF("This writer doesn't manage files", !(this->get_path_type() & io::service::files));
    m_locations = _files;
}

//-----------------------------------------------------------------------------

std::filesystem::path writer::get_folder() const
{
    // WARNING: Pay attention here that no canonical paths are processed when concatenating paths
    // In this case, the behavior is to replace the previous path.

    SIGHT_THROW_IF("This writer doesn't manage folders", !(this->get_path_type() & io::service::folder));

    // Make sure the base folder is in sync (also re-reads file/folder entries to respect the developer choices)
    std::string base_folder;
    this->update_base_folder(base_folder);

    std::filesystem::path location = std::filesystem::path(base_folder);

    // Only use the prefix if we have a base folder,
    // We don't want to modify an user defined path (set through the GUI)
    if(!base_folder.empty() && !m_current_prefix.empty())
    {
        location /= std::filesystem::path(m_current_prefix);
    }

    location /= (m_locations.empty() ? "" : m_locations.front());

    return location;
}

//-----------------------------------------------------------------------------

void writer::set_folder(const std::filesystem::path& _folder)
{
    SIGHT_THROW_IF("This writer doesn't manage folders", !(this->get_path_type() & io::service::folder));
    m_locations.clear();
    m_locations.push_back(_folder);
}

//------------------------------------------------------------------------------

void writer::set_prefix(std::string _prefix)
{
    // Record the current prefix
    m_current_prefix = _prefix;

    this->async_emit(signals::PREFIX_SET);
}

//------------------------------------------------------------------------------

void writer::set_base_folder(std::string _path)
{
    // Record the current prefix
    m_base_folder = _path;

    this->async_emit(signals::BASE_FOLDER_SET);
}

//-----------------------------------------------------------------------------

const io::service::locations_t& writer::get_locations() const
{
    SIGHT_THROW_IF("At least one path must be defined in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void writer::clear_locations()
{
    m_locations.clear();
}

//-----------------------------------------------------------------------------

void writer::configuring()
{
    /// @todo check if this is still needed or usefull or not a static assert at build time...
    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that use paths.",
        !(this->get_path_type() & io::service::type_not_defined)
    );

    SIGHT_ASSERT(
        "This writer cannot manages both io::service::files and io::service::file.",
        (this->get_path_type() & io::service::files) == 0 || (this->get_path_type() & io::service::file) == 0
    );

    this->update_default_locations();
}

//------------------------------------------------------------------------------

sight::service::base::connections_t writer::auto_connections() const
{
    return {
        {m_files, data::signals::MODIFIED, slots::UPDATE_DEFAULT_LOCATIONS},
        {m_folder, data::signals::MODIFIED, slots::UPDATE_DEFAULT_LOCATIONS}
    };
}

//------------------------------------------------------------------------------

void writer::update_default_locations()
{
    io::service::locations_t files;

    if(const auto files_property = m_files.const_lock(); files_property && !files_property->value().empty())
    {
        boost::split(files, files_property->value(), boost::is_any_of(";"), boost::token_compress_on);
    }

    const auto folder_property = m_folder.const_lock();
    this->update_locations(files, folder_property ? folder_property->value() : std::string());
}

//------------------------------------------------------------------------------

void writer::update_locations(
    const io::service::locations_t& _files,
    const std::filesystem::path& _folder
)
{
    // Assertion checks
    SIGHT_ASSERT(
        "This writer does not manage files and a file path is given in the configuration",
        (this->get_path_type() & io::service::file || this->get_path_type() & io::service::files) || (_files.empty())
    );

    // Populate m_locations with files
    if(!_files.empty())
    {
        if((this->get_path_type() & io::service::file) != 0)
        {
            // Single file
            SIGHT_ASSERT(
                "This writer manage single file, but there is more than one file.",
                _files.size() == 1
            );

            this->set_file(_files.front());
        }
        else if((this->get_path_type() & io::service::files) != 0)
        {
            // Multiple files
            this->set_files(_files);
        }
    }

    // Populate m_locations with folder
    if((this->get_path_type() & io::service::folder) != 0 && !_folder.empty())
    {
        this->set_folder(_folder);
    }
}

//------------------------------------------------------------------------------

void writer::update_base_folder(std::string& _out_base_folder) const
{
    const sight::service::config_t config = this->get_config();

    std::string base_folder_cfg = m_base_folder;

    if(base_folder_cfg.empty())
    {
        base_folder_cfg = config.get<std::string>(WRITER_BASE_FOLDER_KEY, m_base_folder);
    }

    if(!base_folder_cfg.empty())
    {
        // We need to check for potential updates to the base folder
        // In case the user updates an associated preference during runtime
        // We also re-read the xml configuration for file and folder if base_folder was set
        sight::ui::preferences preferences;

        // We need to provide the separator, otherwise the 2-args method will be selected
        auto [key, base_folder] = preferences.parsed_get<std::string>(base_folder_cfg);

        // If the returned value is indeed a preference
        if(!key.empty())
        {
            if(!base_folder.empty())
            {
                _out_base_folder = base_folder;
            }
            else
            {
                SIGHT_INFO("No location defined for the preference " << base_folder_cfg);
            }
        }
        else
        {
            if(std::filesystem::exists(base_folder_cfg))
            {
                _out_base_folder = base_folder_cfg;
            }
            else
            {
                SIGHT_ERROR(base_folder_cfg << " is invalid. Reverting to classical selection.");
            }
        }
    }
}

//------------------------------------------------------------------------------

io::service::path_type_t writer::get_path_type() const
{
    return io::service::type_not_defined;
}

//-----------------------------------------------------------------------------

bool writer::has_location_defined() const
{
    std::string base_folder;
    update_base_folder(base_folder);

    // Either we have an absolution location set by the user
    // Or we are using the base_folder value from the xml configuration
    return ((base_folder.empty()) && !m_locations.empty() && m_locations.front().is_absolute())
           || (!base_folder.empty() && ((this->get_path_type() & io::service::file) != 0)
               && !m_locations.empty())
           || (!base_folder.empty() && ((this->get_path_type() & io::service::folder) != 0));
}

//-----------------------------------------------------------------------------

bool writer::has_failed() const
{
    return m_write_failed;
}

//-----------------------------------------------------------------------------

} // namespace sight::io::service
