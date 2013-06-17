/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iosfwd> // #include <strstream>
#include <iomanip>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <fwTools/System.hpp>

#include "fwMemory/stream/in/Raw.hpp"
#include "fwMemory/stream/in/Buffer.hpp"
#include "fwMemory/policy/NeverDump.hpp"
#include "fwMemory/BufferManager.hpp"

namespace fwMemory
{


//-----------------------------------------------------------------------------

BufferManager::BufferManager()
{
    m_dumpPolicy = ::fwMemory::policy::NeverDump::New();
}

//-----------------------------------------------------------------------------

BufferManager::~BufferManager()
{
    // TODO restore dumped buffers
}

//-----------------------------------------------------------------------------

bool BufferManager::registerBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, IBufferManager::LockCountFunctionType lockCount)
{
    SLM_TRACE_FUNC();
    BufferInfo & info = m_bufferInfos[buffer];
    info.lockCount = lockCount;
    m_updated();
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::unregisterBuffer(::fwMemory::IBufferManager::BufferPtrType buffer)
{
    SLM_TRACE_FUNC();

    {
        BufferInfo & info = m_bufferInfos[buffer];

        if(info.isDumped)
        {
            ::boost::filesystem::remove( info.dumpedFile );
        }
    }

    m_bufferInfos.erase(buffer);
    m_updated();
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::allocateBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, SizeType size, ::fwMemory::BufferAllocationPolicy::sptr policy)
{
    SLM_TRACE_FUNC();
    BufferInfo & info = m_bufferInfos[buffer];

    SLM_ASSERT("Unable to allocate a dumped buffer", !info.isDumped);

    if(info.isDumped)
    {
        ::boost::filesystem::remove( info.dumpedFile );
        info.dumpedFile = "";
        info.size = 0;
        info.isDumped = false;
        SLM_ASSERT("requested an allocation of a dumped buffer", 0);
    }

    m_dumpPolicy->allocationRequest( info, buffer, size );

    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::setBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, SizeType size, ::fwMemory::BufferAllocationPolicy::sptr policy)
{
    SLM_TRACE_FUNC();
    BufferInfo & info = m_bufferInfos[buffer];

    SLM_ASSERT("Unable to overwrite a dumped buffer", !info.isDumped);

    m_dumpPolicy->setRequest( info, buffer, size );

    if(info.isDumped)
    {
        ::boost::filesystem::remove( info.dumpedFile );
        info.dumpedFile = "";
        info.size = 0;
        info.isDumped = false;
        SLM_ASSERT("requested a replacement of a dumped buffer", 0);
    }

    info.lastAccess.modified();
    info.size = size;
    info.bufferPolicy = policy;
    info.istreamFactory = ::boost::make_shared< ::fwMemory::stream::in::Buffer >(*buffer, size);
    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::reallocateBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, SizeType newSize)
{
    SLM_TRACE_FUNC();

    BufferInfo & info = m_bufferInfos[buffer];

    m_dumpPolicy->reallocateRequest( info, buffer, newSize );

    info.lastAccess.modified();
    info.size = newSize;

    if(info.isDumped)
    {
        this->restoreBuffer( info, buffer, newSize );
        return false;
    }

    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::destroyBuffer(::fwMemory::IBufferManager::BufferPtrType buffer)
{
    SLM_TRACE_FUNC();
    BufferInfo & info = m_bufferInfos[buffer];

    m_dumpPolicy->destroyRequest( info, buffer );

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

    info.istreamFactory.reset();

    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::swapBuffer(::fwMemory::IBufferManager::BufferPtrType bufA, ::fwMemory::IBufferManager::BufferPtrType bufB)
{
    SLM_TRACE_FUNC();
    BufferInfo & infoA = m_bufferInfos[bufA];
    BufferInfo & infoB = m_bufferInfos[bufB];

    std::swap(*bufA, *bufB);
    std::swap(infoA.size, infoB.size);
    std::swap(infoA.isDumped, infoB.isDumped);
    std::swap(infoA.dumpedFile, infoB.dumpedFile);
    std::swap(infoA.bufferPolicy, infoB.bufferPolicy);
    std::swap(infoA.istreamFactory, infoB.istreamFactory);
    infoA.lastAccess.modified();
    infoB.lastAccess.modified();
    return false;
}
//-----------------------------------------------------------------------------

bool BufferManager::lockBuffer(::fwMemory::IBufferManager::ConstBufferPtrType buffer)
{
    SLM_TRACE_FUNC();

    ::fwMemory::IBufferManager::BufferPtrType castedBuffer = const_cast< ::fwMemory::IBufferManager::BufferPtrType >(buffer);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    m_dumpPolicy->lockRequest( info, castedBuffer );

    if ( info.isDumped )
    {
        bool restored = this->restoreBuffer( buffer );
        OSLM_ASSERT( "restore not OK ( "<< restored << " && " << *buffer <<" != 0 ).", restored && *buffer != 0 );
        FwCoreNotUsedMacro(restored);
    }

    m_lastAccess.modified();

    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::unlockBuffer(::fwMemory::IBufferManager::ConstBufferPtrType buffer)
{
    SLM_TRACE_FUNC();

    ::fwMemory::IBufferManager::BufferPtrType castedBuffer = const_cast< ::fwMemory::IBufferManager::BufferPtrType >(buffer);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    m_dumpPolicy->unlockRequest( info, castedBuffer );

    m_updated();
    return true;
}

//-----------------------------------------------------------------------------

bool BufferManager::dumpBuffer(::fwMemory::IBufferManager::ConstBufferPtrType  buffer)
{

    ::fwMemory::IBufferManager::BufferPtrType castedBuffer = const_cast< ::fwMemory::IBufferManager::BufferPtrType >(buffer);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    return this->dumpBuffer(info, castedBuffer);
}


//-----------------------------------------------------------------------------

bool BufferManager::dumpBuffer(BufferManager::BufferInfo & info, ::fwMemory::IBufferManager::BufferPtrType buffer)
{
    SLM_TRACE_FUNC();
    if ( info.isDumped || info.lockCount() > 0 || info.size == 0 )
    {
        return false;
    }
    OSLM_TRACE("dumping " << buffer);


    ::boost::filesystem::path tmp = ::fwTools::System::getTemporaryFolder();
    info.dumpedFile = ::boost::filesystem::unique_path( tmp / "fwMemory-%%%%-%%%%-%%%%-%%%%.raw" );

    if ( this->writeBuffer(*buffer, info.size, info.dumpedFile) )
    {
        info.istreamFactory = ::boost::make_shared< ::fwMemory::stream::in::Raw >(info.dumpedFile);
        info.bufferPolicy->destroy(*buffer);
        *buffer = NULL;
        info.isDumped = true;

        m_dumpPolicy->dumpSuccess( info, buffer );

        m_updated();
    }

    return info.isDumped;
}

//-----------------------------------------------------------------------------

bool BufferManager::restoreBuffer(::fwMemory::IBufferManager::ConstBufferPtrType  buffer)
{
    ::fwMemory::IBufferManager::BufferPtrType castedBuffer = const_cast< ::fwMemory::IBufferManager::BufferPtrType >(buffer);
    BufferInfo & info = m_bufferInfos[castedBuffer];

    return this->restoreBuffer(info, castedBuffer);
}


//-----------------------------------------------------------------------------

bool BufferManager::restoreBuffer(BufferManager::BufferInfo & info,
                                  ::fwMemory::IBufferManager::BufferPtrType buffer, BufferManager::SizeType allocSize)
{
    SLM_TRACE_FUNC();

    allocSize = ((allocSize) ? allocSize : info.size);
    if ( info.isDumped )
    {
        OSLM_TRACE("Restoring " << buffer);

        info.bufferPolicy->allocate(*buffer, allocSize);

        char * charBuf = static_cast< char * >(*buffer);
        SizeType size = std::min(allocSize, info.size);
        bool notFailed = false;
        {
            SPTR(std::istream) isptr = (*info.istreamFactory)();
            std::istream &is = *isptr;
            SizeType read = is.read(charBuf, size).gcount();

            FW_RAISE_IF(" Bad file size, expected: " << size << ", was: " << read, size - read != 0);
            notFailed = !is.fail();
        }

        if ( notFailed )
        {
            info.isDumped = false;
            if(!info.dumpedFile.empty())
            {
                ::boost::filesystem::remove( info.dumpedFile );
                info.dumpedFile.clear();
            }
            info.lastAccess.modified();

            m_dumpPolicy->restoreSuccess( info, buffer );

            info.istreamFactory = ::boost::make_shared< ::fwMemory::stream::in::Buffer >(*buffer, allocSize);
            m_updated();
            return true;
        }

    }
    return false;
}

//-----------------------------------------------------------------------------

bool BufferManager::writeBuffer(::fwMemory::IBufferManager::ConstBufferType buffer, SizeType size, ::boost::filesystem::path &path)
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

bool BufferManager::readBuffer(::fwMemory::IBufferManager::BufferType buffer, SizeType size, ::boost::filesystem::path &path)
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
    sstr << "nb Elem = " << m_bufferInfos.size() << std::endl;
    sstr    << std::setw(18) << "Buffer" << "->" << std::setw(18) << "Address" << " "
            << std::setw(10) << "Size" << " "
            << std::setw(18) << "Policy" << " "
            << std::setw(6) << "Access" << " "
            << std::setw(4) << "Lock" << " "
            << "DumpStatus" << " "
            << "DumpedFile" << " "
            << std::endl;
    BOOST_FOREACH( BufferInfoMapType::value_type item, m_bufferInfos )
    {
        BufferInfo & info = item.second;
        sstr    << std::setw(18) << item.first << "->" << std::setw(18) << *(item.first) << " "
                << std::setw(10) << info.size << " "
                << std::setw(18) << info.bufferPolicy << " "
                << std::setw(6) << info.lastAccess << " "
                << std::setw(4) << info.lockCount() << " "
                << ((info.isDumped)?"    dumped":"not dumped") << " "
                << info.dumpedFile << " "
                << std::endl;
    }
    return sstr.str();
}

//-----------------------------------------------------------------------------

void BufferManager::setDumpPolicy( ::fwMemory::IPolicy::sptr policy )
{
    m_dumpPolicy = policy;
    policy->setManager(this->getSptr());
    policy->refresh();
}

//-----------------------------------------------------------------------------

::fwMemory::IPolicy::sptr BufferManager::getDumpPolicy() const
{
    return m_dumpPolicy;
}

//-----------------------------------------------------------------------------

BufferManager::SizeType BufferManager::getDumpedBufferSize() const
{
    SizeType dumpedBufferSize = 0;
    BOOST_FOREACH( BufferInfoMapType::value_type item, m_bufferInfos )
    {
        BufferInfo & info = item.second;
        if ( info.isDumped )
        {
            dumpedBufferSize += info.size;
        }
    }
    return dumpedBufferSize;
}

//-----------------------------------------------------------------------------

BufferManager::SizeType BufferManager::getManagedBufferSize() const
{
    SizeType managedBufferSize = 0;
    BOOST_FOREACH( BufferInfoMapType::value_type item, m_bufferInfos )
    {
        BufferInfo & info = item.second;
        managedBufferSize += info.size;
    }
    return managedBufferSize;
}

//-----------------------------------------------------------------------------

bool BufferManager::isDumped(const ::fwMemory::IBufferManager::ConstBufferPtrType buffer) const
{
    ::fwMemory::IBufferManager::BufferPtrType castedBuffer = const_cast< ::fwMemory::IBufferManager::BufferPtrType >(buffer);
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(castedBuffer);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    return iterInfo->second.isDumped;
}

//-----------------------------------------------------------------------------

::boost::filesystem::path BufferManager::getDumpedFilePath(const ::fwMemory::IBufferManager::ConstBufferPtrType buffer) const
{
    ::fwMemory::IBufferManager::BufferPtrType castedBuffer = const_cast< ::fwMemory::IBufferManager::BufferPtrType >(buffer);
    BufferInfoMapType::const_iterator iterInfo = m_bufferInfos.find(castedBuffer);
    FW_RAISE_IF("Buffer is not managed by fwMemory::BufferManager.", iterInfo == m_bufferInfos.end() );
    return iterInfo->second.dumpedFile;
}

//-----------------------------------------------------------------------------

} //namespace fwMemory



