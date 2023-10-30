/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/__/service/reader.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <service/macros.hpp>

namespace sight::io::service
{

// Public slot
const core::com::slots::key_t reader::SET_FILE_FOLDER = "setFileFolder";

// Private slot
static const core::com::slots::key_t READ_FOLDER_SLOT     = "readFolder";
static const core::com::slots::key_t READ_FILE_SLOT       = "readFile";
static const core::com::slots::key_t READ_FILES_SLOT      = "readFiles";
static const core::com::slots::key_t OPEN_LOCATION_DIALOG = "open_location_dialog";

//-----------------------------------------------------------------------------

reader::reader() noexcept
{
    new_slot(READ_FOLDER_SLOT, &reader::read_folder, this);
    new_slot(READ_FILE_SLOT, &reader::read_file, this);
    new_slot(READ_FILES_SLOT, &reader::read_files, this);
    new_slot(OPEN_LOCATION_DIALOG, &reader::open_location_dialog, this);
    new_slot(SET_FILE_FOLDER, &reader::set_file_folder, this);
}

//-----------------------------------------------------------------------------

reader::~reader() noexcept =
    default;

//-----------------------------------------------------------------------------

std::string reader::get_selector_dialog_title()
{
    return "Choose a file";
}

//-----------------------------------------------------------------------------

std::vector<std::string> reader::get_supported_extensions()
{
    return {};
}

//-----------------------------------------------------------------------------

const std::filesystem::path& reader::get_file() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::file));
    SIGHT_THROW_IF("Exactly one file must be defined in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void reader::set_file(const std::filesystem::path& _file)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::file));
    m_locations.clear();
    m_locations.push_back(_file);
}

//-----------------------------------------------------------------------------

const io::service::locations_t& reader::get_files() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::files));
    SIGHT_THROW_IF("At least one file must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void reader::set_files(const io::service::locations_t& _files)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::files));
    m_locations = _files;
}

//-----------------------------------------------------------------------------

const std::filesystem::path& reader::get_folder() const
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->get_path_type() & io::service::folder));
    SIGHT_THROW_IF("Exactly one folder must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void reader::set_folder(const std::filesystem::path& _folder)
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->get_path_type() & io::service::folder));
    m_locations.clear();
    m_locations.push_back(_folder);
}

//-----------------------------------------------------------------------------

void reader::set_file_folder(std::filesystem::path _folder)
{
    SIGHT_THROW_IF(
        "This reader doesn't manage file or files",
        !(this->get_path_type() & io::service::file)
        && !(this->get_path_type() & io::service::files)
    );

    for(auto& file : m_locations)
    {
        file = file.filename();
        file = _folder / file;
    }
}

//-----------------------------------------------------------------------------

const io::service::locations_t& reader::get_locations() const
{
    SIGHT_THROW_IF("At least one path must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void reader::clear_locations()
{
    m_locations.clear();
}

//-----------------------------------------------------------------------------

void reader::configuring()
{
    const config_t config = this->get_config();

    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that use paths.",
        !(this->get_path_type() & io::service::type_not_defined)
    );

    SIGHT_ASSERT(
        "This reader does not manage folders and a folder path is given in the configuration",
        (this->get_path_type() & io::service::folder) || (config.count("folder") == 0)
    );

    SIGHT_ASSERT(
        "This reader does not manage files and a file path is given in the configuration",
        (this->get_path_type() & io::service::file || this->get_path_type() & io::service::files)
        || (config.count("file") == 0)
    );

    m_window_title = config.get("windowTitle", "");

    if((this->get_path_type() & io::service::file) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manages FILE and FILES.", this->get_path_type() & io::service::files);
        SIGHT_THROW_IF("At least one file must be defined in configuration", config.count("file") > 1);
        if(config.count("file") == 1)
        {
            const auto file = config.get<std::string>("file");
            this->set_file(std::filesystem::path(file));
        }
        else if(config.count("resource") == 1)
        {
            const auto resource = config.get<std::string>("resource");
            const auto file     = core::runtime::get_resource_file_path(resource);
            this->set_file(file);
        }
    }

    if((this->get_path_type() & io::service::files) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->get_path_type() & io::service::file);

        io::service::locations_t locations;

        const auto files_cfg = config.equal_range("file");
        for(auto file_cfg = files_cfg.first ; file_cfg != files_cfg.second ; ++file_cfg)
        {
            const auto location = file_cfg->second.get_value<std::string>();
            locations.emplace_back(location);
        }

        const auto resources_cfg = config.equal_range("resource");
        for(auto resource_cfg = resources_cfg.first ; resource_cfg != resources_cfg.second ; ++resource_cfg)
        {
            const auto resource = resource_cfg->second.get_value<std::string>();
            const auto file     = core::runtime::get_resource_file_path(resource);
            locations.push_back(file);
        }

        this->set_files(locations);
    }

    if((this->get_path_type() & io::service::folder) != 0)
    {
        SIGHT_THROW_IF("At least one folder must be defined in configuration", config.count("folder") > 1);
        if(config.count("folder") == 1)
        {
            const auto folder = config.get<std::string>("folder");
            this->set_folder(std::filesystem::path(folder));
        }
        else if(config.count("resource") == 1)
        {
            const auto resource = config.get<std::string>("resource");
            auto folder         = core::runtime::get_module_resource_file_path(resource);
            if(folder.empty())
            {
                // If not found in a module, look into libraries
                folder = core::runtime::get_library_resource_file_path(resource);
                SIGHT_ERROR_IF(
                    "Resource '" + resource + "' has not been found in any module or library",
                    folder.empty()
                );
            }

            this->set_folder(folder);
        }
    }
}

//-----------------------------------------------------------------------------

io::service::path_type_t reader::get_path_type() const
{
    return io::service::type_not_defined;
}

//-----------------------------------------------------------------------------

bool reader::has_location_defined() const
{
    return !m_locations.empty();
}

//-----------------------------------------------------------------------------

bool reader::has_failed() const
{
    return m_read_failed;
}

//-----------------------------------------------------------------------------

void reader::read_folder(std::filesystem::path _folder)
{
    this->set_folder(_folder);
    this->updating();
}

//-----------------------------------------------------------------------------

void reader::read_file(std::filesystem::path _file)
{
    this->set_file(_file);
    this->updating();
}

//-----------------------------------------------------------------------------

void reader::read_files(io::service::locations_t _files)
{
    this->set_files(_files);
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::io::service
