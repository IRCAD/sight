/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/__/preferences.hpp>

namespace sight::io::service
{

// Public signal
const core::com::signals::key_t writer::PREFIX_SET_SIG      = "prefix_set";
const core::com::signals::key_t writer::BASE_FOLDER_SET_SIG = "base_folder_set";

// Public slot
const core::com::slots::key_t writer::SET_PREFIX      = "set_prefix";
const core::com::slots::key_t writer::SET_BASE_FOLDER = "set_base_folder";

// Private slot
static const core::com::slots::key_t OPEN_LOCATION_DIALOG = "open_location_dialog";

//-----------------------------------------------------------------------------

writer::writer() noexcept
{
    new_signal<void_signal_t>(PREFIX_SET_SIG);
    new_signal<void_signal_t>(BASE_FOLDER_SET_SIG);

    new_slot(OPEN_LOCATION_DIALOG, &writer::open_location_dialog, this);
    new_slot(SET_PREFIX, &writer::set_prefix, this);
    new_slot(SET_BASE_FOLDER, &writer::set_base_folder, this);
}

//-----------------------------------------------------------------------------

writer::~writer() noexcept =
    default;

//-----------------------------------------------------------------------------

std::filesystem::path writer::get_file() const
{
    // WARNING: Pay attention here that no canonical paths are processed when concatenating paths
    // In this case, the behavior is to replace the previous path.

    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::file));

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
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::file));
    m_locations.clear();
    m_locations.push_back(_file);
}

//-----------------------------------------------------------------------------

const io::service::locations_t& writer::get_files() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::files));
    SIGHT_THROW_IF("At least one file must be defined in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void writer::set_files(const io::service::locations_t& _files)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::files));
    m_locations = _files;
}

//-----------------------------------------------------------------------------

std::filesystem::path writer::get_folder() const
{
    // WARNING: Pay attention here that no canonical paths are processed when concatenating paths
    // In this case, the behavior is to replace the previous path.

    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->get_path_type() & io::service::folder));

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
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->get_path_type() & io::service::folder));
    m_locations.clear();
    m_locations.push_back(_folder);
}

//------------------------------------------------------------------------------

void writer::set_prefix(std::string _prefix)
{
    // Record the current prefix
    m_current_prefix = _prefix;

    auto sig = this->signal<void_signal_t>(PREFIX_SET_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void writer::set_base_folder(std::string _path)
{
    // Record the current prefix
    m_base_folder = _path;

    auto sig = this->signal<void_signal_t>(BASE_FOLDER_SET_SIG);
    sig->async_emit();
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
    // clear_locations is called to force the user reconfiguring a location
    // after a file has been written
    // When we use base folder, the location contains the "template" folder to be used to output data
    // So in this specific case, we don't clear it
    // if(m_baseFolder.size() == 0)
    {
        m_locations.clear();
    }
}

//-----------------------------------------------------------------------------

void writer::configuring()
{
    const auto& config = this->get_config();

    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that uses paths.",
        !(this->get_path_type() & io::service::type_not_defined)
    );

    SIGHT_ASSERT(
        "This writer does not manage folders and a folder path is given in the configuration",
        (this->get_path_type() & io::service::folder)
        || (config.find("folder") == config.not_found())
    );

    SIGHT_ASSERT(
        "This writer does not manages files and a file path is given in the configuration",
        (this->get_path_type() & io::service::file || this->get_path_type() & io::service::files)
        || (config.find("file") == config.not_found())
    );

    m_window_title = config.get<std::string>("windowTitle", "");

    if((this->get_path_type() & io::service::file) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->get_path_type() & io::service::files);
        SIGHT_THROW_IF("No more than one file must be defined in the configuration", config.count("file") > 1);
        if(const auto file = config.get_optional<std::string>("file"); file.has_value())
        {
            this->set_file(std::filesystem::path(*file));
        }
    }

    if((this->get_path_type() & io::service::files) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->get_path_type() & io::service::file);
        io::service::locations_t locations;
        for(const auto& elt : boost::make_iterator_range(config.equal_range("file")))
        {
            const auto location = elt.second.get_value<std::string>();
            locations.emplace_back(location);
        }

        this->set_files(locations);
    }

    if((this->get_path_type() & io::service::folder) != 0)
    {
        SIGHT_THROW_IF("No more than one folder must be defined in configuration", config.count("folder") > 1);
        if(const auto folder = config.get_optional<std::string>("folder"); folder.has_value())
        {
            this->set_folder(std::filesystem::path(folder.value()));
        }
    }
}

//------------------------------------------------------------------------------

void writer::update_base_folder(std::string& _out_base_folder) const
{
    const sight::service::config_t config = this->get_config();

    std::string base_folder_cfg = m_base_folder;

    if(base_folder_cfg.empty())
    {
        base_folder_cfg = config.get<std::string>("baseFolder", m_base_folder);
    }

    if(!base_folder_cfg.empty())
    {
        // We need to check for potential updates to the base folder
        // In case the user updates an associated preference during runtime
        // We also re-read the xml configuration for file and folder if baseFolder was set

        sight::ui::preferences preferences;

        // We need to provide the separator, otherwise the 2-args method will be selected
        auto [key, baseFolder] = preferences.parsed_get<std::string>(base_folder_cfg);

        // If the returned value is indeed a preference
        if(!key.empty())
        {
            if(!baseFolder.empty())
            {
                _out_base_folder = baseFolder;
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
    // Or we are using the baseFolder value from the xml configuration
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
