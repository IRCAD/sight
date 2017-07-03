/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "io/IReader.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/ConfigurationElementContainer.hpp>

#include <fwServices/macros.hpp>

using fwRuntime::ConfigurationElementContainer;

namespace io
{

static const ::fwCom::Slots::SlotKeyType s_READ_FOLDER_SLOT   = "readFolder";
static const ::fwCom::Slots::SlotKeyType s_READ_FILE_SLOT     = "readFile";
static const ::fwCom::Slots::SlotKeyType s_READ_FILES_SLOT    = "readFiles";
static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_WITH_IHM = "configureWithIHM";

//-----------------------------------------------------------------------------

IReader::IReader() throw()
{
    newSlot(s_READ_FOLDER_SLOT, &IReader::readFolder, this);
    newSlot(s_READ_FILE_SLOT, &IReader::readFile, this);
    newSlot(s_READ_FILES_SLOT, &IReader::readFiles, this);
    newSlot(s_CONFIGURE_WITH_IHM, &IReader::configureWithIHM, this);
}

//-----------------------------------------------------------------------------

IReader::~IReader() throw()
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
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILE));
    FW_RAISE_IF("Exactly one file must be defined in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFile( const ::boost::filesystem::path& file)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const ::io::LocationsType& IReader::getFiles() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILES));
    FW_RAISE_IF("At least one file must be define in location", m_locations.empty() );
    return m_locations;
}

//-----------------------------------------------------------------------------

void IReader::setFiles(const ::io::LocationsType& files)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path& IReader::getFolder() const
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::io::FOLDER));
    FW_RAISE_IF("Exactly one folder must be define in location", m_locations.size() != 1 );
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFolder(const ::boost::filesystem::path& folder)
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::io::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

const ::io::LocationsType& IReader::getLocations() const
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

void IReader::configuring() throw (fwTools::Failed)
{
    SLM_ASSERT("Generic configuring method is only available for io service that use paths.",
               !( this->getIOPathType() & ::io::TYPE_NOT_DEFINED ) );

    SLM_ASSERT("This reader does not manage folders and a folder path is given in the configuration",
               ( this->getIOPathType() & ::io::FOLDER ) || (m_configuration->find("folder").size() == 0) );

    SLM_ASSERT("This reader does not manage files and a file path is given in the configuration",
               ( this->getIOPathType() & ::io::FILE || this->getIOPathType() & ::io::FILES ) ||
               (m_configuration->find("file").size() == 0));

    ::fwRuntime::ConfigurationElement::sptr titleConfig = m_configuration->findConfigurationElement("windowTitle");
    m_windowTitle                                       = titleConfig ? titleConfig->getValue() : "";

    if ( this->getIOPathType() & ::io::FILE )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::io::FILES );
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("file");
        FW_RAISE_IF("At least one file must be defined in configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string file = config.at(0)->getValue();
            this->setFile(::boost::filesystem::path(file));
        }
    }

    if ( this->getIOPathType() & ::io::FILES )
    {
        FW_RAISE_IF("This reader cannot manage FILE and FILES.", this->getIOPathType() & ::io::FILE );
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("file");
        ::io::LocationsType locations;
        for(::fwRuntime::ConfigurationElement::sptr elt :  config)
        {
            std::string location = elt->getValue();
            locations.push_back(::boost::filesystem::path(location));
        }
        this->setFiles(locations);
    }

    if ( this->getIOPathType() & ::io::FOLDER )
    {
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("folder");
        FW_RAISE_IF("At least one folder must be defined in configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string folder = config.at(0)->getValue();
            this->setFolder(::boost::filesystem::path(folder));
        }
    }

}

//-----------------------------------------------------------------------------

::io::IOPathType IReader::getIOPathType() const
{
    return ::io::TYPE_NOT_DEFINED;
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

void IReader::readFiles(::io::LocationsType files)
{
    this->setFiles(files);
    this->updating();
}

//-----------------------------------------------------------------------------

}

