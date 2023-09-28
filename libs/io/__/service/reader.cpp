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
static const core::com::slots::key_t OPEN_LOCATION_DIALOG = "openLocationDialog";

//-----------------------------------------------------------------------------

reader::reader() noexcept
{
    new_slot(READ_FOLDER_SLOT, &reader::readFolder, this);
    new_slot(READ_FILE_SLOT, &reader::readFile, this);
    new_slot(READ_FILES_SLOT, &reader::readFiles, this);
    new_slot(OPEN_LOCATION_DIALOG, &reader::openLocationDialog, this);
    new_slot(SET_FILE_FOLDER, &reader::setFileFolder, this);
}

//-----------------------------------------------------------------------------

reader::~reader() noexcept =
    default;

//-----------------------------------------------------------------------------

std::string reader::getSelectorDialogTitle()
{
    return "Choose a file";
}

//-----------------------------------------------------------------------------

std::vector<std::string> reader::getSupportedExtensions()
{
    return {};
}

//-----------------------------------------------------------------------------

const std::filesystem::path& reader::get_file() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILE));
    SIGHT_THROW_IF("Exactly one file must be defined in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void reader::set_file(const std::filesystem::path& file)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const io::service::LocationsType& reader::get_files() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILES));
    SIGHT_THROW_IF("At least one file must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void reader::set_files(const io::service::LocationsType& files)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

const std::filesystem::path& reader::get_folder() const
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->getIOPathType() & io::service::FOLDER));
    SIGHT_THROW_IF("Exactly one folder must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void reader::set_folder(const std::filesystem::path& folder)
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->getIOPathType() & io::service::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void reader::setFileFolder(std::filesystem::path folder)
{
    SIGHT_THROW_IF(
        "This reader doesn't manage file or files",
        !(this->getIOPathType() & io::service::FILE)
        && !(this->getIOPathType() & io::service::FILES)
    );

    for(auto& file : m_locations)
    {
        file = file.filename();
        file = folder / file;
    }
}

//-----------------------------------------------------------------------------

const io::service::LocationsType& reader::getLocations() const
{
    SIGHT_THROW_IF("At least one path must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void reader::clearLocations()
{
    m_locations.clear();
}

//-----------------------------------------------------------------------------

void reader::configuring()
{
    const ConfigType config = this->getConfiguration();

    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that use paths.",
        !(this->getIOPathType() & io::service::TYPE_NOT_DEFINED)
    );

    SIGHT_ASSERT(
        "This reader does not manage folders and a folder path is given in the configuration",
        (this->getIOPathType() & io::service::FOLDER) || (config.count("folder") == 0)
    );

    SIGHT_ASSERT(
        "This reader does not manage files and a file path is given in the configuration",
        (this->getIOPathType() & io::service::FILE || this->getIOPathType() & io::service::FILES)
        || (config.count("file") == 0)
    );

    m_windowTitle = config.get("windowTitle", "");

    if((this->getIOPathType() & io::service::FILE) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & io::service::FILES);
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

    if((this->getIOPathType() & io::service::FILES) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->getIOPathType() & io::service::FILE);

        io::service::LocationsType locations;

        const auto filesCfg = config.equal_range("file");
        for(auto fileCfg = filesCfg.first ; fileCfg != filesCfg.second ; ++fileCfg)
        {
            const auto location = fileCfg->second.get_value<std::string>();
            locations.emplace_back(location);
        }

        const auto resourcesCfg = config.equal_range("resource");
        for(auto resourceCfg = resourcesCfg.first ; resourceCfg != resourcesCfg.second ; ++resourceCfg)
        {
            const auto resource = resourceCfg->second.get_value<std::string>();
            const auto file     = core::runtime::get_resource_file_path(resource);
            locations.push_back(file);
        }

        this->set_files(locations);
    }

    if((this->getIOPathType() & io::service::FOLDER) != 0)
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

io::service::IOPathType reader::getIOPathType() const
{
    return io::service::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

bool reader::hasLocationDefined() const
{
    return !m_locations.empty();
}

//-----------------------------------------------------------------------------

bool reader::hasFailed() const
{
    return m_readFailed;
}

//-----------------------------------------------------------------------------

void reader::readFolder(std::filesystem::path folder)
{
    this->set_folder(folder);
    this->updating();
}

//-----------------------------------------------------------------------------

void reader::readFile(std::filesystem::path file)
{
    this->set_file(file);
    this->updating();
}

//-----------------------------------------------------------------------------

void reader::readFiles(io::service::LocationsType files)
{
    this->set_files(files);
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::io::service
