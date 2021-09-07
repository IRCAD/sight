/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/base/service/IReader.hpp"

#include <core/com/Slots.hxx>
#include <core/runtime/operations.hpp>

#include <service/macros.hpp>

using sight::core::runtime::ConfigurationElementContainer;

namespace sight::io::base::service
{

// Public slot
const core::com::Slots::SlotKeyType IReader::s_SET_FILE_FOLDER = "setFileFolder";

// Private slot
static const core::com::Slots::SlotKeyType s_READ_FOLDER_SLOT     = "readFolder";
static const core::com::Slots::SlotKeyType s_READ_FILE_SLOT       = "readFile";
static const core::com::Slots::SlotKeyType s_READ_FILES_SLOT      = "readFiles";
static const core::com::Slots::SlotKeyType s_OPEN_LOCATION_DIALOG = "openLocationDialog";

//-----------------------------------------------------------------------------

IReader::IReader() noexcept
{
    newSlot(s_READ_FOLDER_SLOT, &IReader::readFolder, this);
    newSlot(s_READ_FILE_SLOT, &IReader::readFile, this);
    newSlot(s_READ_FILES_SLOT, &IReader::readFiles, this);
    newSlot(s_OPEN_LOCATION_DIALOG, &IReader::openLocationDialog, this);
    newSlot(s_SET_FILE_FOLDER, &IReader::setFileFolder, this);
}

//-----------------------------------------------------------------------------

IReader::~IReader() noexcept
{
}

//-----------------------------------------------------------------------------

std::string IReader::getSelectorDialogTitle()
{
    return "Choose a file";
}

//-----------------------------------------------------------------------------

std::vector<std::string> IReader::getSupportedExtensions()
{
    return std::vector<std::string>();
}

//-----------------------------------------------------------------------------

const std::filesystem::path& IReader::getFile() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::base::service::FILE));
    SIGHT_THROW_IF("Exactly one file must be defined in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFile(const std::filesystem::path& file)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::base::service::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const io::base::service::LocationsType& IReader::getFiles() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::base::service::FILES));
    SIGHT_THROW_IF("At least one file must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void IReader::setFiles(const io::base::service::LocationsType& files)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::base::service::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

const std::filesystem::path& IReader::getFolder() const
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->getIOPathType() & io::base::service::FOLDER));
    SIGHT_THROW_IF("Exactly one folder must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFolder(const std::filesystem::path& folder)
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->getIOPathType() & io::base::service::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void IReader::setFileFolder(std::filesystem::path folder)
{
    SIGHT_THROW_IF(
        "This reader doesn't manage file or files",
        !(this->getIOPathType() & io::base::service::FILE)
        && !(this->getIOPathType() & io::base::service::FILES)
    );

    for(auto& file : m_locations)
    {
        file = file.filename();
        file = folder / file;
    }
}

//-----------------------------------------------------------------------------

const io::base::service::LocationsType& IReader::getLocations() const
{
    SIGHT_THROW_IF("At least one path must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void IReader::clearLocations()
{
    m_locations.clear();
}

//-----------------------------------------------------------------------------

void IReader::configuring()
{
    const ConfigType config = this->getConfigTree();

    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that use paths.",
        !(this->getIOPathType() & io::base::service::TYPE_NOT_DEFINED)
    );

    SIGHT_ASSERT(
        "This reader does not manage folders and a folder path is given in the configuration",
        (this->getIOPathType() & io::base::service::FOLDER) || (config.count("folder") == 0)
    );

    SIGHT_ASSERT(
        "This reader does not manage files and a file path is given in the configuration",
        (this->getIOPathType() & io::base::service::FILE || this->getIOPathType() & io::base::service::FILES)
        || (config.count("file") == 0)
    );

    m_windowTitle = config.get("windowTitle", "");

    if(this->getIOPathType() & io::base::service::FILE)
    {
        SIGHT_THROW_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & io::base::service::FILES);
        SIGHT_THROW_IF("At least one file must be defined in configuration", config.count("file") > 1);
        if(config.count("file") == 1)
        {
            const std::string file = config.get<std::string>("file");
            this->setFile(std::filesystem::path(file));
        }
        else if(config.count("resource") == 1)
        {
            const std::string resource = config.get<std::string>("resource");
            const auto file            = core::runtime::getResourceFilePath(resource);
            this->setFile(file);
        }
    }

    if(this->getIOPathType() & io::base::service::FILES)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->getIOPathType() & io::base::service::FILE);

        io::base::service::LocationsType locations;

        const auto filesCfg = config.equal_range("file");
        for(auto fileCfg = filesCfg.first ; fileCfg != filesCfg.second ; ++fileCfg)
        {
            const std::string location = fileCfg->second.get_value<std::string>();
            locations.push_back(std::filesystem::path(location));
        }

        const auto resourcesCfg = config.equal_range("resource");
        for(auto resourceCfg = resourcesCfg.first ; resourceCfg != resourcesCfg.second ; ++resourceCfg)
        {
            const std::string resource = resourceCfg->second.get_value<std::string>();
            const auto file            = core::runtime::getResourceFilePath(resource);
            locations.push_back(file);
        }

        this->setFiles(locations);
    }

    if(this->getIOPathType() & io::base::service::FOLDER)
    {
        SIGHT_THROW_IF("At least one folder must be defined in configuration", config.count("folder") > 1);
        if(config.count("folder") == 1)
        {
            const std::string folder = config.get<std::string>("folder");
            this->setFolder(std::filesystem::path(folder));
        }
        else if(config.count("resource") == 1)
        {
            const std::string resource = config.get<std::string>("resource");
            auto folder                = core::runtime::getModuleResourceFilePath(resource);
            if(folder.empty())
            {
                // If not found in a module, look into libraries
                folder = core::runtime::getLibraryResourceFilePath(resource);
                SIGHT_ERROR_IF(
                    "Resource '" + resource + "' has not been found in any module or library",
                    folder.empty()
                );
            }

            this->setFolder(folder);
        }
    }
}

//-----------------------------------------------------------------------------

io::base::service::IOPathType IReader::getIOPathType() const
{
    return io::base::service::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

bool IReader::hasLocationDefined() const
{
    return m_locations.size() > 0;
}

//-----------------------------------------------------------------------------

bool IReader::hasFailed() const
{
    return m_readFailed;
}

//-----------------------------------------------------------------------------

void IReader::readFolder(std::filesystem::path folder)
{
    this->setFolder(folder);
    this->updating();
}

//-----------------------------------------------------------------------------

void IReader::readFile(std::filesystem::path file)
{
    this->setFile(file);
    this->updating();
}

//-----------------------------------------------------------------------------

void IReader::readFiles(io::base::service::LocationsType files)
{
    this->setFiles(files);
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::io
