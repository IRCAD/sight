/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "io/IReader.hpp"

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

using fwRuntime::ConfigurationElementContainer;

namespace io
{

IReader::IReader() throw()
{}

//-----------------------------------------------------------------------------

IReader::~IReader() throw()
{}

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

::boost::filesystem::path IReader::getFile() const
{
    FW_RAISE_IF("This reader doesn't manage file", !(this->getIOPathType() & ::io::FILE));
    FW_RAISE_IF("Sorry, one file must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFile( const ::boost::filesystem::path &file)
{
    FW_RAISE_IF("This reader doesn't manage file", !(this->getIOPathType() & ::io::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

::io::LocationsType IReader::getFiles() const
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILES));
    FW_RAISE_IF("Sorry, at least one file must be define in location", m_locations.empty() );
    return m_locations;
}

//-----------------------------------------------------------------------------

void IReader::setFiles(const ::io::LocationsType &files)
{
    FW_RAISE_IF("This reader doesn't manage files", !(this->getIOPathType() & ::io::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

::boost::filesystem::path IReader::getFolder() const
{
    FW_RAISE_IF("This reader doesn't manage folder", !(this->getIOPathType() & ::io::FOLDER));
    FW_RAISE_IF("Sorry, one folder must be define in location", m_locations.size() !=1 );
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void IReader::setFolder(const ::boost::filesystem::path &folder)
{
    FW_RAISE_IF("This reader doesn't manage folder", !(this->getIOPathType() & ::io::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//-----------------------------------------------------------------------------

void IReader::configuring() throw (fwTools::Failed)
{

    if ( this->getIOPathType() & ::io::FILE )
    {
        FW_RAISE_IF("This reader cannot manages FILE and FILES.", this->getIOPathType() & ::io::FILES );
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("file");
        FW_RAISE_IF("Sorry, only one file must be defined in configuration", config.size() > 1 );
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
        BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, config)
        {
            std::string location = elt->getValue();
            locations.push_back(::boost::filesystem::path(location));
        }
        this->setFiles(locations);
    }


    if ( this->getIOPathType() & ::io::FOLDER )
    {
        std::vector< ::fwRuntime::ConfigurationElement::sptr > config = m_configuration->find("folder");
        FW_RAISE_IF("Sorry, only one folder must be defined in configuration", config.size() > 1 );
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
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

bool IReader::hasLocationDefined() const
{
    return m_locations.size() > 0;
}

//-----------------------------------------------------------------------------

}

