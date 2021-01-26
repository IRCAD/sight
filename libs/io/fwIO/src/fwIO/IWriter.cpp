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
#include "fwIO/IWriter.hpp"

#include <core/base.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <services/macros.hpp>

namespace fwIO
{

// Public slot
const core::com::Slots::SlotKeyType IWriter::s_SET_FILE_FOLDER      = "setFileFolder";
const core::com::Slots::SlotKeyType IWriter::s_SET_TIMESTAMP_PREFIX = "setTimestampPrefix";

// Private slot
static const core::com::Slots::SlotKeyType s_CONFIGURE_WITH_IHM   = "configureWithIHM"; // Deprecated
static const core::com::Slots::SlotKeyType s_OPEN_LOCATION_DIALOG = "openLocationDialog";

//-----------------------------------------------------------------------------

IWriter::IWriter() noexcept :
    m_useTimestampPrefix(false),
    m_currentTimestamp(0.0)
{
    newSlot(s_CONFIGURE_WITH_IHM, &IWriter::deprecateConfigureWithIHM, this);
    newSlot(s_OPEN_LOCATION_DIALOG, &IWriter::openLocationDialog, this);
    newSlot(s_SET_FILE_FOLDER, &IWriter::setFileFolder, this);
    newSlot(s_SET_TIMESTAMP_PREFIX, &IWriter::setTimestampPrefix, this);
}

//-----------------------------------------------------------------------------

IWriter::~IWriter() noexcept
{
}

//-----------------------------------------------------------------------------

const std::filesystem::path& IWriter::getFile() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILE));
    FW_RAISE_IF("Exactly one file must be define in location", m_locations.size() != 1);

    m_currentLocation = m_locations.front();
    if(m_useTimestampPrefix)
    {
        std::filesystem::path dirname  = m_currentLocation.parent_path();
        std::filesystem::path basename = m_currentLocation.filename();

        m_currentLocation = dirname / std::filesystem::path(std::to_string(m_currentTimestamp)
                                                            + std::string("-") + basename.string());
    }

    return m_currentLocation;
}

//-----------------------------------------------------------------------------

void IWriter::setFile( const std::filesystem::path& file)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const ::fwIO::LocationsType& IWriter::getFiles() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILES));
    FW_RAISE_IF("At least one file must be define in location", m_locations.empty() );
    return m_locations;
}

//-----------------------------------------------------------------------------

void IWriter::setFiles(const ::fwIO::LocationsType& files)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

const std::filesystem::path& IWriter::getFolder() const
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::fwIO::FOLDER));
    FW_RAISE_IF("Exactly one folder must be define in location", m_locations.size() != 1 );
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IWriter::setFolder(const std::filesystem::path& folder)
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::fwIO::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void IWriter::setFileFolder(std::filesystem::path folder)
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

void IWriter::setTimestampPrefix(core::HiResClock::HiResClockType timestamp)
{
    // At the first signal sent, mark that we want files to be tagged with the prefix
    m_useTimestampPrefix = true;
    // Record the current prefix
    m_currentTimestamp = timestamp;
}

//-----------------------------------------------------------------------------

const ::fwIO::LocationsType& IWriter::getLocations() const
{
    FW_RAISE_IF("At least one path must be define in location", m_locations.empty() );
    return m_locations;
}

//-----------------------------------------------------------------------------

void IWriter::clearLocations()
{
    m_locations.clear();
}

//-----------------------------------------------------------------------------

void IWriter::configuring()
{
    SLM_ASSERT("Generic configuring method is only available for io service that uses paths.",
               !( this->getIOPathType() & ::fwIO::TYPE_NOT_DEFINED ) );

    SLM_ASSERT("This writer does not manage folders and a folder path is given in the configuration",
               ( this->getIOPathType() & ::fwIO::FOLDER ) ||
               (m_configuration->find("folder").size() == 0));

    SLM_ASSERT("This writer does not manages files and a file path is given in the configuration",
               ( this->getIOPathType() & ::fwIO::FILE || this->getIOPathType() & ::fwIO::FILES ) ||
               (m_configuration->find("file").size() == 0));

    core::runtime::ConfigurationElement::sptr titleConfig = m_configuration->findConfigurationElement("windowTitle");
    m_windowTitle = titleConfig ? titleConfig->getValue() : "";

    if ( this->getIOPathType() & ::fwIO::FILE )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::fwIO::FILES );
        std::vector< core::runtime::ConfigurationElement::sptr > config = m_configuration->find("file");
        FW_RAISE_IF("No more than one file must be defined in the configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string file = config.at(0)->getValue();
            this->setFile(std::filesystem::path(file));
        }
    }

    if ( this->getIOPathType() & ::fwIO::FILES )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::fwIO::FILE );
        std::vector< core::runtime::ConfigurationElement::sptr > config = m_configuration->find("file");
        ::fwIO::LocationsType locations;
        for(core::runtime::ConfigurationElement::sptr elt :  config)
        {
            std::string location = elt->getValue();
            locations.push_back(std::filesystem::path(location));
        }
        this->setFiles(locations);
    }

    if ( this->getIOPathType() & ::fwIO::FOLDER )
    {
        std::vector< core::runtime::ConfigurationElement::sptr > config = m_configuration->find("folder");
        FW_RAISE_IF("No more than one folder must be defined in configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string folder = config.at(0)->getValue();
            this->setFolder(std::filesystem::path(folder));
        }
    }
}

//-----------------------------------------------------------------------------

void IWriter::deprecateConfigureWithIHM()
{
    FW_DEPRECATED_MSG("openLocationDialog should be implemented in subclass,"
                      " this method will be a pure virtual in sight 22.0.", 22.0);
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void IWriter::openLocationDialog()
{
    FW_DEPRECATED_MSG("openLocationDialog should be implemented in subclass,"
                      " this method will be a pure virtual in sight 22.0.", 22.0);
    this->configureWithIHM();
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType IWriter::getIOPathType() const
{
    return ::fwIO::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

bool IWriter::hasLocationDefined() const
{
    return m_locations.size() > 0;
}

//-----------------------------------------------------------------------------

bool IWriter::hasFailed() const
{
    return m_writeFailed;
}

//-----------------------------------------------------------------------------

}
