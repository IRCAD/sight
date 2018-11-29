/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwIO/IReader.hpp"

#include <fwCom/Slots.hxx>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

using fwRuntime::ConfigurationElementContainer;

namespace fwIO
{

// Public slot
const ::fwCom::Slots::SlotKeyType IReader::s_SET_FILE_FOLDER = "setFileFolder";

// Private slot
static const ::fwCom::Slots::SlotKeyType s_READ_FOLDER_SLOT   = "readFolder";
static const ::fwCom::Slots::SlotKeyType s_READ_FILE_SLOT     = "readFile";
static const ::fwCom::Slots::SlotKeyType s_READ_FILES_SLOT    = "readFiles";
static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_WITH_IHM = "configureWithIHM";

//-----------------------------------------------------------------------------

IReader::IReader() noexcept
{
    newSlot(s_READ_FOLDER_SLOT, &IReader::readFolder, this);
    newSlot(s_READ_FILE_SLOT, &IReader::readFile, this);
    newSlot(s_READ_FILES_SLOT, &IReader::readFiles, this);
    newSlot(s_CONFIGURE_WITH_IHM, &IReader::configureWithIHM, this);
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

std::vector< std::string > IReader::getSupportedExtensions()
{
    return std::vector< std::string >();
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path& IReader::getFile() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILE));
    FW_RAISE_IF("Exactly one file must be defined in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFile( const ::boost::filesystem::path& file)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const ::fwIO::LocationsType& IReader::getFiles() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILES));
    FW_RAISE_IF("At least one file must be define in location", m_locations.empty() );
    return m_locations;
}

//-----------------------------------------------------------------------------

void IReader::setFiles(const ::fwIO::LocationsType& files)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path& IReader::getFolder() const
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::fwIO::FOLDER));
    FW_RAISE_IF("Exactly one folder must be define in location", m_locations.size() != 1 );
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFolder(const ::boost::filesystem::path& folder)
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::fwIO::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void IReader::setFileFolder(boost::filesystem::path folder)
{
    FW_RAISE_IF("This reader doesn't manage file or files",
                !(this->getIOPathType() & ::fwIO::FILE) && !(this->getIOPathType() & ::fwIO::FILES));

    for(auto& file : m_locations)
    {
        file = file.filename();
        file = folder / file;
    }
}

//-----------------------------------------------------------------------------

const ::fwIO::LocationsType& IReader::getLocations() const
{
    FW_RAISE_IF("At least one path must be define in location", m_locations.empty() );
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

    SLM_ASSERT("Generic configuring method is only available for io service that use paths.",
               !( this->getIOPathType() & ::fwIO::TYPE_NOT_DEFINED ) );

    SLM_ASSERT("This reader does not manage folders and a folder path is given in the configuration",
               ( this->getIOPathType() & ::fwIO::FOLDER ) || (config.count("folder") == 0) );

    SLM_ASSERT("This reader does not manage files and a file path is given in the configuration",
               ( this->getIOPathType() & ::fwIO::FILE || this->getIOPathType() & ::fwIO::FILES ) ||
               (config.count("file") == 0));

    m_windowTitle = config.get("windowTitle", "");

    if ( this->getIOPathType() & ::fwIO::FILE )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::fwIO::FILES );
        FW_RAISE_IF("At least one file must be defined in configuration", config.count("file") > 1 );
        if (config.count("file") == 1)
        {
            const std::string file = config.get<std::string>("file");
            this->setFile(::boost::filesystem::path(file));
        }
        else if (config.count("resource") == 1)
        {
            const std::string resource = config.get<std::string>("resource");
            const auto file            = ::fwRuntime::getResourceFilePath(resource);
            this->setFile(file);
        }
    }

    if ( this->getIOPathType() & ::fwIO::FILES )
    {
        FW_RAISE_IF("This reader cannot manage FILE and FILES.", this->getIOPathType() & ::fwIO::FILE );

        ::fwIO::LocationsType locations;

        const auto filesCfg = config.equal_range("file");
        for (auto fileCfg = filesCfg.first; fileCfg != filesCfg.second; ++fileCfg)
        {
            const std::string location = fileCfg->second.get_value<std::string>();
            locations.push_back(::boost::filesystem::path(location));
        }

        const auto resourcesCfg = config.equal_range("resource");
        for (auto resourceCfg = resourcesCfg.first; resourceCfg != resourcesCfg.second; ++resourceCfg)
        {
            const std::string resource = resourceCfg->second.get_value<std::string>();
            const auto file            = ::fwRuntime::getResourceFilePath(resource);
            locations.push_back(file);
        }
        this->setFiles(locations);
    }

    if ( this->getIOPathType() & ::fwIO::FOLDER )
    {
        FW_RAISE_IF("At least one folder must be defined in configuration", config.count("folder") > 1 );
        if (config.count("folder") == 1)
        {
            const std::string folder = config.get<std::string>("folder");
            this->setFolder(::boost::filesystem::path(folder));
        }
        else if (config.count("resource") == 1)
        {
            const std::string resource = config.get<std::string>("resource");
            auto folder                = ::fwRuntime::getBundleResourceFilePath(resource);
            if(folder.empty())
            {
                // If not found in a bundle, look into libraries
                folder = ::fwRuntime::getLibraryResourceFilePath(resource);
                SLM_ERROR_IF("Resource '" + resource + "' has not been found in any bundle or library", folder.empty());
            }

            this->setFolder(folder);
        }
    }
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType IReader::getIOPathType() const
{
    return ::fwIO::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

bool IReader::hasLocationDefined() const
{
    return m_locations.size() > 0;
}

//-----------------------------------------------------------------------------

void IReader::readFolder(::boost::filesystem::path folder)
{
    this->setFolder(folder);
    this->updating();
}

//-----------------------------------------------------------------------------

void IReader::readFile(::boost::filesystem::path file)
{
    this->setFile(file);
    this->updating();
}

//-----------------------------------------------------------------------------

void IReader::readFiles(::fwIO::LocationsType files)
{
    this->setFiles(files);
    this->updating();
}

//-----------------------------------------------------------------------------

}
