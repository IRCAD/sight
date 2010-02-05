#include "fwXML/FSLocation.hpp"

namespace fwXML
{

FSLocation::FSLocation()
{
}

FSLocation::~FSLocation()
{
}

std::string & FSLocation::extension()
{
    assert( m_extension.empty() || m_extension[0]=='.' );

    return m_extension;
}


boost::filesystem::path & FSLocation::filename()
{
    return m_filename;
}


boost::filesystem::path & FSLocation::localFolder()
{
    return m_localFolder;
}


boost::filesystem::path & FSLocation::rootFolder()
{
    return m_rootFolder;
}


boost::filesystem::path  FSLocation::getFullPath() const
{
    return m_rootFolder / m_localFolder / getFullFilename();
}


boost::filesystem::path  FSLocation::getFullFilename() const
{
    assert ( boost::filesystem::path( m_filename.string() + m_extension).empty() == false  ); // a valid name

    return boost::filesystem::path( m_filename.string() + m_extension);
}

boost::filesystem::path  FSLocation::getLocalPath() const
{
    return  m_localFolder / m_filename;
}


}
