/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iosfwd> // #include <strstream>
#include <iomanip>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>

#include <fwTools/System.hpp>

#include "fwMemory/BufferManager.hpp"

namespace fwMemory
{

//-----------------------------------------------------------------------------

BufferManager::BufferManager()
{}

//-----------------------------------------------------------------------------

BufferManager::~BufferManager()
{
    // TODO restore dumped buffers
}

//-----------------------------------------------------------------------------

bool BufferManager::registerBuffer(void ** buffer, long * lockCount)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lockCount = lockCount;
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::unregisterBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();

    {
        DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];

        if(info.isDumped)
        {
            ::boost::filesystem::remove( info.dumpedFile );
        }
    }

    m_dumpedBufferInfos.erase(buffer);
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];

    SLM_ASSERT("Unable to allocate a dumped buffer", !info.isDumped);

    if(info.isDumped)
    {
        ::boost::filesystem::remove( info.dumpedFile );
        info.dumpedFile = "";
        info.isDumped = false;
        SLM_ASSERT("requested an allocation of a dumped buffer", 0);
    }

    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];

    SLM_ASSERT("Unable to overwrite a dumped buffer", !info.isDumped);

    if(info.isDumped)
    {
        ::boost::filesystem::remove( info.dumpedFile );
        info.dumpedFile = "";
        info.isDumped = false;
        SLM_ASSERT("requested a replacement of a dumped buffer", 0);
    }

    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::reallocateBuffer(void ** buffer, SizeType newSize)
{
    SLM_TRACE_FUNC();

    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];

    SLM_ASSERT("Unable to reallocate a dumped buffer", !info.isDumped);

    info.lastAccess.modified();
    info.size = newSize;

    if(info.isDumped)
    {
        this->restoreBuffer( buffer, newSize );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::destroyBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfo & info = m_dumpedBufferInfos[buffer];
    info.lastAccess.modified();
    info.size = 0;

    info.bufferPolicy.reset();

    if(info.isDumped)
    {
        ::boost::filesystem::remove( info.dumpedFile );
        info.dumpedFile = "";
        info.isDumped = false;
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::lockBuffer(const void * const * buffer)
{
    SLM_TRACE_FUNC();
    void **castedBuffer = const_cast<void **>(buffer);
    bool restored = this->restoreBuffer( castedBuffer );
    OSLM_ASSERT( "restore not OK ( "<< *castedBuffer <<" ).", !restored || *castedBuffer != 0 );
    m_lastAccess.modified();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::unlockBuffer(const void * const * buffer)
{
    SLM_TRACE_FUNC();
    void **castedBuffer = const_cast<void **>(buffer);
    bool dumped = this->dumpBuffer( castedBuffer );
    OSLM_ASSERT( "Dump not OK ( "<< *castedBuffer <<" ).", !dumped || *castedBuffer == 0 );
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::dumpBuffer(void ** buffer)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfoMapType::iterator item = m_dumpedBufferInfos.find(buffer);
    OSLM_ASSERT( "Sorry, try dumping a unreferenced buffer ( "<< buffer <<" ).",item != m_dumpedBufferInfos.end() );
    DumpedBufferInfo & info = item->second;
    if ( info.isDumped || *(info.lockCount) > 0 || info.size == 0 )
    {
        return false;
    }
    OSLM_TRACE("dumping " << buffer);


    ::boost::filesystem::path tmp = ::fwTools::System::getTemporaryFolder();
    info.dumpedFile = ::boost::filesystem::unique_path( tmp / "fwMemory-%%%%-%%%%-%%%%-%%%%.raw" );

    if ( this->writeBuffer(*buffer, info.size, info.dumpedFile) )
    {
        info.bufferPolicy->destroy(*buffer);
        *buffer = NULL;
        info.isDumped = true;
    }

    return info.isDumped;
}

//-----------------------------------------------------------------------------

bool BufferManager::restoreBuffer(void ** buffer, BufferManager::SizeType allocSize)
{
    SLM_TRACE_FUNC();
    DumpedBufferInfoMapType::iterator item = m_dumpedBufferInfos.find(buffer);
    OSLM_ASSERT( "Unable to dump a unreferenced buffer ( "<< buffer <<" ).",item != m_dumpedBufferInfos.end() );
    DumpedBufferInfo & info = item->second;

    allocSize = ((allocSize) ? allocSize : info.size);
    if ( info.isDumped )
    {
        OSLM_TRACE("Restoring " << buffer);

        info.bufferPolicy->allocate(*buffer, allocSize);

        if ( this->readBuffer(*buffer, std::min(allocSize, info.size), info.dumpedFile) )
        {
            info.isDumped = false;
            ::boost::filesystem::remove( info.dumpedFile );
            info.dumpedFile = "";
            info.lastAccess.modified();
            return true;
        }

    }
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::writeBuffer(const void * buffer, SizeType size, ::boost::filesystem::path &path)
{
    ::boost::filesystem::ofstream fs(path, std::ios::binary|std::ios::trunc);
    FW_RAISE_IF("Memory management : Unable to open " << path, !fs.good());
    const char * charBuf = static_cast< const char * >(buffer);
    OSLM_TRACE("writing " << path);
    fs.write(charBuf, size);
    fs.close();
    return !fs.bad();
}

//-----------------------------------------------------------------------------

bool BufferManager::readBuffer(void * buffer, SizeType size, ::boost::filesystem::path &path)
{
    ::boost::filesystem::ifstream fs(path, std::ios::in|std::ios::binary|std::ios::ate);
    FW_RAISE_IF("Unable to read " << path, !fs.good());

    std::streampos fileSize = fs.tellg();
    fs.seekg(0, std::ios::beg);

    FW_RAISE_IF(path << ": Bad file size, expected: " << size << ", was: " << fileSize,
            size - fileSize != 0);

    OSLM_TRACE("reading " << path);
    char * charBuf = static_cast< char * >(buffer);
    fs.read(charBuf, size);

    fs.close();
    return !fs.bad();
}

//-----------------------------------------------------------------------------

std::string BufferManager::toString() const
{
    std::stringstream sstr ("");
    sstr << "nb Elem = " << m_dumpedBufferInfos.size() << std::endl;
    sstr    << std::setw(18) << "Buffer" << "->" << std::setw(18) << "Address" << " "
            << std::setw(10) << "Size" << " "
            << std::setw(18) << "Policy" << " "
            << std::setw(6) << "Access" << " "
            << std::setw(4) << "Lock" << " "
            << "DumpStatus" << " "
            << "DumpedFile" << " "
            << std::endl;
    BOOST_FOREACH( DumpedBufferInfoMapType::value_type item, m_dumpedBufferInfos )
    {
        DumpedBufferInfo & info = item.second;
        sstr    << std::setw(18) << item.first << "->" << std::setw(18) << *(item.first) << " "
                << std::setw(10) << info.size << " "
                << std::setw(18) << info.bufferPolicy << " "
                << std::setw(6) << info.lastAccess << " "
                << std::setw(4) << *(info.lockCount) << " "
                << ((info.isDumped)?"    dumped":"not dumped") << " "
                << info.dumpedFile << " "
                << std::endl;
    }
    return sstr.str();
}

//-----------------------------------------------------------------------------

} //namespace fwMemory



