/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwXML/FSLocation.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

FSLocation::FSLocation()
{}

//------------------------------------------------------------------------------

FSLocation::~FSLocation()
{}

//------------------------------------------------------------------------------

std::string & FSLocation::extension()
{
    SLM_ASSERT("Not valid extension", m_extension.empty() || m_extension[0]=='.' );

    return m_extension;
}

//------------------------------------------------------------------------------

::boost::filesystem::path & FSLocation::filename()
{
    return m_filename;
}

//------------------------------------------------------------------------------

::boost::filesystem::path & FSLocation::localFolder()
{
    return m_localFolder;
}

//------------------------------------------------------------------------------

::boost::filesystem::path & FSLocation::rootFolder()
{
    return m_rootFolder;
}

//------------------------------------------------------------------------------

::boost::filesystem::path  FSLocation::getFullPath() const
{
    return m_rootFolder / m_localFolder / this->getFullFilename();
}

//------------------------------------------------------------------------------

::boost::filesystem::path  FSLocation::getFullFilename() const
{
    ::boost::filesystem::path fullFilename( m_filename.string() + m_extension);
    SLM_ASSERT("Filename is empty", !fullFilename.empty());

    return fullFilename;
}

//------------------------------------------------------------------------------

::boost::filesystem::path  FSLocation::getLocalPath() const
{
    return  m_localFolder / m_filename;
}

//------------------------------------------------------------------------------

}
