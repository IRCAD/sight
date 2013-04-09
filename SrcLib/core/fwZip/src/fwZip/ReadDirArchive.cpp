#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCore/exceptionmacros.hpp>

#include "unzip.h"
#include "fwZip/ReadDirArchive.hpp"
#include "fwZip/exception/Read.hpp"

namespace fwZip
{

//-----------------------------------------------------------------------------

ReadDirArchive::ReadDirArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive)
{}

//-----------------------------------------------------------------------------

ReadDirArchive::~ReadDirArchive()
{
    this->closeFile();
}

//-----------------------------------------------------------------------------

std::istream& ReadDirArchive::getFile(const ::boost::filesystem::path &path)
{
    FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("File '" +  path.string() + "' "
                                     "in archive '" + m_archive.string() + "' doesn't exist."),
             !this->exists(m_archive / path));

    this->closeFile();
    m_infile.open((m_archive / path).string().c_str(), std::fstream::binary | std::fstream::in);
    return m_infile;
}

//-----------------------------------------------------------------------------

void ReadDirArchive::closeFile()
{
    if(m_infile.is_open())
    {
        m_infile.close();
    }
}

//-----------------------------------------------------------------------------

bool ReadDirArchive::exists(const ::boost::filesystem::path &path)
{
    return !::boost::filesystem::exists(m_archive / path);
}

}

