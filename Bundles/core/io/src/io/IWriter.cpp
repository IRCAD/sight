/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include "io/IWriter.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>

namespace io
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
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILE));
    FW_RAISE_IF("Exactly one file must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IWriter::setFile( const ::boost::filesystem::path& file)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const ::io::LocationsType& IWriter::getFiles() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILES));
    FW_RAISE_IF("At least one file must be define in location", m_locations.empty() );
    return m_locations;
}

//-----------------------------------------------------------------------------

void IWriter::setFiles(const ::io::LocationsType& files)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path& IWriter::getFolder() const
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::io::FOLDER));
    FW_RAISE_IF("Exactly one folder must be define in location", m_locations.size() != 1 );
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IWriter::setFolder(const ::boost::filesystem::path& folder)
{
    FW_RAISE_IF("This reader doesn't manage folders", !(this->getIOPathType() & ::io::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void IWriter::setFileFolder(boost::filesystem::path folder)
{
    FW_RAISE_IF("This reader doesn't manage file or files",
                !(this->getIOPathType() & ::io::FILE) && !(this->getIOPathType() & ::io::FILES));

    for(auto& file : m_locations)
    {
        file = file.filename();
        file = folder / file;
    }
}

//-----------------------------------------------------------------------------

const ::io::LocationsType& IWriter::getLocations() const
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
               !( this->getIOPathType() & ::io::TYPE_NOT_DEFINED ) );

    SLM_ASSERT("This writer does not manage folders and a folder path is given in the configuration",
               ( this->getIOPathType() & ::io::FOLDER ) ||
               (m_configuration->find("folder").size() == 0));

    SLM_ASSERT("This writer does not manages files and a file path is given in the configuration",
               ( this->getIOPathType() & ::io::FILE || this->getIOPathType() & ::io::FILES ) ||
               (m_configuration->find("file").size() == 0));

    ::fwRuntime::ConfigurationElement::sptr titleConfig = m_configuration->findConfigurationElement("windowTitle");
    m_windowTitle                                       = titleConfig ? titleConfig->getValue() : "";

    if ( this->getIOPathType() & ::io::FILE )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::io::FILES );
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("file");
        FW_RAISE_IF("No more than one file must be defined in the configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string file = config.at(0)->getValue();
            this->setFile(::boost::filesystem::path(file));
        }
    }

    if ( this->getIOPathType() & ::io::FILES )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::io::FILE );
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
        FW_RAISE_IF("No more than one folder must be defined in configuration", config.size() > 1 );
        if (config.size() == 1)
        {
            std::string folder = config.at(0)->getValue();
            this->setFolder(::boost::filesystem::path(folder));
        }
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType IWriter::getIOPathType() const
{
    return ::io::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

bool IWriter::hasLocationDefined() const
{
    return m_locations.size() > 0;
}

//-----------------------------------------------------------------------------

}

