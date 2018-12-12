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
#include "fwIO/IWriter.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>

namespace fwIO
{

// Public slot
const ::fwCom::Slots::SlotKeyType IWriter::s_SET_FILE_FOLDER = "setFileFolder";

// Private slot
static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_WITH_IHM = "configureWithIHM";

//-----------------------------------------------------------------------------

IWriter::IWriter() noexcept
{
    newSlot(s_CONFIGURE_WITH_IHM, &IWriter::configureWithIHM, this);
    newSlot(s_SET_FILE_FOLDER, &IWriter::setFileFolder, this);
}

//-----------------------------------------------------------------------------

IWriter::~IWriter() noexcept
{
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path& IWriter::getFile() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::fwIO::FILE));
    FW_RAISE_IF("Exactly one file must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IWriter::setFile( const ::boost::filesystem::path& file)
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

const ::boost::filesystem::path& IWriter::getFolder() const
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::fwIO::FOLDER));
    FW_RAISE_IF("Exactly one folder must be define in location", m_locations.size() != 1 );
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IWriter::setFolder(const ::boost::filesystem::path& folder)
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::fwIO::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void IWriter::setFileFolder(boost::filesystem::path folder)
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

    ::fwRuntime::ConfigurationElement::sptr titleConfig = m_configuration->findConfigurationElement("windowTitle");
    m_windowTitle                                       = titleConfig ? titleConfig->getValue() : "";

    if ( this->getIOPathType() & ::fwIO::FILE )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::fwIO::FILES );
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("file");
        FW_RAISE_IF("No more than one file must be defined in the configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string file = config.at(0)->getValue();
            this->setFile(::boost::filesystem::path(file));
        }
    }

    if ( this->getIOPathType() & ::fwIO::FILES )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::fwIO::FILE );
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("file");
        ::fwIO::LocationsType locations;
        for(::fwRuntime::ConfigurationElement::sptr elt :  config)
        {
            std::string location = elt->getValue();
            locations.push_back(::boost::filesystem::path(location));
        }
        this->setFiles(locations);
    }

    if ( this->getIOPathType() & ::fwIO::FOLDER )
    {
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("folder");
        FW_RAISE_IF("No more than one folder must be defined in configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string folder = config.at(0)->getValue();
            this->setFolder(::boost::filesystem::path(folder));
        }
    }
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

}
