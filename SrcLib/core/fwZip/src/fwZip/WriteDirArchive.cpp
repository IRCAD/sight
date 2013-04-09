#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/iostreams/stream_buffer.hpp>

#include <fwCore/exceptionmacros.hpp>

#include "zip.h"
#include "fwZip/WriteDirArchive.hpp"
#include "fwZip/exception/Write.hpp"

namespace fwZip
{

//-----------------------------------------------------------------------------

WriteDirArchive::WriteDirArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive)
{
    if(!::boost::filesystem::exists(m_archive))
    {
        ::boost::filesystem::create_directories(m_archive);
    }
}

//-----------------------------------------------------------------------------

WriteDirArchive::~WriteDirArchive()
{
    this->closeFile();
}

//-----------------------------------------------------------------------------

std::ostream& WriteDirArchive::createFile(const ::boost::filesystem::path &path)
{
    const ::boost::filesystem::path file = m_archive / path;
    const ::boost::filesystem::path parentFile = file.parent_path();
    if(!::boost::filesystem::exists(file.parent_path()))
    {
        ::boost::filesystem::create_directories(parentFile);
    }

    this->closeFile();
    m_outfile.open(file.string().c_str(), std::fstream::binary | std::fstream::out | std::fstream::trunc);
    return m_outfile;
}

//-----------------------------------------------------------------------------

void WriteDirArchive::closeFile()
{
    if(m_outfile.is_open())
    {
        m_outfile.flush();
        m_outfile.close();
    }
}

//-----------------------------------------------------------------------------

bool WriteDirArchive::createDir(const ::boost::filesystem::path &path)
{
    return ::boost::filesystem::create_directories(m_archive/path);
}

//-----------------------------------------------------------------------------


}
